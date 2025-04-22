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

	// Addressing modes
    std::any visitDRegister(parser68000::DRegisterContext* context);
	std::any visitARegister(parser68000::ARegisterContext* context);
	std::any visitARegisterIndirect(parser68000::ARegisterIndirectContext* context);
	std::any visitARegisterIndirectPostIncrement(parser68000::ARegisterIndirectPostIncrementContext* ctx);
    std::any visitARegisterIndirectPreDecrement(parser68000::ARegisterIndirectPreDecrementContext* ctx);
	std::any visitARegisterIndirectDisplacement(parser68000::ARegisterIndirectDisplacementContext* ctx);
	std::any visitARegisterIndirectIndex(parser68000::ARegisterIndirectIndexContext* ctx);
	std::any visitAbsoluteShort(parser68000::AbsoluteShortContext* ctx);
	std::any visitAbsoluteLong(parser68000::AbsoluteLongContext* ctx);
    std::any visitPcIndirectDisplacement(parser68000::PcIndirectDisplacementContext* ctx);
    std::any visitPcIndirectIndex(parser68000::PcIndirectIndexContext* ctx);
    std::any visitImmediateData(parser68000::ImmediateDataContext* ctx);

};
