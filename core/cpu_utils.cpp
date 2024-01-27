#include "core.h"
#include "instructions.h"
#include "cpu.h"

namespace mc68000
{
	template <typename T> int32_t signed_cast(uint64_t value);

	template <> int32_t signed_cast <uint8_t>(uint64_t value) { return static_cast<int8_t>(value); }
	template <> int32_t signed_cast<uint16_t>(uint64_t value) { return static_cast<int16_t>(value); }
	template <> int32_t signed_cast<uint32_t>(uint64_t value) { return static_cast<int32_t>(value); }

	// ==========
	// ADD
	// ==========
	template <typename T> void Cpu::add(uint16_t sourceEffectiveAddress, uint16_t destinationEffectiveAdress)
	{
		uint32_t source = readAt<T>(sourceEffectiveAddress);
		uint32_t destination = readAt<T>(destinationEffectiveAdress);
		uint64_t result = (uint64_t)destination + (uint64_t)source;
		writeAt<T>(destinationEffectiveAdress, static_cast<T>(result));

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
		uint32_t destination = readAt<T>(destinationEffectiveAdress);
		uint64_t result = (uint64_t)destination + (uint64_t)data;
		writeAt<T>(destinationEffectiveAdress, static_cast<T>(result));

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

	// =========
	// LOGICAL Operations
	// =========
	template <typename T> void Cpu::logical(uint16_t sourceEffectiveAddress, uint16_t destinationEffectiveAdress, uint32_t (*op)(uint32_t lhs, uint32_t rhs))
	{
		uint32_t source = readAt<T>(sourceEffectiveAddress);
		uint32_t destination = readAt<T>(destinationEffectiveAdress);
		uint32_t result = op(source, destination);
		writeAt<T>(destinationEffectiveAdress, result);

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
		uint32_t source = readAt<T>(sourceEffectiveAddress);
		uint32_t destination = readAt<T>(destinationEffectiveAdress);
		uint64_t result = (uint64_t)destination - (uint64_t)source;
		writeAt<T>(destinationEffectiveAdress, static_cast<T>(result));

		statusRegister.n = signed_cast<T>(result) < 0;
		statusRegister.z = static_cast<T>(result) == 0;
		statusRegister.c = signed_cast<T>(result >> 1) < 0;
		statusRegister.x = statusRegister.c;
		statusRegister.v = signed_cast<T>((destination ^ source) & (destination ^ result)) < 0;
	}
	template void Cpu::sub<uint8_t>(uint16_t sourceEffectiveAddress, uint16_t destinationEffectiveAdress);
	template void Cpu::sub<uint16_t>(uint16_t sourceEffectiveAddress, uint16_t destinationEffectiveAdress);
	template void Cpu::sub<uint32_t>(uint16_t sourceEffectiveAddress, uint16_t destinationEffectiveAdress);

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

	template <typename T> void Cpu::cmp(uint16_t sourceEffectiveAddress, uint16_t destinationEffectiveAdress)
	{
		uint32_t source = readAt<T>(sourceEffectiveAddress);
		uint32_t destination = readAt<T>(destinationEffectiveAdress);
		uint64_t result = (uint64_t)destination - (uint64_t)source;

		statusRegister.n = signed_cast<T>(result) < 0;
		statusRegister.z = static_cast<T>(result) == 0;
		statusRegister.c = signed_cast<T>(result >> 1) < 0;
		statusRegister.v = signed_cast<T>((source ^ destination) & (destination ^ result)) < 0;
	}
	template void Cpu::cmp<uint8_t>(uint16_t sourceEffectiveAddress, uint16_t destinationEffectiveAdress);
	template void Cpu::cmp<uint16_t>(uint16_t sourceEffectiveAddress, uint16_t destinationEffectiveAdress);
	template void Cpu::cmp<uint32_t>(uint16_t sourceEffectiveAddress, uint16_t destinationEffectiveAdress);

	template <typename T> T Cpu::readAt(uint16_t ea)
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
	template uint8_t Cpu::readAt<uint8_t>(uint16_t ea);
	template uint16_t Cpu::readAt<uint16_t>(uint16_t ea);
	template uint32_t Cpu::readAt<uint32_t>(uint16_t ea);

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

	template <typename T> void Cpu::writeAt(uint16_t  ea, T data)
	{
		unsigned short eam = ea >> 3;
		unsigned short reg = ea & 7u;
		switch (eam)
		{
		case 0:
			dRegisters[reg] = maskDRegister(dRegisters[reg], sizeof(T)) | data;
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
	template void Cpu::writeAt<uint8_t>(uint16_t ea, uint8_t data);
	template void Cpu::writeAt<uint16_t>(uint16_t ea, uint16_t data);
	template void Cpu::writeAt<uint32_t>(uint16_t ea, uint32_t data);

	template<typename T> void Cpu::move(uint16_t sourceEffectiveAddress, uint16_t  destinationEffectiveAddress)
	{
		T source = readAt<T>(sourceEffectiveAddress);
		writeAt<T>(destinationEffectiveAddress, source);
		statusRegister.c = 0;
		statusRegister.v = 0;
		statusRegister.z = source == 0 ? 1 : 0;
		statusRegister.n = (source >> (sizeof(T) * 8 - 1)) ? 1 : 0;
	}
	template void Cpu::move<uint8_t>(uint16_t sourceEffectiveAddress, uint16_t  destinationEffectiveAddress);
	template void Cpu::move<uint16_t>(uint16_t sourceEffectiveAddress, uint16_t  destinationEffectiveAddress);
	template void Cpu::move<uint32_t>(uint16_t sourceEffectiveAddress, uint16_t  destinationEffectiveAddress);


}