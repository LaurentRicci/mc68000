#include "visitor.h"

using namespace std;
using namespace mc68000;

uint16_t visitor::finalize_instruction(uint16_t opcode)
{
	code.push_back(opcode);
	currentAddress += 2;
	if (!extensionsList.empty())
	{
		code.insert(code.end(), extensionsList.begin(), extensionsList.end());
	}
	currentAddress += 2 * (uint32_t) extensionsList.size();
	extensionsList.clear();
	return opcode;
}

any visitor::generateCode(tree::ParseTree* tree)
{
	pass = 0;
	tree->accept(this);
	code.clear();
	extensionsList.clear();
	currentAddress = 0;
	incompleteBinary = false;

	pass = 1;
	auto result = tree->accept(this);

	return result;
}

any visitor::visitProg(parser68000::ProgContext* ctx)
{
    size_t n = ctx->children.size();
	vector<any> results;
	for (size_t i = 0; i < n; i++)
	{
		auto result = ctx->children[i]->accept(this);
		results.push_back(result);
	}

    return results;
}

any visitor::visitLine_instructionSection(parser68000::Line_instructionSectionContext* ctx)
{
	size = 1;
	if (incompleteBinary)
	{
		currentAddress += 1;
		incompleteBinary = false;
	}
	any result;
	for (auto child : ctx->children)
	{
		result = visit(child);
	}
	return result;
}

any visitor::visitLabelSection(parser68000::LabelSectionContext* ctx)
{
	if (pass == 0 && !ctx->value.empty())
	{
		if (symbols.find(ctx->value) != symbols.end())
		{
			addPass0Error("Label name conflicts with symbol: " + ctx->value, ctx);
		}
		else if (labels.find(ctx->value) == labels.end())
		{
			labels[ctx->value] = currentAddress;
		}
		else
		{
			addPass0Error("Duplicate label: " + ctx->value, ctx);
		}
	}
	return any();
}

/// <summary>
/// ABCD dRegister COMMA dRegister 
/// SBCD dRegister COMMA dRegister 
/// </summary>
any visitor::visitAbcdSbcd_dRegister(parser68000::AbcdSbcd_dRegisterContext* ctx)
{
	uint16_t code = any_cast<uint16_t>(visit(ctx->children[0]));
    uint16_t dy = any_cast<uint16_t>(visit(ctx->children[1])) & 0b111;
    uint16_t dx = any_cast<uint16_t>(visit(ctx->children[3])) & 0b111;

    uint16_t opcode = 0b0000'000'10000'0'000 | (code << 12)| (dx << 9) | dy;
    return finalize_instruction(opcode);
}

/// <summary>
/// ABCD aRegisterIndirectPreDecrement COMMA aRegisterIndirectPreDecrement
/// SBCD aRegisterIndirectPreDecrement COMMA aRegisterIndirectPreDecrement
/// </summary>
any visitor::visitAbcdSbcd_indirect(parser68000::AbcdSbcd_indirectContext* ctx) 
{
	uint16_t code = any_cast<uint16_t>(visit(ctx->children[0]));
	uint16_t ay = any_cast<uint16_t>(visit(ctx->children[1])) & 0b111;
    uint16_t ax = any_cast<uint16_t>(visit(ctx->children[3])) & 0b111;

    uint16_t opcode = 0b0000'000'10000'1'000 | (code << 12) | (ax << 9) | ay;
    return finalize_instruction(opcode);
}

/// <summary>
/// ADD size? addressingMode COMMA dRegister 
/// SUB size? addressingMode COMMA dRegister 
/// </summary>
any visitor::visitAdd_to_dRegister(parser68000::Add_to_dRegisterContext* ctx)
{
	uint16_t code = any_cast<uint16_t>(visit(ctx->children[0]));
	size = 1;
	int arg = 1;
	if (ctx->children.size() == 5)
	{
		size = any_cast<uint16_t>(visit(ctx->children[1]));
		arg++; // the optional size is included so there is one extra child
	}

	uint16_t effectiveAddress = any_cast<uint16_t>(visit(ctx->children[arg]));
	uint16_t dReg = any_cast<uint16_t>(visit(ctx->children[arg+2])) & 0b111;

	uint16_t opcode = 0b0000'000'000'000'000 | (code << 12) | (dReg << 9) | (size << 6) | effectiveAddress;
	return finalize_instruction(opcode);
}

/// <summary>
/// ADD size? dRegister COMMA addressingMode
/// SUB size? dRegister COMMA addressingMode
/// </summary>
any visitor::visitAdd_from_dRegister(parser68000::Add_from_dRegisterContext* ctx)
{
	uint16_t code = any_cast<uint16_t>(visit(ctx->children[0]));
	size = 1;
	int arg = 1;
	if (ctx->children.size() == 5)
	{
		size = any_cast<uint16_t>(visit(ctx->children[1]));
		arg++; // the optional size is included so there is one extra child
	}
	uint16_t dReg = any_cast<uint16_t>(visit(ctx->children[arg])) & 0b111;
	uint16_t effectiveAddress = any_cast<uint16_t>(visit(ctx->children[arg+2]));

	uint16_t opcode = (0b0000'000'100'000'000 | (code << 12) | (dReg << 9) | (size << 6) | effectiveAddress);
	return finalize_instruction(opcode);
}

/// <summary>
/// ADDA size? addressingMode COMMA aRegister
/// SUBA size? addressingMode COMMA aRegister
/// </summary>
/// <param name="ctx"></param>
/// <returns></returns>
any visitor::visitAdda(parser68000::AddaContext* ctx)
{
	uint16_t code = any_cast<uint16_t>(visit(ctx->children[0]));
	size = 1;
	uint16_t opmode = 0b011;
	int arg = 1;
	if (ctx->children.size() == 5) // if there is a size specified
	{
		size = any_cast<uint16_t>(visit(ctx->children[1]));
		if (size == 0)
		{
			addError("invalid size only .W or .L are supported", ctx->children[1]);
		}
		else if (size == 2)
		{
			opmode = 0b111;
		}

		arg++; // the optional size is included so there is one extra child
	}
	uint16_t effectiveAddress = any_cast<uint16_t>(visit(ctx->children[arg]));
	uint16_t aReg = any_cast<uint16_t>(visit(ctx->children[arg+2])) & 0b111;
	uint16_t opcode = 0b0000'000'011'000'000 | (code << 12) | (aReg << 9) | (opmode << 6) | effectiveAddress;
	return finalize_instruction(opcode);
}

/// <summary>
/// ADDQ size? HASH number COMMA addressingMode
/// SUBQ size? HASH number COMMA addressingMode
/// </summary>
any visitor::visitAddq(parser68000::AddqContext* ctx)
{
	uint16_t code = any_cast<uint16_t>(visit(ctx->children[0]));
	size = 1;
	int arg = 1;
	if (ctx->children.size() == 6) // if there is a size specified
	{
		size = any_cast<uint16_t>(visit(ctx->children[1]));
		arg++; // the optional size is included so there is one extra child
	}
	int32_t immediate_data = getIntegerValue(ctx->address());
	if (immediate_data <= 0 || immediate_data > 8)
	{
		addError("Immediate data for ADDQ/SUBQ must be between 0 and 8", ctx->children[arg+1]);
		immediate_data = 0; // reset to 0 to avoid further errors
	}
	else if (immediate_data == 8)
	{
		immediate_data = 0; // 8 is treated as 0 in the instruction encoding
	}

	uint16_t effectiveAddress = any_cast<uint16_t>(visit(ctx->children[arg + 3]));
	if (!isValidAddressingMode(effectiveAddress, 0b111111111000))
	{
		addError("Invalid addressing mode: ", ctx->children[arg + 3]);
	}
	else if (size == 0 && 0b001'000 == (effectiveAddress & 0b111'000))
	{
		addError("ADDQ/SUBQ to Address register should be word or long", ctx->children[arg + 3]);
	}
	uint16_t opcode = 0b0101'000'0'00'000'000 | (immediate_data << 9) | (code << 8) | (size << 6) | effectiveAddress;
	return finalize_instruction(opcode);
}

/// <summary>
/// ADDX size? dRegister COMMA dRegister
/// SUBX size? dRegister COMMA dRegister
/// </summary>
any visitor::visitAddx_dRegister(parser68000::Addx_dRegisterContext* ctx)
{
	uint16_t code = any_cast<uint16_t>(visit(ctx->children[0]));
	size = 1;
	int arg = 1;
	if (ctx->children.size() == 5) // if there is a size specified
	{
		size = any_cast<uint16_t>(visit(ctx->children[1]));
		arg++; // the optional size is included so there is one extra child
	}
	uint16_t dy = any_cast<uint16_t>(visit(ctx->children[arg])) & 0b111;
	uint16_t dx = any_cast<uint16_t>(visit(ctx->children[arg+2])) & 0b111;

	uint16_t opcode = 0b0000'000'1'00'00'0'000 | (code << 12) | (dx << 9) | (size << 6) | dy;
	return finalize_instruction(opcode);
}

