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

    virtual std::any visitAndi2ccr(parser68000::Andi2ccrContext* ctx) override;
	virtual std::any visitAndi2sr(parser68000::Andi2srContext* ctx) override;

    virtual std::any visitAslAsr_dRegister(parser68000::AslAsr_dRegisterContext* ctx) override;
    virtual std::any visitAslAsr_immediateData(parser68000::AslAsr_immediateDataContext* ctx) override;
    virtual std::any visitAslAsr_addressingMode(parser68000::AslAsr_addressingModeContext* ctx) override;

    std::any visitNop(parser68000::NopContext* ctx) override;

    virtual std::any visitImmediate(parser68000::ImmediateContext* ctx) override;


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

    virtual std::any visitShiftInstruction(parser68000::ShiftInstructionContext* ctx) override {
        return ctx->value;
	}

    // Utilities
    uint16_t finalize_instruction(uint16_t opcode);
    void addError(const std::string& message, tree::ParseTree* ctx);
    void addPass0Error(const std::string& message, tree::ParseTree* ctx);
    bool isValidAddressingMode(unsigned short ea, unsigned short acceptable);
};
