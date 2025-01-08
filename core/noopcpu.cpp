// core.cpp
#include "noopcpu.h"
#include "core.h"
#include "instructions.h"

namespace mc68000
{

	uint16_t NoOpCpu::abcd(uint16_t)
	{
		return instructions::ABCD;
	}

	uint16_t NoOpCpu::add(uint16_t)
	{
		return instructions::ADD;
	}

	uint16_t NoOpCpu::adda(uint16_t)
	{
		return instructions::ADDA;
	}

	uint16_t NoOpCpu::addi(uint16_t)
	{
		return instructions::ADDI;
	}

	uint16_t NoOpCpu::addq(uint16_t)
	{
		return instructions::ADDQ;
	}

	uint16_t NoOpCpu::addx(uint16_t)
	{
		return instructions::ADDX;
	}

	uint16_t NoOpCpu::and_(uint16_t)
	{
		return instructions::AND;
	}

	uint16_t NoOpCpu::andi(uint16_t)
	{
		return instructions::ANDI;
	}

	uint16_t NoOpCpu::andi2ccr(uint16_t)
	{
		return instructions::ANDI2CCR;
	}
	uint16_t NoOpCpu::andi2sr(uint16_t)
	{
		return instructions::ANDI2SR;
	}
	uint16_t NoOpCpu::asl_memory(uint16_t)
	{
		return instructions::ASL;
	}

	uint16_t NoOpCpu::asl_register(uint16_t)
	{
		return instructions::ASL;
	}

	uint16_t NoOpCpu::asr_memory(uint16_t)
	{
		return instructions::ASR;
	}

	uint16_t NoOpCpu::asr_register(uint16_t)
	{
		return instructions::ASR;
	}

	uint16_t NoOpCpu::bra(uint16_t)
	{
		return instructions::BRA;
	}
	uint16_t NoOpCpu::bhi(uint16_t)
	{
		return instructions::BHI;
	}
	uint16_t NoOpCpu::bls(uint16_t)
	{
		return instructions::BLS;
	}
	uint16_t NoOpCpu::bcc(uint16_t)
	{
		return instructions::BCC;
	}
	uint16_t NoOpCpu::bcs(uint16_t)
	{
		return instructions::BCS;
	}
	uint16_t NoOpCpu::bne(uint16_t)
	{
		return instructions::BNE;
	}
	uint16_t NoOpCpu::beq(uint16_t)
	{
		return instructions::BEQ;
	}
	uint16_t NoOpCpu::bvc(uint16_t)
	{
		return instructions::BVC;
	}
	uint16_t NoOpCpu::bvs(uint16_t)
	{
		return instructions::BVS;
	}
	uint16_t NoOpCpu::bpl(uint16_t)
	{
		return instructions::BPL;
	}
	uint16_t NoOpCpu::bmi(uint16_t)
	{
		return instructions::BMI;
	}
	uint16_t NoOpCpu::bge(uint16_t)
	{
		return instructions::BGE;
	}
	uint16_t NoOpCpu::blt(uint16_t)
	{
		return instructions::BLT;
	}
	uint16_t NoOpCpu::bgt(uint16_t)
	{
		return instructions::BGT;
	}
	uint16_t NoOpCpu::ble(uint16_t)
	{
		return instructions::BLE;
	}

	uint16_t NoOpCpu::bchg_r(uint16_t)
	{
		return instructions::BCHG_R;
	}

	uint16_t NoOpCpu::bchg_i(uint16_t)
	{
		return instructions::BCHG_I;
	}

	uint16_t NoOpCpu::bclr_r(uint16_t)
	{
		return instructions::BCLR_R;
	}

	uint16_t NoOpCpu::bclr_i(uint16_t)
	{
		return instructions::BCLR_I;
	}

	uint16_t NoOpCpu::bset_r(uint16_t)
	{
		return instructions::BSET_R;
	}

	uint16_t NoOpCpu::bset_i(uint16_t)
	{
		return instructions::BSET_I;
	}

