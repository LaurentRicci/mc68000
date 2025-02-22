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
    uint16_t dy = std::any_cast<uint16_t>(visitDRegister(static_cast<parser68000::DRegisterContext*>(ctx->children[1])));
    uint16_t dx = std::any_cast<uint16_t>(visitDRegister(static_cast<parser68000::DRegisterContext*>(ctx->children[3])));

    uint16_t opcode = 0b1100'000'10000'0'000 | (dx << 9) | dy;
    return opcode;
}

std::any visitor::visitAbcd_indirect(parser68000::Abcd_indirectContext* ctx) 
{
    uint16_t ay = std::any_cast<uint16_t>(visitARegisterIndirectPreDecrement(static_cast<parser68000::ARegisterIndirectPreDecrementContext*>(ctx->children[1])));
    uint16_t ax = std::any_cast<uint16_t>(visitARegisterIndirectPreDecrement(static_cast<parser68000::ARegisterIndirectPreDecrementContext*>(ctx->children[3])));

    uint16_t opcode = 0b1100'000'10000'1'000 | (ax << 9) | ay;
    return opcode;
}

std::any visitor::visitDRegister(parser68000::DRegisterContext* context)
{
    auto s = context->getText().substr(1);
    return (uint16_t)std::stoi(s);
}

std::any visitor::visitARegisterIndirectPreDecrement(parser68000::ARegisterIndirectPreDecrementContext* ctx)
{
    auto s = ctx->children[2]->getText().substr(1);
    return (uint16_t)std::stoi(s);
}
