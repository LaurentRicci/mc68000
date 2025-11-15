#pragma once
#include "parser68000BaseVisitor.h"
#include <tree/ParseTree.h>
#include "errors.h"

using namespace antlr4;
using namespace std;

class visitor : public parser68000BaseVisitor
{
public: 
    visitor(vector<uint16_t>& code, map<string, uint32_t>& labels, vector<uint32_t>& labelsLocation, mc68000::errors& errorList) 
        : code(code), labels(labels), errorList(errorList) 
    {
    }
    ~visitor() override = default;
    any generateCode(tree::ParseTree* tree);

private:
    vector<uint16_t>& code;                       // the assembly code as a results of the parsing
    vector<uint16_t> extensionsList;              // the list of extra words for the addressing mode
    int pass = 0;                                      // the current pass (0 = first pass, 1 = second pass)
    uint32_t currentAddress = 0;                       // the current address in the code
    uint16_t size = 1;                                 // the size of the current instruction

    map<string, uint32_t>& labels;           // the labels found in the code label -> index
    map<string, any> symbols;                          // the symbol table (from the EQU directive)

	vector<uint8_t> dcBytes;                      // the data in the dc section
	bool incompleteBinary = false;                     // true if the last instruction was a dc.b with an odd number of bytes
    parser68000::LabelSectionContext* labelCtx;        // the current label section

    mc68000::errors& errorList;                        // the list of errors found during the parsing

private:
    any visitProg(parser68000::ProgContext* ctx) override;
    any visitLine_instructionSection(parser68000::Line_instructionSectionContext* ctx) override;
    any visitLine_directiveSection(parser68000::Line_directiveSectionContext* ctx) override;
    any visitLabelSection(parser68000::LabelSectionContext* ctx) override;

    // Instructions
    virtual any visitAbcdSbcd_dRegister(parser68000::AbcdSbcd_dRegisterContext* ctx) override;
    virtual any visitAbcdSbcd_indirect(parser68000::AbcdSbcd_indirectContext* ctx) override;

    virtual any visitAdd_to_dRegister(parser68000::Add_to_dRegisterContext* ctx) override;
    virtual any visitAdd_from_dRegister(parser68000::Add_from_dRegisterContext* ctx) override;

    virtual any visitAdda(parser68000::AddaContext* ctx) override;
    virtual any visitAddq(parser68000::AddqContext* ctx) override;

    virtual any visitAddx_dRegister(parser68000::Addx_dRegisterContext* ctx) override;
    virtual any visitAddx_indirect(parser68000::Addx_indirectContext* ctx) override;
    
    virtual any visitAndOr_to_dRegister(parser68000::AndOr_to_dRegisterContext* ctx) override;
    virtual any visitAndOr_from_dRegister(parser68000::AndOr_from_dRegisterContext* ctx) override;

    virtual any visitToCCR(parser68000::ToCCRContext* ctx) override;
    virtual any visitToSR(parser68000::ToSRContext* ctx) override;

    virtual any visitAslAsr_dRegister(parser68000::AslAsr_dRegisterContext* ctx) override;
    virtual any visitAslAsr_immediateData(parser68000::AslAsr_immediateDataContext* ctx) override;
    virtual any visitAslAsr_addressingMode(parser68000::AslAsr_addressingModeContext* ctx) override;

    virtual any visitBcc(parser68000::BccContext* ctx) override;

    virtual any visitBit_dRegister(parser68000::Bit_dRegisterContext* ctx) override;
    virtual any visitBit_immediateData(parser68000::Bit_immediateDataContext* ctx) override;

