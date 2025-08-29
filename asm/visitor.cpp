#include "visitor.h"

using namespace std;
using namespace mc68000;

uint16_t visitor::finalize_instruction(uint16_t opcode)
{
	code.push_back(opcode);
	currentAddress += 2;
	if (!extensionsList.empty())
	{
		code.insert(code.end(), extensionsList.begin(), extensionsList.end());
	}
	currentAddress += 2 * (uint32_t) extensionsList.size();
	extensionsList.clear();
	return opcode;
}

any visitor::generateCode(tree::ParseTree* tree)
{
	pass = 0;
	tree->accept(this);
	code.clear();
	extensionsList.clear();
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

	uint16_t effectiveAddress = any_cast<uint16_t>(visit(ctx->children[arg]));
	uint16_t dReg = any_cast<uint16_t>(visit(ctx->children[arg+2])) & 0b111;

	uint16_t opcode = 0b1101'000'000'000'000 | (dReg << 9) | (size << 6) | effectiveAddress;
	return finalize_instruction(opcode);
}

any visitor::visitAdd_from_dRegister(parser68000::Add_from_dRegisterContext* ctx)
{
	uint16_t size = 1; 
	int arg = 1;
	if (ctx->children.size() == 5)
	{
		size = any_cast<uint16_t>(visit(ctx->children[1]));
		arg++; // the optional size is included so there is one extra child
	}
	uint16_t dReg = any_cast<uint16_t>(visit(ctx->children[arg])) & 0b111;
	uint16_t effectiveAddress = any_cast<uint16_t>(visit(ctx->children[arg+2]));

	uint16_t opcode = (0b1101'000'100'000'000 | (dReg << 9) | (size << 6) | effectiveAddress);
	return finalize_instruction(opcode);
}

any visitor::visitAdda(parser68000::AddaContext* ctx)
{
	uint16_t opmode = 0b011;
	int arg = 1;
	if (ctx->children.size() == 5) // if there is a size specified
	{
		size = any_cast<uint16_t>(visit(ctx->children[1]));
		if (size == 0)
		{
			addError("invalid size only .W or .L are supported", ctx->children[1]);
		}
		else if (size == 2)
		{
			opmode = 0b111;
		}

		arg++; // the optional size is included so there is one extra child
	}
	uint16_t effectiveAddress = any_cast<uint16_t>(visit(ctx->children[arg]));
	uint16_t aReg = any_cast<uint16_t>(visit(ctx->children[arg+2])) & 0b111;
	uint16_t opcode = 0b1101'000'011'000'000 | (aReg << 9) | (opmode << 6) | effectiveAddress;
	return finalize_instruction(opcode);
}

/// <summary>
/// ADDQ size? HASH number COMMA addressingMode
/// </summary>
any visitor::visitAddq(parser68000::AddqContext* ctx)
{
	size = 1;
	int arg = 1;
	if (ctx->children.size() == 6) // if there is a size specified
	{
		size = any_cast<uint16_t>(visit(ctx->children[1]));
		arg++; // the optional size is included so there is one extra child
	}
	int32_t immediate_data = any_cast<int32_t>(visit(ctx->children[arg+1]));
	if (immediate_data <= 0 || immediate_data > 8)
	{
		addError("Immediate data for ADDQ must be between 0 and 8", ctx->children[arg+1]);
		immediate_data = 0; // reset to 0 to avoid further errors
	}
	else if (immediate_data == 8)
	{
		immediate_data = 0; // 8 is treated as 0 in the instruction encoding
	}

	uint16_t effectiveAddress = any_cast<uint16_t>(visit(ctx->children[arg + 3]));
	if (!isValidAddressingMode(effectiveAddress, 0b111111111000))
	{
		addError("Invalid addressing mode: ", ctx->children[arg + 3]);
	}
	else if (size == 0 && 0b001'000 == (effectiveAddress & 0b111'000))
	{
		addError("ADDQ to Address register should be word or long", ctx->children[arg + 3]);
	}
	uint16_t opcode = 0b0101'000'0'00'000'000 | (immediate_data << 9) | (size << 6) | effectiveAddress;
	return finalize_instruction(opcode);
}

