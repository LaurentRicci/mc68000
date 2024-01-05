#include "core.h"
#include "instructions.h"
#include "cpu.h"
#include <iostream>

namespace mc68000
{
	Cpu::Cpu(const memory& memory) :
		dRegisters{ 0 },
		aRegisters{ 0 },
		d0(dRegisters[0]),
		d1(dRegisters[1]),
		d2(dRegisters[2]),
		d3(dRegisters[3]),
		d4(dRegisters[4]),
		d5(dRegisters[5]),
		d6(dRegisters[6]),
		d7(dRegisters[7]),

		a0(aRegisters[0]),
		a1(aRegisters[1]),
		a2(aRegisters[2]),
		a3(aRegisters[3]),
		a4(aRegisters[4]),
		a5(aRegisters[5]),
		a6(aRegisters[6]),
		a7(aRegisters[7]),

		mem(localMemory),
		sr(statusRegister)
	{
		localMemory = memory;
		handlers = setup<Cpu>();
	}

	Cpu::~Cpu()
	{
		delete handlers;
	}

	void Cpu::reset()
	{
		for (auto& dRegister : dRegisters)
			dRegister = 0;

		for (auto& aRegister : aRegisters)
			aRegister = 0;
		pc = 0;
	}

	void Cpu::reset(const memory& memory)
	{
		reset();
		localMemory = memory;
	}

	void Cpu::start(uint32_t startPc, uint32_t startSP)
	{
		done = false;
		pc = startPc;
		aRegisters[7] = startSP;
		while (!done)
		{
			uint16_t x = localMemory.getWord(pc);
			pc += 2;
			(this->*handlers[x])(x);
		}
	}

	void Cpu::setARegister(int reg, uint32_t value)
	{
		aRegisters[reg] = value;
	}

	// =================================================================================================
	// cpu instruction handlers
	// =================================================================================================

	// ==========
	// ABCD
	// ==========

