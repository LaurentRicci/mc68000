// core.cpp
#include "noopcpu.h"
#include "core.h"
#include "instructions.h"

namespace mc68000
{

	unsigned short NoOpCpu::abcd(unsigned short)
	{
		return instructions::ABCD;
	}

	unsigned short NoOpCpu::add(unsigned short)
	{
		return instructions::ADD;
	}

	unsigned short NoOpCpu::adda(unsigned short)
	{
		return instructions::ADDA;
	}

	unsigned short NoOpCpu::addi(unsigned short)
	{
		return instructions::ADDI;
	}

	unsigned short NoOpCpu::addq(unsigned short)
	{
		return instructions::ADDQ;
	}

	unsigned short NoOpCpu::addx(unsigned short)
	{
		return instructions::ADDX;
	}

	unsigned short NoOpCpu::and_(unsigned short)
	{
		return instructions::AND;
	}

	unsigned short NoOpCpu::andi(unsigned short)
	{
		return instructions::ANDI;
	}

	unsigned short NoOpCpu::andi2ccr(unsigned short)
	{
		return instructions::ANDI2CCR;
	}
	unsigned short NoOpCpu::andi2sr(unsigned short)
	{
		return instructions::ANDI2SR;
	}
	unsigned short NoOpCpu::asl_memory(unsigned short)
	{
		return instructions::ASL;
	}

	unsigned short NoOpCpu::asl_register(unsigned short)
	{
		return instructions::ASL;
	}

	unsigned short NoOpCpu::asr_memory(unsigned short)
	{
		return instructions::ASR;
	}

	unsigned short NoOpCpu::asr_register(unsigned short)
	{
		return instructions::ASR;
	}

	unsigned short NoOpCpu::bra(unsigned short)
	{
		return instructions::BRA;
	}
	unsigned short NoOpCpu::bhi(unsigned short)
	{
		return instructions::BHI;
	}
	unsigned short NoOpCpu::bls(unsigned short)
	{
		return instructions::BLS;
	}
	unsigned short NoOpCpu::bcc(unsigned short)
	{
		return instructions::BCC;
	}
	unsigned short NoOpCpu::bcs(unsigned short)
	{
		return instructions::BCS;
	}
	unsigned short NoOpCpu::bne(unsigned short)
	{
		return instructions::BNE;
	}
	unsigned short NoOpCpu::beq(unsigned short)
	{
		return instructions::BEQ;
	}
	unsigned short NoOpCpu::bvc(unsigned short)
	{
		return instructions::BVC;
	}
	unsigned short NoOpCpu::bvs(unsigned short)
	{
		return instructions::BVS;
	}
	unsigned short NoOpCpu::bpl(unsigned short)
	{
		return instructions::BPL;
	}
	unsigned short NoOpCpu::bmi(unsigned short)
	{
		return instructions::BMI;
	}
	unsigned short NoOpCpu::bge(unsigned short)
	{
		return instructions::BGE;
	}
	unsigned short NoOpCpu::blt(unsigned short)
	{
		return instructions::BLT;
	}
	unsigned short NoOpCpu::bgt(unsigned short)
	{
		return instructions::BGT;
	}
	unsigned short NoOpCpu::ble(unsigned short)
	{
		return instructions::BLE;
	}

	unsigned short NoOpCpu::bchg_r(unsigned short)
	{
		return instructions::BCHG_R;
	}

	unsigned short NoOpCpu::bchg_i(unsigned short)
	{
		return instructions::BCHG_I;
	}

	unsigned short NoOpCpu::bclr_r(unsigned short)
	{
		return instructions::BCLR_R;
	}

	unsigned short NoOpCpu::bclr_i(unsigned short)
	{
		return instructions::BCLR_I;
	}

	unsigned short NoOpCpu::bset_r(unsigned short)
	{
		return instructions::BSET_R;
	}

	unsigned short NoOpCpu::bset_i(unsigned short)
	{
		return instructions::BSET_I;
	}

	unsigned short NoOpCpu::bsr(unsigned short)
	{
		return instructions::BSR;
	}

	unsigned short NoOpCpu::btst_r(unsigned short)
	{
		return instructions::BTST_R;
	}

	unsigned short NoOpCpu::btst_i(unsigned short)
	{
		return instructions::BTST_I;
	}

	unsigned short NoOpCpu::chk(unsigned short)
	{
		return instructions::CHK;
	}

	unsigned short NoOpCpu::clr(unsigned short)
	{
		return instructions::CLR;
	}


	unsigned short NoOpCpu::cmp(unsigned short)
	{
		return instructions::CMP;
	}

	unsigned short NoOpCpu::cmpa(unsigned short)
	{
		return instructions::CMPA;
	}

	unsigned short NoOpCpu::cmpi(unsigned short)
	{
		return instructions::CMPI;
	}

	unsigned short NoOpCpu::cmpm(unsigned short)
	{
		return instructions::CMPM;
	}

	unsigned short NoOpCpu::dbcc(unsigned short)
	{
		return instructions::DBCC;
	}

	unsigned short NoOpCpu::divs(unsigned short)
	{
		return instructions::DIVS;
	}

	unsigned short NoOpCpu::divu(unsigned short)
	{
		return instructions::DIVU;
	}

	unsigned short NoOpCpu::eor(unsigned short)
	{
		return instructions::EOR;
	}

	unsigned short NoOpCpu::eori(unsigned short)
	{
		return instructions::EORI;
	}

	unsigned short NoOpCpu::eori2ccr(unsigned short)
	{
		return instructions::EORI2CCR;
	}

	unsigned short NoOpCpu::exg(unsigned short)
	{
		return instructions::EXG;
	}