/// <summary>
/// ADDX size? dRegister COMMA dRegister
/// </summary>
any visitor::visitAddx_dRegister(parser68000::Addx_dRegisterContext* ctx)
{
	size = 1;
	int arg = 1;
	if (ctx->children.size() == 5) // if there is a size specified
	{
		size = any_cast<uint16_t>(visit(ctx->children[1]));
		arg++; // the optional size is included so there is one extra child
	}
	uint16_t dy = any_cast<uint16_t>(visit(ctx->children[arg])) & 0b111;
	uint16_t dx = any_cast<uint16_t>(visit(ctx->children[arg+2])) & 0b111;

	uint16_t opcode = 0b1101'000'1'00'00'0'000 | (dx << 9) | (size << 6) | dy;
	return finalize_instruction(opcode);
}

/// <summary>
/// ADDX size? aRegisterIndirectPreDecrement COMMA aRegisterIndirectPreDecrement
/// </summary>
any visitor::visitAddx_indirect(parser68000::Addx_indirectContext* ctx)
{
	size = 1;
	int arg = 1;
	if (ctx->children.size() == 5) // if there is a size specified
	{
		size = any_cast<uint16_t>(visit(ctx->children[1]));
		arg++; // the optional size is included so there is one extra child
	}
	uint16_t ay = any_cast<uint16_t>(visit(ctx->children[arg])) & 0b111;
	uint16_t ax = any_cast<uint16_t>(visit(ctx->children[arg+2])) & 0b111;

	uint16_t opcode = 0b1101'000'1'00'00'1'000 | (ax << 9) | (size << 6) | ay;
	return finalize_instruction(opcode);
}

