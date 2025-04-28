#pragma once
#include "parser68000BaseVisitor.h"
#include <tree/ParseTree.h>

using namespace antlr4;

class visitor : public parser68000BaseVisitor
{
    std::any visitProg(parser68000::ProgContext* ctx);
    std::any visitLine_instructionSection(parser68000::Line_instructionSectionContext* ctx);
    std::any visitInstructionSection_abcd(parser68000::InstructionSection_abcdContext* ctx);

    std::any visitAbcd_dRegister(parser68000::Abcd_dRegisterContext* ctx);
    std::any visitAbcd_indirect(parser68000::Abcd_indirectContext* ctx);

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


};
