#include <string>

#include "disasm.h"
#include "instructions.h"
namespace mc68000
{
	extern const char* const dregisters[8];
	extern const char* const aregisters[8];

	extern const char* const AddressRegisterIndirect[8];
	extern const char* const AddressRegisterIndirectPost[8];
	extern const char* const AddressRegisterIndirectPre[8];
	extern const char* const Sizes[];

	uint16_t DisAsm::fetchNextWord()
	{
		if (swapMemory)
		{
			const uint8_t* p8 = reinterpret_cast<const uint8_t*>(memory + pc++);

			uint16_t word = (*p8 << 8) | *(p8 + 1);

			return word;

		}
		else
		{
			return memory[pc++];
		}
	}

	uint32_t DisAsm::fetchRelativeAddress()
	{
		uint16_t offset;
		if (swapMemory)
		{
			const uint8_t* p8 = reinterpret_cast<const uint8_t*>(memory + pc);
			offset = (*p8 << 8) | *(p8 + 1);
		}
		else
		{
			offset = memory[pc];
		}
		uint32_t address = origin + (pc * 2)  + (int16_t) offset;
		pc++;
		return address;
	}

	void DisAsm::reset(const uint16_t* mem)
	{
		memory = mem;
		pc = 0;
	}

	std::string DisAsm::decodeEffectiveAddress(unsigned short ea, bool isLongOperation)
	{
		unsigned short eam = ea >> 3;
		unsigned short reg = ea & 7u;
		switch (eam)
		{
		case 0:
			return dregisters[reg];
		case 1:
			return aregisters[reg];
		case 2:
			return AddressRegisterIndirect[reg];
		case 3:
			return AddressRegisterIndirectPost[reg];
		case 4:
			return AddressRegisterIndirectPre[reg];
		case 5:
		{
			auto displacement = fetchNextWord();
			return std::to_string(displacement) + AddressRegisterIndirect[reg];
		}
		case 6:
		{
			auto extension = fetchNextWord();
			bool isAddressRegister = extension & 0x8000;
			unsigned short extensionReg = (extension >> 12) & 7;
			bool isLongIndexSize = (extension & 0x0800);
			unsigned short scale = (extension >> 9) & 3;
			auto displacement = (extension & 0xff);

			std::string result = std::to_string(displacement) + "(";
			result += aregisters[reg];
			result += ",";
			result += isAddressRegister ? aregisters[extensionReg] : dregisters[extensionReg];
			result += ")";
			return result;
		}
		case 7:
		{
			switch (reg)
			{
			case 0: // (xxx).w
			{
                uint32_t immediate = fetchNextWord();
                auto it = symbolTable.find(immediate);
                if (it != symbolTable.end())
                {
                    return it->second;
                }
				std::string result = "$";
				result += toHex(immediate);
				result += ".w";
				return result;
			}
			case 1: // (xxx).l
			{
				uint32_t immediate = fetchNextWord() << 16;
				immediate |= fetchNextWord();
                auto it = symbolTable.find(immediate);
                if (it != symbolTable.end())
                {
                    return it->second;
                }
                std::string result = "$";
                result += toHex(immediate);
				result += ".l";
				return result;
			}
			case 2: // d16(PC)
			{
				auto displacement = fetchNextWord();
				std::string result = "offset_0x" + toHex(displacement) + "(pc)";
				return result;
			}
			case 3: // d8(pc,xn)
			{
				auto extension = fetchNextWord();
				bool isAddressRegister = extension & 0x8000;
				unsigned short extensionReg = (extension >> 12) & 7;
				bool isLongIndexSize = (extension & 0x0800);
				unsigned short scale = (extension >> 9) & 3;
				auto displacement = (extension & 0xff);

				std::string result = "offset_0x" + toHex(displacement) + "(pc,";
				result += isAddressRegister ? aregisters[extensionReg] : dregisters[extensionReg];
				result += ")";
				return result;
			}
			case 4: // #
			{
				if (isLongOperation)
				{
					uint32_t immediate = fetchNextWord() << 16;
					immediate |= fetchNextWord();
                    auto it = symbolTable.find(immediate);
                    if (it != symbolTable.end())
                    {
                        return it->second;
                    }
                    std::string result = "#$";
					result += toHex(immediate);
                    return result;
                }
				else
				{
                    uint32_t immediate = fetchNextWord();
                    auto it = symbolTable.find(immediate);
                    if (it != symbolTable.end())
                    {
                        return it->second;
                    }
                    std::string result = "#$";
					result += toHex(immediate);
                    return result;
                }
			}
			default:
				return "<ea>";
			}
		}
		default:
			return "<ea>";
		}
	}