/// <summary>
/// AND size? addressingMode COMMA dRegister #and_to_dRegister
/// </summary>
any visitor::visitAnd_to_dRegister(parser68000::And_to_dRegisterContext* ctx)
{
	auto sz = ctx->children.size();
	uint16_t size = 1;
	int arg = 1;
	if (sz == 5)
	{
		size = any_cast<uint16_t>(visit(ctx->children[1]));
		arg++; // the optional size is included so there is one extra child
	}

	uint16_t effectiveAddress = any_cast<uint16_t>(visit(ctx->children[arg]));
	if (!isValidAddressingMode(effectiveAddress, 0b101111'111111))
	{
		addError("Invalid addressing mode: ", ctx->children[arg]);
	}
	uint16_t dReg = any_cast<uint16_t>(visit(ctx->children[arg + 2])) & 0b111;

	uint16_t opcode = 0b1100'000'000'000'000 | (dReg << 9) | (size << 6) | effectiveAddress;
	return finalize_instruction(opcode);
}

/// <summary>
/// AND size? dRegister COMMA addressingMode #and_from_dRegister
/// </summary>
any visitor::visitAnd_from_dRegister(parser68000::And_from_dRegisterContext* ctx)
{
	uint16_t size = 1;
	int arg = 1;
	if (ctx->children.size() == 5)
	{
		size = any_cast<uint16_t>(visit(ctx->children[1]));
		arg++; // the optional size is included so there is one extra child
	}
	uint16_t dReg = any_cast<uint16_t>(visit(ctx->children[arg])) & 0b111;
	uint16_t effectiveAddress = any_cast<uint16_t>(visit(ctx->children[arg + 2]));
	if (!isValidAddressingMode(effectiveAddress, 0b001111'111000))
	{
		addError("Invalid addressing mode: ", ctx->children[arg + 2]);
	}

	uint16_t opcode = (0b1100'000'100'000'000 | (dReg << 9) | (size << 6) | effectiveAddress);
	return finalize_instruction(opcode);
}

/// <summary>
///  XXXI size? immediateData COMMA addressingMode
/// </summary>
any visitor::visitImmediate(parser68000::ImmediateContext* ctx)
{
	uint16_t specific_opcode = any_cast<uint16_t>(visit(ctx->children[0]));
	size = 1;
	int arg = 1;
	if (ctx->children.size() == 5) // if there is a size specified
	{
		size = any_cast<uint16_t>(visit(ctx->children[1]));
		arg++; // the optional size is included so there is one extra child
	}
	visit(ctx->children[arg]);

	uint16_t effectiveAddress = any_cast<uint16_t>(visit(ctx->children[arg + 2]));
	if (!isValidAddressingMode(effectiveAddress, 0b101111111000))
	{
		addError("Invalid addressing mode: ", ctx->children[arg + 2]);
	}
	uint16_t opcode = 0b0000'0000'00'000'000 | (specific_opcode << 8) | (size << 6) | effectiveAddress;
	return finalize_instruction(opcode);
}

any visitor::visitAndi2ccr(parser68000::Andi2ccrContext* ctx)
{
	size = 0;
	visit(ctx->children[1]);

	uint16_t opcode = 0b0000'0010'0011'1100 ;
	return finalize_instruction(opcode);
}

any visitor::visitAndi2sr(parser68000::Andi2srContext* ctx)
{
	size = 1;
	visit(ctx->children[1]);

	uint16_t opcode = 0b0000'0010'0111'1100;
	return finalize_instruction(opcode);
}

/// <summary>
/// ASL/ASR size? dRegister COMMA dRegister
/// </summary>
any visitor::visitAslAsr_dRegister(parser68000::AslAsr_dRegisterContext* ctx)
{
	uint16_t direction = any_cast<uint16_t>(visit(ctx->children[0]));

	size = 1;
	int arg = 1;
	if (ctx->children.size() == 5) // if there is a size specified
	{
		size = any_cast<uint16_t>(visit(ctx->children[1]));
		arg++; // the optional size is included so there is one extra child
	}
	uint16_t dx = any_cast<uint16_t>(visit(ctx->children[arg])) & 0b111;
	uint16_t dy = any_cast<uint16_t>(visit(ctx->children[arg + 2])) & 0b111;

	uint16_t opcode = 0b1110'000'0'00'1'00'000 | (dx << 9) | (direction << 8) | (size << 6) | dy;
	return finalize_instruction(opcode);
}

/// <summary>
/// ASL/ASR size? HASH number COMMA dRegister
/// </summary>
any visitor::visitAslAsr_immediateData(parser68000::AslAsr_immediateDataContext* ctx)
{
	uint16_t direction = any_cast<uint16_t>(visit(ctx->children[0]));

	size = 1;
	int arg = 2;
	if (ctx->children.size() == 6) // if there is a size specified
	{
		size = any_cast<uint16_t>(visit(ctx->children[1]));
		arg++; // the optional size is included so there is one extra child
	}
	int32_t immediate_data = any_cast<int32_t>(visit(ctx->children[arg]));
	if (immediate_data <= 0 || immediate_data > 8)
	{
		addError("Immediate data for ASL/ASR must be between 0 and 8", ctx->children[arg + 1]);
		immediate_data = 0; // reset to 0 to avoid further errors
	}
	else if (immediate_data == 8)
	{
		immediate_data = 0; // 8 is treated as 0 in the instruction encoding
	}
	uint16_t dy = any_cast<uint16_t>(visit(ctx->children[arg + 2])) & 0b111;

	uint16_t opcode = 0b1110'000'0'00'0'00'000 | (immediate_data << 9) | (direction << 8) | (size << 6) | dy;
	return finalize_instruction(opcode);
}

/// <summary>
/// ASL/ASR size? addressingMode
/// </summary>
any visitor::visitAslAsr_addressingMode(parser68000::AslAsr_addressingModeContext* ctx)
{
	uint16_t direction = any_cast<uint16_t>(visit(ctx->children[0]));

	size = 1;
	int arg = 1;
	if (ctx->children.size() == 3) // if there is a size specified
	{
		size = any_cast<uint16_t>(visit(ctx->children[1]));
		if (size != 1)
		{
			addError("Invalid size for ASL/ASR, only .W is supported", ctx->children[1]);
			size = 1;
		}
		arg++; // the optional size is included so there is one extra child
	}
	uint16_t effectiveAddress = any_cast<uint16_t>(visit(ctx->children[arg]));
	if (!isValidAddressingMode(effectiveAddress, 0b001111111000))
	{
		addError("Invalid addressing mode: ", ctx->children[arg]);
	}
	uint16_t opcode = 0b1110'000'0'11'000'000 | (direction << 8) | effectiveAddress;
	return finalize_instruction(opcode);
}

/// <summary>
/// BCC address
/// </summary>
any visitor::visitBcc(parser68000::BccContext* ctx)
{
	uint16_t condition = any_cast<uint16_t>(visit(ctx->children[0]));

	uint32_t target = 0;
	any address = visit(ctx->children[1]);
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
			target = 0; // Add a placeholder for the label
		}
		else
		{
			target = it->second;
		}
	}
	else if (address.type() == typeid(int32_t))
	{
		target = any_cast<int32_t>(address);
	}

	int32_t offset = target - currentAddress - 2; // -2 because the next instruction will be at currentAddress + 2
	if (offset > 0x7fff || offset < -0x8000)
	{
		addError("Address doesn't fit on in one word: " + std::to_string(offset), ctx->children[1]);
		offset = 0x100; // to force 2 words for the instruction 
	}
	uint16_t opcode = 0;
	if (address.type() == typeid(int32_t) && offset >= -128 && offset <= 127) // offset fits in one byte
	{
		uint16_t offset8 = (uint16_t)(offset & 0xff);
		opcode = 0b0110'0000'0000'0000 | (condition << 8) | offset8;
	}
	else
	{
		// The offset could potentially fit in one byte but during the 1st pass the exact offset may be unknown
		// so we need to always reserve two words for the instruction to avoid further address calculation errors. 
		uint16_t offset16 = (uint16_t)(offset & 0xffff);
		opcode = 0b0110'0000'0000'0000 | (condition << 8);
		extensionsList.push_back(offset16); // Add the offset as an extension
	}
	return finalize_instruction(opcode);
}

/// <summary>
/// BCHG dRegister COMMA addressingMode
/// </summary>
any visitor::visitBchg_dRegister(parser68000::Bchg_dRegisterContext* ctx) 
{
	size = 1;
	uint16_t dReg = any_cast<uint16_t>(visit(ctx->children[1])) & 0b111;
	uint16_t effectiveAddress = any_cast<uint16_t>(visit(ctx->children[3]));
	if (!isValidAddressingMode(effectiveAddress, 0b101111111000))
	{
		addError("Invalid addressing mode: ", ctx->children[3]);
	}
	uint16_t opcode = 0b0000'000'101'000'000 | (dReg << 9) | effectiveAddress;
	return finalize_instruction(opcode);
}

/// <summary>
/// BCHG HASH number COMMA addressingMode
/// </summary>
any visitor::visitBchg_immediateData(parser68000::Bchg_immediateDataContext* ctx)
{
	size = 0;
	int32_t immediate_data = any_cast<int32_t>(visit(ctx->children[2]));
	uint16_t data8 = (uint16_t)(immediate_data & 0xff);
	extensionsList.push_back(data8);

	uint16_t effectiveAddress = any_cast<uint16_t>(visit(ctx->children[4]));
	if (!isValidAddressingMode(effectiveAddress, 0b101111111000))
	{
		addError("Invalid addressing mode: ", ctx->children[4]);
	}
	if ((effectiveAddress & 0b111'000) == 0b000'000)
	{
		size = 2; // if the addressing mode is data register direct then the size must be long
	}

	if (immediate_data < 0 || (size == 0 && immediate_data > 7) || (size == 2 && immediate_data > 31))
	{
		addError(size == 0 
			? "Immediate data for Bchg must be between 0 and 7"
			: "Immediate data for Bchg must be between 0 and 31", ctx->children[2]);
	}

	uint16_t opcode = 0b0000'100'001'000'000 | effectiveAddress;
	return finalize_instruction(opcode);
}

/// <summary>
/// NOP
/// </summary>
any visitor::visitNop(parser68000::NopContext* ctx)
{
	uint16_t opcode = 0b0100'1110'0111'0001;
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
    auto s = ctx->getText().substr(1);
	auto reg = stoi(s);
    return (uint16_t) (0b000'000 | reg);
}

/// <summary>
/// Addressing mode for address registers: A0
/// </summary>
any visitor::visitARegister(parser68000::ARegisterContext* ctx)
{
	auto s = ctx->getText().substr(1);
    auto reg = stoi(s);
    return (uint16_t)(0b001'000 | reg);
}

/// <summary>
/// Addressing mode for address register indirect: (A0)
/// </summary>
any visitor::visitARegisterIndirect(parser68000::ARegisterIndirectContext* context)
{
	auto s = context->children[1]->getText().substr(1);
	auto reg = stoi(s);
	return (uint16_t)(0b010'000 | reg);
}

/// <summary>
/// Addressing mode for address register indirect post increment: (A0)+
/// </summary>
any visitor::visitARegisterIndirectPostIncrement(parser68000::ARegisterIndirectPostIncrementContext* ctx)
{
	auto s = ctx->children[1]->getText().substr(1);
	auto reg = stoi(s);
	return (uint16_t)(0b011'000 | reg);
}

/// <summary>
/// Addressing mode for address register indirect pre decrement: -(A0)
/// </summary>
any visitor::visitARegisterIndirectPreDecrement(parser68000::ARegisterIndirectPreDecrementContext* ctx)
{
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
	extensionsList.push_back((uint16_t)(displacement & 0xffff));

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
	extensionsList.push_back((index << 11) | displacement8);
	
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
			extensionsList.push_back(0); // Add a placeholder for the label
		}
		else
		{
			uint32_t displacement = it->second;
			if (displacement > 0x7fff || displacement < -0x8000)
			{
				addError("Address doesn't fit on in one word: " + label, ctx->children[0]);
			}
			uint16_t displacement16 = (uint16_t)(displacement & 0xffff);
			extensionsList.push_back(displacement16);
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
		extensionsList.push_back(displacement16);
	}

	return (uint16_t)(0b111'000);
}

/// <summary>
/// Addressing mode for absolute long: 42L
/// </summary>
any visitor::visitAbsoluteLong(parser68000::AbsoluteLongContext* ctx)
{
	int32_t displacement = any_cast<int32_t>(visit(ctx->children[0]));
	uint16_t displacementHigh = (uint16_t)(displacement >> 16);
	uint16_t displacementLow = (uint16_t)(displacement & 0xffff);
	extensionsList.push_back(displacementHigh);
	extensionsList.push_back(displacementLow);

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
	extensionsList.push_back((uint16_t)(displacement & 0xffff));
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
	extensionsList.push_back((index << 11) | displacement8);

	return (uint16_t)(0b111'011);
}

/// <summary>
/// Addressing mode for immediate data: #42
/// </summary>
any visitor::visitImmediateData(parser68000::ImmediateDataContext* ctx)
{
	int32_t immediate_data = any_cast<int32_t>(visit(ctx->children[1]));
	if (size == 0)
	{
		if (immediate_data > 0x7f || immediate_data < -0x80)
		{
			addError("Immediate data doesn't fit on in one byte: " + std::to_string(immediate_data), ctx->children[1]);
		}
		uint16_t data8 = (uint16_t)(immediate_data & 0xff);
		extensionsList.push_back(data8);
	}
	else if (size == 1)
	{
		if (immediate_data > 0x7fff || immediate_data < -0x8000)
		{
			addError("Immediate data doesn't fit on in one word: " + std::to_string(immediate_data), ctx->children[1]);
		}
		uint16_t data16 = (uint16_t)(immediate_data & 0xffff);
		extensionsList.push_back(data16);
	}
	else
	{
		uint16_t dataHigh = (uint16_t)(immediate_data >> 16);
		uint16_t dataLow = (uint16_t)(immediate_data & 0xffff);
		extensionsList.push_back(dataHigh);
		extensionsList.push_back(dataLow);
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

bool visitor::isValidAddressingMode(unsigned short effectiveAddress, unsigned short acceptable)
{
	static const unsigned masks[] = {
		1 << 11, 1 << 10, 1 << 9, 1 << 8, 1 << 7, 1 << 6, 1 << 5, // mode 000 to 110
		1 << 4, 1 << 3, 1 << 2, 1 << 1, 1                         // mode 111 
	};
	unsigned mode = effectiveAddress >> 3;
	if (mode != 7)
	{
		return (acceptable & masks[mode]) == masks[mode];
	}

	static const unsigned offsets[] = { 7, 8, 10, 11, 9 }; // 000, 001, 100, 010, 011
	mode = effectiveAddress & 7;
	if (mode > 4)
	{
		return false;
	}
	auto offset = offsets[mode];
	return (acceptable & masks[offset]) == masks[offset];
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
