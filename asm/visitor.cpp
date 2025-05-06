#include "visitor.h"

std::any visitor::visitProg(parser68000::ProgContext* ctx)
{
    size_t n = ctx->children.size();
	std::vector<std::any> results;
	for (size_t i = 0; i < n; i++)
	{
		auto result = ctx->children[i]->accept(this);
		results.push_back(result);
	}

    return results;
}

std::any visitor::visitLine_instructionSection(parser68000::Line_instructionSectionContext* ctx)
{
	size = 1;
    std::any result = ctx->children[1]->accept(this);
    return result;
}



std::any visitor::visitAbcd_dRegister(parser68000::Abcd_dRegisterContext* ctx)
{
    uint16_t dy = std::any_cast<uint16_t>(visit(ctx->children[1])) & 0b111;
    uint16_t dx = std::any_cast<uint16_t>(visit(ctx->children[3])) & 0b111;

    uint16_t opcode = 0b1100'000'10000'0'000 | (dx << 9) | dy;
    return opcode;
}

std::any visitor::visitAbcd_indirect(parser68000::Abcd_indirectContext* ctx) 
{
    uint16_t ay = std::any_cast<uint16_t>(visit(ctx->children[1])) & 0b111;
    uint16_t ax = std::any_cast<uint16_t>(visit(ctx->children[3])) & 0b111;

    uint16_t opcode = 0b1100'000'10000'1'000 | (ax << 9) | ay;
    return opcode;
}
uint16_t visitor::finalize_instruction(uint16_t opcode)
{
	code.push_back(opcode);
	if (extensionCount > 0)
	{
		for (int i = 0; i < extensionCount; i++)
		{
			code.push_back(extensions[i]);
		}
	}
	return opcode;
}
std::any visitor::visitAdd_to_dRegister(parser68000::Add_to_dRegisterContext* ctx)
{
	auto sz = ctx->children.size();
	uint16_t size = 1;
	int arg = 1;
	if (sz == 5)
	{
		size = std::any_cast<uint16_t>(visit(ctx->children[1]));
		arg++; // the optional size is included so there is one extra child
	}

	uint16_t ae = std::any_cast<uint16_t>(visit(ctx->children[arg]));
	auto count = extensionCount;
	uint16_t dReg = std::any_cast<uint16_t>(visit(ctx->children[arg+2])) & 0b111;
	extensionCount = count;

	uint16_t opcode = 0b1101'000'000'000'000 | (dReg << 9) | (size << 6) | ae;
	return finalize_instruction(opcode);
}
std::any visitor::visitAdd_from_dRegister(parser68000::Add_from_dRegisterContext* ctx)
{
	auto sz = ctx->children.size();
	uint16_t size = 1; 
	int arg = 1;
	if (sz == 5) 
	{
		size = std::any_cast<uint16_t>(visit(ctx->children[1]));
		arg++; // the optional size is included so there is one extra child
	}
	uint16_t dReg = std::any_cast<uint16_t>(visit(ctx->children[arg])) & 0b111;
	uint16_t ae = std::any_cast<uint16_t>(visit(ctx->children[arg+2]));

	uint16_t opcode = (0b1101'000'100'000'000 | (dReg << 9) | (size << 6) | ae);
	return finalize_instruction(opcode);
}

