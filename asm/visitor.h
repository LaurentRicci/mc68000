#pragma once
#include "parser68000BaseVisitor.h"
#include <tree/ParseTree.h>
#include "errors.h"

using namespace antlr4;

class visitor : public parser68000BaseVisitor
{
public: 
    visitor(std::vector<uint16_t>& code, std::map<std::string, uint32_t>& labels, std::vector<uint32_t>& labelsLocation, mc68000::errors& errorList) 
        : code(code), labels(labels), errorList(errorList) 
    {
    }
	~visitor() override = default;
    std::any generateCode(tree::ParseTree* tree);

private:
    std::vector<uint16_t>& code;                       // the assembly code as a results of the parsing
    
    //uint16_t extensionCount = 0;                       // the number of extra words for the addressing mode
    //uint16_t extensions[2] = { 0,0 };                  // the extra words for the addressing mode
	std::vector<uint16_t> extensionsList;         // the list of extra words for the addressing mode

    std::map<std::string, uint32_t>& labels;           // the labels found in the code label -> index
	int pass = 0;                                      // the current pass (0 = first pass, 1 = second pass)
    uint32_t currentAddress = 0;                       // the current address in the code

    uint16_t size = 1;                                 // the size of the current instruction

	mc68000::errors& errorList;                                 // the list of errors found during the parsing

private:
    std::any visitProg(parser68000::ProgContext* ctx) override;
    std::any visitLine_instructionSection(parser68000::Line_instructionSectionContext* ctx) override;
    std::any visitLabelSection(parser68000::LabelSectionContext* ctx) override;

	// Instructions
    virtual std::any visitAbcd_dRegister(parser68000::Abcd_dRegisterContext* ctx) override;
    virtual std::any visitAbcd_indirect(parser68000::Abcd_indirectContext* ctx) override;

    virtual std::any visitAdd_to_dRegister(parser68000::Add_to_dRegisterContext* ctx) override;
    virtual std::any visitAdd_from_dRegister(parser68000::Add_from_dRegisterContext* ctx) override;

    virtual std::any visitAdda(parser68000::AddaContext* ctx) override;
    virtual std::any visitAddq(parser68000::AddqContext* ctx) override;

    virtual std::any visitAddx_dRegister(parser68000::Addx_dRegisterContext* ctx) override;
    virtual std::any visitAddx_indirect(parser68000::Addx_indirectContext* ctx) override;
    
    virtual std::any visitAnd_to_dRegister(parser68000::And_to_dRegisterContext* ctx) override;
    virtual std::any visitAnd_from_dRegister(parser68000::And_from_dRegisterContext* ctx) override;

    virtual std::any visitToCCR(parser68000::ToCCRContext* ctx) override;
	virtual std::any visitToSR(parser68000::ToSRContext* ctx) override;

    virtual std::any visitAslAsr_dRegister(parser68000::AslAsr_dRegisterContext* ctx) override;
    virtual std::any visitAslAsr_immediateData(parser68000::AslAsr_immediateDataContext* ctx) override;
    virtual std::any visitAslAsr_addressingMode(parser68000::AslAsr_addressingModeContext* ctx) override;

	virtual std::any visitBcc(parser68000::BccContext* ctx) override;

	virtual std::any visitBit_dRegister(parser68000::Bit_dRegisterContext* ctx) override;
	virtual std::any visitBit_immediateData(parser68000::Bit_immediateDataContext* ctx) override;

	virtual std::any visitChk(parser68000::ChkContext* ctx) override;
    virtual std::any visitClr(parser68000::ClrContext* ctx) override;
	virtual std::any visitCmp(parser68000::CmpContext* ctx) override;
    virtual std::any visitCmpa(parser68000::CmpaContext* ctx) override;
    virtual std::any visitCmpm(parser68000::CmpmContext* ctx) override;
    virtual std::any visitDbcc(parser68000::DbccContext* ctx) override;
    virtual std::any visitDivs(parser68000::DivsContext* ctx) override;
    virtual std::any visitDivu(parser68000::DivuContext* ctx) override;
    virtual std::any visitEor(parser68000::EorContext* ctx) override;
    virtual std::any visitExg(parser68000::ExgContext* ctx) override;
	virtual std::any visitExt(parser68000::ExtContext* ctx) override;
	virtual std::any visitIllegal(parser68000::IllegalContext* ctx) override;
    virtual std::any visitImmediate(parser68000::ImmediateContext* ctx) override;
    virtual std::any visitJmp(parser68000::JmpContext* ctx) override;
    virtual std::any visitJsr(parser68000::JsrContext* ctx) override;
    virtual std::any visitLea(parser68000::LeaContext* ctx) override;
	virtual std::any visitLink(parser68000::LinkContext* ctx) override;

    virtual std::any visitLslLsr_dRegister(parser68000::LslLsr_dRegisterContext* ctx) override;
    virtual std::any visitLslLsr_immediateData(parser68000::LslLsr_immediateDataContext* ctx) override;
    virtual std::any visitLslLsr_addressingMode(parser68000::LslLsr_addressingModeContext* ctx) override;

	virtual std::any visitMove(parser68000::MoveContext* ctx) override;
    virtual std::any visitMovea(parser68000::MoveaContext* ctx) override;
    virtual std::any visitMoveusp(parser68000::MoveuspContext* ctx) override;
    virtual std::any visitMove2usp(parser68000::Move2uspContext* ctx) override;
    virtual std::any visitMovesr(parser68000::MovesrContext* ctx) override;
    virtual std::any visitMove2sr(parser68000::Move2srContext* ctx) override;
    virtual std::any visitMovem_toMemory(parser68000::Movem_toMemoryContext* ctx) override;
    virtual std::any visitMovem_fromMemory(parser68000::Movem_fromMemoryContext* ctx) override;
    virtual std::any visitMovep_toMemory(parser68000::Movep_toMemoryContext* ctx) override;
    virtual std::any visitMovep_fromMemory(parser68000::Movep_fromMemoryContext* ctx) override;
	virtual std::any visitMoveq(parser68000::MoveqContext* ctx) override;

