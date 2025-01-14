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

	void Cpu::setSupervisorMode(bool super)
	{
		statusRegister.s = super ? 1 : 0;
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

	void Cpu::setDRegister(int reg, uint32_t value)
	{
		dRegisters[reg] = value;
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

	uint16_t Cpu::abcd(uint16_t opcode)
	{
		uint8_t register1 = opcode & 0b111;
		uint8_t register2 = (opcode >> 9) & 0b111;
		bool useAddressRegister = (opcode & 0b1000);

		uint8_t op1;
		uint8_t op2;
		if (useAddressRegister)
		{
			op1 = readAt<uint8_t>(0b100'000u | register1, false);
			op2 = readAt<uint8_t>(0b100'000u | register2, true);
		}
		else
		{
			op1 = dRegisters[register1];
			op2 = dRegisters[register2];
		}
		uint16_t m1 = (op1 & 0x0f) + (op2 & 0x0f) + statusRegister.x;
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
			writeAt<uint8_t>(0b010'000u | register2, static_cast<uint8_t>(result), true);
		}
		else
		{
			dRegisters[register2] = (dRegisters[register2] & 0xffffff00) | (result & 0xff);
		}

		return instructions::ABCD;
	}

	// ==========
	// ADD
	// ==========
	uint16_t Cpu::add(uint16_t opcode)
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
	// ADDA
	// ==========
	uint16_t Cpu::adda(uint16_t opcode)
	{
		// The Condition Codes are unaffected so the template add<> method can't be used
		uint16_t sourceEffectiveAddress = opcode & 0b111'111u;
		uint16_t destinationRegister = (opcode >> 9) & 0b111;

		bool isLongOperation = (opcode & 0x100) == 0x100;
		if (isLongOperation)
		{
			uint32_t operand = readAt<uint32_t>(sourceEffectiveAddress, false);
			aRegisters[destinationRegister] += operand;
		}
		else
		{
			uint16_t operand = readAt<uint16_t>(sourceEffectiveAddress, false);
			uint32_t extended = (int16_t)operand;
			aRegisters[destinationRegister] += extended;
		}
		return instructions::ADDA;
	}

	// ==========
	// ADDI
	// ==========
	uint16_t Cpu::addi(uint16_t opcode)
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

	uint16_t Cpu::addq(uint16_t opcode)
	{
			uint16_t destinationEffectiveAdress = opcode & 0b111'111;
			bool isAddressRegister = (opcode & 0b111'000) == 0b001'000;
			uint16_t size = (opcode >> 6) & 0b11;
			uint32_t source = (opcode >> 9) & 0b111;
			if (source == 0) source = 8;

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


	/// <summary>
	/// ADDX: Add Extended
	/// </summary>
	uint16_t Cpu::addx(uint16_t opcode)
	{
		uint16_t register1 = opcode & 0b111;
		uint16_t register2 = (opcode >> 9) & 0b111;
		bool useAddressRegister = (opcode & 0b1000);
		uint16_t size = (opcode >> 6) & 0b11;

		switch (size)
		{
		case 0:
			addx<uint8_t>(register1, register2, useAddressRegister);
			break;
		case 1:
			addx<uint16_t>(register1, register2, useAddressRegister);
			break;
		case 2:
			addx<uint32_t>(register1, register2, useAddressRegister);
			break;
		default:
			throw "addx: invalid size";
		}
		return instructions::ADDX;
	}

	// ==========
	// AND
	// ==========
	uint16_t Cpu::and_(uint16_t opcode)
	{
		logical(opcode, [](uint32_t lhs, uint32_t rhs) { return lhs & rhs; });
		return instructions::AND;
	}

	// ==========
	// ANDI
	// ==========
	uint16_t Cpu::andi(uint16_t opcode)
	{
		logicalImmediate(opcode, [](uint32_t lhs, uint32_t rhs) { return lhs & rhs; });
		return instructions::ANDI;
	}

	// ==========
	// ANDI to CCR
	// ==========
	uint16_t Cpu::andi2ccr(uint16_t opcode)
	{
		uint16_t sourceEffectiveAddress = 0b111'100;
		uint8_t source = readAt<uint8_t>(sourceEffectiveAddress, false);
		statusRegister.c &= (source & 1);
		statusRegister.v &= (source >> 1) & 1;
		statusRegister.z &= (source >> 2) & 1;
		statusRegister.n &= (source >> 3) & 1;
		statusRegister.x &= (source >> 4) & 1;

		return instructions::ANDI2CCR;
	}

	/// <summary>
	/// ANDI to SR
	/// </summary>
	uint16_t Cpu::andi2sr(uint16_t)
	{
		if (sr.s)
		{
			uint16_t data = readAt<uint16_t>(0b111'100, false);
			uint16_t sr = statusRegister;
			statusRegister = (uint16_t)(sr & data);
		}
		else
		{
			handleException(Exceptions::PRIVILEGE_VIOLATION);
		}
		return instructions::ANDI2SR;
	}
	// ==========
	// ASL
	// ==========
	uint16_t Cpu::asl_memory(uint16_t opcode)
	{
		return shiftLeftMemory(opcode, instructions::ASL);
	}

	uint16_t Cpu::asl_register(uint16_t opcode)
	{
		return shiftLeftRegister(opcode, instructions::ASL);
	}

	// ==========
	// ASR
	// ==========
	uint16_t Cpu::asr_memory(uint16_t opcode)
	{
		uint16_t effectiveAddress = opcode & 0b111'111;
		uint16_t memory = readAt<uint16_t>(effectiveAddress, true);
		uint16_t bit15 = memory & 0x8000;
		uint16_t bit0 = memory & 1;
		memory >>= 1;
		if (bit15)
		{
			memory |= 0x8000;
		}
		writeAt<uint16_t>(effectiveAddress, memory, true);
		statusRegister.c = statusRegister.x = bit0;
		return instructions::ASR;
	}

	uint16_t Cpu::asr_register(uint16_t opcode)
	{
		return shiftRightRegister(opcode, instructions::ASR, false);
	}

	// ==========
	// Bcc
	// ==========
	uint16_t Cpu::bra(uint16_t opcode)
	{
		pc = getTargetAddress(opcode);
		return instructions::BRA;
	}

	uint16_t Cpu::bhi(uint16_t opcode)
	{
		auto targetAddress = getTargetAddress(opcode);
		if (sr.hi())
		{
			pc = targetAddress;
		}
		return instructions::BHI;
	}

	uint16_t Cpu::bls(uint16_t opcode)
	{
		auto targetAddress = getTargetAddress(opcode);
		if (sr.ls())
		{
			pc = targetAddress;
		}
		return instructions::BLS;
	}

	uint16_t Cpu::bcc(uint16_t opcode)
	{
		auto targetAddress = getTargetAddress(opcode);
		if (sr.cc())
		{
			pc = targetAddress;
		}
		return instructions::BCC;
	}

	uint16_t Cpu::bcs(uint16_t opcode)
	{
		auto targetAddress = getTargetAddress(opcode);
		if (sr.cs())
		{
			pc = targetAddress;
		}
		return instructions::BCS;
	
	}
	uint16_t Cpu::bne(uint16_t opcode)
	{
		auto targetAddress = getTargetAddress(opcode);
		if (sr.ne())
		{
			pc = targetAddress;
		}
		return instructions::BNE;
	}

	uint16_t Cpu::beq(uint16_t opcode)
	{
		auto targetAddress = getTargetAddress(opcode);
		if (sr.eq())
		{
			pc = targetAddress;
		}
		return instructions::BEQ;
	}

	uint16_t Cpu::bvc(uint16_t opcode)
	{
		auto targetAddress = getTargetAddress(opcode);
		if (sr.vc())
		{
			pc = targetAddress;
		}
		return instructions::BVC;
	}

	uint16_t Cpu::bvs(uint16_t opcode)
	{
		auto targetAddress = getTargetAddress(opcode);
		if (sr.vs())
		{
			pc = targetAddress;
		}
		return instructions::BVS;
	}

	uint16_t Cpu::bpl(uint16_t opcode)
	{
		auto targetAddress = getTargetAddress(opcode);
		if (sr.pl())
		{
			pc = targetAddress;
		}
		return instructions::BPL;
	}

	uint16_t Cpu::bmi(uint16_t opcode)
	{
		auto targetAddress = getTargetAddress(opcode);
		if (sr.mi())
		{
			pc = targetAddress;
		}
		return instructions::BMI;
	}

	uint16_t Cpu::bge(uint16_t opcode)
	{
		auto targetAddress = getTargetAddress(opcode);
		if (sr.ge())
		{
			pc = targetAddress;
		}
		return instructions::BGE;
	}

	uint16_t Cpu::blt(uint16_t opcode)
	{
		auto targetAddress = getTargetAddress(opcode);
		if (sr.lt())
		{
			pc = targetAddress;
		}
		return instructions::BLT;
	}

	uint16_t Cpu::bgt(uint16_t opcode)
	{
		auto targetAddress = getTargetAddress(opcode);
		if (sr.gt())
		{
			pc = targetAddress;
		}
		return instructions::BGT;
	}

	uint16_t Cpu::ble(uint16_t opcode)
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
			uint8_t data = readAt<uint8_t>(opcode & 0b111'111, true);
			statusRegister.z = (data & bitToTest) == 0;
			switch (operation)
			{
				case BCLR: data &= ~bitToTest; break;
				case BSET: data |= bitToTest; break;
				case BCHG: data ^= bitToTest; break;
				case BTST: break;
			}
			writeAt<uint8_t>(opcode & 0b111'111, data, true);
		}
	}

	uint16_t Cpu::bchg_r(uint16_t opcode)
	{
		uint8_t reg = (opcode >> 9) & 0b111;
		uint32_t bit = dRegisters[reg];
		bchg(opcode, bit,BCHG);
		return instructions::BCHG_R;
	}

	uint16_t Cpu::bchg_i(uint16_t opcode)
	{
		uint8_t bit = readAt<uint8_t>(0b111'100, false);
		bchg(opcode, bit, BCHG);
		return instructions::BCHG_I;
	}

	// ==========
	// BCLR
	// ==========
	uint16_t Cpu::bclr_r(uint16_t opcode)
	{
		uint8_t reg = (opcode >> 9) & 0b111;
		uint32_t bit = dRegisters[reg];
		bchg(opcode, bit, BCLR);
		return instructions::BCLR_R;
	}

	uint16_t Cpu::bclr_i(uint16_t opcode)
	{
		uint8_t bit = readAt<uint8_t>(0b111'100, false);
		bchg(opcode, bit, BCLR);
		return instructions::BCLR_I;
	}

	// ==========
	// BSET
	// ==========
	uint16_t Cpu::bset_r(uint16_t opcode)
	{
		uint8_t reg = (opcode >> 9) & 0b111;
		uint32_t bit = dRegisters[reg];
		bchg(opcode, bit, BSET);
		return instructions::BSET_R;
	}

	uint16_t Cpu::bset_i(uint16_t opcode)
	{
		uint8_t bit = readAt<uint8_t>(0b111'100, false);
		bchg(opcode, bit, BSET);
		return instructions::BSET_I;
	}

	// ==========
	// BSR
	// ==========
	uint16_t Cpu::bsr(uint16_t opcode)
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
		writeAt<uint32_t>(0b100'111, pc, false);

		pc = currentPc + displacement;
		return instructions::BSR;
	}

	// ==========
	// BTST
	// ==========
	uint16_t Cpu::btst_r(uint16_t opcode)
	{
		uint8_t reg = (opcode >> 9) & 0b111;
		uint32_t bit = dRegisters[reg];
		bchg(opcode, bit, BTST);
		return instructions::BTST_R;
	}

	uint16_t Cpu::btst_i(uint16_t opcode)
	{
		uint8_t bit = readAt<uint8_t>(0b111'100, false);
		bchg(opcode, bit, BTST);
		return instructions::BTST_I;
	}

	// ==========
	// CHK
	// ==========
	uint16_t Cpu::chk(uint16_t opcode)
	{
		uint8_t reg = (opcode >> 9) & 0b111;
		int16_t value = (int16_t)(dRegisters[reg] & 0xffff);
		uint16_t upperBound = readAt<uint16_t>(opcode & 0b111'111, false);
		if (value < 0 || value > upperBound)
		{
			// The condition codes are set according to the result of the comparison.
			statusRegister.n = (value < 0) ? 1 : 0;
			if (chkHandlers != nullptr)
			{
				chkHandlers(value, upperBound, 0, 0);
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
	uint16_t Cpu::clr(uint16_t opcode)
	{
		uint16_t destinationEffectiveAddress = opcode & 0b111'111;

		uint16_t size = (opcode >> 6) & 0b11;
		switch (size)
		{
		case 0:
			writeAt<uint8_t>(destinationEffectiveAddress, 0, false);
			break;
		case 1:
			writeAt<uint16_t>(destinationEffectiveAddress, 0, false);
			break;
		case 2:
			writeAt<uint32_t>(destinationEffectiveAddress, 0, false);
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
	uint16_t Cpu::cmp(uint16_t opcode)
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
	uint16_t Cpu::cmpa(uint16_t opcode)
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
	uint16_t Cpu::cmpi(uint16_t opcode)
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
	uint16_t Cpu::cmpm(uint16_t opcode)
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
	uint16_t Cpu::dbcc(uint16_t opcode)
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
	uint16_t Cpu::divs(uint16_t opcode)
	{
		uint16_t source = readAt<uint16_t>(opcode & 0b111'111, false);
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
	uint16_t Cpu::divu(uint16_t opcode)
	{
		uint16_t source = readAt<uint16_t>(opcode & 0b111'111, false);
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
	uint16_t Cpu::eor(uint16_t opcode)
	{
		logical(opcode, [](uint32_t lhs, uint32_t rhs) { return lhs ^ rhs; });
		return instructions::EOR;
	}

	// ==========
	// EORI
	// ==========
	uint16_t Cpu::eori(uint16_t opcode)
	{
		logicalImmediate(opcode, [](uint32_t lhs, uint32_t rhs) { return lhs ^ rhs; });
		return instructions::EORI;
	}

	// ==========
	// EORI to CCR
	// ==========
	uint16_t Cpu::eori2ccr(uint16_t opcode)
	{
		uint16_t sourceEffectiveAddress = 0b111'100;
		uint8_t source = readAt<uint8_t>(sourceEffectiveAddress, false);
		statusRegister.c ^= (source & 1);
		statusRegister.v ^= (source >> 1) & 1;
		statusRegister.z ^= (source >> 2) & 1;
		statusRegister.n ^= (source >> 3) & 1;
		statusRegister.x ^= (source >> 4) & 1;

		return instructions::EORI2CCR;
	}

	/// <summary>
	/// EORI to SR: Exclusive OR Immediate to Status Register
	/// </summary>
	uint16_t Cpu::eori2sr(uint16_t opcode)
	{
		if (sr.s)
		{
			uint16_t data = readAt<uint16_t>(0b111'100, false);
			uint16_t sr = statusRegister;
			statusRegister = (uint16_t)(sr ^ data);
		}
		else
		{
			handleException(Exceptions::PRIVILEGE_VIOLATION);
		}
		return instructions::EORI2SR;
	}

	// ==========
	// EXG
	// ==========
	uint16_t Cpu::exg(uint16_t opcode)
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
	uint16_t Cpu::ext(uint16_t opcode)
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
	uint16_t Cpu::illegal(uint16_t)
	{
		handleException(Exceptions::ILLEGAL_INSTRUCTION);
		return instructions::ILLEGAL;
	}

	// ==========
	// JMP
	// ==========
	uint16_t Cpu::jmp(uint16_t opcode)
	{
		uint32_t address = getEffectiveAddress(opcode);
		pc = address;

		return instructions::JMP;
	}

	// ==========
	// JSR
	// ==========
	uint16_t Cpu::jsr(uint16_t opcode)
	{
		uint32_t address = getEffectiveAddress(opcode);
		writeAt<uint32_t>(0b100'111, pc, false);
		pc = address;

		return instructions::JSR;
	}

	// ==========
	// LEA
	// ==========
	uint16_t Cpu::lea(uint16_t opcode)
	{
		uint32_t address = getEffectiveAddress(opcode);

		uint16_t destinationRegister = (opcode >> 9) & 0b111;
		aRegisters[destinationRegister] = address;

		return instructions::LEA;
	}

	// ==========
	// LINK
	// ==========
	uint16_t Cpu::link(uint16_t opcode)
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
	uint16_t Cpu::lsl_memory(uint16_t opcode)
	{
		return shiftLeftMemory(opcode, instructions::LSL);
	}

	uint16_t Cpu::lsl_register(uint16_t opcode)
	{
		return shiftLeftRegister(opcode, instructions::LSL);
	}

	// ==========
	// LSR
	// ==========
	uint16_t Cpu::lsr_memory(uint16_t opcode)
	{
		uint16_t effectiveAddress = opcode & 0b111'111;
		uint16_t memory = readAt<uint16_t>(effectiveAddress, true);
		uint16_t bit0 = memory & 1;
		memory >>= 1;
		memory &= 0x7fff;
		writeAt<uint16_t>(effectiveAddress, memory, true);
		statusRegister.c = statusRegister.x = bit0;
		return instructions::LSR;
	}

	uint16_t Cpu::lsr_register(uint16_t opcode)
	{
		return shiftRightRegister(opcode, instructions::LSR, true);
	}

	// ==========
	// MOVE
	// ==========
	uint16_t Cpu::move(uint16_t opcode)
	{
		uint16_t size = opcode >> 12;
		uint16_t sourceEffectiveAddress = opcode & 0b111111u;

		// the destination is inverted: register - mode instead of mode - register
		uint16_t destination = (opcode >> 6) & 0b111111u;
		uint16_t destinationMode = destination & 0b111u;
		uint16_t destinationRegister = destination >> 3;
		uint16_t destinationEffectiveAddress = (destinationMode << 3) | destinationRegister;

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
	uint16_t Cpu::movea(uint16_t opcode)
	{
		uint16_t size = opcode >> 12;
		uint16_t sourceEffectiveAddress = opcode & 0b111'111u;

		// the destination is inverted: register - mode instead of mode - register
		uint16_t destinationRegister = (opcode >> 9) & 0b111u;
		uint16_t destinationEffectiveAddress = (1 << 3) | destinationRegister;

		switch (size)
		{
			case 3: // Word operation; the source operand is sign-extended to a long operand and all 32 bits are loaded into the address register.
			{
				uint16_t source = readAt<uint16_t>(sourceEffectiveAddress, false);
				int32_t extendedSource = (int16_t)source;
				writeAt<uint32_t>(destinationEffectiveAddress, extendedSource, false);
				break;
			}
			case 2:
			{
				uint32_t source = readAt<uint32_t>(sourceEffectiveAddress, false);
				writeAt<uint32_t>(destinationEffectiveAddress, source, false);
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
	uint16_t Cpu::move2ccr(uint16_t opcode)
	{
		uint16_t sourceEffectiveAddress = opcode & 0b111'111u;
		uint16_t source = readAt<uint16_t>(sourceEffectiveAddress, false);

		statusRegister.c = source & 0x1;
		statusRegister.v = (source & 0x2) >> 1;
		statusRegister.z = (source & 0x4) >> 2;
		statusRegister.n = (source & 0x8) >> 3;
		statusRegister.x = (source & 0x10) >> 4;

		return instructions::MOVE2CCR;
	}

	// ==========
	// MOVE from SR
	// ==========
	uint16_t Cpu::movesr(uint16_t opcode)
	{
		uint16_t effectiveAddress = opcode & 0b111'111u;
		writeAt<uint16_t>(effectiveAddress, sr, false);

		return instructions::MOVESR;
	}

	// ==========
	// MOVE to SR
	// ==========
	uint16_t Cpu::move2sr(uint16_t opcode)
	{
		if (sr.s)
		{
			uint16_t data = readAt<uint16_t>(0b111'100, false);
			statusRegister = data;
		}
		else
		{
			handleException(Exceptions::PRIVILEGE_VIOLATION);
		}

		return instructions::MOVE2SR;
	}

	// ==========
	// MOVEM
	// ==========
	uint16_t Cpu::movem(uint16_t opcode)
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
							effectiveAddress += 2;
						}
						else
						{
							localMemory.set<uint32_t>(effectiveAddress, dRegisters[i]);
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
							effectiveAddress += 2;
						}
						else
						{
							localMemory.set<uint32_t>(effectiveAddress, aRegisters[i]);
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
	/// <summary>
	/// MOVEP: Move Peripheral
	/// </summary>
	uint16_t Cpu::movep(uint16_t opcode)
	{
		uint16_t dRegister = (opcode >> 9) & 0b111;
		uint16_t aRegister = opcode & 0b111;
		uint16_t opmode = (opcode >> 6) & 0b111;
		uint16_t displacement = localMemory.get<uint16_t>(pc);
		pc += 2;

		uint32_t effectiveAddress = aRegisters[aRegister] + displacement;
		switch (opmode)
		{
			case 0b100: // Transfer word from memory to register.
			{
				uint8_t m1 = localMemory.get<uint8_t>(effectiveAddress);
				uint8_t m2 = localMemory.get<uint8_t>(effectiveAddress + 2 );
				dRegisters[dRegister] &= 0xffff0000;
				dRegisters[dRegister] |= (m1 << 8) | m2;
				break;
			}
			case 0b101: // Transfer long from memory to register.
			{
				uint8_t m1 = localMemory.get<uint8_t>(effectiveAddress);
				uint8_t m2 = localMemory.get<uint8_t>(effectiveAddress + 2);
				uint8_t m3 = localMemory.get<uint8_t>(effectiveAddress + 4);
				uint8_t m4 = localMemory.get<uint8_t>(effectiveAddress + 6);
				dRegisters[dRegister] = (m1 << 24) | (m2 << 16) | (m3 << 8) | m4;
				break;
			}
			case 0b110: // Transfer word from register to memory.
			{
				localMemory.set<uint8_t>(effectiveAddress + 0, (dRegisters[dRegister] & 0xff00) >> 8);
				localMemory.set<uint8_t>(effectiveAddress + 2, (dRegisters[dRegister] & 0x00ff) >> 0);
				break;
			}
			case 0b111: // Transfer long from register to memory.
			{
				localMemory.set<uint8_t>(effectiveAddress + 0, (dRegisters[dRegister] & 0xff000000) >> 24);
				localMemory.set<uint8_t>(effectiveAddress + 2, (dRegisters[dRegister] & 0x00ff0000) >> 16);
				localMemory.set<uint8_t>(effectiveAddress + 4, (dRegisters[dRegister] & 0x0000ff00) >> 8);
				localMemory.set<uint8_t>(effectiveAddress + 6, (dRegisters[dRegister] & 0x000000ff) >> 0);
				break;
			}
			default:
				throw "movep: invalid opmode";
		}
		return instructions::MOVEP;
	}

	// ==========
	// MOVEQ
	// ==========
	uint16_t Cpu::moveq(uint16_t opcode)
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
	
	/// <summary>
	/// MULS: Signed multiply
	/// </summary>
	uint16_t Cpu::muls(uint16_t opcode)
	{
		int32_t source = static_cast<int16_t>(readAt<uint16_t>(opcode & 0b111'111, false));
		uint8_t reg = (opcode >> 9) & 0b111;
		int32_t destination = static_cast<int16_t>(dRegisters[reg] & 0xffff);

		destination *= source;
		dRegisters[reg] = destination;
		statusRegister.n = (destination < 0);
		statusRegister.z = (destination == 0);
		statusRegister.v = 0;
		statusRegister.c = 0;

		return instructions::MULS;
	}

	/// <summary>
	/// MULU: Unsigned multiply
	/// </summary>
	uint16_t Cpu::mulu(uint16_t opcode)
	{
		uint32_t source = readAt<uint16_t>(opcode & 0b111'111, false);
		uint8_t reg = (opcode >> 9) & 0b111;
		uint32_t destination = dRegisters[reg] & 0xffff;

		destination *= source;
		dRegisters[reg] = destination;
		statusRegister.n = (destination < 0);
		statusRegister.z = (destination == 0);
		statusRegister.v = 0;
		statusRegister.c = 0;

		return instructions::MULU;
	}

	/// <summary>
	/// NBCD: Negate Decimal with Extend
	/// </summary>
	uint16_t Cpu::nbcd(uint16_t opcode)
	{
		uint8_t value = readAt<uint8_t>(opcode & 0b111'111, false);
		uint16_t m1 = value & 0x0f;
		uint16_t m2 = value & 0xf0;

		int16_t decimalValue = 10 * (m2 >> 4) + m1 + statusRegister.x;
		decimalValue = 100 - decimalValue;
		if (decimalValue == 100) decimalValue = 0;
		m1 = decimalValue % 10;
		m2 = decimalValue / 10;
		value = (m2 << 4) | m1;
		writeAt<uint8_t>(opcode & 0b111'111, value, false);

		statusRegister.x = statusRegister.c = (decimalValue == 0 ? 0 : 1);
		if (value != 0) statusRegister.z = 0; // Z is cleared if the result is nonzero; unchanged otherwise.

		return instructions::NBCD;
	}

	/// <summary>
	/// NEG: Negate
	/// </summary>
	/// <param name=""></param>
	/// <returns></returns>
	uint16_t Cpu::neg(uint16_t opcode)
	{
		uint16_t destinationEffectiveAddress = opcode & 0b111'111;
		uint16_t size = (opcode >> 6) & 0b11;
		switch (size)
		{
		case 0:
			neg<uint8_t>(destinationEffectiveAddress);
			break;
		case 1:
			neg<uint16_t>(destinationEffectiveAddress);
			break;
		case 2:
			neg<uint32_t>(destinationEffectiveAddress);
			break;
		default:
			throw "neg: invalid size";
		}
		return instructions::NEG;
	}

	/// <summary>
	/// NEGX: Negate with Extend
	/// </summary>
	uint16_t Cpu::negx(uint16_t opcode)
	{
		uint16_t destinationEffectiveAddress = opcode & 0b111'111;
		uint16_t size = (opcode >> 6) & 0b11;
		switch (size)
		{
		case 0:
			negx<uint8_t>(destinationEffectiveAddress);
			break;
		case 1:
			negx<uint16_t>(destinationEffectiveAddress);
			break;
		case 2:
			negx<uint32_t>(destinationEffectiveAddress);
			break;
		default:
			throw "negx: invalid size";
		}
		return instructions::NEGX;
	}

	/// <summary>
	/// NOP: No Operation
	/// </summary>
	uint16_t Cpu::nop(uint16_t)
	{
		return instructions::NOP;
	}

	/// <summary>
	/// NOT: Complement
	/// </summary>
	uint16_t Cpu::not_(uint16_t opcode)
	{
		uint16_t destinationEffectiveAddress = opcode & 0b111'111;
		uint16_t size = (opcode >> 6) & 0b11;
		switch (size)
		{
		case 0:
			not_<uint8_t>(destinationEffectiveAddress);
			break;
		case 1:
			not_<uint16_t>(destinationEffectiveAddress);
			break;
		case 2:
			not_<uint32_t>(destinationEffectiveAddress);
			break;
		default:
			throw "not: invalid size";
		}
		return instructions::NOT;
	}

	// ==========
	// OR
	// ==========
	uint16_t Cpu::or_(uint16_t opcode)
	{
		logical(opcode, [](uint32_t lhs, uint32_t rhs) { return lhs | rhs; });
		return instructions::OR;
	}

	// ==========
	// ORI
	// ==========
	uint16_t Cpu::ori(uint16_t opcode)
	{
		logicalImmediate(opcode, [](uint32_t lhs, uint32_t rhs) { return lhs | rhs; });
		return instructions::ORI;
	}

	// ==========
	// ORI to CCR
	// ==========
	uint16_t Cpu::ori2ccr(uint16_t)
	{
		uint16_t sourceEffectiveAddress = 0b111'100;
		uint8_t source = readAt<uint8_t>(sourceEffectiveAddress, false);
		statusRegister.c |= (source & 1);
		statusRegister.v |= (source >> 1) & 1;
		statusRegister.z |= (source >> 2) & 1;
		statusRegister.n |= (source >> 3) & 1;
		statusRegister.x |= (source >> 4) & 1;

		return instructions::ORI2CCR;
	}

	/// <summary>
	/// ORI to SR: OR Immediate to Status Register
	/// </summary>
	uint16_t Cpu::ori2sr(uint16_t)
	{
		if (sr.s)
		{
			uint16_t data = readAt<uint16_t>(0b111'100, false);
			uint16_t sr = statusRegister;
			statusRegister = (uint16_t)(sr | data);
		}
		else
		{
			handleException(Exceptions::PRIVILEGE_VIOLATION);
		}
		return instructions::EORI2SR;
	}

	/// <summary>
	/// PEA: Push Effective Address
	/// </summary>
	uint16_t Cpu::pea(uint16_t opcode)
	{
		uint32_t address = getEffectiveAddress(opcode);
		writeAt<uint32_t>(0b100'111, address, false);

		return instructions::PEA;
	}

	/// <summary>
	/// ROL: Rotate Left
	/// </summary>
	uint16_t Cpu::rol_memory(uint16_t opcode)
	{
		uint16_t effectiveAddress = opcode & 0b111'111;
		uint16_t data = readAt<uint16_t>(effectiveAddress, true);
		uint16_t bit15 = data >> 15;
		data <<= 1;
		data |= bit15;
		writeAt<uint16_t>(effectiveAddress, data, true);
		statusRegister.c = bit15;
		statusRegister.v = 0;
		statusRegister.z = data == 0;
		statusRegister.n = (data & (1<<15)) ? 1 : 0;

		return instructions::ROL;
	}

	/// <summary>
	/// ROR: Rotate Right
	/// </summary>
	uint16_t Cpu::ror_memory(uint16_t opcode)
	{
		uint16_t effectiveAddress = opcode & 0b111'111;
		uint16_t data = readAt<uint16_t>(effectiveAddress, true);
		uint16_t bit1 = data & 1;
		data >>= 1;
		data |= bit1 << 15;
		writeAt<uint16_t>(effectiveAddress, data, true);
		statusRegister.c = bit1;
		statusRegister.v = 0;
		statusRegister.z = data == 0;
		statusRegister.n = (data & (1 << 15)) ? 1 : 0;

		return instructions::ROR;
	}

	uint16_t Cpu::rotate_register(uint16_t opcode, rotateFunction fn)
	{
		uint16_t destinationRegister = opcode & 0b111;
		uint16_t count;
		if (opcode & (1 << 5))
		{
			// Rotate count is in the register
			uint16_t reg = (opcode >> 9) & 0b111;
			count = dRegisters[reg] & 0x3f;
		}
		else
		{
			// Rotate count is in the instruction
			count = (opcode >> 9) & 0b111;
			if (count == 0) count = 8;
		}
        (*this.*(fn))(destinationRegister, count);
		return instructions::ROL;

	}
	/// <summary>
	/// ROL: Rotate Left
	/// </summary>
	uint16_t Cpu::rol_register(uint16_t opcode)
	{
		uint16_t size = (opcode >> 6) & 0b11;
		switch (size)
		{
			case 0:
				rotate_register(opcode, &Cpu::rotateLeft<uint8_t>);
				break;
			case 1:
				rotate_register(opcode, &Cpu::rotateLeft<uint16_t>);
				break;
			case 2:
				rotate_register(opcode, &Cpu::rotateLeft<uint32_t>);
				break;
			default:
				throw "rol: invalid size";
		}
		return instructions::ROL;
	}

	/// <summary>
	/// ROR: Rotate Right
	/// </summary>
	uint16_t Cpu::ror_register(uint16_t opcode)
	{
		uint16_t size = (opcode >> 6) & 0b11;
		switch (size)
		{
		case 0:
			rotate_register(opcode, &Cpu::rotateRight<uint8_t>);
			break;
		case 1:
			rotate_register(opcode, &Cpu::rotateRight<uint16_t>);
			break;
		case 2:
			rotate_register(opcode, &Cpu::rotateRight<uint32_t>);
			break;
		default:
			throw "ror: invalid size";
		}
		return instructions::ROR;
	}

	/// <summary>
	/// ROXL: Rotate Left with Extend
	/// </summary>
	uint16_t Cpu::roxl_memory(uint16_t opcode)
	{
		uint16_t effectiveAddress = opcode & 0b111'111;
		uint16_t data = readAt<uint16_t>(effectiveAddress, true);
		uint16_t bit15 = data >> 15;
		data <<= 1;
		data |= statusRegister.x;
		writeAt<uint16_t>(effectiveAddress, data, true);

		statusRegister.x = bit15;
		statusRegister.c = bit15;
		statusRegister.v = 0;
		statusRegister.z = data == 0;
		statusRegister.n = (data & (1 << 15)) ? 1 : 0;

		return instructions::ROXL;
	}

	/// <summary>
	/// ROXR: Rotate Right with Extend
	/// </summary>
	uint16_t Cpu::roxr_memory(uint16_t opcode)
	{
		uint16_t effectiveAddress = opcode & 0b111'111;
		uint16_t data = readAt<uint16_t>(effectiveAddress, true);
		uint16_t bit1 = data & 1;
		data >>= 1;
		data |= statusRegister.x << 15;
		writeAt<uint16_t>(effectiveAddress, data, true);

		statusRegister.x = bit1;
		statusRegister.c = bit1;
		statusRegister.v = 0;
		statusRegister.z = data == 0;
		statusRegister.n = (data & (1 << 15)) ? 1 : 0;

		return instructions::ROXR;
	}

	/// <summary>
	/// ROXL: Rotate Left with Extend
	/// </summary>
	uint16_t Cpu::roxl_register(uint16_t opcode)
	{
		uint16_t size = (opcode >> 6) & 0b11;
		switch (size)
		{
		case 0:
			rotate_register(opcode, &Cpu::rotateLeftWithExtend<uint8_t>);
			break;
		case 1:
			rotate_register(opcode, &Cpu::rotateLeftWithExtend<uint16_t>);
			break;
		case 2:
			rotate_register(opcode, &Cpu::rotateLeftWithExtend<uint32_t>);
			break;
		default:
			throw "roxl: invalid size";
		}

		return instructions::ROXL;
	}

	/// <summary>
	/// ROXR: Rotate Right with Extend
	/// </summary>
	uint16_t Cpu::roxr_register(uint16_t opcode)
	{
		uint16_t size = (opcode >> 6) & 0b11;
		switch (size)
		{
		case 0:
			rotate_register(opcode, &Cpu::rotateRightWithExtend<uint8_t>);
			break;
		case 1:
			rotate_register(opcode, &Cpu::rotateRightWithExtend<uint16_t>);
			break;
		case 2:
			rotate_register(opcode, &Cpu::rotateRightWithExtend<uint32_t>);
			break;
		default:
			throw "roxl: invalid size";
		}

		return instructions::ROXR;
	}

	/// <summary>
	/// RTE: Return from Exception
	/// </summary>
	uint16_t Cpu::rte(uint16_t opcode)
	{
		if (sr.s)
		{
			statusRegister = readAt<uint16_t>(0b011'111, false);
			pc = readAt<uint32_t>(0b011'111, false);
		}
		else
		{
			handleException(Exceptions::PRIVILEGE_VIOLATION);
		}
		return instructions::RTE;
	}

	/// <summary>
	/// RTR: Return and Restore
	/// </summary>
	uint16_t Cpu::rtr(uint16_t opcode)
	{
		uint16_t ccr = readAt<uint16_t>(0b011'111, false);
		statusRegister = static_cast<uint8_t>(ccr & 0xff);
		pc = readAt<uint32_t>(0b011'111, false);
		return instructions::RTR;
	}

	/// <summary>
	/// RTS: Return from Subroutine
	/// </summary>
	uint16_t Cpu::rts(uint16_t opcode)
	{
		pc = readAt<uint32_t>(0b011'111, false);
		return instructions::RTS;
	}

	/// <summary>
	/// SBCD:
	/// </summary>
	uint16_t Cpu::sbcd(uint16_t opcode)
	{
		uint8_t register1 = opcode & 0b111;
		uint8_t register2 = (opcode >> 9) & 0b111;
		bool useAddressRegister = (opcode & 0b1000);

		uint8_t op1;
		uint8_t op2;
		if (useAddressRegister)
		{
			op1 = readAt<uint8_t>(0b100'000u | register1, false);
			op2 = readAt<uint8_t>(0b100'000u | register2, true);
		}
		else
		{
			op1 = dRegisters[register1];
			op2 = dRegisters[register2];
		}
		uint16_t op1_low = (op1 & 0x0f) + statusRegister.x;
		uint16_t op1_high = op1 & 0xf0;
		uint16_t op2_low = op2 & 0x0f;
		uint16_t op2_high = op2 & 0xf0;

		uint16_t m1;
		uint16_t m2;
		if (op1_low <= op2_low)
		{
			m1 = op2_low - op1_low;
			if (op1_high <= op2_high)
			{
				m2 = op2_high - op1_high;
				statusRegister.x = 0;
				statusRegister.c = 0;
			}
			else
			{
				m2 = (op2_high + 10 * 0x10) - op1_high;
				statusRegister.x = 1;
				statusRegister.c = 1;
			}
		}
		else
		{
			m1 = (op2_low + 10) - op1_low;
			if (op1_high-0x10 <= op2_high)
			{
				m2 = op2_high-0x10 - op1_high;
				statusRegister.x = 0;
				statusRegister.c = 0;
			}
			else
			{
				m2 = (op2_high + 10*0x10 - 0x10) - op1_high;
				statusRegister.x = 1;
				statusRegister.c = 1;
			}
		}

		uint16_t result = m2 + m1;
		if (result != 0)
		{
			statusRegister.z = 0;
		}
		if (useAddressRegister)
		{
			writeAt<uint8_t>(0b010'000u | register2, static_cast<uint8_t>(result), true);
		}
		else
		{
			dRegisters[register2] = (dRegisters[register2] & 0xffffff00) | (result & 0xff);
		}
		return instructions::SBCD;
	}

	/// <summary>
	/// Scc: Set Condition Code
	/// </summary>
	uint16_t Cpu::scc(uint16_t opcode)
	{
		uint16_t effectiveAddress = opcode & 0b111'111;
		auto data = readAt<uint8_t>(effectiveAddress, true);

		// evaluate the condition
		uint16_t conditionCode = (opcode >> 8) & 0b1111;
		bool condition = sr.condition(conditionCode);
		if (condition)
		{
			data = 0xff;
		}
		else
		{
			data = 0;
		}
		writeAt<uint8_t>(effectiveAddress, data, true);
		return instructions::SCC;
	}

	// ==========
	// STOP
	// ==========
	uint16_t Cpu::stop(uint16_t opcode)
	{
		uint16_t extension = localMemory.get<uint16_t>(pc);
		pc += 2;
		if (sr.s)
		{
			statusRegister = extension;
			done = true;
		}
		else
		{
			handleException(Exceptions::PRIVILEGE_VIOLATION);
		}

		return instructions::STOP;
	}

	// ==========
	// SUB
	// ==========
	uint16_t Cpu::sub(uint16_t opcode)
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

	/// <summary>
	/// SUBA: Subtract from Address Register
	/// </summary>
	uint16_t Cpu::suba(uint16_t opcode)
	{
		// The Condition Codes are unaffected so the template add<> method can't be used
		uint16_t sourceEffectiveAddress = opcode & 0b111'111u;
		uint16_t destinationRegister = (opcode >> 9) & 0b111;

		bool isLongOperation = ((opcode >> 6) & 0b111) == 0b111;
		if (isLongOperation)
		{
			uint32_t operand = readAt<uint32_t>(sourceEffectiveAddress, false);
			aRegisters[destinationRegister] -= operand;
		}
		else
		{
			uint16_t operand = readAt<uint16_t>(sourceEffectiveAddress, false);
			uint32_t extended = (int16_t)operand;
			aRegisters[destinationRegister] -= extended;
		}

		return instructions::SUBA;
	}

	/// <summary>
	/// SUBI: Subtract Immediate
	/// </summary>
	uint16_t Cpu::subi(uint16_t opcode)
	{
		uint16_t sourceEffectiveAddress = 0b111'100;
		uint16_t destinationEffectiveAddress = opcode & 0b111'111;
		uint16_t size = (opcode >> 6) & 0b11;
		switch (size)
		{
			case 0:
			{
				sub<uint8_t>(sourceEffectiveAddress, destinationEffectiveAddress);
				break;
			}
			case 1:
			{
				sub<uint16_t>(sourceEffectiveAddress, destinationEffectiveAddress);
				break;
			}
			case 2:
			{
				sub<uint32_t>(sourceEffectiveAddress, destinationEffectiveAddress);
				break;
			}
		}
		return instructions::SUBI;
	}

	/// <summary>
	/// SUBQ: Subtract Quick
	/// </summary>
	uint16_t Cpu::subq(uint16_t opcode)
	{
		uint16_t destinationEffectiveAdress = opcode & 0b111'111;
		bool isAddressRegister = (opcode & 0b111'000) == 0b001'000;
		uint16_t size = (opcode >> 6) & 0b11;
		uint32_t source = (opcode >> 9) & 0b111;
		if (source == 0) source = 8;

		if (isAddressRegister)
		{
			if (size == 0)
			{
				throw "subq: invalid size";
			}
			subq<uint32_t>(source, destinationEffectiveAdress);
		}
		else
		{
			switch (size)
			{
				case 0:
					subq<uint8_t>(source, destinationEffectiveAdress);
					break;
				case 1:
					subq<uint16_t>(source, destinationEffectiveAdress);
					break;
				case 2:
					subq<uint32_t>(source, destinationEffectiveAdress);
					break;
				default:
					throw "subq: invalid size";
			}
		}
		return instructions::SUBQ;
	}

	/// <summary>
	/// SUBX: Subtract with Extend
	/// </summary>
	uint16_t Cpu::subx(uint16_t opcode)
	{
		uint16_t register1 = opcode & 0b111;
		uint16_t register2 = (opcode >> 9) & 0b111;
		bool useAddressRegister = (opcode & 0b1000);
		uint16_t size = (opcode >> 6) & 0b11;

		switch (size)
		{
		case 0:
			subx<uint8_t>(register1, register2, useAddressRegister);
			break;
		case 1:
			subx<uint16_t>(register1, register2, useAddressRegister);
			break;
		case 2:
			subx<uint32_t>(register1, register2, useAddressRegister);
			break;
		default:
			throw "subx: invalid size";
		}
		return instructions::SUBX;
	}

	/// <summary>
	/// SWAP: Swap Register Halves
	/// </summary>
	uint16_t Cpu::swap(uint16_t opcode)
	{
		uint16_t destinationRegister = opcode & 0b111;
		uint32_t value = dRegisters[destinationRegister];
		value = (value >> 16) | (value << 16);
		dRegisters[destinationRegister] = value;

		statusRegister.n = static_cast<int32_t>(value) < 0;
		statusRegister.z = value == 0;
		statusRegister.v = 0;
		statusRegister.c = 0;

		return instructions::SWAP;
	}

	/// <summary>
	/// TAS: Test and Set an Operand
	/// </summary>
	uint16_t Cpu::tas(uint16_t opcode)
	{
		uint16_t effectiveAddress = opcode & 0b111'111;
		uint8_t value = readAt<uint8_t>(effectiveAddress, true);
		statusRegister.n = (value & 0x80) ? 1 : 0;
		statusRegister.z = (value == 0) ? 1 : 0;
		statusRegister.v = 0;
		statusRegister.c = 0;
		value |= 0x80;
		writeAt<uint8_t>(effectiveAddress, value, true);

		return instructions::TAS;
	}

	// ==========
	// TRAP
	// ==========
	uint16_t Cpu::trap(uint16_t opcode)
	{
		uint8_t trapNumber = opcode & 0b1111;
		handleException(Exceptions::TRAP + trapNumber);
		return instructions::TRAP;
	}

	uint16_t Cpu::trapv(uint16_t opcode)
	{
		if (statusRegister.v)
		{
			handleException(Exceptions::TRAPV);
		}
		return instructions::TRAPV;
	}

	/// <summary>
	/// TST: Test Operand
	/// </summary>
	uint16_t Cpu::tst(uint16_t opcode)
	{
		uint16_t effectiveAddress = opcode & 0b111'111;
		uint16_t size = (opcode >> 6) & 0b11;
		int32_t value32;
		switch (size)
		{
			case 0:
			{
				uint8_t value = readAt<uint8_t>(effectiveAddress, false);
				value32 = static_cast<int8_t>(value);
				break;
			}
			case 1:
			{
				uint16_t value = readAt<uint16_t>(effectiveAddress, false);
				value32 = static_cast<int16_t>(value);
				break;
			}
			case 2:
			{
				uint32_t value = readAt<uint32_t>(effectiveAddress, false);
				value32 = static_cast<int32_t>(value);
				break;
			}
			default:
			{
				throw "tst: invalid size";
			}
		}
		statusRegister.n = value32 < 0;
		statusRegister.z = value32 == 0;
		statusRegister.v = 0;
		statusRegister.c = 0;
		return instructions::TST;
	}

	/// <summary>
	/// UNLK: Unlink
	/// </summary>
	uint16_t Cpu::unlk(uint16_t opcode)
	{
		uint8_t reg = opcode & 0b111;
		aRegisters[7] = aRegisters[reg];
		aRegisters[reg] = readAt<uint32_t>(0b011'111, false);

		return instructions::UNLK;
	}

	uint16_t Cpu::unknown(uint16_t opcode)
	{
		if (opcode == 0xffff)
		{
			done = true;
			return instructions::UNKNOWN;
		}
		throw "unknown instruction";
	}

}