    virtual any visitChk(parser68000::ChkContext* ctx) override;
    virtual any visitClr(parser68000::ClrContext* ctx) override;
    virtual any visitCmp(parser68000::CmpContext* ctx) override;
    virtual any visitCmpa(parser68000::CmpaContext* ctx) override;
    virtual any visitCmpm(parser68000::CmpmContext* ctx) override;
    virtual any visitDbcc(parser68000::DbccContext* ctx) override;
    virtual any visitDivs(parser68000::DivsContext* ctx) override;
    virtual any visitDivu(parser68000::DivuContext* ctx) override;
    virtual any visitEor(parser68000::EorContext* ctx) override;
    virtual any visitExg(parser68000::ExgContext* ctx) override;
    virtual any visitExt(parser68000::ExtContext* ctx) override;
    virtual any visitIllegal(parser68000::IllegalContext* ctx) override;
    virtual any visitImmediate(parser68000::ImmediateContext* ctx) override;
    virtual any visitJmp(parser68000::JmpContext* ctx) override;
    virtual any visitJsr(parser68000::JsrContext* ctx) override;
    virtual any visitLea(parser68000::LeaContext* ctx) override;
    virtual any visitLink(parser68000::LinkContext* ctx) override;

    virtual any visitLslLsr_dRegister(parser68000::LslLsr_dRegisterContext* ctx) override;
    virtual any visitLslLsr_immediateData(parser68000::LslLsr_immediateDataContext* ctx) override;
    virtual any visitLslLsr_addressingMode(parser68000::LslLsr_addressingModeContext* ctx) override;

    virtual any visitMove(parser68000::MoveContext* ctx) override;
    virtual any visitMovea(parser68000::MoveaContext* ctx) override;
    virtual any visitMoveusp(parser68000::MoveuspContext* ctx) override;
    virtual any visitMove2usp(parser68000::Move2uspContext* ctx) override;
    virtual any visitMovesr(parser68000::MovesrContext* ctx) override;
    virtual any visitMove2sr(parser68000::Move2srContext* ctx) override;
    virtual any visitMovem_toMemory(parser68000::Movem_toMemoryContext* ctx) override;
    virtual any visitMovem_fromMemory(parser68000::Movem_fromMemoryContext* ctx) override;
    virtual any visitMovep_toMemory(parser68000::Movep_toMemoryContext* ctx) override;
    virtual any visitMovep_fromMemory(parser68000::Movep_fromMemoryContext* ctx) override;
    virtual any visitMoveq(parser68000::MoveqContext* ctx) override;

    virtual any visitMuls(parser68000::MulsContext* ctx) override;
    virtual any visitMulu(parser68000::MuluContext* ctx) override;
    virtual any visitNbcd(parser68000::NbcdContext* ctx) override;
    virtual any visitNeg(parser68000::NegContext* ctx) override;
    virtual any visitNegx(parser68000::NegxContext* ctx) override;
    virtual any visitNop(parser68000::NopContext* ctx) override;
    virtual any visitNot(parser68000::NotContext* ctx) override;
    virtual any visitPea(parser68000::PeaContext* ctx) override;
    virtual any visitResetInstruction(parser68000::ResetInstructionContext* ctx) override;

    virtual any visitRolRor_dRegister(parser68000::RolRor_dRegisterContext* ctx) override;
    virtual any visitRolRor_immediateData(parser68000::RolRor_immediateDataContext* ctx) override;
    virtual any visitRolRor_addressingMode(parser68000::RolRor_addressingModeContext* ctx) override;
    virtual any visitRoxlRoxr_dRegister(parser68000::RoxlRoxr_dRegisterContext* ctx) override;
    virtual any visitRoxlRoxr_immediateData(parser68000::RoxlRoxr_immediateDataContext* ctx) override;
    virtual any visitRoxlRoxr_addressingMode(parser68000::RoxlRoxr_addressingModeContext* ctx) override;

