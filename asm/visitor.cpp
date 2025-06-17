#include "visitor.h"

using namespace std;
using namespace mc68000;

uint16_t visitor::finalize_instruction(uint16_t opcode)
{
	code.push_back(opcode);
	currentAddress++;
	if (extensionCount > 0)
	{
		for (int i = 0; i < extensionCount; i++)
		{
			currentAddress++;
			code.push_back(extensions[i]);
		}
	}
	return opcode;
}

any visitor::generateCode(tree::ParseTree* tree)
{
	pass = 0;
	tree->accept(this);
	code.clear();
	currentAddress = 0;
	pass = 1;
	auto result = tree->accept(this);

	return result;
}

any visitor::visitProg(parser68000::ProgContext* ctx)
{
    size_t n = ctx->children.size();
	vector<any> results;
	for (size_t i = 0; i < n; i++)
	{
		auto result = ctx->children[i]->accept(this);
		results.push_back(result);
	}

    return results;
}

any visitor::visitLine_instructionSection(parser68000::Line_instructionSectionContext* ctx)
{
	size = 1;
    any result = ctx->children[1]->accept(this);
    return result;
}

any visitor::visitLabelSection(parser68000::LabelSectionContext* ctx)
{
	if (pass == 0 && !ctx->value.empty())
	{
		if (labels.find(ctx->value) == labels.end())
		{
			labels[ctx->value] = currentAddress;
		}
		else
		{
			addPass0Error("Duplicate label: " + ctx->value, ctx);
		}
	}
	return any();
}

any visitor::visitAbcd_dRegister(parser68000::Abcd_dRegisterContext* ctx)
{
    uint16_t dy = any_cast<uint16_t>(visit(ctx->children[1])) & 0b111;
    uint16_t dx = any_cast<uint16_t>(visit(ctx->children[3])) & 0b111;

    uint16_t opcode = 0b1100'000'10000'0'000 | (dx << 9) | dy;
    return opcode;
}

any visitor::visitAbcd_indirect(parser68000::Abcd_indirectContext* ctx) 
{
    uint16_t ay = any_cast<uint16_t>(visit(ctx->children[1])) & 0b111;
    uint16_t ax = any_cast<uint16_t>(visit(ctx->children[3])) & 0b111;

    uint16_t opcode = 0b1100'000'10000'1'000 | (ax << 9) | ay;
    return opcode;
}

any visitor::visitAdd_to_dRegister(parser68000::Add_to_dRegisterContext* ctx)
{
	auto sz = ctx->children.size();
	uint16_t size = 1;
	int arg = 1;
	if (sz == 5)
	{
		size = any_cast<uint16_t>(visit(ctx->children[1]));
		arg++; // the optional size is included so there is one extra child
	}

	uint16_t ae = any_cast<uint16_t>(visit(ctx->children[arg]));
	auto count = extensionCount;
	uint16_t dReg = any_cast<uint16_t>(visit(ctx->children[arg+2])) & 0b111;
	extensionCount = count;

	uint16_t opcode = 0b1101'000'000'000'000 | (dReg << 9) | (size << 6) | ae;
	return finalize_instruction(opcode);
}

any visitor::visitAdd_from_dRegister(parser68000::Add_from_dRegisterContext* ctx)
{
	auto sz = ctx->children.size();
	uint16_t size = 1; 
	int arg = 1;
	if (sz == 5) 
	{
		size = any_cast<uint16_t>(visit(ctx->children[1]));
		arg++; // the optional size is included so there is one extra child
	}
	uint16_t dReg = any_cast<uint16_t>(visit(ctx->children[arg])) & 0b111;
	uint16_t ae = any_cast<uint16_t>(visit(ctx->children[arg+2]));

	uint16_t opcode = (0b1101'000'100'000'000 | (dReg << 9) | (size << 6) | ae);
	return finalize_instruction(opcode);
}

any visitor::visitNop(parser68000::NopContext* ctx)
{
	uint16_t opcode = 0b0100'1110'0111'0001;
	extensionCount = 0;
	return finalize_instruction(opcode);
}

