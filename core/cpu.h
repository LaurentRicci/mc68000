#pragma once
#include "core.h"
#include "memory.h"

namespace mc68000
{
	struct StatusRegister
	{
		uint8_t c : 1;
		uint8_t v : 1;
		uint8_t z : 1;
		uint8_t n : 1;
		uint8_t x : 1;
		StatusRegister()
		{
			c = v = z = n = x = 0;
		}
	};

	class Cpu
	{
		//
		// Instruction handlers
		//
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

		unsigned short rtr(unsigned short);
		unsigned short rts(unsigned short);

		unsigned short scc(unsigned short);
		unsigned short swap(unsigned short);
		unsigned short tas(unsigned short);

		unsigned short trap(unsigned short);
		unsigned short trapv(unsigned short);
		unsigned short tst(unsigned short);
		unsigned short unlk(unsigned short);

		using t_handler = unsigned short (Cpu::*)(unsigned short);
		friend t_handler* setup<Cpu>();

		t_handler* handlers;
		
		//
		// trap handlers
		// 
	private:
		using trapHandler_t = void (*)(uint32_t d0, uint32_t a0);
		trapHandler_t trapHandlers[16];

		//
		// internal helpers
		//
	private:
		template <typename T> T readAt(uint16_t ea);
		template <typename T> void writeAt(uint16_t ea, T data);
		template <typename T> void move(uint16_t from, uint16_t to);
		uint32_t getTargetAddress(uint16_t opcode);

		template <typename T> void add(uint16_t srcEffectiveAdress, uint16_t dstEffectiveAdress);
		template <typename T> void addq(uint32_t data, uint16_t dstEffectiveAdress);
		template <typename T> void and_(uint16_t srcEffectiveAdress, uint16_t dstEffectiveAdress);
		template <typename T> void cmp(uint16_t srcEffectiveAdress, uint16_t dstEffectiveAdress);
		template <typename T> void or_(uint16_t srcEffectiveAdress, uint16_t dstEffectiveAdress);


		//
		// private members
		//
	private:
		unsigned int dRegisters[8];
		unsigned int aRegisters[8];
		StatusRegister statusRegister;
		uint32_t pc;
		memory localMemory;
		bool done;

		//
		// public methods
		//
	public:
		Cpu(const memory& memory);
		~Cpu();

		void reset();
		void reset(const memory& memory);
		void start(uint32_t startPc, uint32_t startSP = 0);
		void setARegister(int reg, uint32_t value);
		void registerTrapHandler(int trapNumber, trapHandler_t traphandler);

		//
		// public fields
		//
	public:
		const unsigned int& d0;
		const unsigned int& d1;
		const unsigned int& d2;
		const unsigned int& d3;
		const unsigned int& d4;
		const unsigned int& d5;
		const unsigned int& d6;
		const unsigned int& d7;

		const unsigned int& a0;
		const unsigned int& a1;
		const unsigned int& a2;
		const unsigned int& a3;
		const unsigned int& a4;
		const unsigned int& a5;
		const unsigned int& a6;
		const unsigned int& a7;
		const StatusRegister& sr;
		const memory& mem;
	};
}