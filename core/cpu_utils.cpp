#include <cassert>

#include "core.h"
#include "instructions.h"
#include "cpu.h"
#include "exceptions.h"

namespace mc68000
{
	template <typename T> int32_t signed_cast(uint64_t value);

	template <> int32_t signed_cast <uint8_t>(uint64_t value) { return static_cast<int8_t>(value); }
	template <> int32_t signed_cast<uint16_t>(uint64_t value) { return static_cast<int16_t>(value); }
	template <> int32_t signed_cast<uint32_t>(uint64_t value) { return static_cast<int32_t>(value); }

	template <typename T> T mostSignificantBit(T value);

	template <> uint8_t  mostSignificantBit<uint8_t>(uint8_t value)   { return value & 0x80; }
	template <> uint16_t mostSignificantBit<uint16_t>(uint16_t value) { return value & 0x8000; }
	template <> uint32_t mostSignificantBit<uint32_t>(uint32_t value) { return value & 0x80000000; }

	template <typename T> bool isMostSignificantBitSet(T value);
	template <> bool  isMostSignificantBitSet<uint8_t>(uint8_t value)  { return value & 0x80; }
	template <> bool isMostSignificantBitSet<uint16_t>(uint16_t value) { return value & 0x8000; }
	template <> bool isMostSignificantBitSet<uint32_t>(uint32_t value) { return value & 0x80000000; }

	template <typename T> T subPart(uint32_t value);
	template <> uint8_t  subPart<uint8_t>(uint32_t value)  { return value & 0xff; }
	template <> uint16_t subPart<uint16_t>(uint32_t value) { return value & 0xffff; }
	template <> uint32_t subPart<uint32_t>(uint32_t value) { return value; }

	template <typename T> uint32_t setSubPart(uint32_t& current, T value);
	template <> uint32_t setSubPart<uint8_t>(uint32_t& current, uint8_t value)   { current = (current & 0xffffff00) | value; return current; }
	template <> uint32_t setSubPart<uint16_t>(uint32_t& current, uint16_t value) { current = (current & 0xffff0000) | value; return current; }
	template <> uint32_t setSubPart<uint32_t>(uint32_t& current, uint32_t value) { current = value; return current; }
	// ==========
	// ADD
	// ==========
	template <typename T> void Cpu::add(uint16_t sourceEffectiveAddress, uint16_t destinationEffectiveAdress)
	{
		uint32_t source = readAt<T>(sourceEffectiveAddress, false);
		uint32_t destination = readAt<T>(destinationEffectiveAdress, true);
		uint64_t result = (uint64_t)destination + (uint64_t)source;
		writeAt<T>(destinationEffectiveAdress, static_cast<T>(result), true);

		statusRegister.n = signed_cast<T>(result) < 0;
		statusRegister.z = static_cast<T>(result) == 0;
		statusRegister.c = signed_cast<T>(result >> 1) < 0;
		statusRegister.x = statusRegister.c;
		statusRegister.v = signed_cast<T>((source ^ result) & (destination ^ result)) < 0;
	}
	template void Cpu::add<uint8_t>(uint16_t sourceEffectiveAddress, uint16_t destinationEffectiveAdress);
	template void Cpu::add<uint16_t>(uint16_t sourceEffectiveAddress, uint16_t destinationEffectiveAdress);
	template void Cpu::add<uint32_t>(uint16_t sourceEffectiveAddress, uint16_t destinationEffectiveAdress);