    virtual std::any visitMuls(parser68000::MulsContext* ctx) override;
    virtual std::any visitMulu(parser68000::MuluContext* ctx) override;
	virtual std::any visitNbcd(parser68000::NbcdContext* ctx) override;
    virtual std::any visitNeg(parser68000::NegContext* ctx) override;
    virtual std::any visitNegx(parser68000::NegxContext* ctx) override;
    virtual std::any visitNop(parser68000::NopContext* ctx) override;
    virtual std::any visitNot(parser68000::NotContext* ctx) override;

    // Register list
    virtual std::any visitRegisterList(parser68000::RegisterListContext* ctx) override;
    virtual std::any visitRegisterListRegister(parser68000::RegisterListRegisterContext* ctx) override;
    virtual std::any visitRegisterListRange(parser68000::RegisterListRangeContext* ctx) override;
    virtual std::any visitRegisterRange(parser68000::RegisterRangeContext* ctx) override;

	// Addressing modes
    std::any visitDRegister(parser68000::DRegisterContext* context) override;
	std::any visitARegister(parser68000::ARegisterContext* context) override;
	std::any visitARegisterIndirect(parser68000::ARegisterIndirectContext* context) override;
	std::any visitARegisterIndirectPostIncrement(parser68000::ARegisterIndirectPostIncrementContext* ctx) override;
    std::any visitARegisterIndirectPreDecrement(parser68000::ARegisterIndirectPreDecrementContext* ctx) override;

    std::any visitARegisterIndirectDisplacement(tree::ParseTree* pDisplacement, tree::ParseTree* pRregistr);
    std::any visitARegisterIndirectDisplacementOld(parser68000::ARegisterIndirectDisplacementOldContext* ctx) override;
    std::any visitARegisterIndirectDisplacementNew(parser68000::ARegisterIndirectDisplacementNewContext* ctx) override;

    std::any visitARegisterIndirectDisplacement(tree::ParseTree* pDisplacement, tree::ParseTree* pRregistr, tree::ParseTree* pIndex);
    std::any visitARegisterIndirectIndexOld(parser68000::ARegisterIndirectIndexOldContext* ctx) override;
    std::any visitARegisterIndirectIndexNew(parser68000::ARegisterIndirectIndexNewContext* ctx) override;
    std::any visitAbsolute(parser68000::AbsoluteContext* ctx) override; 
    std::any visitAbsoluteShort(parser68000::AbsoluteShortContext* ctx) override;
	std::any visitAbsoluteLong(parser68000::AbsoluteLongContext* ctx) override;
    std::any visitPcIndirectDisplacement(parser68000::PcIndirectDisplacementContext* ctx) override;
    std::any visitPcIndirectIndex(parser68000::PcIndirectIndexContext* ctx) override;
    std::any visitImmediateData(parser68000::ImmediateDataContext* ctx) override;
    std::any visitAdRegister(parser68000::AdRegisterContext* ctx) override;
    std::any visitAdRegisterSize(parser68000::AdRegisterSizeContext* ctx) override;

    virtual std::any visitSize(parser68000::SizeContext* ctx) override {
        size = std::any_cast<uint16_t>(ctx->value); //TODO: why this line?
        return ctx->value;
    }

    virtual std::any visitNumber(parser68000::NumberContext* ctx) override {
        return ctx->value;
    }

    virtual std::any visitAddress(parser68000::AddressContext* ctx) override {  
       return ctx->value;  
    }

    virtual std::any visitImmediateInstruction(parser68000::ImmediateInstructionContext* ctx) override {
        return ctx->value;
    }

    virtual std::any visitToCCRorSRInstruction(parser68000::ToCCRorSRInstructionContext* ctx) override {
        return ctx->value;
    }

    virtual std::any visitShiftInstruction(parser68000::ShiftInstructionContext* ctx) override {
        return ctx->value;
	}

    virtual std::any visitBccInstruction(parser68000::BccInstructionContext* ctx) override {
        return ctx->value;
    }

    virtual std::any visitBitInstruction(parser68000::BitInstructionContext* ctx) override {
        return ctx->value;
    }

    virtual std::any visitDbccInstruction(parser68000::DbccInstructionContext* ctx) override {
        return ctx->value;
    }

    virtual std::any visitLogicalShiftInstruction(parser68000::LogicalShiftInstructionContext* ctx) override {
        return ctx->value;
    }
    // Utilities
    uint16_t finalize_instruction(uint16_t opcode);
    void addError(const std::string& message, tree::ParseTree* ctx);
    void addPass0Error(const std::string& message, tree::ParseTree* ctx);
    bool isValidAddressingMode(unsigned short ea, unsigned short acceptable);
    std::any visitDiv(tree::ParseTree* ctx, bool isSigned);
    std::any visitMul(tree::ParseTree* ctx, bool isSigned);
    std::any visitAbsoluteSize(tree::ParseTree* ctx, int size);
    std::any visitShiftRegister(tree::ParseTree* ctx, uint16_t code);
    std::any visitShiftImmediate(tree::ParseTree* ctx, uint16_t code);
	std::any visitShiftAddressingMode(tree::ParseTree* ctx, uint16_t code);
};