    virtual any visitRte(parser68000::RteContext* ctx) override;
    virtual any visitRtr(parser68000::RtrContext* ctx) override;
    virtual any visitRts(parser68000::RtsContext* ctx) override;
	virtual any visitScc(parser68000::SccContext* ctx) override;
    virtual any visitStop(parser68000::StopContext* ctx) override;
    virtual any visitSwap(parser68000::SwapContext* ctx) override;
    virtual any visitTas(parser68000::TasContext* cts) override;
	virtual any visitTrap(parser68000::TrapContext* ctx) override;
	virtual any visitTrapv(parser68000::TrapvContext* ctx) override;
	virtual any visitTst(parser68000::TstContext* ctx) override;
	virtual any visitUnlk(parser68000::UnlkContext* ctx) override;
    // ====================================================================================================
    // directives
    // ====================================================================================================
    virtual any visitDc(parser68000::DcContext* ctx) override;
    virtual any visitEqu(parser68000::EquContext* ctx) override;
    virtual any visitDs(parser68000::DsContext* ctx) override;
    virtual any visitDataList(parser68000::DataListContext* ctx) override;
	virtual any visitExpression(parser68000::ExpressionContext* ctx) override;
    virtual any visitAdditiveExpr(parser68000::AdditiveExprContext* ctx) override;
    virtual any visitDleNumber(parser68000::DleNumberContext* ctx) override;
    virtual any visitDleString(parser68000::DleStringContext* ctx) override;
    virtual any visitDleIdentifier(parser68000::DleIdentifierContext* ctx) override;
    virtual any visitDleStar(parser68000::DleStarContext* ctx) override;
    virtual any visitDleExpression(parser68000::DleExpressionContext* ctx) override;
	virtual any visitOrg(parser68000::OrgContext* ctx) override;
    // ====================================================================================================
    // Register list
    // ====================================================================================================
    virtual any visitRegisterList(parser68000::RegisterListContext* ctx) override;
    virtual any visitRegisterListRegister(parser68000::RegisterListRegisterContext* ctx) override;
    virtual any visitRegisterListRange(parser68000::RegisterListRangeContext* ctx) override;
    virtual any visitRegisterRange(parser68000::RegisterRangeContext* ctx) override;

    // ====================================================================================================
    // Addressing modes
    // ====================================================================================================
    any visitDRegister(parser68000::DRegisterContext* context) override;
    any visitARegister(parser68000::ARegisterContext* context) override;
    any visitARegisterIndirect(parser68000::ARegisterIndirectContext* context) override;
    any visitARegisterIndirectPostIncrement(parser68000::ARegisterIndirectPostIncrementContext* ctx) override;
    any visitARegisterIndirectPreDecrement(parser68000::ARegisterIndirectPreDecrementContext* ctx) override;

    any visitARegisterIndirectDisplacement(tree::ParseTree* pDisplacement, tree::ParseTree* pRregistr);
    any visitARegisterIndirectDisplacementOld(parser68000::ARegisterIndirectDisplacementOldContext* ctx) override;
    any visitARegisterIndirectDisplacementNew(parser68000::ARegisterIndirectDisplacementNewContext* ctx) override;

    any visitARegisterIndirectDisplacement(tree::ParseTree* pDisplacement, tree::ParseTree* pRregistr, tree::ParseTree* pIndex);
    any visitARegisterIndirectIndexOld(parser68000::ARegisterIndirectIndexOldContext* ctx) override;
    any visitARegisterIndirectIndexNew(parser68000::ARegisterIndirectIndexNewContext* ctx) override;
    any visitAbsolute(parser68000::AbsoluteContext* ctx) override; 
    any visitAbsoluteShort(parser68000::AbsoluteShortContext* ctx) override;
    any visitAbsoluteLong(parser68000::AbsoluteLongContext* ctx) override;
    any visitPcIndirectDisplacement(parser68000::PcIndirectDisplacementContext* ctx) override;
    any visitPcIndirectIndex(parser68000::PcIndirectIndexContext* ctx) override;
    any visitImmediateData(parser68000::ImmediateDataContext* ctx) override;
    any visitAdRegister(parser68000::AdRegisterContext* ctx) override;
    any visitAdRegisterSize(parser68000::AdRegisterSizeContext* ctx) override;