/// <summary>
/// ADDX size? aRegisterIndirectPreDecrement COMMA aRegisterIndirectPreDecrement
/// </summary>
any visitor::visitAddx_indirect(parser68000::Addx_indirectContext* ctx)
{
	uint16_t code = any_cast<uint16_t>(visit(ctx->children[0]));
	size = 1;
	int arg = 1;
	if (ctx->children.size() == 5) // if there is a size specified
	{
		size = any_cast<uint16_t>(visit(ctx->children[1]));
		arg++; // the optional size is included so there is one extra child
	}
	uint16_t ay = any_cast<uint16_t>(visit(ctx->children[arg])) & 0b111;
	uint16_t ax = any_cast<uint16_t>(visit(ctx->children[arg+2])) & 0b111;

	uint16_t opcode = 0b0000'000'1'00'00'1'000 | (code << 12) | (ax << 9) | (size << 6) | ay;
	return finalize_instruction(opcode);
}

/// <summary>
/// AND size? addressingMode COMMA dRegister #and_to_dRegister
/// OR  size? addressingMode COMMA dRegister #and_to_dRegister
/// </summary>
any visitor::visitAndOr_to_dRegister(parser68000::AndOr_to_dRegisterContext* ctx)
{
	uint16_t instructionCode = any_cast<uint16_t>(visit(ctx->children[0]));
	uint16_t size = 1;
	int arg = 1;
	if (ctx->children.size() == 5)
	{
		size = any_cast<uint16_t>(visit(ctx->children[1]));
		arg++; // the optional size is included so there is one extra child
	}

	uint16_t effectiveAddress = any_cast<uint16_t>(visit(ctx->children[arg]));
	if (!isValidAddressingMode(effectiveAddress, 0b101111'111111))
	{
		addError("Invalid addressing mode: ", ctx->children[arg]);
	}
	uint16_t dReg = any_cast<uint16_t>(visit(ctx->children[arg + 2])) & 0b111;

	uint16_t opcode = 0b0000'000'000'000'000 | (instructionCode << 12) | (dReg << 9) | (size << 6) | effectiveAddress;
	return finalize_instruction(opcode);
}

/// <summary>
/// AND size? dRegister COMMA addressingMode #and_from_dRegister
/// OR  size? dRegister COMMA addressingMode #and_from_dRegister
/// </summary>
any visitor::visitAndOr_from_dRegister(parser68000::AndOr_from_dRegisterContext* ctx)
{
	uint16_t instructionCode = any_cast<uint16_t>(visit(ctx->children[0]));
	uint16_t size = 1;
	int arg = 1;
	if (ctx->children.size() == 5)
	{
		size = any_cast<uint16_t>(visit(ctx->children[1]));
		arg++; // the optional size is included so there is one extra child
	}
	uint16_t dReg = any_cast<uint16_t>(visit(ctx->children[arg])) & 0b111;
	uint16_t effectiveAddress = any_cast<uint16_t>(visit(ctx->children[arg + 2]));
	if (!isValidAddressingMode(effectiveAddress, 0b001111'111000))
	{
		addError("Invalid addressing mode: ", ctx->children[arg + 2]);
	}

	uint16_t opcode = 0b0000'000'100'000'000 | (instructionCode << 12) | (dReg << 9) | (size << 6) | effectiveAddress;
	return finalize_instruction(opcode);
}

/// <summary>
///  XXXI size? immediateData COMMA addressingMode
/// </summary>
any visitor::visitImmediate(parser68000::ImmediateContext* ctx)
{
	uint16_t specific_opcode = any_cast<uint16_t>(visit(ctx->children[0]));
	size = 1;
	int arg = 1;
	if (ctx->children.size() == 5) // if there is a size specified
	{
		size = any_cast<uint16_t>(visit(ctx->children[1]));
		arg++; // the optional size is included so there is one extra child
	}
	visit(ctx->children[arg]);

	uint16_t effectiveAddress = any_cast<uint16_t>(visit(ctx->children[arg + 2]));
	if (!isValidAddressingMode(effectiveAddress, 0b101111111000))
	{
		addError("Invalid addressing mode: ", ctx->children[arg + 2]);
	}
	uint16_t opcode = 0b0000'0000'00'000'000 | (specific_opcode << 8) | (size << 6) | effectiveAddress;
	return finalize_instruction(opcode);
}

/// <summary>
/// ANDI immediateData COMMA CCR
/// EORI immediateData COMMA CCR
/// ORI  immediateData COMMA CCR
/// </summary>
any visitor::visitToCCR(parser68000::ToCCRContext* ctx)
{
	size = 0;
	uint16_t operation = any_cast<uint16_t>(visit(ctx->children[0]));
	visit(ctx->children[1]);

	uint16_t opcode = 0b0000'0000'0011'1100 | (operation << 8);
	return finalize_instruction(opcode);
}

/// <summary>
/// ANDI immediateData COMMA SR
/// EORI immediateData COMMA SR
/// ORI  immediateData COMMA SR
/// </summary>
any visitor::visitToSR(parser68000::ToSRContext* ctx)
{
	size = 1;
	uint16_t operation = any_cast<uint16_t>(visit(ctx->children[0]));
	visit(ctx->children[1]);

	uint16_t opcode = 0b0000'0000'0111'1100 | (operation << 8);
	return finalize_instruction(opcode);
}

/// <summary>
/// ASL/ASR size? dRegister COMMA dRegister
/// </summary>
any visitor::visitAslAsr_dRegister(parser68000::AslAsr_dRegisterContext* ctx)
{
	return visitShiftRegister(ctx, 0b00);
}

/// <summary>
/// ASL/ASR size? HASH number COMMA dRegister
/// </summary>
any visitor::visitAslAsr_immediateData(parser68000::AslAsr_immediateDataContext* ctx)
{
	return visitShiftImmediate(ctx, 0b00);
}

/// <summary>
/// ASL/ASR size? addressingMode
/// </summary>
any visitor::visitAslAsr_addressingMode(parser68000::AslAsr_addressingModeContext* ctx)
{
	return visitShiftAddressingMode(ctx, 0b00);
}

/// <summary>
/// ASL/ASR/LSL/LSR size? dRegister COMMA dRegister
/// </summary>
/// <param name="ctx">Pointer to the parse tree context.</param>
/// <param name="code">Instruction code differentiator: ASL/ASR 0 LSL/LSR 1.</param>
/// <returns>The opcode.</returns>
any visitor::visitShiftRegister(tree::ParseTree* ctx, uint16_t code)
{
	uint16_t direction = any_cast<uint16_t>(visit(ctx->children[0]));

	size = 1;
	int arg = 1;
	if (ctx->children.size() == 5) // if there is a size specified
	{
		size = any_cast<uint16_t>(visit(ctx->children[1]));
		arg++; // the optional size is included so there is one extra child
	}
	uint16_t dx = any_cast<uint16_t>(visit(ctx->children[arg])) & 0b111;
	uint16_t dy = any_cast<uint16_t>(visit(ctx->children[arg + 2])) & 0b111;

	uint16_t opcode = 0b1110'000'0'00'1'00'000 | (dx << 9) | (direction << 8) | (size << 6) | (code << 3) | dy;
	return finalize_instruction(opcode);
}

/// <summary>
/// ASL/ASR/LSL/LSR size? HASH address COMMA dRegister
/// </summary>
/// <param name="ctx">Pointer to the parse tree context.</param>
/// <param name="code">Instruction code differentiator: ASL/ASR 0 LSL/LSR 1.</param>
/// <returns>The opcode.</returns>
any visitor::visitShiftImmediate(tree::ParseTree* ctx, uint16_t code)
{
	uint16_t direction = any_cast<uint16_t>(visit(ctx->children[0]));

	size = 1;
	int arg = 2;
	if (ctx->children.size() == 6) // if there is a size specified
	{
		size = any_cast<uint16_t>(visit(ctx->children[1]));
		arg++; // the optional size is included so there is one extra child
	}
	parser68000::AddressContext* addressCtx = dynamic_cast<parser68000::AddressContext*>(ctx->children[arg]);
	int32_t immediate_data = getIntegerValue(addressCtx);
	if (immediate_data <= 0 || immediate_data > 8)
	{
		addError("Immediate data must be between 0 and 8", ctx->children[arg]);
		immediate_data = 0; // reset to 0 to avoid further errors
	}
	else if (immediate_data == 8)
	{
		immediate_data = 0; // 8 is treated as 0 in the instruction encoding
	}
	uint16_t dy = any_cast<uint16_t>(visit(ctx->children[arg + 2])) & 0b111;

	uint16_t opcode = 0b1110'0'00'0'00'0'00'000 | (immediate_data << 9) | (direction << 8) | (size << 6) | (code << 3) | dy;
	return finalize_instruction(opcode);
}

/// <summary>
/// ASL/ASR/LSL/LSR size? addressingMode
/// </summary>
/// <param name="ctx">Pointer to the parse tree context.</param>
/// <param name="code">Instruction code differentiator: ASL/ASR 0 LSL/LSR 1.</param>
/// <returns></returns>
any visitor::visitShiftAddressingMode(tree::ParseTree* ctx, uint16_t code)
{
	uint16_t direction = any_cast<uint16_t>(visit(ctx->children[0]));

	size = 1;
	int arg = 1;
	if (ctx->children.size() == 3) // if there is a size specified
	{
		size = any_cast<uint16_t>(visit(ctx->children[1]));
		if (size != 1)
		{
			addError("Invalid size for ASL/ASR, only .W is supported", ctx->children[1]);
			size = 1;
		}
		arg++; // the optional size is included so there is one extra child
	}
	uint16_t effectiveAddress = any_cast<uint16_t>(visit(ctx->children[arg]));
	if (!isValidAddressingMode(effectiveAddress, 0b001111'111000))
	{
		addError("Invalid addressing mode: ", ctx->children[arg]);
	}
	uint16_t opcode = 0b1110'000'0'11'000'000 | (code << 9) | (direction << 8) | effectiveAddress;
	return finalize_instruction(opcode);
}

/// <summary>
/// BCC address
/// </summary>
any visitor::visitBcc(parser68000::BccContext* ctx)
{
	uint16_t condition = any_cast<uint16_t>(visit(ctx->children[0]));

	uint32_t target = getAddressValue(ctx->address());
	any address = visit(ctx->children[1]);

	int32_t offset = target - currentAddress - 2; // -2 because the next instruction will be at currentAddress + 2
	if (offset > 0x7fff || offset < -0x8000)
	{
		addError("Address doesn't fit on in one word: " + std::to_string(offset), ctx->children[1]);
		offset = 0x100; // to force 2 words for the instruction 
	}
	uint16_t opcode = 0;

	if (address.type() == typeid(string)) // label
	{
		// During the 1st pass we don't know the actual offset yet so we have to reserve two words for the instruction
		uint16_t offset16 = (uint16_t)(offset & 0xffff);
		opcode = 0b0110'0000'0000'0000 | (condition << 8);
		extensionsList.push_back(offset16); // Add the offset as an extension
	}
	else
	{
		if (offset >= -128 && offset <= 127) // offset fits in one byte
		{
			uint16_t offset8 = (uint16_t)(offset & 0xff);
			opcode = 0b0110'0000'0000'0000 | (condition << 8) | offset8;
		}
		else
		{
			uint16_t offset16 = (uint16_t)(offset & 0xffff);
			opcode = 0b0110'0000'0000'0000 | (condition << 8);
			extensionsList.push_back(offset16); // Add the offset as an extension
		}
	}
	return finalize_instruction(opcode);
}

/// <summary>
/// BCHG dRegister COMMA addressingMode
/// </summary>
any visitor::visitBit_dRegister(parser68000::Bit_dRegisterContext* ctx) 
{
	uint16_t instruction = any_cast<uint16_t>(visit(ctx->children[0]));

	size = 1;
	uint16_t dReg = any_cast<uint16_t>(visit(ctx->children[1])) & 0b111;
	uint16_t effectiveAddress = any_cast<uint16_t>(visit(ctx->children[3]));
	if (!isValidAddressingMode(effectiveAddress, 0b101111111000))
	{
		addError("Invalid addressing mode: ", ctx->children[3]);
	}
	uint16_t opcode = 0b0000'000'1'00'000'000 | (dReg << 9) | (instruction << 6) | effectiveAddress;
	return finalize_instruction(opcode);
}

/// <summary>
/// BCHG HASH number COMMA addressingMode
/// </summary>
any visitor::visitBit_immediateData(parser68000::Bit_immediateDataContext* ctx)
{
	uint16_t instruction = any_cast<uint16_t>(visit(ctx->children[0]));

	size = 0;
	int32_t immediate_data = getIntegerValue(ctx->address());
	uint16_t data8 = (uint16_t)(immediate_data & 0xff);
	extensionsList.push_back(data8);

	uint16_t effectiveAddress = any_cast<uint16_t>(visit(ctx->children[4]));
	if (!isValidAddressingMode(effectiveAddress, 0b101111111000))
	{
		addError("Invalid addressing mode: ", ctx->children[4]);
	}
	if ((effectiveAddress & 0b111'000) == 0b000'000)
	{
		size = 2; // if the addressing mode is data register direct then the size must be long
	}

	if (immediate_data < 0 || (size == 0 && immediate_data > 7) || (size == 2 && immediate_data > 31))
	{
		addError(size == 0 
			? "Immediate data for Bchg must be between 0 and 7"
			: "Immediate data for Bchg must be between 0 and 31", ctx->children[2]);
	}

	uint16_t opcode = 0b0000'100'0'00'000'000 | (instruction << 6) | effectiveAddress;
	return finalize_instruction(opcode);
}
/// <summary>
/// CHK addressingMode COMMA dRegister
/// </summary>
any visitor::visitChk(parser68000::ChkContext* ctx)
{
	size = 1;
	uint16_t effectiveAddress = any_cast<uint16_t>(visit(ctx->children[1]));
	if (!isValidAddressingMode(effectiveAddress, 0b101111111111))
	{
		addError("Invalid addressing mode: ", ctx->children[1]);
	}
	uint16_t dReg = any_cast<uint16_t>(visit(ctx->children[3]));
	uint16_t opcode = 0b0100'000'110'000'000 | (dReg << 9) | effectiveAddress;
	return finalize_instruction(opcode);
}

/// <summary>
/// CLR size? addressingMode
/// </summary>
any visitor::visitClr(parser68000::ClrContext* ctx)
{
	size = 1;
	int arg = 1;
	if (ctx->children.size() == 3) // if there is a size specified
	{
		size = any_cast<uint16_t>(visit(ctx->children[1]));
		arg++; // the optional size is included so there is one extra child
	}
	uint16_t effectiveAddress = any_cast<uint16_t>(visit(ctx->children[arg]));
	if (!isValidAddressingMode(effectiveAddress, 0b101111111000))
	{
		addError("Invalid addressing mode: ", ctx->children[arg]);
	}
	uint16_t opcode = 0b0100'0010'00'000'000 | (size << 6) | effectiveAddress;
	return finalize_instruction(opcode);
}

/// <summary>
/// CMP size? addressingMode COMMA dRegister
/// </summary>
any visitor::visitCmp(parser68000::CmpContext* ctx)
{
	size = 1;
	int arg = 1;
	if (ctx->children.size() == 5) // if there is a size specified
	{
		size = any_cast<uint16_t>(visit(ctx->children[1]));
		arg++; // the optional size is included so there is one extra child
	}
	uint16_t effectiveAddress = any_cast<uint16_t>(visit(ctx->children[arg]));
	if (size == 0 && (effectiveAddress & 0b111'000) == 0b001'000)
	{
		addError("CMP to Address register should be word or long", ctx->children[arg]);
	}
	uint16_t dReg = any_cast<uint16_t>(visit(ctx->children[arg + 2])) & 0b111;
	uint16_t opcode = 0b1011'000'000'000'000 | (dReg << 9) | (size << 6) | effectiveAddress;
	return finalize_instruction(opcode);
}

/// <summary>
/// CMPA size? addressingMode COMMA aRegister
/// </summary>
any visitor::visitCmpa(parser68000::CmpaContext* ctx)
{
	size = 1;
	uint16_t opSize = 0b011;
	int arg = 1;
	if (ctx->children.size() == 5) // if there is a size specified
	{
		size = any_cast<uint16_t>(visit(ctx->children[1]));
		if (size == 0)
		{
			addError("invalid size only .W or .L are supported", ctx->children[1]);
		}
		else if (size == 1)
		{
			opSize = 0b011; // for CMPA size .W is encoded as 0b011
		}
		else if (size == 2)
		{
			opSize = 0b111; // for CMPA size .L is encoded as 0b111
		}
		arg++; // the optional size is included so there is one extra child
	}
	uint16_t effectiveAddress = any_cast<uint16_t>(visit(ctx->children[arg]));
	uint16_t aReg = any_cast<uint16_t>(visit(ctx->children[arg + 2])) & 0b111;
	uint16_t opcode = 0b1011'000'000'000'000 | (aReg << 9) | (opSize << 6) | effectiveAddress;
	return finalize_instruction(opcode);
}

/// <summary>
/// CMPM size? aRegisterIndirectPreDecrement COMMA aRegisterIndirectPreDecrement
/// </summary>
any visitor::visitCmpm(parser68000::CmpmContext* ctx)
{
	size = 1;
	int arg = 1;
	if (ctx->children.size() == 5) // if there is a size specified
	{
		size = any_cast<uint16_t>(visit(ctx->children[1]));
		arg++; // the optional size is included so there is one extra child
	}

	uint16_t ay = any_cast<uint16_t>(visit(ctx->children[arg])) & 0b111;
	uint16_t ax = any_cast<uint16_t>(visit(ctx->children[arg+2])) & 0b111;
	uint16_t opcode = 0b1011'000'1'00'001'000 | (ax << 9) | (size << 6) | ay;
	return finalize_instruction(opcode);
}

/// <summary>
/// DBcc Dn, address
/// </summary>
any visitor::visitDbcc(parser68000::DbccContext* ctx)
{
	uint16_t condition = any_cast<uint16_t>(visit(ctx->children[0]));
	uint16_t dReg = any_cast<uint16_t>(visit(ctx->children[1])) & 0b111;

	uint32_t target = getAddressValue(ctx->address());
	int32_t offset = target - currentAddress - 2; // -2 because the next instruction will be at currentAddress + 2
	if (offset > 0x7fff || offset < -0x8000) 
	{
		addError("Address doesn't fit in one word: " + std::to_string(offset), ctx->children[3]);
		offset = 0x100;
	}
	uint16_t opcode = 0b0101'0000'11001'000 | (condition << 8) | dReg ;
	extensionsList.push_back((uint16_t)(offset & 0xffff));
	return finalize_instruction(opcode);
}

any visitor::visitDiv(tree::ParseTree* ctx, bool isSigned)
{
	size = 1;
	uint16_t effectiveAddress = any_cast<uint16_t>(visit(ctx->children[1]));
	if (!isValidAddressingMode(effectiveAddress, 0b101111111111))
	{
		addError("Invalid addressing mode: ", ctx->children[1]);
	}
	uint16_t dReg = any_cast<uint16_t>(visit(ctx->children[3])) & 0b111;
	uint16_t opcode = (isSigned? 0b1000'000'111'000'000 : 0b1000'000'011'000'000) | (dReg << 9) | effectiveAddress;
	return finalize_instruction(opcode);
}
/// <summary>
/// DIVS size? addressingMode COMMA dRegister
/// </summary>
any visitor::visitDivs(parser68000::DivsContext* ctx)
{
	return visitDiv(ctx, true);
}

/// <summary>
/// DIVS size? addressingMode COMMA dRegister
/// </summary>
any visitor::visitDivu(parser68000::DivuContext* ctx)
{
	return visitDiv(ctx, false);
}

/// <summary>
/// EOR size? dRegister COMMA addressingMode 
/// </summary>
/// <param name="ctx"></param>
/// <returns></returns>
any visitor::visitEor(parser68000::EorContext* ctx)
{
	size = 1;
	int arg = 1;
	if (ctx->children.size() == 5) // if there is a size specified
	{
		size = any_cast<uint16_t>(visit(ctx->children[1]));
		arg++; // the optional size is included so there is one extra child
	}
	uint16_t dReg = any_cast<uint16_t>(visit(ctx->children[arg])) & 0b111;
	uint16_t effectiveAddress = any_cast<uint16_t>(visit(ctx->children[arg+2]));
	if (!isValidAddressingMode(effectiveAddress, 0b101111'111000))
	{
		addError("Invalid addressing mode: ", ctx->children[arg+2]);
	}

	uint16_t opcode = 0b1011'000'100'000'000 | (dReg << 9) | (size << 6) | effectiveAddress;
	return finalize_instruction(opcode);
}

/// <summary>
/// EXG adRegister COMMA adRegister
/// </summary>
any visitor::visitExg(parser68000::ExgContext* ctx)
{
	size = 2;
	auto txt1 = ctx->children[1]->getText();
	uint16_t reg1 = aRegister(txt1.substr(1));
	bool isDataReg1 = (txt1[0] == 'D') || (txt1[0] == 'd');

	auto txt2 = ctx->children[3]->getText();
	uint16_t reg2 = aRegister(txt2.substr(1));
	bool isDataReg2 = (txt2[0] == 'D') || (txt2[0] == 'd');

	uint16_t opmode = 0b10001; // default to one data and one address register
	if (isDataReg1 && isDataReg2)
	{
		opmode = 0b01000; // both are data registers
	}
	else if (!isDataReg1 && !isDataReg2)
	{
		opmode = 0b01001; // both are address registers
	}

	uint16_t opcode = 0b1100'000'1'00000'000 | (reg1 << 9) | (opmode << 3) | reg2;
	return finalize_instruction(opcode);
}

/// <summary>
/// EXT size? dRegister
/// </summary>
any visitor::visitExt(parser68000::ExtContext* ctx)
{
	uint16_t opmode = 0b010;
	int arg = 1;
	if (ctx->children.size() == 3) // if there is a size specified
	{
		size = any_cast<uint16_t>(visit(ctx->children[1]));
		arg++; // the optional size is included so there is one extra child
		if (size == 0)
		{
			addError("invalid size only .W or .L are supported", ctx->children[1]);
		}
		else if (size == 2)
		{
			opmode = 0b011; // for EXT size .L is encoded as 0b11
		}
	}
	uint16_t dReg = any_cast<uint16_t>(visit(ctx->children[arg])) & 0b111;
	uint16_t opcode = 0b0100'100'000'000'000 | (opmode << 6) | dReg ;
	return finalize_instruction(opcode);
}

/// <summary>
/// ILLEGAL
/// </summary>
any visitor::visitIllegal(parser68000::IllegalContext* ctx)
{
	uint16_t opcode = 0b0100'1010'1111'1100;
	return finalize_instruction(opcode);
}

/// <summary>
/// JMP addressingMode
/// </summary>
any visitor::visitJmp(parser68000::JmpContext* ctx)
{
	uint16_t effectiveAddress = any_cast<uint16_t>(visit(ctx->children[1]));
	if (!isValidAddressingMode(effectiveAddress, 0b001001'111110))
	{
		addError("Invalid addressing mode: ", ctx->children[1]);
	}
	uint16_t opcode = 0b0100'1110'11'000'000 | effectiveAddress;
	return finalize_instruction(opcode);
}

/// <summary>
/// JSR addressingMode
/// </summary>
any visitor::visitJsr(parser68000::JsrContext* ctx)
{
	uint16_t effectiveAddress = any_cast<uint16_t>(visit(ctx->children[1]));
	if (!isValidAddressingMode(effectiveAddress, 0b001001'111110))
	{
		addError("Invalid addressing mode: ", ctx->children[1]);
	}
	uint16_t opcode = 0b0100'1110'10'000'000 | effectiveAddress;
	return finalize_instruction(opcode);
}

any visitor::visitMul(tree::ParseTree* ctx, bool isSigned)
{
	size = 1;
	uint16_t effectiveAddress = any_cast<uint16_t>(visit(ctx->children[1]));
	if (!isValidAddressingMode(effectiveAddress, 0b101111111111))
	{
		addError("Invalid addressing mode: ", ctx->children[1]);
	}
	uint16_t dReg = any_cast<uint16_t>(visit(ctx->children[3])) & 0b111;
	uint16_t opcode = (isSigned ? 0b1100'000'111'000'000 : 0b1100'000'011'000'000) | (dReg << 9) | effectiveAddress;
	return finalize_instruction(opcode);
}

/// <summary>
/// LEA addressingMode COMMA aRegister
/// </summary>
any visitor::visitLea(parser68000::LeaContext* ctx)
{
	size = 2;
	uint16_t effectiveAddress = any_cast<uint16_t>(visit(ctx->children[1]));
	if (!isValidAddressingMode(effectiveAddress, 0b001001'111110))
	{
		addError("Invalid addressing mode: ", ctx->children[1]);
	}
	uint16_t aReg = any_cast<uint16_t>(visit(ctx->children[3])) & 0b111;
	uint16_t opcode = 0b0100'000'111'000'000 | (aReg << 9) | effectiveAddress;
	return finalize_instruction(opcode);
}

/// <summary>
/// LINK aRegister COMMA immediateData
/// </summary>
any visitor::visitLink(parser68000::LinkContext* ctx)
{
	size = 1;
	uint16_t aReg = any_cast<uint16_t>(visit(ctx->children[1])) & 0b111;
	int32_t displacement = getIntegerValue(ctx->address());
	if (displacement > 0x7fff || displacement < -0x8000)
	{
		addError("Displacement doesn't fit on in one word: " + std::to_string(displacement), ctx->address());
	}
	uint16_t data16 = (uint16_t)(displacement & 0xffff);
	extensionsList.push_back(data16);

	uint16_t opcode = 0b0100'1110'0101'0'000 | aReg;
	return finalize_instruction(opcode);
}

/// <summary>
/// LSL/LSR size? dRegister COMMA dRegister
/// </summary>
any visitor::visitLslLsr_dRegister(parser68000::LslLsr_dRegisterContext* ctx)
{
	return visitShiftRegister(ctx, 0b01);
}

/// <summary>
/// LSL/LSR size? HASH number COMMA dRegister
/// </summary>
any visitor::visitLslLsr_immediateData(parser68000::LslLsr_immediateDataContext* ctx)
{
	return visitShiftImmediate(ctx, 0b01);
}

/// <summary>
/// LSL/LSR size? addressingMode 
/// </summary>
any visitor::visitLslLsr_addressingMode(parser68000::LslLsr_addressingModeContext* ctx)
{
	return visitShiftAddressingMode(ctx, 0b01);
}

/// <summary>
/// MOVE size? addressingMode COMMA addressingMode
/// </summary>
any visitor::visitMove(parser68000::MoveContext* ctx)
{
	size = 1;
	int arg = 1;
	if (ctx->children.size() == 5) // if there is a size specified
	{
		size = any_cast<uint16_t>(visit(ctx->children[1]));
		arg++; // the optional size is included so there is one extra child
	}
	uint16_t sourceEffectiveAddress = any_cast<uint16_t>(visit(ctx->children[arg]));
	if (size == 0 && (sourceEffectiveAddress & 0b111'000) == 0b001'000)
	{
		addError("Move from Address register should be word or long", ctx->children[arg]);
	}
	uint16_t destinationEffectiveAddress = any_cast<uint16_t>(visit(ctx->children[arg + 2]));
	if (size == 0 && (destinationEffectiveAddress & 0b111'000) == 0b001'000)
	{
		addError("Move to Address register should be word or long", ctx->children[arg + 2]);
	}
	if (!isValidAddressingMode(destinationEffectiveAddress, 0b111111'111000))
	{
		// Here we allow address register direct as destination as equivalent to MOVEA 
		addError("Invalid destination addressing mode: ", ctx->children[arg + 2]);
	}
	uint16_t destinationMode = (destinationEffectiveAddress >> 3) & 0b111;
	uint16_t destinationRegister = destinationEffectiveAddress & 0b111;
	uint16_t opSize = size;
	switch (size)
	{
	case 0: opSize = 0b01; break; // byte
	case 1: opSize = 0b11; break; // word
	case 2: opSize = 0b10; break; // long
	}

	uint16_t opcode = 0b00'00'000'000'000'000 | (opSize << 12) | (destinationRegister << 9) | (destinationMode << 6) | sourceEffectiveAddress;
	return finalize_instruction(opcode);
}

/// <summary>
/// MOVEA size? addressingMode COMMA aRegister
/// </summary>
any visitor::visitMovea(parser68000::MoveaContext* ctx)
{
	size = 1;
	int arg = 1;
	if (ctx->children.size() == 5) // if there is a size specified
	{
		size = any_cast<uint16_t>(visit(ctx->children[1]));
		arg++; // the optional size is included so there is one extra child
	}
	if (size == 0)
	{
		addError("MOVEA should be word or long", ctx->children[arg + 2]);
		size = 1; // default to word to avoid further errors
	}
	uint16_t sourceEffectiveAddress = any_cast<uint16_t>(visit(ctx->children[arg]));
	uint16_t aReg = any_cast<uint16_t>(visit(ctx->children[arg+2])) & 0b111;
	uint16_t opSize = size;
	switch (size)
	{
	case 1: opSize = 0b11; break; // word
	case 2: opSize = 0b10; break; // long
	}

	uint16_t opcode = 0b00'00'000'001'000'000 | (opSize << 12) | (aReg << 9) | sourceEffectiveAddress;
	return finalize_instruction(opcode);
}

/// <summary>
/// MOVE USP COMMA aRegister
/// </summary>
any visitor::visitMoveusp(parser68000::MoveuspContext* ctx)
{
	size = 2;
	uint16_t aReg = any_cast<uint16_t>(visit(ctx->children[3])) & 0b111;
	uint16_t opcode = 0b0100'1110'0110'1'000 | aReg;
	return finalize_instruction(opcode);
}

/// <summary>
/// MOVE aRegister COMMA USP
/// </summary>
any visitor::visitMove2usp(parser68000::Move2uspContext* ctx)
{
	size = 2;
	uint16_t aReg = any_cast<uint16_t>(visit(ctx->children[1])) & 0b111;
	uint16_t opcode = 0b0100'1110'0110'0'000 | aReg;
	return finalize_instruction(opcode);
}

/// <summary>
/// MOVE SR COMMA addressingMode
/// </summary>
any visitor::visitMovesr(parser68000::MovesrContext* ctx)
{
	size = 1;
	uint16_t destinationEffectiveAddress = any_cast<uint16_t>(visit(ctx->children[3]));
	if (!isValidAddressingMode(destinationEffectiveAddress, 0b101111'111000))
	{
		addError("Invalid addressing mode: ", ctx->children[3]);
	}
	uint16_t opcode = 0b0100'0000'11'000'000 | destinationEffectiveAddress;
	return finalize_instruction(opcode);
}

/// <summary>
/// MOVE addressingMode COMMA SR
/// </summary>
any visitor::visitMove2sr(parser68000::Move2srContext* ctx)
{
	size = 1;
	uint16_t sourceEffectiveAddress = any_cast<uint16_t>(visit(ctx->children[1]));
	if (!isValidAddressingMode(sourceEffectiveAddress, 0b101111'111111))
	{
		addError("Invalid addressing mode: ", ctx->children[1]);
	}
	uint16_t opcode = 0b0100'0110'11'000'000 | sourceEffectiveAddress;
	return finalize_instruction(opcode);
}
inline uint16_t invertRegisterList(uint16_t regList)
{
	uint16_t regListInverted = 0;
	for (int i = 0; i < 16; i++)
	{
		if (regList & (1 << i))
		{
			regListInverted |= (1 << (15 - i));
		}
	}
	return regListInverted;
}

/// <summary>
/// MOVEM size? registerList COMMA addressingMode
/// </summary>
any visitor::visitMovem_toMemory(parser68000::Movem_toMemoryContext* ctx)
{
	size = 1;
	int arg = 1;
	if (ctx->children.size() == 5) // if there is a size specified
	{
		size = any_cast<uint16_t>(visit(ctx->children[1]));
		arg++; // the optional size is included so there is one extra child
		if (size == 0)
		{
			addError("MOVEM should be word or long", ctx->children[arg + 2]);
			size = 1; // default to word to avoid further errors
		}
	}
	uint16_t opSize = size;
	switch (size)
	{
		case 1: opSize = 0; break; // word
		case 2: opSize = 1; break; // long
	}

	uint16_t regList = any_cast<uint16_t>(visit(ctx->children[arg]));
	extensionsList.push_back(regList);
	uint16_t effectiveAddress = any_cast<uint16_t>(visit(ctx->children[arg + 2]));
	if (!isValidAddressingMode(effectiveAddress, 0b001011'111000))
	{
		addError("Invalid addressing mode: ", ctx->children[arg + 2]);
	}
	if ((effectiveAddress & 0b111'000) == 0b100'000)
	{
		// effective address is predecrement mode, the register list is inverted
		regList = invertRegisterList(regList);
		extensionsList[0] = regList;
	}

	uint16_t opcode = 0b0100'1'0'001'0'000'000 | (opSize << 6) | effectiveAddress;
	return finalize_instruction(opcode);
}

/// <summary>
/// MOVEM size? addressingMode COMMA registerList
/// </summary>
any visitor::visitMovem_fromMemory(parser68000::Movem_fromMemoryContext* ctx)
{
	size = 1;
	int arg = 1;
	if (ctx->children.size() == 5) // if there is a size specified
	{
		size = any_cast<uint16_t>(visit(ctx->children[1]));
		arg++; // the optional size is included so there is one extra child
		if (size == 0)
		{
			addError("MOVEM should be word or long", ctx->children[arg + 2]);
			size = 1; // default to word to avoid further errors
		}
	}
	uint16_t opSize = size;
	switch (size)
	{
		case 1: opSize = 0; break; // word
		case 2: opSize = 1; break; // long
	}

	uint16_t regList = any_cast<uint16_t>(visit(ctx->children[arg + 2]));
	extensionsList.push_back(regList);

	uint16_t effectiveAddress = any_cast<uint16_t>(visit(ctx->children[arg]));
	if (!isValidAddressingMode(effectiveAddress, 0b001101'111110))
	{
		addError("Invalid addressing mode: ", ctx->children[arg + 2]);
	}

	uint16_t opcode = 0b0100'1'1'001'0'000'000 | (opSize << 6) | effectiveAddress;
	return finalize_instruction(opcode);
}

/// <summary>
/// MOVEP size? dRegister COMMA aRegisterIndirectDisplacement 
/// </summary>
any visitor::visitMovep_toMemory(parser68000::Movep_toMemoryContext* ctx)
{
	size = 1;
	int arg = 1;
	if (ctx->children.size() == 5) // if there is a size specified
	{
		size = any_cast<uint16_t>(visit(ctx->children[1]));
		arg++; // the optional size is included so there is one extra child
		if (size == 0)
		{
			addError("MOVEP should be word or long", ctx->children[arg + 2]);
			size = 1; // default to word to avoid further errors
		}
	}
	uint16_t opSize = size;
	switch (size)
	{
		case 1: opSize = 0b110; break; // word
		case 2: opSize = 0b111; break; // long
	}
	uint16_t dReg = any_cast<uint16_t>(visit(ctx->children[arg])) & 0b111;
	uint16_t effectiveAddress = any_cast<uint16_t>(visit(ctx->children[arg + 2]));
	assert((effectiveAddress & 0b111'000) == 0b101'000);
	uint16_t aReg = effectiveAddress & 0b111;

	uint16_t opcode = 0b0000'000'000'001'000 | (dReg << 9) | (opSize << 6) | aReg;
	return finalize_instruction(opcode);
}

/// <summary>
/// MOVEP size? aRegisterIndirectDisplacement COMMA dRegister
/// </summary>
any visitor::visitMovep_fromMemory(parser68000::Movep_fromMemoryContext* ctx)
{
	size = 1;
	int arg = 1;
	if (ctx->children.size() == 5) // if there is a size specified
	{
		size = any_cast<uint16_t>(visit(ctx->children[1]));
		arg++; // the optional size is included so there is one extra child
		if (size == 0)
		{
			addError("MOVEP should be word or long", ctx->children[arg + 2]);
			size = 1; // default to word to avoid further errors
		}
	}
	uint16_t opSize = size;
	switch (size)
	{
		case 1: opSize = 0b100; break; // word
		case 2: opSize = 0b101; break; // long
	}
	uint16_t dReg = any_cast<uint16_t>(visit(ctx->children[arg+2])) & 0b111;
	uint16_t effectiveAddress = any_cast<uint16_t>(visit(ctx->children[arg]));
	assert((effectiveAddress & 0b111'000) == 0b101'000);
	uint16_t aReg = effectiveAddress & 0b111;

	uint16_t opcode = 0b0000'000'000'001'000 | (dReg << 9) | (opSize << 6) | aReg;
	return finalize_instruction(opcode);
}

/// <summary>
/// MOVEQ HASH address COMMA dRegister
/// </summary>
any visitor::visitMoveq(parser68000::MoveqContext* ctx)
{
	size = 2;
	int32_t immediate_data = getIntegerValue(ctx->address());

	if (immediate_data < -128 || immediate_data > 127)
	{
		addError("Immediate data for MOVEQ must be between -128 and 127", ctx->children[1]);
		immediate_data = 0;
	}
	uint16_t data8 = (uint16_t)(immediate_data & 0xff);
	uint16_t dReg = any_cast<uint16_t>(visit(ctx->children[4])) & 0b111;
	uint16_t opcode = 0b0111'000'0'0000'0000 | (dReg << 9) | data8;
	return finalize_instruction(opcode);
}

/// <summary>
/// MULS size? addressingMode COMMA dRegister
/// </summary>
any visitor::visitMuls(parser68000::MulsContext* ctx)
{
	return visitMul(ctx, true);
}

/// <summary>
/// MULU size? addressingMode COMMA dRegister
/// </summary>
any visitor::visitMulu(parser68000::MuluContext* ctx)
{
	return visitMul(ctx, false);
}

/// <summary>
/// NBCD addressingMode
/// </summary>
any visitor::visitNbcd(parser68000::NbcdContext* ctx)
{
	size = 0;
	uint16_t effectiveAddress = any_cast<uint16_t>(visit(ctx->children[1]));
	if (!isValidAddressingMode(effectiveAddress, 0b101111'111000))
	{
		addError("Invalid addressing mode: ", ctx->children[1]);
	}
	uint16_t opcode = 0b0100'1000'00'000'000 | effectiveAddress;
	return finalize_instruction(opcode);
}

/// <summary>
/// NEG size? addressingMode
/// </summary>
any visitor::visitNeg(parser68000::NegContext* ctx)
{
	size = 1;
	int arg = 1;
	if (ctx->children.size() == 3) // if there is a size specified
	{
		size = any_cast<uint16_t>(visit(ctx->children[1]));
		arg++; // the optional size is included so there is one extra child
	}
	uint16_t effectiveAddress = any_cast<uint16_t>(visit(ctx->children[arg]));
	if (!isValidAddressingMode(effectiveAddress, 0b101111'111000))
	{
		addError("Invalid addressing mode: ", ctx->children[arg]);
	}
	uint16_t opcode = 0b0100'0100'00'000'000 | (size << 6) | effectiveAddress;
	return finalize_instruction(opcode);
}

/// <summary>
/// NEGX size? addressingMode
/// </summary>
any visitor::visitNegx(parser68000::NegxContext* ctx)
{
	size = 1;
	int arg = 1;
	if (ctx->children.size() == 3) // if there is a size specified
	{
		size = any_cast<uint16_t>(visit(ctx->children[1]));
		arg++; // the optional size is included so there is one extra child
	}
	uint16_t effectiveAddress = any_cast<uint16_t>(visit(ctx->children[arg]));
	if (!isValidAddressingMode(effectiveAddress, 0b101111'111000))
	{
		addError("Invalid addressing mode: ", ctx->children[arg]);
	}
	uint16_t opcode = 0b0100'0000'00'000'000 | (size << 6) | effectiveAddress;
	return finalize_instruction(opcode);
}

/// <summary>
/// NOT size? addressingMode
/// </summary>
any visitor::visitNot(parser68000::NotContext* ctx)
{
	size = 1;
	int arg = 1;
	if (ctx->children.size() == 3) // if there is a size specified
	{
		size = any_cast<uint16_t>(visit(ctx->children[1]));
		arg++; // the optional size is included so there is one extra child
	}
	uint16_t effectiveAddress = any_cast<uint16_t>(visit(ctx->children[arg]));
	if (!isValidAddressingMode(effectiveAddress, 0b101111'111000))
	{
		addError("Invalid addressing mode: ", ctx->children[arg]);
	}
	uint16_t opcode = 0b0100'0110'00'000'000 | (size << 6) | effectiveAddress;
	return finalize_instruction(opcode);
}

/// <summary>
/// NOP
/// </summary>
any visitor::visitNop(parser68000::NopContext* ctx)
{
	uint16_t opcode = 0b0100'1110'0111'0001;
	return finalize_instruction(opcode);
}

/// <summary>
/// PEA addressingMode
/// </summary>
any visitor::visitPea(parser68000::PeaContext* ctx)
{
	size = 2;
	uint16_t effectiveAddress = any_cast<uint16_t>(visit(ctx->children[1]));
	if (!isValidAddressingMode(effectiveAddress, 0b001001'111110))
	{
		addError("Invalid addressing mode: ", ctx->children[1]);
	}
	uint16_t opcode = 0b0100'1000'01'000'000 | effectiveAddress;
	return finalize_instruction(opcode);
}

/// <summary>
/// RESET
/// </summary>
any visitor::visitResetInstruction(parser68000::ResetInstructionContext* ctx)
{
	uint16_t opcode = 0b0100'1110'0111'0000;
	return finalize_instruction(opcode);
}

/// <summary>
/// ROL/ROR size? dRegister COMMA dRegister
/// </summary>
any visitor::visitRolRor_dRegister(parser68000::RolRor_dRegisterContext* ctx)
{
	return visitShiftRegister(ctx, 0b11);
}

/// <summary>
/// ROL/ROR size? HASH number COMMA dRegister
/// </summary>
any visitor::visitRolRor_immediateData(parser68000::RolRor_immediateDataContext* ctx)
{
	return visitShiftImmediate(ctx, 0b11);
}

/// <summary>
/// ROL/ROR size? addressingMode 
/// </summary>
any visitor::visitRolRor_addressingMode(parser68000::RolRor_addressingModeContext* ctx)
{
	return visitShiftAddressingMode(ctx, 0b11);
}

/// <summary>
/// ROXL/ROXR size? dRegister COMMA dRegister
/// </summary>
any visitor::visitRoxlRoxr_dRegister(parser68000::RoxlRoxr_dRegisterContext* ctx)
{
	return visitShiftRegister(ctx, 0b10);
}

/// <summary>
/// ROXL/ROXR size? HASH number COMMA dRegister
/// </summary>
any visitor::visitRoxlRoxr_immediateData(parser68000::RoxlRoxr_immediateDataContext* ctx)
{
	return visitShiftImmediate(ctx, 0b10);
}

/// <summary>
/// ROXL/ROXR size? addressingMode 
/// </summary>
any visitor::visitRoxlRoxr_addressingMode(parser68000::RoxlRoxr_addressingModeContext* ctx)
{
	return visitShiftAddressingMode(ctx, 0b10);
}

/// <summary>
/// RTE
/// </summary>
any visitor::visitRte(parser68000::RteContext* ctx)
{
	uint16_t opcode = 0b0100'1110'0111'0011;
	return finalize_instruction(opcode);
}

/// <summary>
/// RTR
/// </summary>
any visitor::visitRtr(parser68000::RtrContext* ctx)
{
	uint16_t opcode = 0b0100'1110'0111'0111;
	return finalize_instruction(opcode);
}

/// <summary>
/// RTS
/// </summary>
any visitor::visitRts(parser68000::RtsContext* ctx)
{
	uint16_t opcode = 0b0100'1110'0111'0101;
	return finalize_instruction(opcode);
}

/// <summary>
/// SCC addressingMode
/// </summary>
/// <param name="ctx"></param>
/// <returns></returns>
any visitor::visitScc(parser68000::SccContext* ctx)
{
	size = 1;
	uint16_t condition = any_cast<uint16_t>(visit(ctx->children[0])) & 0b1111;
	uint16_t effectiveAddress = any_cast<uint16_t>(visit(ctx->children[1]));
	if (!isValidAddressingMode(effectiveAddress, 0b101111'111000))
	{
		addError("Invalid addressing mode: ", ctx->children[1]);
	}
	uint16_t opcode = 0b0101'0000'11'000'000 | (condition << 8) | effectiveAddress;
	return finalize_instruction(opcode);
}

/// <summary>
/// STOP immediateData
/// </summary>
any visitor::visitStop(parser68000::StopContext* ctx)
{
	size = 1;
	any _ = visit(ctx->children[1]); // immediate data
	uint16_t opcode = 0b0100'1110'0111'0010;
	return finalize_instruction(opcode);
}

/// <summary>
/// SWAP dRegister
/// </summary>
any visitor::visitSwap(parser68000::SwapContext* ctx)
{
	size = 1;
	uint16_t dReg = any_cast<uint16_t>(visit(ctx->children[1])) & 0b111;
	uint16_t opcode = 0b0100'1000'0100'0'000 | dReg;
	return finalize_instruction(opcode);
}

/// <summary>
/// TAS addressingMode
/// </summary>
any visitor::visitTas(parser68000::TasContext* ctx)
{
	size = 0; // TAS is always byte size
	uint16_t effectiveAddress = any_cast<uint16_t>(visit(ctx->children[1]));
	if (!isValidAddressingMode(effectiveAddress, 0b101111111000))
	{
		addError("Invalid addressing mode: ", ctx->children[1]);
	}
	uint16_t opcode = 0b0100'1010'11'000'000 | effectiveAddress;
	return finalize_instruction(opcode);
}

/// <summary>
/// TRAP HASH number
/// </summary>
any visitor::visitTrap(parser68000::TrapContext* ctx)
{
	int32_t vector = getIntegerValue(ctx->address());
	if (vector < 0 || vector > 15)
	{
		addError("Trap vector must be between 0 and 15", ctx->children[2]);
		vector = 0;
	}
	uint16_t opcode = 0b0100'1110'0100'0000 | vector;
	return finalize_instruction(opcode);
}

/// <summary>
/// TRAPV
/// </summary>
any visitor::visitTrapv(parser68000::TrapvContext* ctx)
{
	uint16_t opcode = 0b0100'1110'0111'0110;
	return finalize_instruction(opcode);
}

/// <summary>
/// TST size? addressingMode
/// </summary>
any visitor::visitTst(parser68000::TstContext* ctx)
{
	size = 1;
	int arg = 1;
	if (ctx->children.size() == 3) // if there is a size specified
	{
		size = any_cast<uint16_t>(visit(ctx->children[1]));
		arg++; // the optional size is included so there is one extra child
	}
	uint16_t effectiveAddress = any_cast<uint16_t>(visit(ctx->children[arg]));
	if (!isValidAddressingMode(effectiveAddress, 0b101111'111000))
	{
		addError("Invalid addressing mode: ", ctx->children[arg]);
	}
	uint16_t opcode = 0b0100'1010'00'000'000 | (size << 6) | effectiveAddress;
	return finalize_instruction(opcode);
}
any visitor::visitUnlk(parser68000::UnlkContext* ctx)
{
	uint16_t aReg = any_cast<uint16_t>(visit(ctx->children[1])) & 0b111;
	uint16_t opcode = 0b0100'1110'0101'1'000 | aReg;
	return finalize_instruction(opcode);
}

// ====================================================================================================
// Register lists
// ====================================================================================================
any visitor::visitRegisterList(parser68000::RegisterListContext* ctx)
{
	uint16_t regList = 0;
	for (size_t i = 0; i < ctx->children.size(); i += 2) // skip separators
	{
		regList |= any_cast<uint16_t>(visit(ctx->children[i]));
	}
	return regList;
}
any visitor::visitRegisterListRegister(parser68000::RegisterListRegisterContext* ctx)
{
	auto txt = ctx->children[0]->getText();
	uint16_t reg = stoi(txt.substr(1));
	bool isDataReg = (txt[0] == 'D') || (txt[0] == 'd');
	if (isDataReg)
	{
		return (uint16_t)(1 << reg);
	}
	else
	{
		return (uint16_t)(1 << (8 + reg));
	}
}
any visitor::visitRegisterListRange(parser68000::RegisterListRangeContext* ctx)
{
	return visit(ctx->children[0]);
}
any visitor::visitRegisterRange(parser68000::RegisterRangeContext* ctx)
{
	auto txt1 = ctx->children[0]->getText();
	uint16_t reg1 = stoi(txt1.substr(1));
	bool isDataReg1 = (txt1[0] == 'D') || (txt1[0] == 'd');

	auto txt2 = ctx->children[2]->getText();
	uint16_t reg2 = stoi(txt2.substr(1));
	bool isDataReg2 = (txt2[0] == 'D') || (txt2[0] == 'd');

	if (isDataReg1 != isDataReg2)
	{
		addError("Register range must be both data or both address registers", ctx);
		return (uint16_t)0;
	}
	if (reg1 > reg2)
	{
		addError("Invalid register range, start register must be less than or equal to end register", ctx);
		return (uint16_t)0;
	}
	uint16_t regList = 0;
	if (isDataReg1)
	{
		for (uint16_t r = reg1; r <= reg2; r++)
		{
			regList |= (1 << r);
		}
	}
	else
	{
		for (uint16_t r = reg1; r <= reg2; r++)
		{
			regList |= (1 << (8 + r));
		}
	}
	return regList;
}
// ====================================================================================================
// Addressing modes
// ====================================================================================================

/// <summary>
/// Addressing mode for data registers: D0
/// </summary>
any visitor::visitDRegister(parser68000::DRegisterContext* ctx)
{
    auto s = ctx->getText().substr(1);
	auto reg = stoi(s);
    return (uint16_t) (0b000'000 | reg);
}

/// <summary>
/// Addressing mode for address registers: A0
/// </summary>
any visitor::visitARegister(parser68000::ARegisterContext* ctx)
{
    auto reg = aRegister(ctx->getText().substr(1));
    return (uint16_t)(0b001'000 | reg);
}

/// <summary>
/// Addressing mode for address register indirect: (A0)
/// </summary>
any visitor::visitARegisterIndirect(parser68000::ARegisterIndirectContext* context)
{
	auto s = context->children[1]->getText().substr(1);
	auto reg = aRegister(s);
	return (uint16_t)(0b010'000 | reg);
}

/// <summary>
/// Addressing mode for address register indirect post increment: (A0)+
/// </summary>
any visitor::visitARegisterIndirectPostIncrement(parser68000::ARegisterIndirectPostIncrementContext* ctx)
{
	auto s = ctx->children[1]->getText().substr(1);
	auto reg = aRegister(s);
	return (uint16_t)(0b011'000 | reg);
}

/// <summary>
/// Addressing mode for address register indirect pre decrement: -(A0)
/// </summary>
any visitor::visitARegisterIndirectPreDecrement(parser68000::ARegisterIndirectPreDecrementContext* ctx)
{
	auto s = ctx->children[2]->getText().substr(1);
	auto reg = aRegister(s);
    return (uint16_t)(0b100'000 | reg);
}

/// <summary>
/// Addressing mode for address register indirect with displacement
/// This is a helper function to handle both old and new syntax for address register indirect with displacement
/// It is used to avoid code duplication in the visitARegisterIndirectDisplacementOld and visitARegisterIndirectDisplacementNew methods
/// </summary>
any visitor::visitARegisterIndirectDisplacement(tree::ParseTree* pDisplacement, tree::ParseTree* pRegistr)
{
	int32_t displacement = any_cast<int32_t>(visit(pDisplacement));
	if (displacement > 0x7fff || displacement < -0x8000)
	{
		addError("Displacement doesn't fit on in one word: " + std::to_string(displacement), pDisplacement);
	}
	extensionsList.push_back((uint16_t)(displacement & 0xffff));

	auto s = pRegistr->getText().substr(1);
	auto reg = aRegister(s);
	return (uint16_t)(0b101'000 | reg);
}

/// <summary>
/// Addressing mode for address register indirect with displacement: 42(A0)
/// </summary>
any visitor::visitARegisterIndirectDisplacementOld(parser68000::ARegisterIndirectDisplacementOldContext* ctx)
{
	return visitARegisterIndirectDisplacement(ctx->children[0], ctx->children[2]);
}

/// <summary>
/// Addressing mode for address register indirect with displacement: (42,A0)
/// </summary>
any visitor::visitARegisterIndirectDisplacementNew(parser68000::ARegisterIndirectDisplacementNewContext* ctx)
{
	return visitARegisterIndirectDisplacement(ctx->children[1], ctx->children[3]);
}

/// <summary>
/// Addressing mode for address register indirect with index
/// This is a helper function to handle both old and new syntax for address register indirect with index
/// It is used to avoid code duplication in the visitARegisterIndirectIndexOld and visitARegisterIndirectIndexNew methods
/// </summary>
any visitor::visitARegisterIndirectDisplacement(tree::ParseTree* pDisplacement, tree::ParseTree* pRregistr, tree::ParseTree* pIndex)
{
	int32_t displacement = any_cast<int32_t>(visit(pDisplacement));
	if (displacement > 0x7f || displacement < -0x80)
	{
		addError("Displacement doesn't fit on in one byte: " + std::to_string(displacement), pDisplacement);
	}
	uint16_t displacement8 = (uint16_t)(displacement & 0xff);

	uint16_t index = any_cast<uint16_t>(visit(pIndex));
	extensionsList.push_back((index << 11) | displacement8);
	
	auto s = pRregistr->getText().substr(1);
	auto reg = aRegister(s);
	return (uint16_t)(0b110'000 | reg);
}

/// <summary>
/// Addressing mode for address register indirect with index: 42(A0, D1)
/// </summary>
any visitor::visitARegisterIndirectIndexOld(parser68000::ARegisterIndirectIndexOldContext* ctx)
{
	return visitARegisterIndirectDisplacement(ctx->children[0], ctx->children[2], ctx->children[4]);
}

/// <summary>
/// Addressing mode for address register indirect with index: (42, A0, D1)
/// </summary>
any visitor::visitARegisterIndirectIndexNew(parser68000::ARegisterIndirectIndexNewContext* ctx)
{
	return visitARegisterIndirectDisplacement(ctx->children[1], ctx->children[3], ctx->children[5]);
}

uint32_t visitor::getAddressValue(parser68000::AddressContext* ctx)
{
	any address = visit(ctx);
	uint32_t target;
	if (address.type() == typeid(std::string))
	{
		std::string label = any_cast<std::string>(address);
		// search first for a symbol
		auto it = symbols.find(label);
		if (it != symbols.end())
		{
			if (it->second.type() != typeid(int32_t))
			{
				addError("Argument is not an number: " + label, ctx);
				target = 0;
			}
			else
			{
				int32_t value = any_cast<int32_t>(it->second);
				if (value < 0)
				{
					addError("Address cannot be negative: " + label, ctx);
					value = 0;
				}
				target = static_cast<uint32_t>(value);
			}
		}
		else
		{
			// then search for a label
			auto it = labels.find(label);
			if (it == labels.end())
			{
				// During the 1st pass the label may not be defined yet
				if (pass != 0)
				{
					addError("Label not found: " + label, ctx);
				}
				target = 0;
			}
			else
			{
				target = it->second;
			}
		}
	}
	else if (address.type() == typeid(char))
	{
		char c = any_cast<char>(address);
		if (c == '*')
		{
			target = this->currentAddress;
		}
		else
		{
			addError("Syntax error; * expected", ctx);
			target = 0xabcd;
		}
	}
	else
	{
		int32_t value = any_cast<int32_t>(address);
		if (value < 0)
		{
			addError("Address cannot be negative: " + std::to_string(value), ctx);
			value = 0;
		}
		target = static_cast<uint32_t>(value);
	}
	return target;
}
int32_t visitor::getIntegerValue(parser68000::AddressContext* ctx)
{
	any address = visit(ctx);
	int32_t target;
	if (address.type() == typeid(std::string))
	{
		std::string label = any_cast<std::string>(address);
		// search first for a symbol
		auto it = symbols.find(label);
		if (it != symbols.end())
		{
			if (it->second.type() != typeid(int32_t))
			{
				addError("Argument is not an number: " + label, ctx);
				target = 0;
			}
			else
			{
				target = any_cast<int32_t>(it->second);
			}
		}
		else
		{
			// then search for a label
			auto it = labels.find(label);
			if (it == labels.end())
			{
				// During the 1st pass the label may not be defined yet
				if (pass != 0)
				{
					addError("Label not found: " + label, ctx);
				}
				target = 0;
			}
			else
			{
				if (it->second > INT32_MAX)
				{
					addError("Label value too large for integer: " + label, ctx);
					target = INT32_MAX;
				}
				else
				{
					target = static_cast<int32_t>(it->second);
				}
			}
		}
	}
	else if (address.type() == typeid(char))
	{
		char c = any_cast<char>(address);
		if (c == '*')
		{
			target = this->currentAddress;
		}
		else
		{
			addError("Syntax error; * expected", ctx);
			target = 0x5555;
		}
	}
	else
	{
		target = any_cast<int32_t>(address);
	}
	return target;
}

/// <summary>
/// Helper function to handle absolute addressing modes with different sizes
/// </summary>
/// <param name="ctx">The parse tree context</param>
/// <param name="size">The size of the absolute address (0 = not provided, 1 = word, 2 = long)</param>
any visitor::visitAbsoluteSize(parser68000::AddressContext* ctx, int size)
{
	uint32_t target = getAddressValue(ctx);

	switch (size)
	{
		case 0: // size wasn't provided
			if (target > 0x7fff && target < 0xffff8000)
			{
				// The address doesn't fit in one word so we need to use long
				uint16_t displacementHigh = (uint16_t)(target >> 16);
				uint16_t displacementLow = (uint16_t)(target & 0xffff);
				extensionsList.push_back(displacementHigh);
				extensionsList.push_back(displacementLow);

				return (uint16_t)(0b111'001);
			}
			else
			{
				uint16_t displacement16 = (uint16_t)(target & 0xffff);
				extensionsList.push_back(displacement16);
				return (uint16_t)(0b111'000);
			}
			break;
		case 1: // size is word
			{
				if (target > 0x7fff && target < 0xffff8000)
				{
					addError("Address doesn't fit on in one word: " , ctx->children[0]);
				}
				uint16_t displacement16 = (uint16_t)(target & 0xffff);
				extensionsList.push_back(displacement16);
				return (uint16_t)(0b111'000);
			}
		break;
			case 2: // size is long
			{
				uint16_t displacementHigh = (uint16_t)(target >> 16);
				uint16_t displacementLow = (uint16_t)(target & 0xffff);
				extensionsList.push_back(displacementHigh);
				extensionsList.push_back(displacementLow);

				return (uint16_t)(0b111'001);
			}
			break;
		default:
			assert(!"we shouldnot be there");
			break;
	}
	return (uint16_t)(0b111'000);
}
/// <summary>
/// Addressing mode for absolute. Size not provided
/// </summary>
any visitor::visitAbsolute(parser68000::AbsoluteContext* ctx)
{
	return visitAbsoluteSize(ctx->address(), 0);
}
/// <summary>
/// Addressing mode for absolute long: 42.W
/// </summary>
any visitor::visitAbsoluteShort(parser68000::AbsoluteShortContext* ctx)
{
	return visitAbsoluteSize(ctx->address(), 1);
}

/// <summary>
/// Addressing mode for absolute long: 42.L
/// </summary>
any visitor::visitAbsoluteLong(parser68000::AbsoluteLongContext* ctx)
{
	return visitAbsoluteSize(ctx->address(), 2);
}

/// <summary>
/// Addressing mode for PC indirect with displacement: 42(PC)
/// </summary>
any visitor::visitPcIndirectDisplacement(parser68000::PcIndirectDisplacementContext* ctx)
{
	int32_t displacement = any_cast<int32_t>(visit(ctx->children[0]));
	if (displacement > 0x7fff || displacement < -0x8000)
	{
		addError("Displacement doesn't fit on in one word: " + std::to_string(displacement), ctx->children[0]);
	}
	extensionsList.push_back((uint16_t)(displacement & 0xffff));
	return (uint16_t)(0b111'010);
}

/// <summary>
/// Addressing mode for PC indirect with index: 42(PC, D1)
/// </summary>
any visitor::visitPcIndirectIndex(parser68000::PcIndirectIndexContext* ctx)
{
	int32_t displacement = any_cast<int32_t>(visit(ctx->children[0]));
	if (displacement > 0x7f || displacement < -0x80)
	{
		addError("Displacement doesn't fit on in one byte: " + std::to_string(displacement), ctx->children[0]);
	}
	uint16_t displacement8 = (uint16_t)(displacement & 0xff);

	uint16_t index = any_cast<uint16_t>(visit(ctx->children[4]));
	extensionsList.push_back((index << 11) | displacement8);

	return (uint16_t)(0b111'011);
}

/// <summary>
/// Addressing mode for immediate data: #42 Or #ID
/// </summary>
any visitor::visitImmediateData(parser68000::ImmediateDataContext* ctx)
{
	any immediate_data = visit(ctx->children[1]);

	int32_t target = getIntegerValue(ctx->address());

	if (size == 0)
	{
		if (target > 0x7f || target < -0x80)
		{
			addError("Immediate data doesn't fit on in one byte: " + std::to_string(target), ctx->children[1]);
		}
		uint16_t data8 = (uint16_t)(target & 0xff);
		extensionsList.push_back(data8);
	}
	else if (size == 1)
	{
		if (target > 0x7fff || target < -0x8000)
		{
			addError("Immediate data doesn't fit on in one word: " + std::to_string(target), ctx->children[1]);
		}
		uint16_t data16 = (uint16_t)(target & 0xffff);
		extensionsList.push_back(data16);
	}
	else
	{
		uint16_t dataHigh = (uint16_t)(target >> 16);
		uint16_t dataLow = (uint16_t)(target & 0xffff);
		extensionsList.push_back(dataHigh);
		extensionsList.push_back(dataLow);
	}
	return (uint16_t)(0b111'100);
}

// ====================================================================================================
// Addressing modes helpers
// ====================================================================================================

any visitor::visitAdRegister(parser68000::AdRegisterContext* ctx)
{
	auto s = ctx->children[0]->getText();
	// The returned value is formatted for the aRegisterIndirectIndex addressing mode
	if (s[0] == 'd' || s[0] == 'D')
	{
		auto reg = stoi(s.substr(1));
		return (uint16_t)(0b0'000'0 | (reg << 1));
	}
	else
	{
		auto reg = aRegister(s.substr(1));
		return (uint16_t)(0b1'000'0 | (reg << 1));
	}
}

any visitor::visitAdRegisterSize(parser68000::AdRegisterSizeContext* ctx)
{
	uint16_t adRegister = any_cast<uint16_t>(visit(ctx->children[0]));
	if (ctx->children.size() == 2)
	{
		int16_t size = any_cast<uint16_t>(visit(ctx->children[1]));
		if (size == 0)
		{
			addError("Invalid size", ctx->children[1]);
		}
		else if (size == 2)
		{
			adRegister |= 0b0'000'1; // size is long
		}
	}
	return adRegister;
}

bool visitor::isValidAddressingMode(unsigned short effectiveAddress, unsigned short acceptable)
{
	static const unsigned masks[] = {
		1 << 11, 1 << 10, 1 << 9, 1 << 8, 1 << 7, 1 << 6, 1 << 5, // mode 000 to 110
		1 << 4, 1 << 3, 1 << 2, 1 << 1, 1                         // mode 111 
	};
	unsigned mode = effectiveAddress >> 3;
	if (mode != 7)
	{
		return (acceptable & masks[mode]) == masks[mode];
	}

	static const unsigned offsets[] = { 7, 8, 10, 11, 9 }; // 000, 001, 100, 010, 011
	mode = effectiveAddress & 7;
	if (mode > 4)
	{
		return false;
	}
	auto offset = offsets[mode];
	return (acceptable & masks[offset]) == masks[offset];
}


void visitor::addError(const std::string& message, tree::ParseTree* ctx)
{
    if (pass == 1) 
	{
        if (auto* prc = dynamic_cast<antlr4::ParserRuleContext*>(ctx)) 
		{
			size_t line = prc->getStart()->getLine();
			size_t col = prc->getStart()->getCharPositionInLine();
            errorList.add(message, line, col);
        } 
		else if (auto* tni = dynamic_cast<antlr4::tree::TerminalNodeImpl*>(ctx))
		{
			size_t line = tni->symbol->getLine();
			size_t col = tni->symbol->getCharPositionInLine();
			errorList.add(message, line, col);
		}
		else
		{
			errorList.add(message, 0, 0);
		}
    }
}

void visitor::addPass0Error(const std::string& message, tree::ParseTree* ctx)
{
	if (auto* prc = dynamic_cast<antlr4::ParserRuleContext*>(ctx))
	{
		size_t line = prc->getStart()->getLine();
		size_t col = prc->getStart()->getCharPositionInLine();
		errorList.add(message, line, col);
	}
	else
	{
		errorList.add(message, 0, 0);
	}
}


