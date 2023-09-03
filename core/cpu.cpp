#include "core.h"
#include "instructions.h"
#include "cpu.h"
#include <iostream>

namespace mc68000
{
	template<> core<cpu_t>::t_handler core<cpu_t>::handlers[] = {};
	template<> bool core<cpu_t>::initialized = false;

	cpu::cpu(const memory& memory) :
		d0(this->operator*()->dRegisters[0]),
		d1(this->operator*()->dRegisters[1]),
		d2(this->operator*()->dRegisters[2]),
		d3(this->operator*()->dRegisters[3]),
		d4(this->operator*()->dRegisters[4]),
		d5(this->operator*()->dRegisters[5]),
		d6(this->operator*()->dRegisters[6]),
		d7(this->operator*()->dRegisters[7]),

		a0(this->operator*()->aRegisters[0]),
		a1(this->operator*()->aRegisters[1]),
		a2(this->operator*()->aRegisters[2]),
		a3(this->operator*()->aRegisters[3]),
		a4(this->operator*()->aRegisters[4]),
		a5(this->operator*()->aRegisters[5]),
		a6(this->operator*()->aRegisters[6]),
		a7(this->operator*()->aRegisters[7])
	{
		this->operator*()->localMemory = memory;
	}

	void cpu_t::reset()
	{
		for (auto& dRegister : dRegisters)
			dRegister = 0;

		for (auto& aRegister : aRegisters)
			aRegister = 0;
		pc = 0;
	}

	void cpu_t::reset(const memory& memory)
	{
		reset();
		localMemory = memory;
	}

	void cpu_t::start(unsigned int startPc)
	{
		pc = startPc;
		uint16_t x = localMemory.getWord(pc);
		pc += 2;
		(this->*core<cpu_t>::handlers[x])(x);
	}

	// =================================================================================================
	// cpu instruction handlers
	// =================================================================================================

	unsigned short cpu_t::abcd(unsigned short)
	{
		return instructions::ABCD;
	}

	unsigned short cpu_t::adda(unsigned short)
	{
		return instructions::ADDA;
	}

	unsigned short cpu_t::add(unsigned short)
	{
		return instructions::ADD;
	}

	unsigned short cpu_t::addi(unsigned short opcode)
	{
		return instructions::ADDI;
	}

	unsigned short cpu_t::addq(unsigned short)
	{
		return instructions::ADDQ;
	}

	unsigned short cpu_t::addx(unsigned short)
	{
		return instructions::ADDX;
	}

	unsigned short cpu_t::and_(unsigned short)
	{
		return instructions::AND;
	}

	unsigned short cpu_t::andi(unsigned short)
	{
		return instructions::ANDI;
	}

	unsigned short cpu_t::andi2ccr(unsigned short)
	{
		return instructions::ANDI2CCR;
	}
	unsigned short cpu_t::andi2sr(unsigned short)
	{
		return instructions::ANDI2SR;
	}
	unsigned short cpu_t::asl_memory(unsigned short)
	{
		return instructions::ASL;
	}

	unsigned short cpu_t::asl_register(unsigned short)
	{
		return instructions::ASL;
	}

	unsigned short cpu_t::asr_memory(unsigned short)
	{
		return instructions::ASR;
	}

	unsigned short cpu_t::asr_register(unsigned short)
	{
		return instructions::ASR;
	}

	unsigned short cpu_t::bra(unsigned short)
	{
		return instructions::BRA;
	}
	unsigned short cpu_t::bhi(unsigned short opcode)
	{
		return instructions::BHI;
	}
	unsigned short cpu_t::bls(unsigned short opcode)
	{
		return instructions::BLS;
	}
	unsigned short cpu_t::bcc(unsigned short opcode)
	{
		return instructions::BCC;
	}
	unsigned short cpu_t::bcs(unsigned short opcode)
	{
		return instructions::BCS;
	}
	unsigned short cpu_t::bne(unsigned short opcode)
	{
		return instructions::BNE;
	}
	unsigned short cpu_t::beq(unsigned short opcode)
	{
		return instructions::BEQ;
	}
	unsigned short cpu_t::bvc(unsigned short opcode)
	{
		return instructions::BVC;
	}
	unsigned short cpu_t::bvs(unsigned short opcode)
	{
		return instructions::BVS;
	}
	unsigned short cpu_t::bpl(unsigned short opcode)
	{
		return instructions::BPL;
	}
	unsigned short cpu_t::bmi(unsigned short opcode)
	{
		return instructions::BMI;
	}
	unsigned short cpu_t::bge(unsigned short opcode)
	{
		return instructions::BGE;
	}
	unsigned short cpu_t::blt(unsigned short opcode)
	{
		return instructions::BLT;
	}
	unsigned short cpu_t::bgt(unsigned short opcode)
	{
		return instructions::BGT;
	}
	unsigned short cpu_t::ble(unsigned short opcode)
	{
		return instructions::BLE;
	}