	uint16_t NoOpCpu::bsr(uint16_t)
	{
		return instructions::BSR;
	}

	uint16_t NoOpCpu::btst_r(uint16_t)
	{
		return instructions::BTST_R;
	}

	uint16_t NoOpCpu::btst_i(uint16_t)
	{
		return instructions::BTST_I;
	}

	uint16_t NoOpCpu::chk(uint16_t)
	{
		return instructions::CHK;
	}

	uint16_t NoOpCpu::clr(uint16_t)
	{
		return instructions::CLR;
	}


	uint16_t NoOpCpu::cmp(uint16_t)
	{
		return instructions::CMP;
	}

	uint16_t NoOpCpu::cmpa(uint16_t)
	{
		return instructions::CMPA;
	}

	uint16_t NoOpCpu::cmpi(uint16_t)
	{
		return instructions::CMPI;
	}

	uint16_t NoOpCpu::cmpm(uint16_t)
	{
		return instructions::CMPM;
	}

	uint16_t NoOpCpu::dbcc(uint16_t)
	{
		return instructions::DBCC;
	}

	uint16_t NoOpCpu::divs(uint16_t)
	{
		return instructions::DIVS;
	}

	uint16_t NoOpCpu::divu(uint16_t)
	{
		return instructions::DIVU;
	}

	uint16_t NoOpCpu::eor(uint16_t)
	{
		return instructions::EOR;
	}

	uint16_t NoOpCpu::eori(uint16_t)
	{
		return instructions::EORI;
	}

	uint16_t NoOpCpu::eori2ccr(uint16_t)
	{
		return instructions::EORI2CCR;
	}

	uint16_t NoOpCpu::eori2sr(uint16_t)
	{
		return instructions::EORI2SR;
	}

	uint16_t NoOpCpu::exg(uint16_t)
	{
		return instructions::EXG;
	}

	uint16_t NoOpCpu::ext(uint16_t)
	{
		return instructions::EXT;
	}

	uint16_t NoOpCpu::illegal(uint16_t)
	{
		return instructions::ILLEGAL;
	}

	uint16_t NoOpCpu::jmp(uint16_t)
	{
		return instructions::JMP;
	}

	uint16_t NoOpCpu::jsr(uint16_t)
	{
		return instructions::JSR;
	}

	uint16_t NoOpCpu::lea(uint16_t)
	{
		return instructions::LEA;
	}

	uint16_t NoOpCpu::link(uint16_t)
	{
		return instructions::LINK;
	}

	uint16_t NoOpCpu::lsl_memory(uint16_t)
	{
		return instructions::LSL;
	}

	uint16_t NoOpCpu::lsl_register(uint16_t)
	{
		return instructions::LSL;
	}

	uint16_t NoOpCpu::lsr_memory(uint16_t)
	{
		return instructions::LSR;
	}

	uint16_t NoOpCpu::lsr_register(uint16_t)
	{
		return instructions::LSR;
	}

	uint16_t NoOpCpu::move(uint16_t)
	{
		return instructions::MOVE;
	}

	uint16_t NoOpCpu::movea(uint16_t)
	{
		return instructions::MOVEA;
	}

	uint16_t NoOpCpu::move2ccr(uint16_t)
	{
		return instructions::MOVE2CCR;
	}

	uint16_t NoOpCpu::movesr(uint16_t)
	{
		return instructions::MOVESR;
	}

	uint16_t NoOpCpu::move2sr(uint16_t)
	{
		return instructions::MOVE2SR;
	}

	uint16_t NoOpCpu::movem(uint16_t)
	{
		return instructions::MOVEM;
	}

	uint16_t NoOpCpu::movep(uint16_t)
	{
		return instructions::MOVEP;
	}

	uint16_t NoOpCpu::moveq(uint16_t)
	{
		return instructions::MOVEQ;
	}

	uint16_t NoOpCpu::muls(uint16_t)
	{
		return instructions::MULS;
	}