    // ====================================================================================================
    // enumeration support
    // ====================================================================================================
    virtual any visitSize(parser68000::SizeContext* ctx) override {
        return ctx->value;
    }

    virtual any visitNumber(parser68000::NumberContext* ctx) override {
        return ctx->value;
    }

    virtual any visitAddress(parser68000::AddressContext* ctx) override {  
       return ctx->value;  
    }

    virtual any visitImmediateInstruction(parser68000::ImmediateInstructionContext* ctx) override {
        return ctx->value;
    }

    virtual any visitToCCRorSRInstruction(parser68000::ToCCRorSRInstructionContext* ctx) override {
        return ctx->value;
    }

    virtual any visitShiftInstruction(parser68000::ShiftInstructionContext* ctx) override {
        return ctx->value;
    }

    virtual any visitBccInstruction(parser68000::BccInstructionContext* ctx) override {
        return ctx->value;
    }

    virtual any visitBitInstruction(parser68000::BitInstructionContext* ctx) override {
        return ctx->value;
    }

    virtual any visitDbccInstruction(parser68000::DbccInstructionContext* ctx) override {
        return ctx->value;
    }

    virtual any visitLogicalShiftInstruction(parser68000::LogicalShiftInstructionContext* ctx) override {
        return ctx->value;
    }

    virtual any visitAndOrInstruction(parser68000::AndOrInstructionContext* ctx) override {
        return ctx->value;
    }

    virtual any visitRotateInstruction(parser68000::RotateInstructionContext* ctx) override {
        return ctx->value;
    }

    virtual any visitRotateXInstruction(parser68000::RotateXInstructionContext* ctx) override {
        return ctx->value;
    }

    virtual any visitAbcdSbcdInstruction(parser68000::AbcdSbcdInstructionContext* ctx) override {
        return ctx->value;
    }

    virtual any visitSccInstruction(parser68000::SccInstructionContext* ctx) override {
        return ctx->value;
    }

    virtual any visitAddSubInstruction(parser68000::AddSubInstructionContext* ctx) override {
        return ctx->value;
    }

    virtual any visitAddaSubaInstruction(parser68000::AddaSubaInstructionContext* ctx) override {
        return ctx->value;
    }

    virtual any visitAddqSubqInstruction(parser68000::AddqSubqInstructionContext* ctx) override {
        return ctx->value;
    }

    virtual any visitAddxSubxInstruction(parser68000::AddxSubxInstructionContext* ctx) override {
        return ctx->value;
    }
    virtual any visitBlockAddress(parser68000::BlockAddressContext* ctx) override {
        return ctx->value;
    }

    // ====================================================================================================
    // Utilities
    // ====================================================================================================
    inline uint16_t aRegister(const string& s)
    {
        if (s == "p" || s == "P")
        {
            // special case for A7 which is also the stack pointer
            return (uint16_t)7;
        }
        return (uint16_t)stoi(s);
    }

    uint16_t finalize_instruction(uint16_t opcode);
    void addError(const string& message, tree::ParseTree* ctx);
    void addPass0Error(const string& message, tree::ParseTree* ctx);
    bool isValidAddressingMode(unsigned short ea, unsigned short acceptable);
    any visitDiv(tree::ParseTree* ctx, bool isSigned);
    any visitMul(tree::ParseTree* ctx, bool isSigned);
    any visitAbsoluteSize(parser68000::AddressContext* ctx, int size);
    any visitShiftRegister(tree::ParseTree* ctx, uint16_t code);
    any visitShiftImmediate(tree::ParseTree* ctx, uint16_t code);
    any visitShiftAddressingMode(tree::ParseTree* ctx, uint16_t code);
	uint32_t getAddressValue(parser68000::AddressContext* ctx);
    uint32_t getAddressValue(parser68000::BlockAddressContext* ctx);
    int32_t  getIntegerValue(parser68000::AddressContext* ctx);
    int32_t  getDisplacementValue(tree::ParseTree* pDisplacement);

};
