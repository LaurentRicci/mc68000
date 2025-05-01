#pragma once
#include "parser68000BaseVisitor.h"
#include <tree/ParseTree.h>

using namespace antlr4;

class visitor : public parser68000BaseVisitor
{
public: 
    visitor(std::vector<uint16_t>& code) : code(code), extensionCount(0) {}
	~visitor() override = default;

private:
	uint16_t extensionCount;  // the number of extra words for the addressing mode
	uint16_t extensions[2];   // the extra words for the addressing mode
	std::vector<uint16_t>& code; // the results of the parsing

private:
    std::any visitProg(parser68000::ProgContext* ctx);
    std::any visitLine_instructionSection(parser68000::Line_instructionSectionContext* ctx);
    std::any visitInstructionSection_abcd(parser68000::InstructionSection_abcdContext* ctx);

    std::any visitAbcd_dRegister(parser68000::Abcd_dRegisterContext* ctx);
    std::any visitAbcd_indirect(parser68000::Abcd_indirectContext* ctx);

    uint16_t finalize_instruction(uint16_t opcode);

    std::any visitAdd_to_dRegister(parser68000::Add_to_dRegisterContext* ctx) override;
    std::any visitAdd_from_dRegister(parser68000::Add_from_dRegisterContext* ctx) override;

	// Addressing modes
    std::any visitDRegister(parser68000::DRegisterContext* context) override;
	std::any visitARegister(parser68000::ARegisterContext* context) override;
	std::any visitARegisterIndirect(parser68000::ARegisterIndirectContext* context) override;
	std::any visitARegisterIndirectPostIncrement(parser68000::ARegisterIndirectPostIncrementContext* ctx) override;
    std::any visitARegisterIndirectPreDecrement(parser68000::ARegisterIndirectPreDecrementContext* ctx) override;
	std::any visitARegisterIndirectDisplacement(parser68000::ARegisterIndirectDisplacementContext* ctx) override;
	std::any visitARegisterIndirectIndex(parser68000::ARegisterIndirectIndexContext* ctx) override;
	std::any visitAbsoluteShort(parser68000::AbsoluteShortContext* ctx) override;
	std::any visitAbsoluteLong(parser68000::AbsoluteLongContext* ctx) override;
    std::any visitPcIndirectDisplacement(parser68000::PcIndirectDisplacementContext* ctx) override;
    std::any visitPcIndirectIndex(parser68000::PcIndirectIndexContext* ctx) override;
    std::any visitImmediateData(parser68000::ImmediateDataContext* ctx) override;

    virtual std::any visitSize(parser68000::SizeContext* ctx) override {
        return ctx->value;
    }

    virtual std::any visitNumber(parser68000::NumberContext* ctx) override {
        return ctx->value;
    }

};
