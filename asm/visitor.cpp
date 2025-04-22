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
    tree::ParseTree* node = ctx;
    size_t n = node->children.size();
    uint16_t opcode = std::any_cast<uint16_t>(visitInstructionSection_abcd(static_cast<parser68000::InstructionSection_abcdContext*>(node->children[1])));

    std::any result = node->children[1]->accept(this);
    return result;
//	return visitChildren(ctx);
}

std::any visitor::visitInstructionSection_abcd(parser68000::InstructionSection_abcdContext* ctx)
{
	std::any result = ctx->children[0]->accept(this);
    return result;

	//uint16_t opcode = std::any_cast<uint16_t>(visitAbcd_dRegister(static_cast<parser68000::Abcd_dRegisterContext*>(ctx->abcd())));
	//return opcode;
}


std::any visitor::visitAbcd_dRegister(parser68000::Abcd_dRegisterContext* ctx)
{
    uint16_t dy = std::any_cast<uint16_t>(visitDRegister(static_cast<parser68000::DRegisterContext*>(ctx->children[1]))) & 0b111;
    uint16_t dx = std::any_cast<uint16_t>(visitDRegister(static_cast<parser68000::DRegisterContext*>(ctx->children[3]))) & 0b111;

    uint16_t opcode = 0b1100'000'10000'0'000 | (dx << 9) | dy;
    return opcode;
}

std::any visitor::visitAbcd_indirect(parser68000::Abcd_indirectContext* ctx) 
{
    uint16_t ay = std::any_cast<uint16_t>(visitARegisterIndirectPreDecrement(static_cast<parser68000::ARegisterIndirectPreDecrementContext*>(ctx->children[1]))) & 0b111;
    uint16_t ax = std::any_cast<uint16_t>(visitARegisterIndirectPreDecrement(static_cast<parser68000::ARegisterIndirectPreDecrementContext*>(ctx->children[3]))) & 0b111;

    uint16_t opcode = 0b1100'000'10000'1'000 | (ax << 9) | ay;
    return opcode;
}
// ====================================================================================================
// Addressing modes
// ====================================================================================================
std::any visitor::visitDRegister(parser68000::DRegisterContext* ctx)
{
    auto s = ctx->getText().substr(1);
	auto reg = std::stoi(s);
    return (uint16_t) (0b000'000 | reg);
}

std::any visitor::visitARegister(parser68000::ARegisterContext* ctx)
{
    auto s = ctx->getText().substr(1);
    auto reg = std::stoi(s);
    return (uint16_t)(0b001'000 | reg);
}

std::any visitor::visitARegisterIndirect(parser68000::ARegisterIndirectContext* context)
{
	auto s = context->children[1]->getText().substr(1);
	auto reg = std::stoi(s);
	return (uint16_t)(0b010'000 | reg);
}

std::any visitor::visitARegisterIndirectPostIncrement(parser68000::ARegisterIndirectPostIncrementContext* ctx)
{
	auto s = ctx->children[2]->getText().substr(1);
	auto reg = std::stoi(s);
	return (uint16_t)(0b011'000 | reg);
}

std::any visitor::visitARegisterIndirectPreDecrement(parser68000::ARegisterIndirectPreDecrementContext* ctx)
{
    auto s = ctx->children[2]->getText().substr(1);
	auto reg = std::stoi(s);
    return (uint16_t)(0b100'000 | reg);
}

std::any visitor::visitARegisterIndirectDisplacement(parser68000::ARegisterIndirectDisplacementContext* ctx)
{
	auto s = ctx->children[2]->getText().substr(1);
	auto reg = std::stoi(s);
	return (uint16_t)(0b101'000 | reg);
}
std::any visitor::visitARegisterIndirectIndex(parser68000::ARegisterIndirectIndexContext* ctx)
{
	auto s = ctx->children[2]->getText().substr(1);
	auto reg = std::stoi(s);
	return (uint16_t)(0b110'000 | reg);
}
std::any visitor::visitAbsoluteShort(parser68000::AbsoluteShortContext* ctx)
{
	return (uint16_t)(0b111'000);
}
std::any visitor::visitAbsoluteLong(parser68000::AbsoluteLongContext* ctx)
{
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
	return (uint16_t)(0b111'100);
}



