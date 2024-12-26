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
				std::string result = "$";
				result += toHex(fetchNextWord());
				result += ".w";
				return result;
			}
			case 1: // (xxx).l
			{
				std::string result = "$";
				uint32_t immediate = fetchNextWord() << 16;
				immediate |= fetchNextWord();
				result += toHex(immediate);
				result += ".l";
				return result;
			}
			case 2: // d16(PC)
			{
				auto displacement = fetchNextWord();
				std::string result = std::to_string(displacement) + "(pc)";
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

				std::string result = std::to_string(displacement) + "(pc,";
				result += isAddressRegister ? aregisters[extensionReg] : dregisters[extensionReg];
				result += ")";
				return result;
			}
			case 4: // #
			{
				std::string result = "#$";
				
				if (isLongOperation)
				{
					uint32_t immediate = fetchNextWord() << 16;
					immediate |= fetchNextWord();
					result += toHex(immediate);
				}
				else
				{
					result += toHex(fetchNextWord());
				}
				return result;
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
			disassembly += toHex(immediate);
			disassembly += toHex(fetchNextWord());
		}
		disassembly += ",";
		disassembly += decodeEffectiveAddress(effectiveAddress);

		return instructions::CMPI;
	}

	/// <summary>
	/// Disassemble Bcc familly instructions
	/// </summary>
	uint16_t DisAsm::disassembleBccInstruction(const char* instructionName, uint16_t instructionId, uint16_t opcode)
	{
		disassembly = instructionName;
		disassembly += " $";

		uint16_t offset = opcode & 0xff;
		if (offset == 0)
		{
			disassembly += toHex(fetchNextWord());
		}
		else if (offset == 0xff)
		{
			disassembly += toHex(fetchNextWord());
			disassembly += toHex(fetchNextWord());
		}
		else
		{
			disassembly += toHex(offset);
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


}