	unsigned short DisAsm::disassembleImmediateInstruction(const char* instructionName, unsigned short instructionId, unsigned short opcode)
	{
		const char* sizes[] = { ".b #$", ".w #$", ".l #$" };

		unsigned short lsb = (opcode & 0xff);
		unsigned short size = lsb >> 6;
		unsigned short effectiveAddress = lsb & 0b111111u;

		disassembly = instructionName;
		disassembly += sizes[size];

		unsigned short immediate = fetchNextWord();
		if (size == 0)
		{
			disassembly += toHex(immediate & 0xff);
		}
		else if (size == 1)
		{
			disassembly += toHex(immediate);
		}
		else
		{
			uint32_t value = immediate << 16;
			value |= fetchNextWord();
			disassembly += toHex(value);
		}
		disassembly += ",";
		disassembly += decodeEffectiveAddress(effectiveAddress, size == 0b10);

		return instructions::CMPI;
	}

	/// <summary>
	/// Disassemble Bcc familly instructions
	/// </summary>
	uint16_t DisAsm::disassembleBccInstruction(const char* instructionName, uint16_t instructionId, uint16_t opcode)
	{
		disassembly = instructionName;
		disassembly += " ";

		uint8_t offset = opcode & 0xff;
		if (offset == 0)
		{
			disassembly += toHexDollar(fetchRelativeAddress());
		}
		else
		{
			disassembly += toHexDollar(origin + (pc * 2) + (int8_t) offset);
		}

		return instructionId;
	}

	/// <summary>
	/// Handle ADDX and SUBX instructions
	/// </summary>
	uint16_t DisAsm::disassembleAddxSubx(const char* instructionName, uint16_t instructionId, uint16_t opcode)
	{
		disassembly = instructionName;

		uint16_t size = (opcode >> 6) & 0b11;
		disassembly += Sizes[size];

		unsigned short registerX = (opcode >> 9) & 7;
		unsigned short registerY = opcode & 7;
		bool isMemoryOperation = opcode & 8;

		if (isMemoryOperation)
		{
			disassembly += AddressRegisterIndirectPre[registerY];
			disassembly += ",";
			disassembly += AddressRegisterIndirectPre[registerX];
		}
		else
		{
			disassembly += dregisters[registerY];
			disassembly += ",";
			disassembly += dregisters[registerX];
		}
		return instructionId;
	}