	unsigned short cpu_t::bchg_r(unsigned short)
	{
		return instructions::BCHG_R;
	}

	unsigned short cpu_t::bchg_i(unsigned short)
	{
		return instructions::BCHG_I;
	}

	unsigned short cpu_t::bclr_r(unsigned short)
	{
		return instructions::BCLR_R;
	}

	unsigned short cpu_t::bclr_i(unsigned short)
	{
		return instructions::BCLR_I;
	}

	unsigned short cpu_t::bset_r(unsigned short)
	{
		return instructions::BSET_R;
	}

	unsigned short cpu_t::bset_i(unsigned short)
	{
		return instructions::BSET_I;
	}

	unsigned short cpu_t::bsr(unsigned short)
	{
		return instructions::BSR;
	}

	unsigned short cpu_t::btst_r(unsigned short)
	{
		return instructions::BTST_R;
	}

	unsigned short cpu_t::btst_i(unsigned short)
	{
		return instructions::BTST_I;
	}

	unsigned short cpu_t::chk(unsigned short)
	{
		return instructions::CHK;
	}

	unsigned short cpu_t::clr(unsigned short)
	{
		return instructions::CLR;
	}


	unsigned short cpu_t::cmp(unsigned short)
	{
		return instructions::CMP;
	}

	unsigned short cpu_t::cmpa(unsigned short)
	{
		return instructions::CMPA;
	}

	unsigned short cpu_t::cmpi(unsigned short opcode)
	{
		return instructions::CMPI;
	}

	unsigned short cpu_t::cmpm(unsigned short)
	{
		return instructions::CMPM;
	}

	unsigned short cpu_t::dbcc(unsigned short)
	{
		return instructions::DBCC;
	}

	unsigned short cpu_t::divs(unsigned short)
	{
		return instructions::DIVS;
	}

	unsigned short cpu_t::divu(unsigned short)
	{
		return instructions::DIVU;
	}

	unsigned short cpu_t::eor(unsigned short)
	{
		return instructions::EOR;
	}

	unsigned short cpu_t::eori(unsigned short)
	{
		return instructions::EORI;
	}

	unsigned short cpu_t::eori2ccr(unsigned short)
	{
		return instructions::EORI2CCR;
	}

	unsigned short cpu_t::exg(unsigned short)
	{
		return instructions::EXG;
	}

	unsigned short cpu_t::ext(unsigned short)
	{
		return instructions::EXT;
	}

	unsigned short cpu_t::illegal(unsigned short)
	{
		return instructions::ILLEGAL;
	}

	unsigned short cpu_t::jmp(unsigned short)
	{
		return instructions::JMP;
	}

	unsigned short cpu_t::jsr(unsigned short)
	{
		return instructions::JSR;
	}

	unsigned short cpu_t::lea(unsigned short)
	{
		return instructions::LEA;
	}

	unsigned short cpu_t::link(unsigned short opcode)
	{
		return instructions::LINK;
	}

	unsigned short cpu_t::lsl_memory(unsigned short)
	{
		return instructions::LSL;
	}

	unsigned short cpu_t::lsl_register(unsigned short)
	{
		return instructions::LSL;
	}

	unsigned short cpu_t::lsr_memory(unsigned short)
	{
		return instructions::LSR;
	}

	unsigned short cpu_t::lsr_register(unsigned short)
	{
		return instructions::LSR;
	}

	unsigned char cpu_t::read_b(unsigned short ea)
	{
		unsigned short eam = ea >> 3;
		unsigned short reg = ea & 7u;
		switch (eam)
		{
		case 0:
			return dRegisters[reg] & 0xff;
		case 1:
			return aRegisters[reg] & 0xff;
		case 7:
		{
			switch (reg)
			{
			case 4:
			{
				uint16_t x = localMemory.getWord(pc++);
				return x & 0xff;
			}
			default:
				break;
			}
			break;
		}
		default:
			break;
		}
		throw "incorrect addressing mode";
	}

	void cpu_t::write_b(unsigned short  ea, unsigned char data)
	{
		unsigned short eam = ea >> 3;
		unsigned short reg = ea & 7u;
		switch (eam)
		{
		case 0:
			dRegisters[reg] = data;
			break;
		case 1:
			aRegisters[reg] = data;
			break;
		default:
			break;
		}
	}

	void cpu_t::move_b(unsigned short sourceEffectiveAddress, unsigned short  destinationEffectiveAddress)
	{
		unsigned char source = read_b(sourceEffectiveAddress);
		write_b(destinationEffectiveAddress, source);
	}
	void cpu_t::move_w(unsigned short sourceEffectiveAddress, unsigned short  destinationEffectiveAddress)
	{

	}
	void cpu_t::move_l(unsigned short sourceEffectiveAddress, unsigned short  destinationEffectiveAddress)
	{

	}