	unsigned short Cpu::abcd(unsigned short opcode)
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
			statusRegister.c = 1;
			statusRegister.x = 1;
			result += 0x60;
		}
		else
		{
			statusRegister.c = 0;
			statusRegister.x = 0;
		}
		if (result != 0)
		{
			statusRegister.z = 0;
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

	// ==========
	// ADDA
	// ==========
	unsigned short Cpu::adda(unsigned short opcode)
	{
		// The Condition Codes are unaffected so the template add<> method can't be used
		uint16_t sourceEffectiveAddress = opcode & 0b111'111u;
		uint16_t destinationRegister = (opcode >> 9) & 0b111;

		bool isLongOperation = (opcode & 0x100) == 0x100;
		if (isLongOperation)
		{
			uint32_t operand = readAt<uint32_t>(sourceEffectiveAddress);
			aRegisters[destinationRegister] += operand;
		}
		else
		{
			uint16_t operand = readAt<uint16_t>(sourceEffectiveAddress);
			uint32_t extended = (int16_t) operand;
			aRegisters[destinationRegister] += extended;
		}
		return instructions::ADDA;
	}

	// ==========
	// ADD
	// ==========
	unsigned short Cpu::add(unsigned short opcode)
	{
		uint16_t sourceEffectiveAddress = opcode & 0b111'111;
		uint16_t destinationEffectiveAdress = (opcode >> 9) & 0b111;

		uint16_t size = (opcode >> 6) & 0b111;
		switch (size)
		{
		case 0:
			add<uint8_t>(sourceEffectiveAddress, destinationEffectiveAdress);
			break;
		case 1:
			add<uint16_t>(sourceEffectiveAddress, destinationEffectiveAdress);
			break;
		case 2:
			add<uint32_t>(sourceEffectiveAddress, destinationEffectiveAdress);
			break;
		case 4:
			add<uint8_t>(destinationEffectiveAdress, sourceEffectiveAddress);
			break;
		case 5:
			add<uint16_t>(destinationEffectiveAdress, sourceEffectiveAddress);
			break;
		case 6:
			add<uint32_t>(destinationEffectiveAdress, sourceEffectiveAddress);
			break;
		default:
			throw "add: invalid size";
		}
		return instructions::ADD;
	}
	
	// ==========
	// ADDI
	// ==========
	unsigned short Cpu::addi(unsigned short opcode)
	{
		uint16_t sourceEffectiveAddress = 0b111'100;
		uint16_t destinationEffectiveAdress = opcode & 0b111'111;

		uint16_t size = (opcode >> 6) & 0b11;
		switch (size)
		{
			case 0:
			{
				add<uint8_t>(sourceEffectiveAddress, destinationEffectiveAdress);
				break;
			}
			case 1:
			{
				add<uint16_t>(sourceEffectiveAddress, destinationEffectiveAdress);
				break;
			}
			case 2:
			{
				add<uint32_t>(sourceEffectiveAddress, destinationEffectiveAdress);
				break;
			}
			default:
				throw "addi: invalid size";
		}

		return instructions::ADDI;
	}

	// ==========
	// ADDQ
	// ==========

	unsigned short Cpu::addq(unsigned short opcode)
	{
		uint16_t destinationEffectiveAdress = opcode & 0b111'111;
		bool isAddressRegister = (opcode & 0b111'000) == 0b001'000;
		uint16_t size = (opcode >> 6) & 0b11;
		uint32_t source = (opcode >> 9) & 0b111;

		if (isAddressRegister)
		{
			// When adding to address registers, the condition codes are not altered, and the entire destination address
			// register is used regardless of the operation size.
			if (size == 0)
			{
				throw "addq: invalid size";
			}
			addq<uint32_t>(source, destinationEffectiveAdress);
		}
		else
		{
			switch (size)
			{
				case 0:
					addq<uint8_t>(source, destinationEffectiveAdress);
					break;
				case 1:
					addq<uint16_t>(source, destinationEffectiveAdress);
					break;
				case 2:
					addq<uint32_t>(source, destinationEffectiveAdress);
					break;
				default:
					throw "addq: invalid size";
			}
		}
		return instructions::ADDQ;
	}

	unsigned short Cpu::addx(unsigned short)
	{
		return instructions::ADDX;
	}

	unsigned short Cpu::and_(unsigned short)
	{
		return instructions::AND;
	}

	unsigned short Cpu::andi(unsigned short)
	{
		return instructions::ANDI;
	}

	unsigned short Cpu::andi2ccr(unsigned short)
	{
		return instructions::ANDI2CCR;
	}
	unsigned short Cpu::andi2sr(unsigned short)
	{
		return instructions::ANDI2SR;
	}
	unsigned short Cpu::asl_memory(unsigned short)
	{
		return instructions::ASL;
	}

	unsigned short Cpu::asl_register(unsigned short)
	{
		return instructions::ASL;
	}

	unsigned short Cpu::asr_memory(unsigned short)
	{
		return instructions::ASR;
	}

	unsigned short Cpu::asr_register(unsigned short)
	{
		return instructions::ASR;
	}

	// ==========
	// Bcc
	// ==========
	unsigned short Cpu::bra(unsigned short opcode)
	{
		pc = getTargetAddress(opcode);
		return instructions::BRA;
	}

	unsigned short Cpu::bhi(unsigned short opcode)
	{
		auto targetAddress = getTargetAddress(opcode);
		if (!statusRegister.c && !statusRegister.z)
		{
			pc = targetAddress;
		}
		return instructions::BHI;
	}

	unsigned short Cpu::bls(unsigned short opcode)
	{
		auto targetAddress = getTargetAddress(opcode);
		if (statusRegister.c | statusRegister.z)
		{
			pc = targetAddress;
		}
		return instructions::BLS;
	}

	unsigned short Cpu::bcc(unsigned short opcode)
	{
		auto targetAddress = getTargetAddress(opcode);
		if (!statusRegister.c)
		{
			pc = targetAddress;
		}
		return instructions::BCC;
	}

	unsigned short Cpu::bcs(unsigned short opcode)
	{
		auto targetAddress = getTargetAddress(opcode);
		if (statusRegister.c)
		{
			pc = targetAddress;
		}
		return instructions::BCS;
	
	}
	unsigned short Cpu::bne(unsigned short opcode)
	{
		auto targetAddress = getTargetAddress(opcode);
		if (!statusRegister.z)
		{
			pc = targetAddress;
		}
		return instructions::BNE;
	}

	unsigned short Cpu::beq(unsigned short opcode)
	{
		auto targetAddress = getTargetAddress(opcode);
		if (statusRegister.z)
		{
			pc = targetAddress;
		}
		return instructions::BEQ;
	}

	unsigned short Cpu::bvc(unsigned short opcode)
	{
		auto targetAddress = getTargetAddress(opcode);
		if (!statusRegister.v)
		{
			pc = targetAddress;
		}
		return instructions::BVC;
	}

	unsigned short Cpu::bvs(unsigned short opcode)
	{
		auto targetAddress = getTargetAddress(opcode);
		if (statusRegister.v)
		{
			pc = targetAddress;
		}
		return instructions::BVS;
	}

	unsigned short Cpu::bpl(unsigned short opcode)
	{
		auto targetAddress = getTargetAddress(opcode);
		if (!statusRegister.n)
		{
			pc = targetAddress;
		}
		return instructions::BPL;
	}

	unsigned short Cpu::bmi(unsigned short opcode)
	{
		auto targetAddress = getTargetAddress(opcode);
		if (statusRegister.n)
		{
			pc = targetAddress;
		}
		return instructions::BMI;
	}

	unsigned short Cpu::bge(unsigned short opcode)
	{
		auto targetAddress = getTargetAddress(opcode);
		if (!(statusRegister.n & statusRegister.v))
		{
			pc = targetAddress;
		}
		return instructions::BGE;
	}

	unsigned short Cpu::blt(unsigned short opcode)
	{
		auto targetAddress = getTargetAddress(opcode);
		if (statusRegister.n & statusRegister.v)
		{
			pc = targetAddress;
		}
		return instructions::BLT;
	}

	unsigned short Cpu::bgt(unsigned short opcode)
	{
		auto targetAddress = getTargetAddress(opcode);
		if (!(statusRegister.z | (statusRegister.n & statusRegister.v)))
		{
			pc = targetAddress;
		}
		return instructions::BGT;
	}

	unsigned short Cpu::ble(unsigned short opcode)
	{
		auto targetAddress = getTargetAddress(opcode);
		if (statusRegister.z | (statusRegister.n & statusRegister.v))
		{
			pc = targetAddress;
		}
		return instructions::BLE;
	}

	unsigned short Cpu::bchg_r(unsigned short)
	{
		return instructions::BCHG_R;
	}

	unsigned short Cpu::bchg_i(unsigned short)
	{
		return instructions::BCHG_I;
	}

	unsigned short Cpu::bclr_r(unsigned short)
	{
		return instructions::BCLR_R;
	}

	unsigned short Cpu::bclr_i(unsigned short)
	{
		return instructions::BCLR_I;
	}

	unsigned short Cpu::bset_r(unsigned short)
	{
		return instructions::BSET_R;
	}

	unsigned short Cpu::bset_i(unsigned short)
	{
		return instructions::BSET_I;
	}

	unsigned short Cpu::bsr(unsigned short)
	{
		return instructions::BSR;
	}

	unsigned short Cpu::btst_r(unsigned short)
	{
		return instructions::BTST_R;
	}

	unsigned short Cpu::btst_i(unsigned short)
	{
		return instructions::BTST_I;
	}

	unsigned short Cpu::chk(unsigned short)
	{
		return instructions::CHK;
	}
	// ==========
	// CLR
	// ==========
	unsigned short Cpu::clr(unsigned short opcode)
	{
		uint16_t destinationEffectiveAddress = opcode & 0b111'111;

		uint16_t size = (opcode >> 6) & 0b11;
		switch (size)
		{
		case 0:
			writeAt<uint8_t>(destinationEffectiveAddress, 0);
			break;
		case 1:
			writeAt<uint16_t>(destinationEffectiveAddress, 0);
			break;
		case 2:
			writeAt<uint32_t>(destinationEffectiveAddress, 0);
			break;
		default:
			throw "clr: invalid size";
		}
		statusRegister.n = 0;
		statusRegister.z = 1;
		statusRegister.c = 0;
		statusRegister.v = 0;

		return instructions::CLR;
	}

	// ==========
	// CMP
	// ==========
	unsigned short Cpu::cmp(unsigned short opcode)
	{
		uint16_t sourceEffectiveAddress = opcode & 0b111'111; 
		uint16_t destinationEffectiveAdress = (opcode >> 9) & 0b111;

		uint16_t size = (opcode >> 6) & 0b11;
		switch (size)
		{
		case 0:
			cmp<uint8_t>(sourceEffectiveAddress, destinationEffectiveAdress);
			break;
		case 1:
			cmp<uint16_t>(sourceEffectiveAddress, destinationEffectiveAdress);
			break;
		case 2:
			cmp<uint32_t>(sourceEffectiveAddress, destinationEffectiveAdress);
			break;
		default:
			throw "cmp: invalid size";
		}

		return instructions::CMP;
	}

	// ==========
	// CMPA
	// ==========
	unsigned short Cpu::cmpa(unsigned short opcode)
	{
		uint16_t sourceEffectiveAddress = opcode & 0b111'111; 
		uint16_t destinationEffectiveAdress = ((opcode >> 9) & 0b111) | 0b1'000;

		uint16_t size = (opcode >> 6) & 0b11;
		switch (size)
		{
		case 3:
			cmp<uint16_t>(sourceEffectiveAddress, destinationEffectiveAdress);
			break;
		case 7:
			cmp<uint32_t>(sourceEffectiveAddress, destinationEffectiveAdress);
			break;
		default:
			throw "cmpa: invalid size";
		}

		return instructions::CMPA;
	}

	// ==========
	// CMPI
	// ==========
	unsigned short Cpu::cmpi(unsigned short opcode)
	{
		uint16_t sourceEffectiveAddress = 0b111'100;
		uint16_t destinationEffectiveAdress = opcode & 0b111'111;

		uint16_t size = (opcode >> 6) & 0b11;
		switch (size)
		{
			case 0:
				cmp<uint8_t>(sourceEffectiveAddress, destinationEffectiveAdress);
				break;
			case 1:
				cmp<uint16_t>(sourceEffectiveAddress, destinationEffectiveAdress);
				break;
			case 2:
				cmp<uint32_t>(sourceEffectiveAddress, destinationEffectiveAdress);
				break;
			default:
				throw "cmpi: invalid size";
		}

		return instructions::CMPI;
	}

	unsigned short Cpu::cmpm(unsigned short)
	{
		return instructions::CMPM;
	}

	unsigned short Cpu::dbcc(unsigned short)
	{
		return instructions::DBCC;
	}

	unsigned short Cpu::divs(unsigned short)
	{
		return instructions::DIVS;
	}

	unsigned short Cpu::divu(unsigned short)
	{
		return instructions::DIVU;
	}

	unsigned short Cpu::eor(unsigned short)
	{
		return instructions::EOR;
	}

	unsigned short Cpu::eori(unsigned short)
	{
		return instructions::EORI;
	}

	unsigned short Cpu::eori2ccr(unsigned short)
	{
		return instructions::EORI2CCR;
	}

	unsigned short Cpu::exg(unsigned short)
	{
		return instructions::EXG;
	}

	unsigned short Cpu::ext(unsigned short)
	{
		return instructions::EXT;
	}

	unsigned short Cpu::illegal(unsigned short)
	{
		return instructions::ILLEGAL;
	}

	unsigned short Cpu::jmp(unsigned short)
	{
		return instructions::JMP;
	}

	unsigned short Cpu::jsr(unsigned short)
	{
		return instructions::JSR;
	}

	// ==========
	// LEA
	// ==========
	unsigned short Cpu::lea(unsigned short opcode)
	{
		unsigned short eam = (opcode >> 3) & 0b111;
		unsigned short reg = opcode & 0b111;
		uint32_t address;

		switch (eam)
		{
			case 2:
			{
				address = aRegisters[reg];
				break;
			}
			case 5:
			{
				uint32_t baseAddress = aRegisters[reg];

				uint16_t extension = localMemory.get<uint16_t>(pc);
				pc += 2;
				int32_t offset = (int16_t)extension; // Displacements are always sign-extended to 32 bits prior to being used

				address = (baseAddress + offset);
				break;
			}
			case 6:
			{
				uint32_t baseAddress = aRegisters[reg];

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

				address = baseAddress + displacement + index;
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
						address = (int16_t)extension;
						break;
					}
					case 1:
					{
						address = localMemory.get<uint32_t>(pc);
						pc += 4;
						break;
					}
					case 2:
					{
						uint32_t baseAddress = pc;
						uint16_t extension = localMemory.get<uint16_t>(pc);
						pc += 2;
						int32_t offset = (int16_t)extension;
						address = baseAddress + offset;
						break;
					}
					case 3:
					{
						uint32_t baseAddress = pc;

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

						address = baseAddress + displacement + index;
						break;
					}
					default:
						throw "lea: incorrect addressing mode";
						break;
				}
				break;
			}
			default:
				throw "lea: incorrect addressing mode";
				break;
		}

		uint16_t destinationRegister = (opcode >> 9) & 0b111;
		aRegisters[destinationRegister] = address;

		return instructions::LEA;
	}

	// ==========
	// LINK
	// ==========
	unsigned short Cpu::link(unsigned short opcode)
	{
		uint8_t reg = opcode & 0b111;
		int16_t displacement = static_cast<int16_t>(localMemory.get<uint16_t>(pc));
		pc += 2;

		uint32_t& sp = aRegisters[7];
		sp -= 4;
		localMemory.set(sp, aRegisters[reg]);
		aRegisters[reg] = sp;
		sp += displacement;

		return instructions::LINK;
	}

	unsigned short Cpu::lsl_memory(unsigned short)
	{
		return instructions::LSL;
	}

	unsigned short Cpu::lsl_register(unsigned short)
	{
		return instructions::LSL;
	}

	unsigned short Cpu::lsr_memory(unsigned short)
	{
		return instructions::LSR;
	}

	unsigned short Cpu::lsr_register(unsigned short)
	{
		return instructions::LSR;
	}

	// ==========
	// MOVE
	// ==========
	unsigned short Cpu::move(unsigned short opcode)
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

	// ==========
	// MOVEA
	// ==========
	unsigned short Cpu::movea(unsigned short opcode)
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

	// ==========
	// MOVE to CCR
	// ==========
	unsigned short Cpu::move2ccr(unsigned short)
	{
		uint16_t extension = localMemory.get<uint16_t>(pc);
		pc += 2;

		statusRegister.c = extension & 0x1;
		statusRegister.v = (extension & 0x2) >> 1;
		statusRegister.z = (extension & 0x4) >> 2;
		statusRegister.n = (extension & 0x8) >> 3;
		statusRegister.x = (extension & 0x10) >> 4;

		return instructions::MOVE2CCR;
	}

	unsigned short Cpu::movesr(unsigned short)
	{
		return instructions::MOVESR;
	}

	unsigned short Cpu::movem(unsigned short)
	{
		return instructions::MOVEM;
	}

	unsigned short Cpu::movep(unsigned short)
	{
		return instructions::MOVEP;
	}

	// ==========
	// MOVEQ
	// ==========
	unsigned short Cpu::moveq(unsigned short opcode)
	{
		uint16_t reg = (opcode >> 9) & 0x07;
		int32_t data = (int8_t) (opcode & 0xff);

		dRegisters[reg] = data;

		return instructions::MOVEQ;
	}

	unsigned short Cpu::muls(unsigned short)
	{
		return instructions::MULS;
	}

	unsigned short Cpu::mulu(unsigned short)
	{
		return instructions::MULU;
	}

	unsigned short Cpu::nbcd(unsigned short)
	{
		return instructions::NBCD;
	}

	unsigned short Cpu::neg(unsigned short)
	{
		return instructions::NEG;
	}
	unsigned short Cpu::negx(unsigned short)
	{
		return instructions::NEGX;
	}
	unsigned short Cpu::nop(unsigned short)
	{
		return instructions::NOP;
	}
	unsigned short Cpu::not_(unsigned short)
	{
		return instructions::NOT;
	}

	unsigned short Cpu::or_(unsigned short)
	{
		return instructions::OR;
	}

	unsigned short Cpu::ori(unsigned short)
	{
		return instructions::ORI;
	}

	unsigned short Cpu::ori2ccr(unsigned short)
	{
		return instructions::ORI2CCR;
	}

	unsigned short Cpu::pea(unsigned short)
	{
		return instructions::PEA;
	}

	unsigned short Cpu::rol_memory(unsigned short)
	{
		return instructions::ROL;
	}

	unsigned short Cpu::ror_memory(unsigned short)
	{
		return instructions::ROR;
	}

	unsigned short Cpu::roxl_memory(unsigned short)
	{
		return instructions::ROXL;
	}

	unsigned short Cpu::roxr_memory(unsigned short)
	{
		return instructions::ROXR;
	}

	unsigned short Cpu::rol_register(unsigned short)
	{
		return instructions::ROL;
	}

	unsigned short Cpu::ror_register(unsigned short)
	{
		return instructions::ROR;
	}

	unsigned short Cpu::roxl_register(unsigned short)
	{
		return instructions::ROXL;
	}

	unsigned short Cpu::roxr_register(unsigned short)
	{
		return instructions::ROXR;
	}

	unsigned short Cpu::sbcd(unsigned short)
	{
		return instructions::SBCD;
	}

	unsigned short Cpu::rtr(unsigned short)
	{
		return instructions::RTR;
	}

	unsigned short Cpu::rts(unsigned short)
	{
		return instructions::RTS;
	}

	unsigned short Cpu::scc(unsigned short)
	{
		return instructions::SCC;
	}

	unsigned short Cpu::sub(unsigned short)
	{
		return instructions::SUB;
	}

	unsigned short Cpu::subi(unsigned short)
	{
		return instructions::SUBI;
	}

	unsigned short Cpu::suba(unsigned short)
	{
		return instructions::SUBA;
	}

	unsigned short Cpu::subq(unsigned short)
	{
		return instructions::SUBQ;
	}

	unsigned short Cpu::subx(unsigned short)
	{
		return instructions::SUBX;
	}

	unsigned short Cpu::swap(unsigned short)
	{
		return instructions::SWAP;
	}

	unsigned short Cpu::tas(unsigned short)
	{
		return instructions::TAS;
	}

	unsigned short Cpu::trap(unsigned short)
	{
		done = true;
		return instructions::TRAP;
	}

	unsigned short Cpu::trapv(unsigned short)
	{
		return instructions::TRAPV;
	}

	unsigned short Cpu::tst(unsigned short)
	{
		return instructions::TST;
	}

	unsigned short Cpu::unlk(unsigned short opcode)
	{
		return instructions::UNLK;
	}

	unsigned short Cpu::unknown(unsigned short)
	{
		throw "unknown instruction";
		return instructions::UNKNOWN;
	}

}