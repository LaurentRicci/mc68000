#pragma once
#include <string>
#include <iostream>
#include <sstream>

#include "core.h"

namespace mc68000
{
	inline std::string toHex(unsigned short value)
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
		unsigned short unknown(unsigned short);

		unsigned short abcd(unsigned short);
		unsigned short sbcd(unsigned short);

		unsigned short add(unsigned short);
		unsigned short adda(unsigned short);
		unsigned short cmp(unsigned short);
		unsigned short cmpa(unsigned short);
		unsigned short sub(unsigned short);
		unsigned short suba(unsigned short);
		unsigned short and_(unsigned short);
		unsigned short or_(unsigned short);
		unsigned short eor(unsigned short);

		unsigned short addi(unsigned short);
		unsigned short andi(unsigned short);
		unsigned short cmpi(unsigned short);
		unsigned short eori(unsigned short);
		unsigned short ori(unsigned short);
		unsigned short subi(unsigned short);

		unsigned short addq(unsigned short);
		unsigned short subq(unsigned short);

		unsigned short addx(unsigned short);
		unsigned short subx(unsigned short);

		unsigned short andi2ccr(unsigned short);
		unsigned short andi2sr(unsigned short);

		unsigned short asl_register(unsigned short);
		unsigned short asl_memory(unsigned short);
		unsigned short asr_register(unsigned short);
		unsigned short asr_memory(unsigned short);


		unsigned short bra(unsigned short);
		unsigned short bhi(unsigned short);
		unsigned short bls(unsigned short);
		unsigned short bcc(unsigned short);
		unsigned short bcs(unsigned short);
		unsigned short bne(unsigned short);
		unsigned short beq(unsigned short);
		unsigned short bvc(unsigned short);
		unsigned short bvs(unsigned short);
		unsigned short bpl(unsigned short);
		unsigned short bmi(unsigned short);
		unsigned short bge(unsigned short);
		unsigned short blt(unsigned short);
		unsigned short bgt(unsigned short);
		unsigned short ble(unsigned short);
		unsigned short bsr(unsigned short);

		unsigned short bchg_r(unsigned short);
		unsigned short bset_r(unsigned short);
		unsigned short bclr_r(unsigned short);
		unsigned short bchg_i(unsigned short);
		unsigned short bset_i(unsigned short);
		unsigned short bclr_i(unsigned short);

		unsigned short btst_r(unsigned short);
		unsigned short btst_i(unsigned short);

		unsigned short chk(unsigned short);

		unsigned short clr(unsigned short);

		unsigned short cmpm(unsigned short);

		unsigned short dbcc(unsigned short);

		unsigned short divs(unsigned short);
		unsigned short divu(unsigned short);
		unsigned short muls(unsigned short);
		unsigned short mulu(unsigned short);

		unsigned short eori2ccr(unsigned short);

		unsigned short exg(unsigned short);

		unsigned short ext(unsigned short);

		unsigned short illegal(unsigned short);

		unsigned short jmp(unsigned short);
		unsigned short jsr(unsigned short);

		unsigned short lea(unsigned short);

		unsigned short link(unsigned short);

		unsigned short lsl_register(unsigned short);
		unsigned short lsl_memory(unsigned short);
		unsigned short lsr_register(unsigned short);
		unsigned short lsr_memory(unsigned short);

		unsigned short move(unsigned short);
		unsigned short movea(unsigned short);
		unsigned short move2ccr(unsigned short);
		unsigned short movesr(unsigned short);
		unsigned short movem(unsigned short);
		unsigned short movep(unsigned short);
		unsigned short moveq(unsigned short);

		unsigned short nbcd(unsigned short);

		unsigned short neg(unsigned short);
		unsigned short negx(unsigned short);
		unsigned short nop(unsigned short);
		unsigned short not_(unsigned short);

		unsigned short ori2ccr(unsigned short);

		unsigned short pea(unsigned short);

		unsigned short rol_register(unsigned short);
		unsigned short ror_register(unsigned short);
		unsigned short roxl_register(unsigned short);
		unsigned short roxr_register(unsigned short);

		unsigned short rol_memory(unsigned short);
		unsigned short ror_memory(unsigned short);
		unsigned short roxl_memory(unsigned short);
		unsigned short roxr_memory(unsigned short);

		uint16_t rte(uint16_t);
		unsigned short rtr(unsigned short);
		unsigned short rts(unsigned short);

		unsigned short scc(unsigned short);
		unsigned short stop(unsigned short);
		unsigned short swap(unsigned short);
		unsigned short tas(unsigned short);

		unsigned short trap(unsigned short);
		unsigned short trapv(unsigned short);
		unsigned short tst(unsigned short);
		unsigned short unlk(unsigned short);

		using t_handler = unsigned short (DisAsm::*)(unsigned short);
		friend t_handler* setup<DisAsm>();

		t_handler* handlers;

	private:
		unsigned short fetchNextWord();
		std::string decodeEffectiveAddress(unsigned short ea, bool isLongOperation = false);
		unsigned short disassembleBccInstruction(const char* instructionName, unsigned short instructionId, unsigned short opcode);
		unsigned short disassembleImmediateInstruction(const char* instructionName, unsigned short instructionId, unsigned short opcode);
		uint16_t disassembleAddxSubx(const char* instructionName, uint16_t instructionId, uint16_t opcode);
		void reset(const unsigned short* memory);

	private:
		const unsigned short* pc;
		const unsigned short* memory;
		std::string disassembly;

	public:
		DisAsm();
		~DisAsm();
		std::string disassemble(const unsigned short*);
	};
}