// ====================================================================================================
// Addressing modes
// ====================================================================================================

/// <summary>
/// Addressing mode for data registers: D0
/// </summary>
any visitor::visitDRegister(parser68000::DRegisterContext* ctx)
{
	extensionCount = 0;
    auto s = ctx->getText().substr(1);
	auto reg = stoi(s);
    return (uint16_t) (0b000'000 | reg);
}

/// <summary>
/// Addressing mode for address registers: A0
/// </summary>
any visitor::visitARegister(parser68000::ARegisterContext* ctx)
{
	extensionCount = 0;
	auto s = ctx->getText().substr(1);
    auto reg = stoi(s);
    return (uint16_t)(0b001'000 | reg);
}

/// <summary>
/// Addressing mode for address register indirect: (A0)
/// </summary>
any visitor::visitARegisterIndirect(parser68000::ARegisterIndirectContext* context)
{
	extensionCount = 0;
	auto s = context->children[1]->getText().substr(1);
	auto reg = stoi(s);
	return (uint16_t)(0b010'000 | reg);
}

/// <summary>
/// Addressing mode for address register indirect post increment: (A0)+
/// </summary>
any visitor::visitARegisterIndirectPostIncrement(parser68000::ARegisterIndirectPostIncrementContext* ctx)
{
	extensionCount = 0;
	auto s = ctx->children[1]->getText().substr(1);
	auto reg = stoi(s);
	return (uint16_t)(0b011'000 | reg);
}

/// <summary>
/// Addressing mode for address register indirect pre decrement: -(A0)
/// </summary>
any visitor::visitARegisterIndirectPreDecrement(parser68000::ARegisterIndirectPreDecrementContext* ctx)
{
	extensionCount = 0;
	auto s = ctx->children[2]->getText().substr(1);
	auto reg = stoi(s);
    return (uint16_t)(0b100'000 | reg);
}

/// <summary>
/// Addressing mode for address register indirect with displacement
/// This is a helper function to handle both old and new syntax for address register indirect with displacement
/// It is used to avoid code duplication in the visitARegisterIndirectDisplacementOld and visitARegisterIndirectDisplacementNew methods
/// </summary>
any visitor::visitARegisterIndirectDisplacement(tree::ParseTree* pDisplacement, tree::ParseTree* pRegistr)
{
	int32_t displacement = any_cast<int32_t>(visit(pDisplacement));
	if (displacement > 0x7fff || displacement < -0x8000)
	{
		addError("Displacement doesn't fit on in one word: " + std::to_string(displacement), pDisplacement);
	}
	extensionCount = 1;
	extensions[0] = (uint16_t)(displacement & 0xffff);

	auto s = pRegistr->getText().substr(1);
	auto reg = stoi(s);
	return (uint16_t)(0b101'000 | reg);
}

/// <summary>
/// Addressing mode for address register indirect with displacement: 42(A0)
/// </summary>
any visitor::visitARegisterIndirectDisplacementOld(parser68000::ARegisterIndirectDisplacementOldContext* ctx)
{
	return visitARegisterIndirectDisplacement(ctx->children[0], ctx->children[2]);
}

/// <summary>
/// Addressing mode for address register indirect with displacement: (42,A0)
/// </summary>
any visitor::visitARegisterIndirectDisplacementNew(parser68000::ARegisterIndirectDisplacementNewContext* ctx)
{
	return visitARegisterIndirectDisplacement(ctx->children[1], ctx->children[3]);
}

/// <summary>
/// Addressing mode for address register indirect with index
/// This is a helper function to handle both old and new syntax for address register indirect with index
/// It is used to avoid code duplication in the visitARegisterIndirectIndexOld and visitARegisterIndirectIndexNew methods
/// </summary>
any visitor::visitARegisterIndirectDisplacement(tree::ParseTree* pDisplacement, tree::ParseTree* pRregistr, tree::ParseTree* pIndex)
{
	int32_t displacement = any_cast<int32_t>(visit(pDisplacement));
	if (displacement > 0x7f || displacement < -0x80)
	{
		addError("Displacement doesn't fit on in one byte: " + std::to_string(displacement), pDisplacement);
	}
	uint16_t displacement8 = (uint16_t)(displacement & 0xff);

	uint16_t index = any_cast<uint16_t>(visit(pIndex));
	extensionCount = 1;
	extensions[0] = (index << 11) | displacement8;
	
	auto s = pRregistr->getText().substr(1);
	auto reg = stoi(s);
	return (uint16_t)(0b110'000 | reg);
}

/// <summary>
/// Addressing mode for address register indirect with index: 42(A0, D1)
/// </summary>
any visitor::visitARegisterIndirectIndexOld(parser68000::ARegisterIndirectIndexOldContext* ctx)
{
	return visitARegisterIndirectDisplacement(ctx->children[0], ctx->children[2], ctx->children[4]);
}

/// <summary>
/// Addressing mode for address register indirect with index: (42, A0, D1)
/// </summary>
any visitor::visitARegisterIndirectIndexNew(parser68000::ARegisterIndirectIndexNewContext* ctx)
{
	return visitARegisterIndirectDisplacement(ctx->children[1], ctx->children[3], ctx->children[5]);
}

/// <summary>
/// Addressing mode for absolute short: 42
/// </summary>
any visitor::visitAbsoluteShort(parser68000::AbsoluteShortContext* ctx)
{
	any address = visit(ctx->children[0]);
	if (address.type() == typeid(std::string))
	{
		std::string label = any_cast<std::string>(address);
		auto it = labels.find(label);
		if (it == labels.end())
		{
			// During the 1st pass the label may not be defined yet
			if (pass != 0)
			{
				addError("Label not found: " + label, ctx->children[0]);
			}
			extensionCount = 1;
			extensions[0] = 0;
		}
		else
		{
			uint32_t displacement = it->second;
			if (displacement > 0x7fff || displacement < -0x8000)
			{
				addError("Address doesn't fit on in one word: " + label, ctx->children[0]);
			}
			uint16_t displacement16 = (uint16_t)(displacement & 0xffff);
			extensionCount = 1;
			extensions[0] = displacement16;
		}
	}
	else if (address.type() == typeid(int32_t))
	{
		int32_t displacement = any_cast<int32_t>(address);
		if (displacement > 0x7fff || displacement < -0x8000)
		{
			addError("Address doesn't fit on in one word: " + std::to_string(displacement), ctx->children[0] );
		}
		uint16_t displacement16 = (uint16_t)(displacement & 0xffff);
		extensionCount = 1;
		extensions[0] = displacement16;
	}

	return (uint16_t)(0b111'000);
}

/// <summary>
/// Addressing mode for absolute long: 42L
/// </summary>
any visitor::visitAbsoluteLong(parser68000::AbsoluteLongContext* ctx)
{
	int32_t displacement = any_cast<int32_t>(visit(ctx->children[0]));
	extensionCount = 2;
	uint16_t displacementHigh = (uint16_t)(displacement >> 16);
	uint16_t displacementLow = (uint16_t)(displacement & 0xffff);
	extensions[0] = displacementHigh;
	extensions[1] = displacementLow;

	return (uint16_t)(0b111'001);
}

/// <summary>
/// Addressing mode for PC indirect with displacement: 42(PC)
/// </summary>
any visitor::visitPcIndirectDisplacement(parser68000::PcIndirectDisplacementContext* ctx)
{
	int32_t displacement = any_cast<int32_t>(visit(ctx->children[0]));
	if (displacement > 0x7fff || displacement < -0x8000)
	{
		addError("Displacement doesn't fit on in one word: " + std::to_string(displacement), ctx->children[0]);
	}
	extensionCount = 1;
	extensions[0] = (uint16_t)(displacement & 0xffff);

	return (uint16_t)(0b111'010);
}

/// <summary>
/// Addressing mode for PC indirect with index: 42(PC, D1)
/// </summary>
any visitor::visitPcIndirectIndex(parser68000::PcIndirectIndexContext* ctx)
{
	int32_t displacement = any_cast<int32_t>(visit(ctx->children[0]));
	if (displacement > 0x7f || displacement < -0x80)
	{
		addError("Displacement doesn't fit on in one byte: " + std::to_string(displacement), ctx->children[0]);
	}
	uint16_t displacement8 = (uint16_t)(displacement & 0xff);

	uint16_t index = any_cast<uint16_t>(visit(ctx->children[4]));
	extensionCount = 1;
	extensions[0] = (index << 11) | displacement8;

	return (uint16_t)(0b111'011);
}

/// <summary>
/// Addressing mode for immediate data: #42
/// </summary>
any visitor::visitImmediateData(parser68000::ImmediateDataContext* ctx)
{
	int32_t displacement = any_cast<int32_t>(visit(ctx->children[1]));
	if (size == 0)
	{
		if (displacement > 0x7f || displacement < -0x80)
		{
			addError("Immediate data doesn't fit on in one byte: " + std::to_string(displacement), ctx->children[1]);
		}
		uint16_t displacement8 = (uint16_t)(displacement & 0xff);
		extensionCount = 1;
		extensions[0] = displacement8;
	}
	else if (size == 1)
	{
		if (displacement > 0x7fff || displacement < -0x8000)
		{
			addError("Immediate data doesn't fit on in one word: " + std::to_string(displacement), ctx->children[1]);
		}
		uint16_t displacement16 = (uint16_t)(displacement & 0xffff);
		extensionCount = 1;
		extensions[0] = displacement16;
	}
	else
	{
		extensionCount = 2; 
		uint16_t displacementHigh = (uint16_t)(displacement >> 16);
		uint16_t displacementLow = (uint16_t)(displacement & 0xffff);
		extensions[0] = displacementHigh;
		extensions[1] = displacementLow;
	}
	return (uint16_t)(0b111'100);
}

// ====================================================================================================
// Addressing modes helpers
// ====================================================================================================

any visitor::visitAdRegister(parser68000::AdRegisterContext* ctx)
{
	auto s = ctx->children[0]->getText();
	auto reg = stoi(s.substr(1));
	// The returned value is formatted for the aRegisterIndirectIndex addressing mode
	if (s[0] == 'a' || s[0] == 'A')
	{
		return (uint16_t)(0b1'000'0 | (reg << 1));
	}
	else
	{
		return (uint16_t)(0b0'000'0 | (reg << 1));
	}
}

any visitor::visitAdRegisterSize(parser68000::AdRegisterSizeContext* ctx)
{
	uint16_t adRegister = any_cast<uint16_t>(visit(ctx->children[0]));
	if (ctx->children.size() == 2)
	{
		int16_t size = any_cast<uint16_t>(visit(ctx->children[1]));
		if (size == 0)
		{
			addError("Invalid size", ctx->children[1]);
		}
		else if (size == 2)
		{
			adRegister |= 0b0'000'1; // size is long
		}
	}
	return adRegister;
}


void visitor::addError(const std::string& message, tree::ParseTree* ctx)
{
    if (pass == 1) 
	{
        if (auto* prc = dynamic_cast<antlr4::ParserRuleContext*>(ctx)) 
		{
            int line = prc->getStart()->getLine();
            int col = prc->getStart()->getCharPositionInLine();
            errorList.add(message, line, col);
        } 
		else
		{
            errorList.add(message,0,0);
        }
    }
}

void visitor::addPass0Error(const std::string& message, tree::ParseTree* ctx)
{
	if (auto* prc = dynamic_cast<antlr4::ParserRuleContext*>(ctx))
	{
		int line = prc->getStart()->getLine();
		int col = prc->getStart()->getCharPositionInLine();
		errorList.add(message, line, col);
	}
	else
	{
		errorList.add(message, 0, 0);
	}
}
