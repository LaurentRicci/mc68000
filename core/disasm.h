#pragma once
#include <string>
#include <iostream>
#include <sstream>

#include "core.h"

namespace mc68000
{
	inline std::string toHex(uint32_t value)
	{
		// return std::format("{x}", value);

		std::ostringstream stream;
		stream << std::hex << value;
		return stream.str();
	}

	inline std::string toHexDollar(uint16_t value)
	{
		char buff[32];
		sprintf(buff, "$%x", value);
		return buff;
	}

	class DisAsm
	{
	private:
		uint16_t unknown(uint16_t);

		uint16_t abcd(uint16_t);
		uint16_t sbcd(uint16_t);

		uint16_t add(uint16_t);
		uint16_t adda(uint16_t);
		uint16_t cmp(uint16_t);
		uint16_t cmpa(uint16_t);
		uint16_t sub(uint16_t);
		uint16_t suba(uint16_t);
		uint16_t and_(uint16_t);
		uint16_t or_(uint16_t);
		uint16_t eor(uint16_t);

		uint16_t addi(uint16_t);
		uint16_t andi(uint16_t);
		uint16_t cmpi(uint16_t);
		uint16_t eori(uint16_t);
		uint16_t ori(uint16_t);
		uint16_t subi(uint16_t);

		uint16_t addq(uint16_t);
		uint16_t subq(uint16_t);

		uint16_t addx(uint16_t);
		uint16_t subx(uint16_t);

		uint16_t andi2ccr(uint16_t);
		uint16_t andi2sr(uint16_t);

		uint16_t asl_register(uint16_t);
		uint16_t asl_memory(uint16_t);
		uint16_t asr_register(uint16_t);
		uint16_t asr_memory(uint16_t);


		uint16_t bra(uint16_t);
		uint16_t bhi(uint16_t);
		uint16_t bls(uint16_t);
		uint16_t bcc(uint16_t);
		uint16_t bcs(uint16_t);
		uint16_t bne(uint16_t);
		uint16_t beq(uint16_t);
		uint16_t bvc(uint16_t);
		uint16_t bvs(uint16_t);
		uint16_t bpl(uint16_t);
		uint16_t bmi(uint16_t);
		uint16_t bge(uint16_t);
		uint16_t blt(uint16_t);
		uint16_t bgt(uint16_t);
		uint16_t ble(uint16_t);
		uint16_t bsr(uint16_t);

		uint16_t bchg_r(uint16_t);
		uint16_t bset_r(uint16_t);
		uint16_t bclr_r(uint16_t);
		uint16_t bchg_i(uint16_t);
		uint16_t bset_i(uint16_t);
		uint16_t bclr_i(uint16_t);

		uint16_t btst_r(uint16_t);
		uint16_t btst_i(uint16_t);

		uint16_t chk(uint16_t);

		uint16_t clr(uint16_t);

		uint16_t cmpm(uint16_t);

		uint16_t dbcc(uint16_t);

		uint16_t divs(uint16_t);
		uint16_t divu(uint16_t);
		uint16_t muls(uint16_t);
		uint16_t mulu(uint16_t);

		uint16_t eori2ccr(uint16_t);
		uint16_t eori2sr(uint16_t);

		uint16_t exg(uint16_t);

		uint16_t ext(uint16_t);

		uint16_t illegal(uint16_t);

		uint16_t jmp(uint16_t);
		uint16_t jsr(uint16_t);

		uint16_t lea(uint16_t);

		uint16_t link(uint16_t);

		uint16_t lsl_register(uint16_t);
		uint16_t lsl_memory(uint16_t);
		uint16_t lsr_register(uint16_t);
		uint16_t lsr_memory(uint16_t);

		uint16_t move(uint16_t);
		uint16_t movea(uint16_t);
		uint16_t move2ccr(uint16_t);
		uint16_t movesr(uint16_t);
		uint16_t move2sr(uint16_t);
		uint16_t movem(uint16_t);
		uint16_t movep(uint16_t);
		uint16_t moveq(uint16_t);

		uint16_t nbcd(uint16_t);

		uint16_t neg(uint16_t);
		uint16_t negx(uint16_t);
		uint16_t nop(uint16_t);
		uint16_t not_(uint16_t);

		uint16_t ori2ccr(uint16_t);

		uint16_t pea(uint16_t);

		uint16_t rol_register(uint16_t);
		uint16_t ror_register(uint16_t);
		uint16_t roxl_register(uint16_t);
		uint16_t roxr_register(uint16_t);

		uint16_t rol_memory(uint16_t);
		uint16_t ror_memory(uint16_t);
		uint16_t roxl_memory(uint16_t);
		uint16_t roxr_memory(uint16_t);

		uint16_t rte(uint16_t);
		uint16_t rtr(uint16_t);
		uint16_t rts(uint16_t);

		uint16_t scc(uint16_t);
		uint16_t stop(uint16_t);
		uint16_t swap(uint16_t);
		uint16_t tas(uint16_t);

		uint16_t trap(uint16_t);
		uint16_t trapv(uint16_t);
		uint16_t tst(uint16_t);
		uint16_t unlk(uint16_t);

		using t_handler = uint16_t (DisAsm::*)(uint16_t);
		friend t_handler* setup<DisAsm>();

		t_handler* handlers;

	private:
		uint16_t fetchNextWord();
		std::string decodeEffectiveAddress(uint16_t ea, bool isLongOperation);
		std::string registersToString(uint16_t registers, bool isPredecrement);
		uint16_t disassembleBccInstruction(const char* instructionName, uint16_t instructionId, uint16_t opcode);
		uint16_t disassembleImmediateInstruction(const char* instructionName, uint16_t instructionId, uint16_t opcode);
		uint16_t disassembleBitRegisterInstruction(const char* instructionName, uint16_t instructionId, uint16_t opcode);
		uint16_t disassembleBitImmediateInstruction(const char* instructionName, uint16_t instructionId, uint16_t opcode);
		uint16_t disassembleAddxSubx(const char* instructionName, uint16_t instructionId, uint16_t opcode);
		uint16_t disassembleMulDiv(const char* instructionName, uint16_t instructionId, uint16_t opcode);
		uint16_t disassembleShiftRotate(const char* instructionName, uint16_t instructionId, uint16_t opcode);
		uint16_t disassembleLogical(const char* instructionName, uint16_t instructionId, uint16_t opcode);
		uint16_t disassemble2ccr(const char* instructionName, uint16_t instructionId, uint16_t opcode);
		uint16_t disassemble2sr(const char* instructionName, uint16_t instructionId, uint16_t opcode);
		void reset(const uint16_t* memory);

	private:
		const uint16_t* pc;
		const uint16_t* memory;
		std::string disassembly;

	public:
		DisAsm();
		~DisAsm();
		std::string disassemble(const uint16_t*);
	};
}