	unsigned short NoOpCpu::ext(unsigned short)
	{
		return instructions::EXT;
	}

	unsigned short NoOpCpu::illegal(unsigned short)
	{
		return instructions::ILLEGAL;
	}

	unsigned short NoOpCpu::jmp(unsigned short)
	{
		return instructions::JMP;
	}

	unsigned short NoOpCpu::jsr(unsigned short)
	{
		return instructions::JSR;
	}

	unsigned short NoOpCpu::lea(unsigned short)
	{
		return instructions::LEA;
	}

	unsigned short NoOpCpu::link(unsigned short)
	{
		return instructions::LINK;
	}

	unsigned short NoOpCpu::lsl_memory(unsigned short)
	{
		return instructions::LSL;
	}

	unsigned short NoOpCpu::lsl_register(unsigned short)
	{
		return instructions::LSL;
	}

	unsigned short NoOpCpu::lsr_memory(unsigned short)
	{
		return instructions::LSR;
	}

	unsigned short NoOpCpu::lsr_register(unsigned short)
	{
		return instructions::LSR;
	}

	unsigned short NoOpCpu::move(unsigned short)
	{
		return instructions::MOVE;
	}

	unsigned short NoOpCpu::movea(unsigned short)
	{
		return instructions::MOVEA;
	}

	unsigned short NoOpCpu::move2ccr(unsigned short)
	{
		return instructions::MOVE2CCR;
	}

	unsigned short NoOpCpu::movesr(unsigned short)
	{
		return instructions::MOVESR;
	}

	unsigned short NoOpCpu::movem(unsigned short)
	{
		return instructions::MOVEM;
	}

	unsigned short NoOpCpu::movep(unsigned short)
	{
		return instructions::MOVEP;
	}

	unsigned short NoOpCpu::moveq(unsigned short)
	{
		return instructions::MOVEQ;
	}

	unsigned short NoOpCpu::muls(unsigned short)
	{
		return instructions::MULS;
	}

	unsigned short NoOpCpu::mulu(unsigned short)
	{
		return instructions::MULU;
	}

	unsigned short NoOpCpu::nbcd(unsigned short)
	{
		return instructions::NBCD;
	}

	unsigned short NoOpCpu::neg(unsigned short)
	{
		return instructions::NEG;
	}
	unsigned short NoOpCpu::negx(unsigned short)
	{
		return instructions::NEGX;
	}
	unsigned short NoOpCpu::nop(unsigned short)
	{
		return instructions::NOP;
	}
	unsigned short NoOpCpu::not_(unsigned short)
	{
		return instructions::NOT;
	}

	unsigned short NoOpCpu::or_(unsigned short)
	{
		return instructions::OR;
	}

	unsigned short NoOpCpu::ori(unsigned short)
	{
		return instructions::ORI;
	}

	unsigned short NoOpCpu::ori2ccr(unsigned short)
	{
		return instructions::ORI2CCR;
	}

	unsigned short NoOpCpu::pea(unsigned short)
	{
		return instructions::PEA;
	}

	unsigned short NoOpCpu::rol_memory(unsigned short)
	{
		return instructions::ROL;
	}

	unsigned short NoOpCpu::ror_memory(unsigned short)
	{
		return instructions::ROR;
	}

	unsigned short NoOpCpu::roxl_memory(unsigned short)
	{
		return instructions::ROXL;
	}

	unsigned short NoOpCpu::roxr_memory(unsigned short)
	{
		return instructions::ROXR;
	}

	unsigned short NoOpCpu::rol_register(unsigned short)
	{
		return instructions::ROL;
	}

	unsigned short NoOpCpu::ror_register(unsigned short)
	{
		return instructions::ROR;
	}

	unsigned short NoOpCpu::roxl_register(unsigned short)
	{
		return instructions::ROXL;
	}

	unsigned short NoOpCpu::roxr_register(unsigned short)
	{
		return instructions::ROXR;
	}

	unsigned short NoOpCpu::sbcd(unsigned short)
	{
		return instructions::SBCD;
	}

	unsigned short NoOpCpu::rtr(unsigned short)
	{
		return instructions::RTR;
	}

	unsigned short NoOpCpu::rts(unsigned short)
	{
		return instructions::RTS;
	}

	unsigned short NoOpCpu::scc(unsigned short)
	{
		return instructions::SCC;
	}

	unsigned short NoOpCpu::sub(unsigned short)
	{
		return instructions::SUB;
	}

	unsigned short NoOpCpu::subi(unsigned short)
	{
		return instructions::SUBI;
	}

	unsigned short NoOpCpu::suba(unsigned short)
	{
		return instructions::SUBA;
	}

	unsigned short NoOpCpu::subq(unsigned short)
	{
		return instructions::SUBQ;
	}

	unsigned short NoOpCpu::subx(unsigned short)
	{
		return instructions::SUBX;
	}

	unsigned short NoOpCpu::swap(unsigned short)
	{
		return instructions::SWAP;
	}

	unsigned short NoOpCpu::tas(unsigned short)
	{
		return instructions::TAS;
	}

	unsigned short NoOpCpu::trap(unsigned short)
	{
		return instructions::TRAP;
	}

	unsigned short NoOpCpu::trapv(unsigned short)
	{
		return instructions::TRAPV;
	}

	unsigned short NoOpCpu::tst(unsigned short)
	{
		return instructions::TST;
	}

	unsigned short NoOpCpu::unlk(unsigned short)
	{
		return instructions::UNLK;
	}

	unsigned short NoOpCpu::unknown(unsigned short)
	{
		return instructions::UNKNOWN;
	}
}