	uint16_t NoOpCpu::mulu(uint16_t)
	{
		return instructions::MULU;
	}

	uint16_t NoOpCpu::nbcd(uint16_t)
	{
		return instructions::NBCD;
	}

	uint16_t NoOpCpu::neg(uint16_t)
	{
		return instructions::NEG;
	}
	uint16_t NoOpCpu::negx(uint16_t)
	{
		return instructions::NEGX;
	}
	uint16_t NoOpCpu::nop(uint16_t)
	{
		return instructions::NOP;
	}
	uint16_t NoOpCpu::not_(uint16_t)
	{
		return instructions::NOT;
	}

	uint16_t NoOpCpu::or_(uint16_t)
	{
		return instructions::OR;
	}

	uint16_t NoOpCpu::ori(uint16_t)
	{
		return instructions::ORI;
	}

	uint16_t NoOpCpu::ori2ccr(uint16_t)
	{
		return instructions::ORI2CCR;
	}

	uint16_t NoOpCpu::ori2sr(uint16_t)
	{
		return instructions::ORI2SR;
	}

	uint16_t NoOpCpu::pea(uint16_t)
	{
		return instructions::PEA;
	}

	uint16_t NoOpCpu::rol_memory(uint16_t)
	{
		return instructions::ROL;
	}

	uint16_t NoOpCpu::ror_memory(uint16_t)
	{
		return instructions::ROR;
	}

	uint16_t NoOpCpu::roxl_memory(uint16_t)
	{
		return instructions::ROXL;
	}

	uint16_t NoOpCpu::roxr_memory(uint16_t)
	{
		return instructions::ROXR;
	}

	uint16_t NoOpCpu::rol_register(uint16_t)
	{
		return instructions::ROL;
	}

	uint16_t NoOpCpu::ror_register(uint16_t)
	{
		return instructions::ROR;
	}

	uint16_t NoOpCpu::roxl_register(uint16_t)
	{
		return instructions::ROXL;
	}

	uint16_t NoOpCpu::roxr_register(uint16_t)
	{
		return instructions::ROXR;
	}

	uint16_t NoOpCpu::rte(uint16_t)
	{
		return instructions::RTE;
	}

	uint16_t NoOpCpu::rtr(uint16_t)
	{
		return instructions::RTR;
	}

	uint16_t NoOpCpu::rts(uint16_t)
	{
		return instructions::RTS;
	}

	uint16_t NoOpCpu::sbcd(uint16_t)
	{
		return instructions::SBCD;
	}

	uint16_t NoOpCpu::scc(uint16_t)
	{
		return instructions::SCC;
	}

	uint16_t NoOpCpu::stop(uint16_t)
	{
		return instructions::STOP;
	}

	uint16_t NoOpCpu::sub(uint16_t)
	{
		return instructions::SUB;
	}

	uint16_t NoOpCpu::subi(uint16_t)
	{
		return instructions::SUBI;
	}

	uint16_t NoOpCpu::suba(uint16_t)
	{
		return instructions::SUBA;
	}

	uint16_t NoOpCpu::subq(uint16_t)
	{
		return instructions::SUBQ;
	}

	uint16_t NoOpCpu::subx(uint16_t)
	{
		return instructions::SUBX;
	}

	uint16_t NoOpCpu::swap(uint16_t)
	{
		return instructions::SWAP;
	}

	uint16_t NoOpCpu::tas(uint16_t)
	{
		return instructions::TAS;
	}

	uint16_t NoOpCpu::trap(uint16_t)
	{
		return instructions::TRAP;
	}

	uint16_t NoOpCpu::trapv(uint16_t)
	{
		return instructions::TRAPV;
	}

	uint16_t NoOpCpu::tst(uint16_t)
	{
		return instructions::TST;
	}

	uint16_t NoOpCpu::unlk(uint16_t)
	{
		return instructions::UNLK;
	}

	uint16_t NoOpCpu::unknown(uint16_t)
	{
		return instructions::UNKNOWN;
	}

}