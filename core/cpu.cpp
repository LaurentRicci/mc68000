#include <iostream>
#include <cassert>

#include "core.h"
#include "instructions.h"
#include "cpu.h"
#include "exceptions.h"

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
		for (int i = 0; i < 16; trapHandlers[i++] = nullptr);
		chkHandlers = nullptr;
	}

	Cpu::~Cpu()
	{
		delete[] handlers;
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

	void Cpu::start(uint32_t startPc, uint32_t startSP, uint32_t startSSP)
	{
		done = false;
		pc = startPc;
		aRegisters[7] = startSP;
		usp = startSP;
		ssp = startSSP;

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

	void Cpu::registerTrapHandler(int trapNumber, trapHandler_t trapHandler)
	{
		if (trapNumber < 0 || trapNumber > 15)
		{
			throw "registerTrapHandler: invalid trapNumber";
		}
		trapHandlers[trapNumber] = trapHandler;
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

	// ==========
	// AND
	// ==========

	unsigned short Cpu::and_(unsigned short opcode)
	{
		logical(opcode, [](uint32_t lhs, uint32_t rhs) { return lhs & rhs; });
		return instructions::AND;
	}

	// ==========
	// ANDI
	// ==========

	unsigned short Cpu::andi(unsigned short opcode)
	{
		logicalImmediate(opcode, [](uint32_t lhs, uint32_t rhs) { return lhs & rhs; });
		return instructions::ANDI;
	}

	// ==========
	// ANDI to CCR
	// ==========
	unsigned short Cpu::andi2ccr(unsigned short opcode)
	{
		uint16_t sourceEffectiveAddress = 0b111'100;
		uint8_t source = readAt<uint8_t>(sourceEffectiveAddress);
		statusRegister.c &= (source & 1);
		statusRegister.v &= (source >> 1) & 1;
		statusRegister.z &= (source >> 2) & 1;
		statusRegister.n &= (source >> 3) & 1;
		statusRegister.x &= (source >> 4) & 1;

		return instructions::ANDI2CCR;
	}
	unsigned short Cpu::andi2sr(unsigned short)
	{
		return instructions::ANDI2SR;
	}
	// ==========
	// ASL
	// ==========
	unsigned short Cpu::asl_memory(unsigned short opcode)
	{
		return shiftLeftMemory(opcode, instructions::ASL);
	}

	unsigned short Cpu::asl_register(unsigned short opcode)
	{
		return shiftLeftRegister(opcode, instructions::ASL);
	}

	// ==========
	// ASR
	// ==========
	unsigned short Cpu::asr_memory(unsigned short opcode)
	{
		uint16_t effectiveAddress = opcode & 0b111'111;
		uint16_t memory = readAt<uint16_t>(effectiveAddress);
		uint16_t bit15 = memory & 0x8000;
		uint16_t bit0 = memory & 1;
		memory >>= 1;
		if (bit15)
		{
			memory |= 0x8000;
		}
		writeAt<uint16_t>(effectiveAddress, memory);
		statusRegister.c = statusRegister.x = bit0;
		return instructions::ASR;
	}

	unsigned short Cpu::asr_register(unsigned short opcode)
	{
		return shiftRightRegister(opcode, instructions::ASR, false);
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
		if (sr.hi())
		{
			pc = targetAddress;
		}
		return instructions::BHI;
	}

	unsigned short Cpu::bls(unsigned short opcode)
	{
		auto targetAddress = getTargetAddress(opcode);
		if (sr.ls())
		{
			pc = targetAddress;
		}
		return instructions::BLS;
	}

	unsigned short Cpu::bcc(unsigned short opcode)
	{
		auto targetAddress = getTargetAddress(opcode);
		if (sr.cc())
		{
			pc = targetAddress;
		}
		return instructions::BCC;
	}

	unsigned short Cpu::bcs(unsigned short opcode)
	{
		auto targetAddress = getTargetAddress(opcode);
		if (sr.cs())
		{
			pc = targetAddress;
		}
		return instructions::BCS;
	
	}
	unsigned short Cpu::bne(unsigned short opcode)
	{
		auto targetAddress = getTargetAddress(opcode);
		if (sr.ne())
		{
			pc = targetAddress;
		}
		return instructions::BNE;
	}

	unsigned short Cpu::beq(unsigned short opcode)
	{
		auto targetAddress = getTargetAddress(opcode);
		if (sr.eq())
		{
			pc = targetAddress;
		}
		return instructions::BEQ;
	}

	unsigned short Cpu::bvc(unsigned short opcode)
	{
		auto targetAddress = getTargetAddress(opcode);
		if (sr.vc())
		{
			pc = targetAddress;
		}
		return instructions::BVC;
	}

	unsigned short Cpu::bvs(unsigned short opcode)
	{
		auto targetAddress = getTargetAddress(opcode);
		if (sr.vs())
		{
			pc = targetAddress;
		}
		return instructions::BVS;
	}

	unsigned short Cpu::bpl(unsigned short opcode)
	{
		auto targetAddress = getTargetAddress(opcode);
		if (sr.pl())
		{
			pc = targetAddress;
		}
		return instructions::BPL;
	}

	unsigned short Cpu::bmi(unsigned short opcode)
	{
		auto targetAddress = getTargetAddress(opcode);
		if (sr.mi())
		{
			pc = targetAddress;
		}
		return instructions::BMI;
	}

	unsigned short Cpu::bge(unsigned short opcode)
	{
		auto targetAddress = getTargetAddress(opcode);
		if (sr.ge())
		{
			pc = targetAddress;
		}
		return instructions::BGE;
	}

	unsigned short Cpu::blt(unsigned short opcode)
	{
		auto targetAddress = getTargetAddress(opcode);
		if (sr.lt())
		{
			pc = targetAddress;
		}
		return instructions::BLT;
	}

	unsigned short Cpu::bgt(unsigned short opcode)
	{
		auto targetAddress = getTargetAddress(opcode);
		if (sr.gt())
		{
			pc = targetAddress;
		}
		return instructions::BGT;
	}

	unsigned short Cpu::ble(unsigned short opcode)
	{
		auto targetAddress = getTargetAddress(opcode);
		if (sr.le())
		{
			pc = targetAddress;
		}
		return instructions::BLE;
	}

	// ==========
	// BCHG
	// ==========
	void Cpu::bchg(uint16_t opcode, uint32_t bit, BitOperation operation)
	{
		uint8_t reg = (opcode >> 9) & 0b111;
		if ((opcode & 0b111'000) == 0)
		{
			// a data register is being used. The size is long.
			uint32_t bitToTest = 1 << (bit & 0x1f);
			uint32_t& data = dRegisters[opcode & 0b111];
			statusRegister.z = (data & bitToTest) == 0;
			switch (operation)
			{
				case BCLR: data &= ~bitToTest; break;
				case BSET: data |= bitToTest; break;
				case BCHG: data ^= bitToTest; break;
				case BTST: break;
			}
		}
		else
		{
			// a memory access is being used. The size is byte.
			uint8_t bitToTest = 1 << (bit & 0x7);
			uint8_t data;

			uint16_t mode = opcode & 0b111'000;
			if (mode == 0b011'000 || mode == 0b100'000)
			{
				uint16_t effectiveAddress = 0b010'000 | (opcode & 0b111);
				data = readAt<uint8_t>(effectiveAddress);
			}
			else
			{
				data = readAt<uint8_t>(opcode & 0b111'111);
			}
			statusRegister.z = (data & bitToTest) == 0;
			switch (operation)
			{
				case BCLR: data &= ~bitToTest; break;
				case BSET: data |= bitToTest; break;
				case BCHG: data ^= bitToTest; break;
				case BTST: break;
			}
			writeAt<uint8_t>(opcode & 0b111'111, data);
		}
	}

	unsigned short Cpu::bchg_r(unsigned short opcode)
	{
		uint8_t reg = (opcode >> 9) & 0b111;
		uint32_t bit = dRegisters[reg];
		bchg(opcode, bit,BCHG);
		return instructions::BCHG_R;
	}

	unsigned short Cpu::bchg_i(unsigned short opcode)
	{
		uint8_t bit = readAt<uint8_t>(0b111'100);
		bchg(opcode, bit, BCHG);
		return instructions::BCHG_I;
	}

	// ==========
	// BCLR
	// ==========
	unsigned short Cpu::bclr_r(unsigned short opcode)
	{
		uint8_t reg = (opcode >> 9) & 0b111;
		uint32_t bit = dRegisters[reg];
		bchg(opcode, bit, BCLR);
		return instructions::BCLR_R;
	}

	unsigned short Cpu::bclr_i(unsigned short opcode)
	{
		uint8_t bit = readAt<uint8_t>(0b111'100);
		bchg(opcode, bit, BCLR);
		return instructions::BCLR_I;
	}

	// ==========
	// BSET
	// ==========
	unsigned short Cpu::bset_r(unsigned short opcode)
	{
		uint8_t reg = (opcode >> 9) & 0b111;
		uint32_t bit = dRegisters[reg];
		bchg(opcode, bit, BSET);
		return instructions::BSET_R;
	}

	unsigned short Cpu::bset_i(unsigned short opcode)
	{
		uint8_t bit = readAt<uint8_t>(0b111'100);
		bchg(opcode, bit, BSET);
		return instructions::BSET_I;
	}

	// ==========
	// BSR
	// ==========
	unsigned short Cpu::bsr(unsigned short opcode)
	{
		uint32_t currentPc = pc;
		int32_t displacement;
		uint8_t offset = opcode & 0xff;
		if (offset)
		{
			displacement = (int8_t)offset;
		}
		else
		{
			uint16_t extension = localMemory.get<uint16_t>(pc);
			pc += 2;
			displacement = (int16_t)extension;
		}
		// The PC has been adjusted before being pushed onto the stack
		writeAt<uint32_t>(0b100'111, pc);

		pc = currentPc + displacement;
		return instructions::BSR;
	}

	// ==========
	// BTST
	// ==========
	unsigned short Cpu::btst_r(unsigned short opcode)
	{
		uint8_t reg = (opcode >> 9) & 0b111;
		uint32_t bit = dRegisters[reg];
		bchg(opcode, bit, BTST);
		return instructions::BTST_R;
	}

	unsigned short Cpu::btst_i(unsigned short opcode)
	{
		uint8_t bit = readAt<uint8_t>(0b111'100);
		bchg(opcode, bit, BTST);
		return instructions::BTST_I;
	}

	// ==========
	// CHK
	// ==========
	unsigned short Cpu::chk(unsigned short opcode)
	{
		uint8_t reg = (opcode >> 9) & 0b111;
		int16_t value = (int16_t)(dRegisters[reg] & 0xffff);
		uint16_t upperBound = readAt<uint16_t>(opcode & 0b111'111);
		if (value < 0 || value > upperBound)
		{
			// The condition codes are set according to the result of the comparison.
			statusRegister.n = (value < 0) ? 1 : 0;
			if (chkHandlers != nullptr)
			{
				chkHandlers(value, upperBound);
			}
			else
			{
				done = true;
			}
		}

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
		uint16_t destinationEffectiveAddress = opcode & 0b111'111;

		uint16_t size = (opcode >> 6) & 0b11;
		switch (size)
		{
			case 0:
				cmp<uint8_t>(sourceEffectiveAddress, destinationEffectiveAddress);
				break;
			case 1:
				cmp<uint16_t>(sourceEffectiveAddress, destinationEffectiveAddress);
				break;
			case 2:
				cmp<uint32_t>(sourceEffectiveAddress, destinationEffectiveAddress);
				break;
			default:
				throw "cmpi: invalid size";
		}

		return instructions::CMPI;
	}

	// ==========
	// CMPM
	// ==========
	unsigned short Cpu::cmpm(unsigned short opcode)
	{
		uint16_t rx = (opcode >> 9) & 0b111;
		uint16_t ry = opcode & 0b111;

		uint16_t sourceEffectiveAddress = 0b011'000 | ry;
		uint16_t destinationEffectiveAddress = 0b011'000 | rx;

		uint16_t size = (opcode >> 6) & 0b11;
		switch (size)
		{
		case 0:
			cmp<uint8_t>(sourceEffectiveAddress, destinationEffectiveAddress);
			break;
		case 1:
			cmp<uint16_t>(sourceEffectiveAddress, destinationEffectiveAddress);
			break;
		case 2:
			cmp<uint32_t>(sourceEffectiveAddress, destinationEffectiveAddress);
			break;
		default:
			assert(!"cmpm: invalid size" );
		}

		return instructions::CMPM;
	}

	// ==========
	// DBCC
	// ==========
	unsigned short Cpu::dbcc(unsigned short opcode)
	{
		// get the target address 
		uint32_t address = pc;
		uint16_t extension = localMemory.get<uint16_t>(pc);
		pc += 2;
		int32_t	offset = (int16_t)extension; // Displacements are always sign-extended to 32 bits prior to being used
		address += offset;

		// evaluate the condition
		uint16_t conditionCode = (opcode >> 8) & 0b1111;
		bool condition = sr.condition(conditionCode);
		if (!condition)
		{
			uint32_t& reg = dRegisters[opcode & 0b111];
			uint16_t regW = reg & 0xFFFF;
			regW--;
			reg = (reg & 0xffff0000) | regW;
			if (regW != 0xffff)
			{
				pc = address;
			}
		}
		return instructions::DBCC;
	}

	// ==========
	// DIVS
	// ==========
	unsigned short Cpu::divs(unsigned short opcode)
	{
		uint16_t source = readAt<uint16_t>(opcode & 0b111'111);
		uint8_t reg = (opcode >> 9) & 0b111;
		int32_t destination = dRegisters[reg];

		if (source == 0)
		{
			handleException(Exceptions::DIVISION_BY_ZERO);
		}
		else
		{
			int16_t divisor = static_cast<int16_t>(source);
			int32_t dividend = static_cast<int32_t>(destination);

			int32_t quotient = dividend / divisor;
			int16_t remainder = dividend % divisor;

			if (quotient > 0x7fff || quotient < -0x8000)
			{
				statusRegister.v = 1;
			}
			else
			{
				dRegisters[reg] = (remainder << 16) | (quotient & 0xffff);
				statusRegister.n = quotient < 0;
				statusRegister.z = quotient == 0;
				statusRegister.v = 0;
			}
			statusRegister.c = 0;
		}
		return instructions::DIVS;
	}

	// ==========
	// DIVU
	// ==========
	unsigned short Cpu::divu(unsigned short opcode)
	{
		uint16_t source = readAt<uint16_t>(opcode & 0b111'111);
		uint8_t reg = (opcode >> 9) & 0b111;
		int32_t destination = dRegisters[reg];

		if (source == 0)
		{
			handleException(Exceptions::DIVISION_BY_ZERO);
		}
		else
		{
			uint16_t divisor = (source);
			uint32_t dividend = (destination);

			uint32_t quotient = dividend / divisor;
			uint16_t remainder = dividend % divisor;

			if (quotient > 0xffff)
			{
				statusRegister.v = 1;
			}
			else
			{
				dRegisters[reg] = (remainder << 16) | (quotient & 0xffff);
				statusRegister.n = quotient < 0;
				statusRegister.z = quotient == 0;
				statusRegister.v = 0;
			}
			statusRegister.c = 0;
		}
		return instructions::DIVU;
	}

	// ==========
	// EOR
	// ==========
	unsigned short Cpu::eor(unsigned short opcode)
	{
		logical(opcode, [](uint32_t lhs, uint32_t rhs) { return lhs ^ rhs; });
		return instructions::EOR;
	}

	// ==========
	// EORI
	// ==========
	unsigned short Cpu::eori(unsigned short opcode)
	{
		logicalImmediate(opcode, [](uint32_t lhs, uint32_t rhs) { return lhs ^ rhs; });
		return instructions::EORI;
	}

	// ==========
	// EORI to CCR
	// ==========
	unsigned short Cpu::eori2ccr(unsigned short opcode)
	{
		uint16_t sourceEffectiveAddress = 0b111'100;
		uint8_t source = readAt<uint8_t>(sourceEffectiveAddress);
		statusRegister.c ^= (source & 1);
		statusRegister.v ^= (source >> 1) & 1;
		statusRegister.z ^= (source >> 2) & 1;
		statusRegister.n ^= (source >> 3) & 1;
		statusRegister.x ^= (source >> 4) & 1;

		return instructions::EORI2CCR;
	}

	// ==========
	// EXG
	// ==========
	unsigned short Cpu::exg(unsigned short opcode)
	{
		uint16_t mode = (opcode >> 3) & 0b11111;
		uint16_t regx = (opcode >> 9) & 0b111;
		uint16_t regy = opcode & 0b111;

		switch(mode)
		{
			case 0b01000:
			{
				uint32_t temp = dRegisters[regx];
				dRegisters[regx] = dRegisters[regy];
				dRegisters[regy] = temp;
				break;
			}
			case 0b01001:
			{
				uint32_t temp = aRegisters[regx];
				aRegisters[regx] = aRegisters[regy];
				aRegisters[regy] = temp;
				break;
			}
			case 0b10001:
			{
				uint32_t temp = dRegisters[regx];
				dRegisters[regx] = aRegisters[regy];
				aRegisters[regy] = temp;
				break;
			}
			default:
				throw "exg: invalid mode";
		}
		return instructions::EXG;
	}

	// ==========
	// EXT
	// ==========
	unsigned short Cpu::ext(unsigned short opcode)
	{
		uint16_t reg = opcode & 0b111;
		bool isLong = (opcode >> 6) & 0b1;

		if (isLong)
		{
			uint32_t v = dRegisters[reg] & 0xffff;
			if (v & 0x8000)
			{
				v |= 0xffff0000;
			}
			dRegisters[reg] = v;
		}
		else
		{
			uint16_t v = dRegisters[reg] & 0xff;
			if (v & 0b1000'000)
			{
				v |= 0xff00;
			}
			dRegisters[reg] = (dRegisters[reg] & 0xffff0000) | v;
		}
		return instructions::EXT;
	}

	// ==========
	// ILLEGAL
	// ==========
	unsigned short Cpu::illegal(unsigned short)
	{
		handleException(Exceptions::ILLEGAL_INSTRUCTION);
		return instructions::ILLEGAL;
	}

	// ==========
	// JMP
	// ==========
	unsigned short Cpu::jmp(unsigned short opcode)
	{
		uint32_t address = getEffectiveAddress(opcode);
		pc = address;

		return instructions::JMP;
	}

	// ==========
	// JSR
	// ==========
	unsigned short Cpu::jsr(unsigned short opcode)
	{
		uint32_t address = getEffectiveAddress(opcode);
		writeAt<uint32_t>(0b100'111, pc);
		pc = address;

		return instructions::JSR;
	}

	// ==========
	// LEA
	// ==========
	unsigned short Cpu::lea(unsigned short opcode)
	{
		uint32_t address = getEffectiveAddress(opcode);

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

	// ==========
	// LSL
	// ==========
	unsigned short Cpu::lsl_memory(unsigned short opcode)
	{
		return shiftLeftMemory(opcode, instructions::LSL);
	}

	unsigned short Cpu::lsl_register(unsigned short opcode)
	{
		return shiftLeftRegister(opcode, instructions::LSL);
	}

	// ==========
	// LSR
	// ==========
	unsigned short Cpu::lsr_memory(unsigned short opcode)
	{
		uint16_t effectiveAddress = opcode & 0b111'111;
		uint16_t memory = readAt<uint16_t>(effectiveAddress);
		uint16_t bit0 = memory & 1;
		memory >>= 1;
		memory &= 0x7fff;
		writeAt<uint16_t>(effectiveAddress, memory);
		statusRegister.c = statusRegister.x = bit0;
		return instructions::LSR;
	}

	unsigned short Cpu::lsr_register(unsigned short opcode)
	{
		return shiftRightRegister(opcode, instructions::LSR, true);
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
		unsigned short sourceEffectiveAddress = opcode & 0b111'111u;

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

	// ==========
	// MOVE from SR
	// ==========
	unsigned short Cpu::movesr(unsigned short opcode)
	{
		unsigned short effectiveAddress = opcode & 0b111'111u;
		writeAt<uint16_t>(effectiveAddress, sr);

		return instructions::MOVESR;
	}

	// ==========
	// MOVEM
	// ==========
	unsigned short Cpu::movem(unsigned short opcode)
	{
		uint16_t direction = (opcode >> 10) & 1;
		uint16_t size = (opcode >> 6) & 1;
		uint32_t effectiveAddress = getEffectiveAddress(opcode);
		uint16_t registerList = localMemory.get<uint16_t>(pc);
		pc += 2;

		if (direction == 0)
		{
			// Register to memory
			bool isPredecrement = (opcode & 0b111'000) == 0b100'000;
			if (isPredecrement)
			{
				// If the effective address is specified by the predecrement mode, The registers are stored 
				// starting at the specified address minus the operand length(2 or 4), and the address is 
				// decremented by the operand length following each transfer.The order of storing is 
				// from A7 to A0, then from D7 to D0.
				for (int i = 0; i < 8; i++)
				{
					if (registerList & (1 << i))
					{
						if (size == 0)
						{
							effectiveAddress -= 2;
							localMemory.set<uint16_t>(effectiveAddress, aRegisters[7-i]);
						}
						else
						{
							effectiveAddress -= 4;
							localMemory.set<uint32_t>(effectiveAddress, aRegisters[7-i]);
						}
					}
				}
				for (int i = 0; i < 8; i++)
				{
					if (registerList & (1 << (8+i)))
					{
						if (size == 0)
						{
							effectiveAddress -= 2;
							localMemory.set<uint16_t>(effectiveAddress, dRegisters[7-i]);
						}
						else
						{
							effectiveAddress -= 4;
							localMemory.set<uint32_t>(effectiveAddress, dRegisters[7-i]);
						}
					}
				}
				// When the instruction has completed, the decremented address register contains the address of 
				// the last operand stored.
				aRegisters[opcode & 0b111] = effectiveAddress; // TODO : check if this is correct
			}
			else
			{
				// If the effective address is specified by one of the control modes, the registers are 
				// transferred starting at the specified address, and the address is incremented by the 
				// operand length(2 or 4) following each transfer.The order of the registers is from D0 
				// to D7, then from A0 to A7.
				for (int i = 0; i < 8; i++)
				{
					if (registerList & (1 << i))
					{
						if (size == 0)
						{
							localMemory.set<uint16_t>(effectiveAddress, dRegisters[i]);
							//writeAt<uint16_t>(effectiveAddress, dRegisters[i]);
							effectiveAddress += 2;
						}
						else
						{
							localMemory.set<uint32_t>(effectiveAddress, dRegisters[i]);
							//writeAt<uint32_t>(effectiveAddress, dRegisters[i]);
							effectiveAddress += 4;
						}
					}
				}
				for (int i = 0; i < 8; i++)
				{
					if (registerList & (1 << (8 + i)))
					{
						if (size == 0)
						{
							localMemory.set<uint16_t>(effectiveAddress, aRegisters[i]);
							//writeAt<uint16_t>(effectiveAddress, aRegisters[i]);
							effectiveAddress += 2;
						}
						else
						{
							localMemory.set<uint32_t>(effectiveAddress, aRegisters[i]);
							//writeAt<uint32_t>(effectiveAddress, aRegisters[i]);
							effectiveAddress += 4;
						}
					}
				}
			}
		}
		else
		{
			// Memory to register
			for (int i = 0; i < 8; i++)
			{
				if (registerList & (1 << i))
				{
					if (size == 0)
					{
						dRegisters[i] = static_cast<int16_t>(localMemory.get<uint16_t>(effectiveAddress));
						effectiveAddress += 2;
					}
					else
					{
						dRegisters[i] = localMemory.get<uint32_t>(effectiveAddress);
						effectiveAddress += 4;
					}
				}
			}
			for (int i = 0; i < 8; i++)
			{
				if (registerList & (1 << i))
				{
					if (size == 0)
					{
						aRegisters[i] = static_cast<int16_t>(localMemory.get<uint16_t>(effectiveAddress));
						effectiveAddress += 2;
					}
					else
					{
						aRegisters[i] = localMemory.get<uint32_t>(effectiveAddress);
						effectiveAddress += 4;
					}
				}
			}
			bool isPostincrement = (opcode & 0b111'000) == 0b011'000;
			if (isPostincrement)
			{
				// When the instruction has completed, the incremented address register contains the address of 
				// the last operand loaded plus the operand length.If the addressing register is also loaded from
				// memory, the memory value is ignored and the register is written with the postincremented
				// effective address.
				aRegisters[opcode & 0b111] = effectiveAddress; // TODO : check if this is correct
			}
		}

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
		statusRegister.n = (data < 0);
		statusRegister.z = (data == 0);
		statusRegister.v = 0;
		statusRegister.c = 0;

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

	// ==========
	// OR
	// ==========

	unsigned short Cpu::or_(unsigned short opcode)
	{
		logical(opcode, [](uint32_t lhs, uint32_t rhs) { return lhs | rhs; });
		return instructions::OR;
	}

	// ==========
	// ORI
	// ==========

	unsigned short Cpu::ori(unsigned short opcode)
	{
		logicalImmediate(opcode, [](uint32_t lhs, uint32_t rhs) { return lhs | rhs; });
		return instructions::ORI;
	}

	// ==========
	// ORI to CCR
	// ==========
	unsigned short Cpu::ori2ccr(unsigned short)
	{
		uint16_t sourceEffectiveAddress = 0b111'100;
		uint8_t source = readAt<uint8_t>(sourceEffectiveAddress);
		statusRegister.c |= (source & 1);
		statusRegister.v |= (source >> 1) & 1;
		statusRegister.z |= (source >> 2) & 1;
		statusRegister.n |= (source >> 3) & 1;
		statusRegister.x |= (source >> 4) & 1;

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

	// ==========
	// RTS
	// ==========
	unsigned short Cpu::rts(unsigned short opcode)
	{
		pc = readAt<uint32_t>(0b011'111);
		return instructions::RTS;
	}

	unsigned short Cpu::scc(unsigned short)
	{
		return instructions::SCC;
	}

	// ==========
	// SUB
	// ==========
	unsigned short Cpu::sub(unsigned short opcode)
	{
		uint16_t sourceEffectiveAddress = opcode & 0b111'111;
		uint16_t destinationEffectiveAdress = (opcode >> 9) & 0b111;

		uint16_t size = (opcode >> 6) & 0b111;
		switch (size)
		{
		case 0:
			sub<uint8_t>(sourceEffectiveAddress, destinationEffectiveAdress);
			break;
		case 1:
			sub<uint16_t>(sourceEffectiveAddress, destinationEffectiveAdress);
			break;
		case 2:
			sub<uint32_t>(sourceEffectiveAddress, destinationEffectiveAdress);
			break;
		case 4:
			sub<uint8_t>(destinationEffectiveAdress, sourceEffectiveAddress);
			break;
		case 5:
			sub<uint16_t>(destinationEffectiveAdress, sourceEffectiveAddress);
			break;
		case 6:
			sub<uint32_t>(destinationEffectiveAdress, sourceEffectiveAddress);
			break;
		default:
			throw "sub: invalid size";
		}

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

	// ==========
	// TRAP
	// ==========
	unsigned short Cpu::trap(unsigned short opcode)
	{
		uint8_t trapNumber = opcode & 0b1111;
		handleException(Exceptions::TRAP + trapNumber);
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