	/// <summary>
	/// Handle BCHG, BCLR, BSET, BTST instructions when bit index is from register
	/// </summary>
	uint16_t DisAsm::disassembleBitRegisterInstruction(const char* name, uint16_t instruction, uint16_t opcode)
	{
		disassembly = name;
		uint16_t reg = (opcode >> 9) & 0b111;
		disassembly += dregisters[reg];
		disassembly += ",";
		disassembly += decodeEffectiveAddress(opcode & 0b111'111, Ignore);

		return instruction;
	}

	/// <summary>
	/// Handle BCHG, BCLR, BSET, BTST instructions when bit index is from immediate data
	/// </summary>
	uint16_t DisAsm::disassembleBitImmediateInstruction(const char* name, uint16_t instruction, uint16_t opcode)
	{
		disassembly = name;
		disassembly += " #";
		uint16_t bit = fetchNextWord();
		disassembly += toHexDollar(bit);
		disassembly += ",";
		disassembly += decodeEffectiveAddress(opcode & 0b111'111, Ignore);

		return instruction;
	}

	uint16_t DisAsm::disassembleLogical(const char* name, uint16_t instruction, uint16_t opcode)
	{
		disassembly = name;
		uint16_t size = (opcode >> 6) & 0b11;
		disassembly += Sizes[size];

		uint16_t reg = (opcode >> 9) & 0b111;
		bool fromRegister = opcode & (1 << 8);
		if (fromRegister)
		{
			disassembly += dregisters[reg];
			disassembly += ",";
			disassembly += decodeEffectiveAddress(opcode & 0b111'111, size == 2);
		}
		else
		{
			disassembly += decodeEffectiveAddress(opcode & 0b111'111, size == 2);
			disassembly += ",";
			disassembly += dregisters[reg];
		}
		return instruction;
	}

	uint16_t DisAsm::disassemble2ccr(const char* name, uint16_t instruction, uint16_t opcode)
	{
		disassembly = name;
		disassembly += " #$";
		disassembly += toHex(fetchNextWord());
		disassembly += ",ccr";
		return instruction;
	}

	uint16_t DisAsm::disassemble2sr(const char* name, uint16_t instruction, uint16_t opcode)
	{
		disassembly = name;
		disassembly += " #$";
		disassembly += toHex(fetchNextWord());
		disassembly += ",sr";
		return instruction;
	}

	/// <summary>
	/// Decode a registers list into a string
	/// </summary>
	/// <param name="registers">The register list from movem</param>
	/// <param name="isPredecrement">if true indicates a predecrement operation</param>
	/// <returns>The list of registers as a string</returns>
	std::string DisAsm::registersToString(uint16_t registers, bool isPredecrement)
	{
		std::string result;
		bool first = true;
		bool started = false;
		if (isPredecrement)
		{
			int startIndex = 0;
			for (int i = 0; i < 8; i++)
			{
				if (registers & (1 << (15 - i)))
				{
					if (!started)
					{

						result += first ? "d" : "/d";
						result += std::to_string(i);
						started = true;
						startIndex = i;
						first = false;
					}
					else if (i == 7)
					{
						result += "-d7";
					}
				}
				else
				{
					if (started)
					{
						if (startIndex != i - 1)
						{
							result += "-d";
							result += std::to_string(i - 1);
						}
						started = false;
					}
				}
			}
			started = false;
			for (int i = 0; i < 8; i++)
			{
				if (registers & (1 << (7 - i)))
				{
					if (!started)
					{
						result += first ? "a" : "/a";
						result += std::to_string(i);
						started = true;
						startIndex = i;
						first = false;
					}
					else if (i == 7)
					{
						result += "-a7";
					}
				}
				else
				{
					if (started)
					{
						if (startIndex != i - 1)
						{
							result += "-a";
							result += std::to_string(i - 1);
						}
						started = false;
					}
				}
			}
		}
		else
		{
			int startIndex = 0;
			for (int i = 0; i < 8; i++)
			{
				if (registers & (1 << i))
				{
					if (!started)
					{
						result += first ? "d" : "/d";
						result += std::to_string(i);
						started = true;
						startIndex = i;
						first = false;
					}
					else if (i == 7)
					{
						result += "-d7";
					}
				}
				else
				{
					if (started)
					{
						if (startIndex != i - 1)
						{
							result += "-d";
							result += std::to_string(i - 1);
						}
						started = false;
					}
				}
			}
			started = false;
			for (int i = 0; i < 8; i++)
			{
				if (registers & (1 << (i + 8)))
				{
					if (!started)
					{
						result += first ? "a" : "/a";
						result += std::to_string(i);
						started = true;
						startIndex = i;
						first = false;
					}
					else if (i == 7)
					{
						result += "-a7";
					}
				}
				else
				{
					if (started)
					{
						if (startIndex != i - 1)
						{
							result += "-a";
							result += std::to_string(i - 1);
						}
						started = false;
					}
				}
			}
		}
		return result;
	}

	uint16_t DisAsm::disassembleMulDiv(const char* name, uint16_t instruction, uint16_t opcode)
	{
		disassembly = name;
		uint16_t reg = (opcode >> 9) & 0b111;
		uint16_t sourceEffectiveAddress = opcode & 0b111'111;

		disassembly += decodeEffectiveAddress(sourceEffectiveAddress, AlwaysWord);
		disassembly += ",";
		disassembly += dregisters[reg];

		return instruction;
	}

	uint16_t DisAsm::disassembleShiftRotate(const char* instructionName, uint16_t instructionId, uint16_t opcode)
	{
		disassembly = instructionName;
		uint16_t size = (opcode >> 6) & 0b11;
		if (size != 0b11)
		{
			disassembly += Sizes[size];

			uint16_t source = (opcode >> 9) & 0b111;
			uint16_t destination = opcode & 0b111;

			bool fromRegister = opcode & (1 << 5);
			if (fromRegister)
			{
				disassembly += dregisters[source];
				disassembly += ",";
				disassembly += dregisters[destination];
			}
			else
			{
				disassembly += "#"; 
				disassembly += std::to_string(source ? source : 8);
				disassembly += ",";
				disassembly += dregisters[destination];
			}
		}
		else
		{
			uint16_t sourceEffectiveAddress = opcode & 0b111'111;
			disassembly += decodeEffectiveAddress(sourceEffectiveAddress, Ignore);
		}
		return instructionId;
	}


}