	unsigned short cpu_t::move(unsigned short opcode)
	{
		unsigned short size = opcode >> 12;
		unsigned short sourceEffectiveAddress = opcode & 0b111111u;

		// the destination is inverted: register - mode instead of mode - register
		unsigned short destination = (opcode >> 6) & 0b111111u;
		unsigned short destinationRegister = destination & 0b111u;
		unsigned short destinationMode = destination >> 3;
		unsigned short destinationEffectiveAddress = (destinationRegister << 3) | destinationMode;

		switch (size)
		{
		case 1:
			move_b(sourceEffectiveAddress, destinationEffectiveAddress);
			break;
		case 2:
			move_w(sourceEffectiveAddress, destinationEffectiveAddress);
			break;
		case 3:
			move_l(sourceEffectiveAddress, destinationEffectiveAddress);
			break;
		default:
			throw "invalid size";
		}

		return instructions::MOVE;
	}

	unsigned short cpu_t::movea(unsigned short opcode)
	{
		return instructions::MOVEA;
	}

	unsigned short cpu_t::move2ccr(unsigned short)
	{
		return instructions::MOVE2CCR;
	}

	unsigned short cpu_t::movesr(unsigned short)
	{
		return instructions::MOVESR;
	}

	unsigned short cpu_t::movem(unsigned short)
	{
		return instructions::MOVEM;
	}

	unsigned short cpu_t::movep(unsigned short)
	{
		return instructions::MOVEP;
	}

	unsigned short cpu_t::moveq(unsigned short)
	{
		return instructions::MOVEQ;
	}

	unsigned short cpu_t::muls(unsigned short)
	{
		return instructions::MULS;
	}

	unsigned short cpu_t::mulu(unsigned short)
	{
		return instructions::MULU;
	}

	unsigned short cpu_t::nbcd(unsigned short)
	{
		return instructions::NBCD;
	}

	unsigned short cpu_t::neg(unsigned short)
	{
		return instructions::NEG;
	}
	unsigned short cpu_t::negx(unsigned short)
	{
		return instructions::NEGX;
	}
	unsigned short cpu_t::nop(unsigned short)
	{
		return instructions::NOP;
	}
	unsigned short cpu_t::not_(unsigned short)
	{
		return instructions::NOT;
	}

	unsigned short cpu_t::or_(unsigned short)
	{
		return instructions::OR;
	}

	unsigned short cpu_t::ori(unsigned short)
	{
		return instructions::ORI;
	}

	unsigned short cpu_t::ori2ccr(unsigned short)
	{
		return instructions::ORI2CCR;
	}

	unsigned short cpu_t::pea(unsigned short)
	{
		return instructions::PEA;
	}

	unsigned short cpu_t::rol_memory(unsigned short)
	{
		return instructions::ROL;
	}

	unsigned short cpu_t::ror_memory(unsigned short)
	{
		return instructions::ROR;
	}

	unsigned short cpu_t::roxl_memory(unsigned short)
	{
		return instructions::ROXL;
	}

	unsigned short cpu_t::roxr_memory(unsigned short)
	{
		return instructions::ROXR;
	}

	unsigned short cpu_t::rol_register(unsigned short)
	{
		return instructions::ROL;
	}

	unsigned short cpu_t::ror_register(unsigned short)
	{
		return instructions::ROR;
	}

	unsigned short cpu_t::roxl_register(unsigned short)
	{
		return instructions::ROXL;
	}

	unsigned short cpu_t::roxr_register(unsigned short)
	{
		return instructions::ROXR;
	}

	unsigned short cpu_t::sbcd(unsigned short)
	{
		return instructions::SBCD;
	}

	unsigned short cpu_t::rtr(unsigned short)
	{
		return instructions::RTR;
	}

	unsigned short cpu_t::rts(unsigned short)
	{
		return instructions::RTS;
	}

	unsigned short cpu_t::scc(unsigned short)
	{
		return instructions::SCC;
	}

	unsigned short cpu_t::sub(unsigned short)
	{
		return instructions::SUB;
	}

	unsigned short cpu_t::subi(unsigned short)
	{
		return instructions::SUBI;
	}

	unsigned short cpu_t::suba(unsigned short)
	{
		return instructions::SUBA;
	}

	unsigned short cpu_t::subq(unsigned short)
	{
		return instructions::SUBQ;
	}

	unsigned short cpu_t::subx(unsigned short)
	{
		return instructions::SUBX;
	}

	unsigned short cpu_t::swap(unsigned short)
	{
		return instructions::SWAP;
	}

	unsigned short cpu_t::tas(unsigned short)
	{
		return instructions::TAS;
	}

	unsigned short cpu_t::trap(unsigned short)
	{
		return instructions::TRAP;
	}

	unsigned short cpu_t::trapv(unsigned short)
	{
		return instructions::TRAPV;
	}

	unsigned short cpu_t::tst(unsigned short)
	{
		return instructions::TST;
	}

	unsigned short cpu_t::unlk(unsigned short opcode)
	{
		return instructions::UNLK;
	}

	unsigned short cpu_t::unknown(unsigned short)
	{
		return instructions::UNKNOWN;
	}

}