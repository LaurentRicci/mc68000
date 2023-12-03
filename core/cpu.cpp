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
		a7(this->operator*()->aRegisters[7]),
		sr(this->operator*()->sr),
		mem(this->operator*()->localMemory)
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
		done = false;
		pc = startPc;
		while (!done)
		{
			uint16_t x = localMemory.getWord(pc);
			pc += 2;
			(this->*core<cpu_t>::handlers[x])(x);
		}
	}

	// =================================================================================================
	// cpu instruction handlers
	// =================================================================================================

	unsigned short cpu_t::abcd(unsigned short opcode)
	{
		uint8_t register1 = opcode & 0b111;
		uint8_t register2 = (opcode >> 9) & 0b111;
		bool useAddressRegister = (opcode & 0b1000);

		uint8_t op1;
		uint8_t op2;
		if (useAddressRegister)
		{
			op1 = readAt<uint8_t>(0b100'000u | register1);
			op2 = readAt<uint8_t>(0b100'000u | register2);
		}
		else
		{
			op1 = dRegisters[register1];
			op2 = dRegisters[register2];
		}
		uint16_t m1 = (op1 & 0x0f) + (op2 & 0x0f);
		uint16_t m2 = (op1 & 0xf0) + (op2 & 0xf0);
		uint16_t result = m2 + m1;
		if (m1 >= 10)
		{
			result += 0x6; // in hexadecimal : 8 + 2 = A ; A + 6 = 10 ; in BCD 08 + 02 = 10
		}
		if ((result & 0xfff0) > 0x90)
		{
			sr.c = 1;
			sr.x = 1;
			result += 0x60;
		}
		else
		{
			sr.c = 0;
			sr.x = 0;
		}
		if (result != 0)
		{
			sr.z = 0;
		}
		if (useAddressRegister)
		{
			writeAt<uint8_t>(0b010'000u | register2, static_cast<uint8_t>(result));
		}
		else
		{
			dRegisters[register2] = (dRegisters[register2] & 0xffffff00) | (result & 0xff);
		}

		return instructions::ABCD;
	}

	unsigned short cpu_t::adda(unsigned short opcode)
	{
		uint8_t reg = (opcode >> 9) & 0b111;
		unsigned short sourceEffectiveAddress = opcode & 0b111111u;

		bool isLongOperation = (opcode & 0x100) == 0x100;
		if (isLongOperation)
		{
			uint32_t operand = readAt<uint32_t>(sourceEffectiveAddress);
			aRegisters[reg] += operand;
		}
		else
		{
			uint16_t operand = readAt<uint16_t>(sourceEffectiveAddress);
			uint32_t extended = (int16_t) operand;
			aRegisters[reg] += extended;
		}
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

	uint32_t cpu_t::getTargetAddress(uint16_t opcode)
	{
		uint32_t address = pc;
		int32_t offset;
		uint8_t byteOffset = opcode & 0xff;
		if (byteOffset != 0)
		{
			offset = (int8_t)byteOffset;
		}
		else
		{
			uint16_t extension = localMemory.get<uint16_t>(pc);
			pc += 2;
			offset = (int16_t)extension; // Displacements are always sign-extended to 32 bits prior to being used
		}
		address += offset;
		return address;
	}

	unsigned short cpu_t::bra(unsigned short opcode)
	{
		pc = getTargetAddress(opcode);
		return instructions::BRA;
	}
	unsigned short cpu_t::bhi(unsigned short opcode)
	{
		auto targetAddress = getTargetAddress(opcode);
		if (!sr.c && !sr.z)
		{
			pc = targetAddress;
		}
		return instructions::BHI;
	}
	unsigned short cpu_t::bls(unsigned short opcode)
	{
		auto targetAddress = getTargetAddress(opcode);
		if (sr.c | sr.z)
		{
			pc = targetAddress;
		}
		return instructions::BLS;
	}
	unsigned short cpu_t::bcc(unsigned short opcode)
	{
		auto targetAddress = getTargetAddress(opcode);
		if (!sr.c)
		{
			pc = targetAddress;
		}
		return instructions::BCC;
	}
	unsigned short cpu_t::bcs(unsigned short opcode)
	{
		auto targetAddress = getTargetAddress(opcode);
		if (sr.c)
		{
			pc = targetAddress;
		}
		return instructions::BCS;
	}
	unsigned short cpu_t::bne(unsigned short opcode)
	{
		auto targetAddress = getTargetAddress(opcode);
		if (!sr.z)
		{
			pc = targetAddress;
		}
		return instructions::BNE;
	}
	unsigned short cpu_t::beq(unsigned short opcode)
	{
		auto targetAddress = getTargetAddress(opcode);
		if (sr.z)
		{
			pc = targetAddress;
		}
		return instructions::BEQ;
	}
	unsigned short cpu_t::bvc(unsigned short opcode)
	{
		auto targetAddress = getTargetAddress(opcode);
		if (!sr.v)
		{
			pc = targetAddress;
		}
		return instructions::BVC;
	}
	unsigned short cpu_t::bvs(unsigned short opcode)
	{
		auto targetAddress = getTargetAddress(opcode);
		if (sr.v)
		{
			pc = targetAddress;
		}
		return instructions::BVS;
	}
	unsigned short cpu_t::bpl(unsigned short opcode)
	{
		auto targetAddress = getTargetAddress(opcode);
		if (!sr.n)
		{
			pc = targetAddress;
		}
		return instructions::BPL;
	}
	unsigned short cpu_t::bmi(unsigned short opcode)
	{
		auto targetAddress = getTargetAddress(opcode);
		if (sr.n)
		{
			pc = targetAddress;
		}
		return instructions::BMI;
	}
	unsigned short cpu_t::bge(unsigned short opcode)
	{
		auto targetAddress = getTargetAddress(opcode);
		if (!(sr.n & sr.v))
		{
			pc = targetAddress;
		}
		return instructions::BGE;
	}
	unsigned short cpu_t::blt(unsigned short opcode)
	{
		auto targetAddress = getTargetAddress(opcode);
		if (sr.n & sr.v)
		{
			pc = targetAddress;
		}
		return instructions::BLT;
	}
	unsigned short cpu_t::bgt(unsigned short opcode)
	{
		auto targetAddress = getTargetAddress(opcode);
		if (!(sr.z | (sr.n & sr.v)))
		{
			pc = targetAddress;
		}
		return instructions::BGT;
	}
	unsigned short cpu_t::ble(unsigned short opcode)
	{
		auto targetAddress = getTargetAddress(opcode);
		if (sr.z | (sr.n & sr.v))
		{
			pc = targetAddress;
		}
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

	template <typename T> T cpu_t::readAt(uint16_t ea)
	{
		unsigned short eam = ea >> 3;
		unsigned short reg = ea & 7u;
		switch (eam)
		{
		case 0:
			return static_cast<T>(dRegisters[reg]);
		case 1:
			return static_cast<T>(aRegisters[reg]);
		case 2:
		{
			uint32_t address = aRegisters[reg];
			T x = localMemory.get<T>(address);
			return x;
		}
		case 3:
		{
			uint32_t address = aRegisters[reg];
			T x = localMemory.get<T>(address);
			if (reg != 7)
			{
				aRegisters[reg] += sizeof(T);
			}
			else
			{
				// If the address register is the stack pointer and the operand size is byte, the address is incremented by two to keep the stack pointer aligned to a word boundary
				aRegisters[reg] += 1 ? 2 : sizeof(T);
			}
			return x;
		}
		case 4:
		{
			if (reg != 7)
			{
				aRegisters[reg] -= sizeof(T);
			}
			else
			{
				// If the address register is the stack pointer and the operand size is byte, the address is decremented by two to keep the stack pointer aligned to a word boundary
				aRegisters[reg] -= 1 ? 2 : sizeof(T);
			}
			uint32_t address = aRegisters[reg];
			T x = localMemory.get<T>(address);
			return x;
		}
		case 5:
		{
			uint32_t address = aRegisters[reg];

			uint16_t extension = localMemory.get<uint16_t>(pc);
			pc += 2;
			int32_t offset = (int16_t)extension; // Displacements are always sign-extended to 32 bits prior to being used

			T x = localMemory.get<T>(address+offset);
			return x;
		}
		case 6:
		{
			uint32_t address = aRegisters[reg];

			uint16_t extension = localMemory.get<uint16_t>(pc);
			pc += 2;

			// calculate the index
			bool isAddressRegister = extension & 0x8000;
			unsigned short extensionReg = (extension >> 12) & 7;
			bool isLongIndexSize = (extension & 0x0800);
			int32_t index;
			if (isLongIndexSize)
			{
				index = (isAddressRegister ? aRegisters[extensionReg] : dRegisters[extensionReg]);
			}
			else
			{
				index = (int16_t)((isAddressRegister ? aRegisters[extensionReg] : dRegisters[extensionReg]) & 0xffff);
			}

			// Calculate the displacement
			int32_t displacement = (int16_t) (extension & 0xff);

			T x = localMemory.get<T>(address + displacement + index);
			return x;
		}
		case 7:
		{
			switch (reg)
			{
				case 0:
				{
					uint16_t extension = localMemory.get<uint16_t>(pc);
					pc += 2;
					int32_t address = (int16_t)extension;

					T x = localMemory.get<T>(address);
					return x;
				}
				case 1:
				{
					uint32_t address = localMemory.get<uint32_t>(pc);
					pc += 4;
					T x = localMemory.get<T>(address);
					return x;
				}
				case 2:
				{
					uint32_t address = pc;
					uint16_t extension = localMemory.get<uint16_t>(pc);
					pc += 2;
					int32_t offset = (int16_t)extension;
					address += offset;

					T x = localMemory.get<T>(address);
					return x;
				}
				case 3:
				{
					uint32_t address = pc;

					uint16_t extension = localMemory.get<uint16_t>(pc);
					pc += 2;

					// calculate the index
					bool isAddressRegister = extension & 0x8000;
					unsigned short extensionReg = (extension >> 12) & 7;
					bool isLongIndexSize = (extension & 0x0800);
					int32_t index;
					if (isLongIndexSize)
					{
						index = (isAddressRegister ? aRegisters[extensionReg] : dRegisters[extensionReg]);
					}
					else
					{
						index = (int16_t)((isAddressRegister ? aRegisters[extensionReg] : dRegisters[extensionReg]) & 0xffff);
					}

					// Calculate the displacement
					int32_t displacement = (int16_t)(extension & 0xff);

					T x = localMemory.get<T>(address + displacement + index);
					return x;
				}
				case 4:
				{
					T x;
					if (sizeof(T) == 1)
					{
						x = localMemory.get<T>(pc + 1);
						return x;
					}
					else
					{
						x = localMemory.get<T>(pc);
					}
					pc += 2;
					return x;
				}
				default:
					break;
			}
			break;
		}
		default:
			break;
		}
		throw "readAt: incorrect addressing mode";
	}

	template <typename T> void cpu_t::writeAt(uint16_t  ea, T data)
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
		case 2:
		{
			uint32_t address = aRegisters[reg];
			localMemory.set<T>(address, data);
			break;
		}
		case 3:
		{
			uint32_t address = aRegisters[reg];
			localMemory.set<T>(address, data);
			if (reg != 7)
			{
				aRegisters[reg] += sizeof(T);
			}
			else
			{
				// If the address register is the stack pointer and the operand size is byte, the address is incremented by two to keep the stack pointer aligned to a word boundary
				aRegisters[reg] += 1 ? 2 : sizeof(T); 
			}
			break;
		}
		case 4:
		{
			if (reg != 7)
			{
				aRegisters[reg] -= sizeof(T);
			}
			else
			{
				// If the address register is the stack pointer and the operand size is byte, the address is decremented by two to keep the stack pointer aligned to a word boundary
				aRegisters[reg] -= 1 ? 2 : sizeof(T);
			}
			uint32_t address = aRegisters[reg];
			localMemory.set<T>(address, data);
			break;
		}
		case 5:
		{
			uint32_t address = aRegisters[reg];

			uint16_t extension = localMemory.get<T>(pc);
			pc += sizeof(T) == 1 ? 2 : sizeof(T); // pc must be aligned on a word boundary
			int32_t offset = (int16_t)extension;

			localMemory.set<T>(address + offset, data);
			break;
		}
		case 6:
		{
			uint32_t address = aRegisters[reg];

			uint16_t extension = localMemory.get<uint16_t>(pc);
			pc += 2;

			// calculate the index
			bool isAddressRegister = extension & 0x8000;
			unsigned short extensionReg = (extension >> 12) & 7;
			bool isLongIndexSize = (extension & 0x0800);
			int32_t index;
			if (isLongIndexSize)
			{
				index = (isAddressRegister ? aRegisters[extensionReg] : dRegisters[extensionReg]);
			}
			else
			{
				index = (int16_t)((isAddressRegister ? aRegisters[extensionReg] : dRegisters[extensionReg]) & 0xffff);
			}

			// Calculate the displacement
			int32_t displacement = (int16_t)(extension & 0xff);

			localMemory.set<T>(address + displacement + index, data);
			break;
		}
		case 7:
		{
			switch (reg)
			{
				case 0:
				{
					uint16_t extension = localMemory.get<uint16_t>(pc);
					pc += 2;
					int32_t address = (int16_t)extension;

					localMemory.set<T>(address, data);
					break;
				}
				case 1:
				{
					uint32_t address = localMemory.get<uint32_t>(pc);
					pc += 4;
					localMemory.set<T>(address, data);
					break;
				}
				case 2:
				case 3:
				case 4:
					throw "writeAt: non alterable addressing mode";
			}
			break;
		}
		default:
			throw "writeAt: incorrect addressing mode";
		}
	}

	template<typename T> void cpu_t::move(uint16_t sourceEffectiveAddress, uint16_t  destinationEffectiveAddress)
	{
		T source = readAt<T>(sourceEffectiveAddress);
		writeAt<T>(destinationEffectiveAddress, source);
		sr.c = 0;
		sr.v = 0;
		sr.z = source == 0 ? 1 : 0;
		sr.n = (source >> (sizeof(T) * 8 - 1)) ? 1 : 0;
	}

	unsigned short cpu_t::move(unsigned short opcode)
	{
		unsigned short size = opcode >> 12;
		unsigned short sourceEffectiveAddress = opcode & 0b111111u;

		// the destination is inverted: register - mode instead of mode - register
		unsigned short destination = (opcode >> 6) & 0b111111u;
		unsigned short destinationMode = destination & 0b111u;
		unsigned short destinationRegister = destination >> 3;
		unsigned short destinationEffectiveAddress = (destinationMode << 3) | destinationRegister;

		switch (size)
		{
		case 1:
			move<uint8_t>(sourceEffectiveAddress, destinationEffectiveAddress);
			break;
		case 3:
			move<uint16_t>(sourceEffectiveAddress, destinationEffectiveAddress);
			break;
		case 2:
			move<uint32_t>(sourceEffectiveAddress, destinationEffectiveAddress);
			break;
		default:
			throw "move invalid size";
		}

		return instructions::MOVE;
	}

	unsigned short cpu_t::movea(unsigned short opcode)
	{
		unsigned short size = opcode >> 12;
		unsigned short sourceEffectiveAddress = opcode & 0b111111u;

		// the destination is inverted: register - mode instead of mode - register
		uint16_t destinationRegister = (opcode >> 9) & 0b111u;
		uint16_t destinationEffectiveAddress = (1 << 3) | destinationRegister;

		switch (size)
		{
			case 3: // Word operation; the source operand is sign-extended to a long operand and all 32 bits are loaded into the address register.
			{
				uint16_t source = readAt<uint16_t>(sourceEffectiveAddress);
				int32_t extendedSource = (int16_t)source;
				writeAt<uint32_t>(destinationEffectiveAddress, extendedSource);
				break;
			}
			case 2:
			{
				uint32_t source = readAt<uint32_t>(sourceEffectiveAddress);
				writeAt<uint32_t>(destinationEffectiveAddress, source);
				break;
			}
			default:
				throw "movea invalid size";
		}

		return instructions::MOVEA;
	}

	unsigned short cpu_t::move2ccr(unsigned short)
	{
		uint16_t extension = localMemory.get<uint16_t>(pc);
		pc += 2;

		sr.c = extension & 0x1;
		sr.v = (extension & 0x2) >> 1;
		sr.z = (extension & 0x4) >> 2;
		sr.n = (extension & 0x8) >> 3;
		sr.x = (extension & 0x10) >> 4;

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

	unsigned short cpu_t::moveq(unsigned short opcode)
	{
		uint16_t reg = (opcode >> 9) & 0x07;
		int32_t data = (int8_t) (opcode & 0xff);

		dRegisters[reg] = data;

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
		done = true;
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
		throw "unknown instruction";
		return instructions::UNKNOWN;
	}

}