	// ==========
	// ADDQ
	// ==========
	template <typename T> void Cpu::addq(uint32_t data, uint16_t destinationEffectiveAdress)
	{
		uint32_t destination = readAt<T>(destinationEffectiveAdress, true);
		uint64_t result = (uint64_t)destination + (uint64_t)data;
		writeAt<T>(destinationEffectiveAdress, static_cast<T>(result), true);

		// When adding to address registers, the condition codes are not altered, and the entire destination address
		// register is used regardless of the operation size.
		if ((destinationEffectiveAdress & 0b111'000) != 0b001'000)
		{
			statusRegister.n = signed_cast<T>(result) < 0;
			statusRegister.z = static_cast<T>(result) == 0;
			statusRegister.c = signed_cast<T>(result >> 1) < 0;
			statusRegister.x = statusRegister.c;
			statusRegister.v = signed_cast<T>((data ^ result) & (destination ^ result)) < 0;
		}
	}
	template void Cpu::addq<uint8_t>(uint32_t data, uint16_t destinationEffectiveAdress);
	template void Cpu::addq<uint16_t>(uint32_t data, uint16_t destinationEffectiveAdress);
	template void Cpu::addq<uint32_t>(uint32_t data, uint16_t destinationEffectiveAdress);

	// ==========
	// shiftLeft ASL LSL
	// ==========
	template <typename T> void Cpu::shiftLeft(uint16_t destinationRegister, uint32_t shift)
	{
		T data = subPart<T>(dRegisters[destinationRegister]);
		bool c = false;
		bool v = false;
		for (uint32_t i = 0; i < shift; i++)
		{
			c = isMostSignificantBitSet(data);
			data <<= 1;
			v = v | (c ^ (isMostSignificantBitSet(data)));
		}
		statusRegister.n = signed_cast<T>(data) < 0;
		statusRegister.z = data == 0;
		statusRegister.c = c ? 1 : 0;
		if (shift) statusRegister.x = statusRegister.c;
		statusRegister.v = v ? 1 : 0;
		dRegisters[destinationRegister] = setSubPart<T>(dRegisters[destinationRegister], data);
	}
	template void Cpu::shiftLeft<uint8_t>(uint16_t destinationRegister, uint32_t shift);
	template void Cpu::shiftLeft<uint16_t>(uint16_t destinationRegister, uint32_t shift);
	template void Cpu::shiftLeft<uint32_t>(uint16_t destinationRegister, uint32_t shift);

	// ==========
	// shiftRight ASR LSR
	// ==========
	template <typename T> void Cpu::shiftRight(uint16_t destinationRegister, uint32_t shift, bool logical)
	{
		T data = subPart<T>(dRegisters[destinationRegister]);
		T c = 0;
		T msb = logical ? 0 : mostSignificantBit(data); // LSR push 0 while ASR keep the msb
		for (uint32_t i = 0; i < shift; i++)
		{
			c = data & 1;
			data >>= 1;
			data |= msb;
		}
		statusRegister.n = signed_cast<T>(data) < 0;
		statusRegister.z = data == 0;
		statusRegister.c = c ? 1 : 0;
		if (shift) statusRegister.x = statusRegister.c;
		statusRegister.v = 0;
		dRegisters[destinationRegister] = setSubPart<T>(dRegisters[destinationRegister], data);
	}
	template void Cpu::shiftRight<uint8_t>(uint16_t destinationRegister, uint32_t shift, bool logical);
	template void Cpu::shiftRight<uint16_t>(uint16_t destinationRegister, uint32_t shift, bool logical);
	template void Cpu::shiftRight<uint32_t>(uint16_t destinationRegister, uint32_t shift, bool logical);

	// =========
	// SHIFT Operations
	// =========
	uint16_t Cpu::shiftLeftMemory(uint16_t opcode, uint16_t instruction)
	{
		uint16_t effectiveAddress = opcode & 0b111'111;
		uint16_t memory = readAt<uint16_t>(effectiveAddress, true);
		uint16_t bit15 = memory & 0x8000;
		memory <<= 1;
		writeAt<uint16_t>(effectiveAddress, memory, true);
		statusRegister.c = statusRegister.x = bit15 ? 1 : 0;

		return instruction;
	}

	uint16_t Cpu::shiftLeftRegister(uint16_t opcode, uint16_t instruction)
	{
		uint16_t destinationRegister = opcode & 0b111;
		bool isFromRegister = opcode & 0b100000;
		uint16_t size = (opcode >> 6) & 0b11;
		uint16_t numberOrRegister = (opcode >> 9) & 0b111;
		uint16_t shift = numberOrRegister;
		if (isFromRegister)
		{
			shift = dRegisters[numberOrRegister] % 64;
		}
		switch (size)
		{
			case 0:
			{
				shiftLeft<uint8_t>(destinationRegister, shift);
				break;
			}
			case 1:
			{
				shiftLeft<uint16_t>(destinationRegister, shift);
				break;
			}
			case 2:
			{
				shiftLeft<uint32_t>(destinationRegister, shift);
				break;
			}
			default:
			{
				assert("shiftLeftRegister: wrong size");
			}
		}

		return instruction;
	}

	unsigned short Cpu::shiftRightRegister(unsigned short opcode, uint16_t instruction, bool logical)
	{
		uint16_t destinationRegister = opcode & 0b111;
		bool isFromRegister = opcode & 0b100000;
		uint16_t size = (opcode >> 6) & 0b11;
		uint16_t numberOrRegister = (opcode >> 9) & 0b111;
		uint16_t shift = numberOrRegister;
		if (isFromRegister)
		{
			shift = dRegisters[numberOrRegister] % 64;
		}
		switch (size)
		{
			case 0:
			{
				shiftRight<uint8_t>(destinationRegister, shift, logical);
				break;
			}
			case 1:
			{
				shiftRight<uint16_t>(destinationRegister, shift, logical);
				break;
			}
			case 2:
			{
				shiftRight<uint32_t>(destinationRegister, shift, logical);
				break;
			}
			default:
			{
				assert("asr_register: wrong size");
			}
		}
		return instruction;
	}


	// =========
	// LOGICAL Operations
	// =========
	template <typename T> void Cpu::logical(uint16_t sourceEffectiveAddress, uint16_t destinationEffectiveAdress, uint32_t (*op)(uint32_t lhs, uint32_t rhs))
	{
		uint32_t source = readAt<T>(sourceEffectiveAddress, false);
		uint32_t destination = readAt<T>(destinationEffectiveAdress, true);
		uint32_t result = op(source, destination);
		writeAt<T>(destinationEffectiveAdress, result, true);

		statusRegister.n = signed_cast<T>(result) < 0;
		statusRegister.z = static_cast<T>(result) == 0;
		statusRegister.c = 0;
		statusRegister.v = 0;
	}

	void Cpu::logical(uint16_t opcode, uint32_t(*logicalOperator)(uint32_t lhs, uint32_t rhs))
	{
		uint16_t reg = (opcode >> 9) & 0b111;
		uint16_t mode = (opcode >> 8) & 0b1;
		uint16_t size = (opcode >> 6) & 0b11;
		uint16_t effectiveAddress = opcode & 0b111'111;

		if (mode == 0)
		{
			// < ea > and Dn -> Dn
			switch (size)
			{
			case 0:
				logical<uint8_t>(effectiveAddress, reg, logicalOperator);
				break;
			case 1:
				logical<uint16_t>(effectiveAddress, reg, logicalOperator);
				break;
			case 2:
				logical<uint32_t>(effectiveAddress, reg, logicalOperator);
				break;
			default:
				throw "logical: invalid size";
			}
		}
		else
		{
			// Dn and < ea > -> < ea >
			switch (size)
			{
			case 0:
				logical<uint8_t>(reg, effectiveAddress, logicalOperator);
				break;
			case 1:
				logical<uint16_t>(reg, effectiveAddress, logicalOperator);
				break;
			case 2:
				logical<uint32_t>(reg, effectiveAddress, logicalOperator);
				break;
			default:
				throw "logical: invalid size";
			}
		}
	}

	void Cpu::logicalImmediate(uint16_t opcode, uint32_t(*logicalOperator)(uint32_t lhs, uint32_t rhs))
	{
		uint16_t sourceEffectiveAddress = 0b111'100;
		uint16_t destinationEffectiveAdress = opcode & 0b111'111;

		uint16_t size = (opcode >> 6) & 0b11;
		switch (size)
		{
		case 0:
		{
			logical<uint8_t>(sourceEffectiveAddress, destinationEffectiveAdress, logicalOperator);
			break;
		}
		case 1:
		{
			logical<uint16_t>(sourceEffectiveAddress, destinationEffectiveAdress, logicalOperator);
			break;
		}
		case 2:
		{
			logical<uint32_t>(sourceEffectiveAddress, destinationEffectiveAdress, logicalOperator);
			break;
		}
		default:
			throw "logicalImmediate: invalid size";
		}

	}
	// ==========
	// SUB
	// ==========
	template <typename T> void Cpu::sub(uint16_t sourceEffectiveAddress, uint16_t destinationEffectiveAdress)
	{
		uint32_t source = readAt<T>(sourceEffectiveAddress, false);
		uint32_t destination = readAt<T>(destinationEffectiveAdress, true);
		uint64_t result = (uint64_t)destination - (uint64_t)source;
		writeAt<T>(destinationEffectiveAdress, static_cast<T>(result), true);

		statusRegister.n = signed_cast<T>(result) < 0;
		statusRegister.z = static_cast<T>(result) == 0;
		statusRegister.c = signed_cast<T>(result >> 1) < 0;
		statusRegister.x = statusRegister.c;
		statusRegister.v = signed_cast<T>((destination ^ source) & (destination ^ result)) < 0;
	}
	template void Cpu::sub<uint8_t>(uint16_t sourceEffectiveAddress, uint16_t destinationEffectiveAdress);
	template void Cpu::sub<uint16_t>(uint16_t sourceEffectiveAddress, uint16_t destinationEffectiveAdress);
	template void Cpu::sub<uint32_t>(uint16_t sourceEffectiveAddress, uint16_t destinationEffectiveAdress);

	// ==============
	// Bcc utilities
	// ==============

	uint32_t Cpu::getTargetAddress(uint16_t opcode)
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

	// ==========
	// CMP
	// ==========
	template <typename T> void Cpu::cmp(uint16_t sourceEffectiveAddress, uint16_t destinationEffectiveAdress)
	{
		uint32_t source = readAt<T>(sourceEffectiveAddress, false);
		uint32_t destination = readAt<T>(destinationEffectiveAdress, false);
		uint64_t result = (uint64_t)destination - (uint64_t)source;

		statusRegister.n = signed_cast<T>(result) < 0;
		statusRegister.z = static_cast<T>(result) == 0;
		statusRegister.c = signed_cast<T>(result >> 1) < 0;
		statusRegister.v = signed_cast<T>((source ^ destination) & (destination ^ result)) < 0;
	}
	template void Cpu::cmp<uint8_t>(uint16_t sourceEffectiveAddress, uint16_t destinationEffectiveAdress);
	template void Cpu::cmp<uint16_t>(uint16_t sourceEffectiveAddress, uint16_t destinationEffectiveAdress);
	template void Cpu::cmp<uint32_t>(uint16_t sourceEffectiveAddress, uint16_t destinationEffectiveAdress);

	// ==========
	// NEG
	// ==========
	template <typename T> void Cpu::neg(uint16_t effectiveAdress)
	{
		uint32_t destination = readAt<T>(effectiveAdress, true);
		uint64_t result = 0 - (uint64_t)destination;
		writeAt<T>(effectiveAdress, static_cast<T>(result), true);

		statusRegister.n = signed_cast<T>(result) < 0;
		statusRegister.z = static_cast<T>(result) == 0;
		statusRegister.c = signed_cast<T>(result >> 1) < 0;
		statusRegister.x = statusRegister.c;
		statusRegister.v = signed_cast<T>((destination ^ 0) & (0 ^ result)) < 0;
	}
	template void Cpu::neg<uint8_t>(uint16_t effectiveAdress);
	template void Cpu::neg<uint16_t>(uint16_t effectiveAdress);
	template void Cpu::neg<uint32_t>(uint16_t effectiveAdress);

	// ==========
	// NEGX
	// ==========
	template <typename T> void Cpu::negx(uint16_t effectiveAdress)
	{
		uint32_t destination = readAt<T>(effectiveAdress, true);
		uint64_t result = 0 - (uint64_t)destination - statusRegister.x;
		writeAt<T>(effectiveAdress, static_cast<T>(result), true);

		statusRegister.n = signed_cast<T>(result) < 0;
		if (result != 0) statusRegister.z = 0;
		statusRegister.c = signed_cast<T>(result >> 1) < 0;
		statusRegister.x = statusRegister.c;
		statusRegister.v = signed_cast<T>((destination ^ 0) & (0 ^ result)) < 0;
	}
	template void Cpu::negx<uint8_t>(uint16_t effectiveAdress);
	template void Cpu::negx<uint16_t>(uint16_t effectiveAdress);
	template void Cpu::negx<uint32_t>(uint16_t effectiveAdress);

	// ========================================
	// Memory access through effective address
	// ========================================

	/// <summary>
	/// Read the value described by the effective address
	/// </summary>
	/// <typeparam name="T">Either a 8, 16 or 32 bits integer type</typeparam>
	/// <param name="ea">The effective address</param>
	/// <param name="readModifyWrite">
	/// If true the on-going operation is a read modify write operation and 
	/// in case of post increment or pre decrement operation the address register should be modifier only
	/// once either before (pre-decrement) or after (post-increment) the operation.
	/// </param>
	/// <returns>The value at the effctive address</returns>
	template <typename T> T Cpu::readAt(uint16_t ea, bool readModifyWrite)
	{
		unsigned short eam = ea >> 3;
		unsigned short reg = ea & 7u;
		switch (eam)
		{
		case 0b000:
			return static_cast<T>(dRegisters[reg]);
		case 0b001:
			return static_cast<T>(aRegisters[reg]);
		case 0b010:
		{
			uint32_t address = aRegisters[reg];
			T x = localMemory.get<T>(address);
			return x;
		}
		case 0b011:
		{
			uint32_t address = aRegisters[reg];
			T x = localMemory.get<T>(address);
			if (!readModifyWrite)
			{
				if (reg != 7)
				{
					aRegisters[reg] += sizeof(T);
				}
				else
				{
					// If the address register is the stack pointer and the operand size is byte, the address is incremented by two to keep the stack pointer aligned to a word boundary
					aRegisters[reg] += sizeof(T) == 1 ? 2 : sizeof(T);
				}
			}
			return x;
		}
		case 0b100:
		{
			if (reg != 7)
			{
				aRegisters[reg] -= sizeof(T);
			}
			else
			{
				// If the address register is the stack pointer and the operand size is byte, the address is decremented by two to keep the stack pointer aligned to a word boundary
				aRegisters[reg] -= sizeof(T) == 1 ? 2 : sizeof(T);
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

			T x = localMemory.get<T>(address + offset);
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
			int32_t displacement = (int16_t)(extension & 0xff);

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
					pc += 2; // pc must be aligned on a word boundary
				}
				else
				{
					x = localMemory.get<T>(pc);
					pc += sizeof(T);
				}
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
	template uint8_t Cpu::readAt<uint8_t>(uint16_t ea, bool readModifyWrite);
	template uint16_t Cpu::readAt<uint16_t>(uint16_t ea, bool readModifyWrite);
	template uint32_t Cpu::readAt<uint32_t>(uint16_t ea, bool readModifyWrite);

	/// <summary>
	/// Retrieves the target address from the effective address
	/// </summary>
	/// <param name="ea">The instruction code or the address effective part of the instruction code</param>
	/// <returns>The final address</returns>
	uint32_t Cpu::getEffectiveAddress(uint16_t opcode)
	{
		unsigned short eam = (opcode >> 3) & 0b111;
		unsigned short reg = opcode & 0b111;
		uint32_t address;

		switch (eam)
		{
			case 0b010:
			{
				address = aRegisters[reg];
				break;
			}
			case 0b011:
			{
				// Post increment : the address is incremented after the operation
				address = aRegisters[reg];
				break;
			}
			case 0b100:
			{
				// Pre decrement : return the current address. Don't decrement the address since the size is unknown
				address = aRegisters[reg];
				break;
			}
			case 0b101:
			{
				uint32_t baseAddress = aRegisters[reg];

				uint16_t extension = localMemory.get<uint16_t>(pc);
				pc += 2;
				int32_t offset = (int16_t)extension; // Displacements are always sign-extended to 32 bits prior to being used

				address = (baseAddress + offset);
				break;
			}
			case 0b110:
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
			case 0b111:
			{
				switch (reg)
				{
					case 0b000:
					{
						uint16_t extension = localMemory.get<uint16_t>(pc);
						pc += 2;
						address = (int16_t)extension;
						break;
					}
					case 0b001:
					{
						address = localMemory.get<uint32_t>(pc);
						pc += 4;
						break;
					}
					case 0b010:
					{
						uint32_t baseAddress = pc;
						uint16_t extension = localMemory.get<uint16_t>(pc);
						pc += 2;
						int32_t offset = (int16_t)extension;
						address = baseAddress + offset;
						break;
					}
					case 0b011:
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
				throw "getTargetAddress: incorrect addressing mode";
				break;
		}
		return address;
	}

	uint32_t maskDRegister(uint32_t& reg, size_t size)
	{
		switch (size)
		{
		case 1:
			reg &= 0xffffff00;
			break;
		case 2:
			reg &= 0xffff0000;
			break;
		case 4:
			reg = 0;
		}
		return reg;
	}

	/// <summary>
	/// Store the value at the effective address
	/// </summary>
	/// <typeparam name="T">Either a 8, 16 or 32 bits integer type</typeparam>
	/// <param name="ea">The effective address</param>
	/// <param name="data">The value to store</param>
	/// <param name="readModifyWrite">
	/// If true the on-going operation is a read modify write operation and 
	/// in case of post increment or pre decrement operation the address register should be modifier only
	/// once either before (pre-decrement) or after (post-increment) the operation.
	/// </param>
	template <typename T> void Cpu::writeAt(uint16_t  ea, T data, bool readModifyWrite)
	{
		unsigned short eam = ea >> 3;
		unsigned short reg = ea & 7u;
		switch (eam)
		{
		case 0b000:
			dRegisters[reg] = maskDRegister(dRegisters[reg], sizeof(T)) | data;
			break;
		case 0b001:
			aRegisters[reg] = data;
			break;
		case 0b010:
		{
			uint32_t address = aRegisters[reg];
			localMemory.set<T>(address, data);
			break;
		}
		case 0b011:
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
				aRegisters[reg] += sizeof(T) == 1 ? 2 : sizeof(T);
			}
			break;
		}
		case 0b100:
		{
			if (!readModifyWrite)
			{
				if (reg != 7)
				{
					aRegisters[reg] -= sizeof(T);
				}
				else
				{
					// If the address register is the stack pointer and the operand size is byte, the address is decremented by two to keep the stack pointer aligned to a word boundary
					aRegisters[reg] -= sizeof(T) == 1 ? 2 : sizeof(T);
				}
			}
			uint32_t address = aRegisters[reg];
			localMemory.set<T>(address, data);
			break;
		}
		case 0b101:
		{
			uint32_t address = aRegisters[reg];

			uint16_t extension = localMemory.get<T>(pc);
			pc += sizeof(T) == 1 ? 2 : sizeof(T); // pc must be aligned on a word boundary
			int32_t offset = (int16_t)extension;

			localMemory.set<T>(address + offset, data);
			break;
		}
		case 0b110:
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
		case 0b111:
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
	template void Cpu::writeAt<uint8_t>(uint16_t ea, uint8_t data, bool readModifyWrite);
	template void Cpu::writeAt<uint16_t>(uint16_t ea, uint16_t data, bool readModifyWrite);
	template void Cpu::writeAt<uint32_t>(uint16_t ea, uint32_t data, bool readModifyWrite);

	template<typename T> void Cpu::move(uint16_t sourceEffectiveAddress, uint16_t  destinationEffectiveAddress)
	{
		T source = readAt<T>(sourceEffectiveAddress, false);
		writeAt<T>(destinationEffectiveAddress, source, false);
		statusRegister.c = 0;
		statusRegister.v = 0;
		statusRegister.z = source == 0 ? 1 : 0;
		statusRegister.n = (source >> (sizeof(T) * 8 - 1)) ? 1 : 0;
	}
	template void Cpu::move<uint8_t>(uint16_t sourceEffectiveAddress, uint16_t  destinationEffectiveAddress);
	template void Cpu::move<uint16_t>(uint16_t sourceEffectiveAddress, uint16_t  destinationEffectiveAddress);
	template void Cpu::move<uint32_t>(uint16_t sourceEffectiveAddress, uint16_t  destinationEffectiveAddress);

	// ==========
	// Exception handling
	// ==========
	void Cpu::handleException(uint16_t vector)
	{
		if (vector == Exceptions::RESET)
		{
			// Reset
			done = true;
			return;
		}
		else if (vector >= Exceptions::TRAP && vector <= Exceptions::TRAP + 15)
		{
			// Trap
			if (trapHandlers[vector - Exceptions::TRAP] != nullptr)
			{
				// external handler exists so call it
				trapHandlers[vector - Exceptions::TRAP](d0, a0);
				return;
			}
		}

		try
		{
			uint32_t handler = vector *4;
			uint32_t newPc = localMemory.get<uint32_t>(handler);

			localMemory.set<uint16_t>(ssp, sr);
			ssp -= 2;
			localMemory.set<uint32_t>(ssp, pc);
			ssp -= 4;
			statusRegister.t = 0;
			statusRegister.s = 1;
			usp = aRegisters[7];
			aRegisters[7] = ssp;
			pc = newPc;
			return;
		}
		catch (...)
		{
			done = true;
		}
		return;
	}
}