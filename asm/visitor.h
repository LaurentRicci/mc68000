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

    std::any visitDRegister(parser68000::DRegisterContext* context);
    std::any visitARegisterIndirectPreDecrement(parser68000::ARegisterIndirectPreDecrementContext* ctx);

};