// ====================================================================================================
// Addressing modes
// ====================================================================================================
std::any visitor::visitDRegister(parser68000::DRegisterContext* ctx)
{
	extensionCount = 0;
    auto s = ctx->getText().substr(1);
	auto reg = std::stoi(s);
    return (uint16_t) (0b000'000 | reg);
}

std::any visitor::visitARegister(parser68000::ARegisterContext* ctx)
{
	extensionCount = 0;
	auto s = ctx->getText().substr(1);
    auto reg = std::stoi(s);
    return (uint16_t)(0b001'000 | reg);
}

std::any visitor::visitARegisterIndirect(parser68000::ARegisterIndirectContext* context)
{
	extensionCount = 0;
	auto s = context->children[1]->getText().substr(1);
	auto reg = std::stoi(s);
	return (uint16_t)(0b010'000 | reg);
}

std::any visitor::visitARegisterIndirectPostIncrement(parser68000::ARegisterIndirectPostIncrementContext* ctx)
{
	extensionCount = 0;
	auto s = ctx->children[1]->getText().substr(1);
	auto reg = std::stoi(s);
	return (uint16_t)(0b011'000 | reg);
}

std::any visitor::visitARegisterIndirectPreDecrement(parser68000::ARegisterIndirectPreDecrementContext* ctx)
{
	extensionCount = 0;
	auto s = ctx->children[2]->getText().substr(1);
	auto reg = std::stoi(s);
    return (uint16_t)(0b100'000 | reg);
}

std::any visitor::visitARegisterIndirectDisplacement(parser68000::ARegisterIndirectDisplacementContext* ctx)
{
    int32_t displacement = std::any_cast<int32_t>(visit(ctx->children[0]));
	if (displacement > 0x7fff || displacement < -0x8000)
	{
		// TODO : return error
	}
	extensionCount = 1;
	extensions[0] = (uint16_t)(displacement & 0xffff);

	auto s = ctx->children[2]->getText().substr(1);
	auto reg = std::stoi(s);
	return (uint16_t)(0b101'000 | reg);
}

std::any visitor::visitAdRegister(parser68000::AdRegisterContext* ctx)
{
	auto s = ctx->children[0]->getText();
	auto reg = std::stoi(s.substr(1));
	// The retuned value is formatted for the aRegisterIndirectIndex addressing mode
	if (s[0] == 'a' || s[0] == 'A')
	{
		return (uint16_t)(0b1'000'0 | (reg << 1)
			);
	}
	else
	{
		return (uint16_t)(0b0'000'0 | (reg << 1));
	}
}

std::any visitor::visitAdRegisterSize(parser68000::AdRegisterSizeContext* ctx)
{
	uint16_t adRegister = std::any_cast<uint16_t>(visit(ctx->children[0]));
	if (ctx->children.size() == 2)
	{
		int16_t size = std::any_cast<uint16_t>(visit(ctx->children[1]));
		if (size == 0)
		{
			// TODO : return error
		}
		else if (size == 2)
		{
			adRegister |= 0b0'000'1; // size is long
		}
	}
	return adRegister;
}


std::any visitor::visitARegisterIndirectIndex(parser68000::ARegisterIndirectIndexContext* ctx)
{
	int32_t displacement = std::any_cast<int32_t>(visit(ctx->children[0]));
	if (displacement > 0x7f || displacement < -0x80)
	{
		// TODO : return error
	}
	uint16_t displacement8 = (uint16_t)(displacement & 0xff);

	uint16_t index = std::any_cast<uint16_t>(visit(ctx->children[4]));
	extensionCount = 1;
	extensions[0] = (index << 11) | displacement8;

	auto s = ctx->children[2]->getText().substr(1);
	auto reg = std::stoi(s);
	return (uint16_t)(0b110'000 | reg);
}

std::any visitor::visitAbsoluteShort(parser68000::AbsoluteShortContext* ctx)
{
	int32_t displacement = std::any_cast<int32_t>(visit(ctx->children[0]));
	if (displacement > 0x7fff || displacement < -0x8000)
	{
		// TODO : return error
	}
	uint16_t displacement16 = (uint16_t)(displacement & 0xffff);
	extensionCount = 1;
	extensions[0] = displacement16;

	return (uint16_t)(0b111'000);

}
std::any visitor::visitAbsoluteLong(parser68000::AbsoluteLongContext* ctx)
{
	int32_t displacement = std::any_cast<int32_t>(visit(ctx->children[0]));
	extensionCount = 2;
	uint16_t displacementHigh = (uint16_t)(displacement >> 16);
	uint16_t displacementLow = (uint16_t)(displacement & 0xffff);
	extensions[0] = displacementHigh;
	extensions[1] = displacementLow;

	return (uint16_t)(0b111'001);
}
std::any visitor::visitPcIndirectDisplacement(parser68000::PcIndirectDisplacementContext* ctx)
{
	return (uint16_t)(0b111'010);
}
std::any visitor::visitPcIndirectIndex(parser68000::PcIndirectIndexContext* ctx)
{
	return (uint16_t)(0b111'011);
}
std::any visitor::visitImmediateData(parser68000::ImmediateDataContext* ctx)
{
	int32_t displacement = std::any_cast<int32_t>(visit(ctx->children[1]));
	if (size == 0)
	{
		if (displacement > 0x7f || displacement < -0x80)
		{
			// TODO : return error
		}
		uint16_t displacement8 = (uint16_t)(displacement & 0xff);
		extensionCount = 1;
		extensions[0] = displacement8;
	}
	else if (size == 1)
	{
		if (displacement > 0x7fff || displacement < -0x8000)
		{
			// TODO : return error
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


