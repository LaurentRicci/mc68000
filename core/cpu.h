#pragma once
#include "core.h"
#include "memory.h"
#include "statusregister.h"

namespace mc68000
{
	class Cpu
	{
		//
		// Instruction handlers
		//
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

		using t_handler = uint16_t (Cpu::*)(uint16_t);
		friend t_handler* setup<Cpu>();

		t_handler* handlers;
		
		//
		// trap handlers
		// 
	private:
		using trapHandler_t = void (*)(uint32_t d0, uint32_t a0);
		trapHandler_t trapHandlers[16];
		trapHandler_t chkHandlers;
		//
		// internal datastructures
		// 
	private:
		enum BitOperation
		{
			BCLR,
			BSET,
			BCHG,
			BTST
		};
		//
		// internal helpers
		//
	private:
		template <typename T> T readAt(uint16_t ea, bool readModifyWrite);
		template <typename T> void writeAt(uint16_t ea, T data, bool readModifyWrite);
		template <typename T> void move(uint16_t from, uint16_t to);
		uint32_t getTargetAddress(uint16_t opcode);
		uint32_t getEffectiveAddress(uint16_t opcode);

		template <typename T> void logical(uint16_t srcEffectiveAdress, uint16_t dstEffectiveAdress, uint32_t(*op)(uint32_t, uint32_t));
		void logical(uint16_t opcode, uint32_t(*logicalOperator)(uint32_t, uint32_t));
		void logicalImmediate(uint16_t opcode, uint32_t(*logicalOperator)(uint32_t, uint32_t));

		template <typename T> void shiftLeft(uint16_t destinationRegister, uint32_t shift);
		template <typename T> void shiftRight(uint16_t destinationRegister, uint32_t shift, bool logical);
		uint16_t shiftLeftMemory(uint16_t opcode, uint16_t instruction);
		uint16_t shiftLeftRegister(uint16_t opcode, uint16_t instruction);
		uint16_t shiftRightRegister(uint16_t opcode, uint16_t instruction, bool logical);

		template <typename T> void add(uint16_t srcEffectiveAdress, uint16_t dstEffectiveAdress);
		template <typename T> void addq(uint32_t data, uint16_t dstEffectiveAdress);
		template <typename T> void addx(uint16_t source, uint16_t destination, bool useAddressRegister);
		template <typename T> void cmp(uint16_t srcEffectiveAdress, uint16_t dstEffectiveAdress);
		void bchg(uint16_t opcode, uint32_t bit, BitOperation operation);

		template <typename T> void neg(uint16_t effectiveAdress);
		template <typename T> void negx(uint16_t effectiveAdress);
		template <typename T> void not_(uint16_t effectiveAdress);

		typedef void (Cpu::* rotateFunction)(uint16_t, uint32_t);
		uint16_t rotate_register(uint16_t opcode, rotateFunction fn);

		template <typename T> void rotateLeft(uint16_t destinationRegister, uint32_t shift);
		template <typename T> void rotateRight(uint16_t destinationRegister, uint32_t shift);
		template <typename T> void rotateLeftWithExtend(uint16_t destinationRegister, uint32_t shift);
		template <typename T> void rotateRightWithExtend(uint16_t destinationRegister, uint32_t shift);

		template <typename T> void sub(uint16_t srcEffectiveAdress, uint16_t dstEffectiveAdress);
		template <typename T> void subq(uint32_t data, uint16_t destinationEffectiveAdress);
		template <typename T> void subx(uint16_t source, uint16_t destination, bool useAddressRegister);
		void handleException(uint16_t vector);
		//
		// private members
		//
	private:
		uint32_t dRegisters[8];
		uint32_t aRegisters[8];
		uint32_t usp;
		uint32_t ssp;
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
		void start(uint32_t startPc, uint32_t startSP = 0, uint32_t startUSP = 0);
		void setARegister(int reg, uint32_t value);
		void registerTrapHandler(int trapNumber, trapHandler_t traphandler);

		//
		// public fields
		//
	public:
		const uint32_t& d0;
		const uint32_t& d1;
		const uint32_t& d2;
		const uint32_t& d3;
		const uint32_t& d4;
		const uint32_t& d5;
		const uint32_t& d6;
		const uint32_t& d7;

		const uint32_t& a0;
		const uint32_t& a1;
		const uint32_t& a2;
		const uint32_t& a3;
		const uint32_t& a4;
		const uint32_t& a5;
		const uint32_t& a6;
		const uint32_t& a7;
		const StatusRegister& sr;
		const memory& mem;
	};
}