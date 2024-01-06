#include <string>
#include <iostream>
#include <sstream>

#include "core.h"
#include "instructions.h"
#include "disasm.h"

namespace mc68000
{
	const char* const dregisters[8] = { "d0", "d1", "d2", "d3", "d4", "d5", "d6", "d7" };
	const char* const aregisters[8] = { "a0", "a1", "a2", "a3", "a4", "a5", "a6", "a7" };
	const char* const AddressRegisterIndirect[8] = { "(a0)", "(a1)", "(a2)", "(a3)", "(a4)", "(a5)", "(a6)", "(a7)" };
	const char* const AddressRegisterIndirectPost[8] = { "(a0)+", "(a1)+", "(a2)+", "(a3)+", "(a4)+", "(a5)+", "(a6)+", "(a7)+" };
	const char* const AddressRegisterIndirectPre[8] = { "-(a0)", "-(a1)", "-(a2)", "-(a3)", "-(a4)", "-(a5)", "-(a6)", "-(a7)" };
	const char* const SizesForImmediateInstructions[] = { ".b #$", ".w #$", ".l #$" };
	const char* const Sizes[] = { ".b ", ".w ", ".l " };


	std::string toHex(unsigned short value)
	{
		// return std::format("{x}", value);

		std::ostringstream stream;
		stream << std::hex << value;
		return stream.str();
	}

	DisAsm::DisAsm() : pc(nullptr), memory(nullptr)
	{
		handlers = setup<DisAsm>();
	}

	DisAsm::~DisAsm()
	{
		delete handlers;
	}

	std::string DisAsm::disassemble(const unsigned short* code)
	{
		reset(code);

		auto resultCode = (this->*handlers[*code])(*code);
		return disassembly;
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
					std::string result = "$";
					result += toHex( fetchNextWord());
					result += ".w";
					return result;
				}
				case 1: // (xxx).l
				{
					std::string result = "$";
					result += toHex( fetchNextWord());
					result += toHex(fetchNextWord());
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
					result += toHex(fetchNextWord());
					if (isLongOperation)
					{
						result += toHex(fetchNextWord());
						result += ".l";
					}
					else
					{
						result += ".w";
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

	unsigned short DisAsm::abcd(unsigned short opcode)
	{
		disassembly = "abcd ";
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

		return instructions::ABCD;
	}

	unsigned short DisAsm::add(unsigned short opcode)
	{
		disassembly = "add";
		unsigned short reg = (opcode >> 9) & 7;
		bool isMemoryDestination = opcode & 0x100;
		unsigned short size = (opcode >> 6) & 3;

		disassembly += Sizes[size];
		if (isMemoryDestination)
		{
			disassembly += dregisters[reg];
			disassembly += ",";
			disassembly += decodeEffectiveAddress(opcode & 0b111111u);
		}
		else
		{
			disassembly += decodeEffectiveAddress(opcode & 0b111111u);
			disassembly += ",";
			disassembly += dregisters[reg];
		}
		return instructions::ADD;
	}

	unsigned short DisAsm::adda(unsigned short)
	{
		return instructions::ADDA;
	}

	unsigned short DisAsm::addi(unsigned short opcode)
	{
		return disassembleImmediateInstruction("addi", instructions::ADDI, opcode);
	}

	unsigned short DisAsm::addq(unsigned short)
	{
		return instructions::ADDQ;
	}

	unsigned short DisAsm::addx(unsigned short)
	{
		return instructions::ADDX;
	}

	unsigned short DisAsm::and_(unsigned short)
	{
		return instructions::AND;
	}

	unsigned short DisAsm::andi(unsigned short)
	{
		return instructions::ANDI;
	}

	unsigned short DisAsm::andi2ccr(unsigned short)
	{
		return instructions::ANDI2CCR;
	}
	unsigned short DisAsm::andi2sr(unsigned short)
	{
		return instructions::ANDI2SR;
	}
	unsigned short DisAsm::asl_memory(unsigned short)
	{
		return instructions::ASL;
	}

	unsigned short DisAsm::asl_register(unsigned short)
	{
		return instructions::ASL;
	}

	unsigned short DisAsm::asr_memory(unsigned short)
	{
		return instructions::ASR;
	}

	unsigned short DisAsm::asr_register(unsigned short)
	{
		return instructions::ASR;
	}

	unsigned short DisAsm::disassembleBccInstruction(const char* instructionName, unsigned short instructionId, unsigned short opcode)
	{
		disassembly = instructionName;
		disassembly += " $";

		unsigned short offset = opcode & 0xff;
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

	unsigned short DisAsm::bra(unsigned short)
	{
		return instructions::BRA;
	}
	unsigned short DisAsm::bhi(unsigned short opcode)
	{
		return disassembleBccInstruction("bhi", instructions::BHI, opcode);
	}
	unsigned short DisAsm::bls(unsigned short opcode)
	{
		return disassembleBccInstruction("bls", instructions::BLS, opcode);
	}
	unsigned short DisAsm::bcc(unsigned short opcode)
	{
		return disassembleBccInstruction("bcc", instructions::BCC, opcode);
	}
	unsigned short DisAsm::bcs(unsigned short opcode)
	{
		return disassembleBccInstruction("bcs", instructions::BCS, opcode);
	}
	unsigned short DisAsm::bne(unsigned short opcode)
	{
		return disassembleBccInstruction("bne", instructions::BNE, opcode);
	}
	unsigned short DisAsm::beq(unsigned short opcode)
	{
		return disassembleBccInstruction("beq", instructions::BEQ, opcode);
	}
	unsigned short DisAsm::bvc(unsigned short opcode)
	{
		return disassembleBccInstruction("bvc", instructions::BVC, opcode);
	}
	unsigned short DisAsm::bvs(unsigned short opcode)
	{
		return disassembleBccInstruction("bvs", instructions::BVS, opcode);
	}
	unsigned short DisAsm::bpl(unsigned short opcode)
	{
		return disassembleBccInstruction("bpl", instructions::BPL, opcode);
	}
	unsigned short DisAsm::bmi(unsigned short opcode)
	{
		return disassembleBccInstruction("bmi", instructions::BMI, opcode);
	}
	unsigned short DisAsm::bge(unsigned short opcode)
	{
		return disassembleBccInstruction("bge", instructions::BGE, opcode);
	}
	unsigned short DisAsm::blt(unsigned short opcode)
	{
		return disassembleBccInstruction("blt", instructions::BLT, opcode);
	}
	unsigned short DisAsm::bgt(unsigned short opcode)
	{
		return disassembleBccInstruction("bgt", instructions::BGT, opcode);
	}
	unsigned short DisAsm::ble(unsigned short opcode)
	{
		return disassembleBccInstruction("ble", instructions::BLE, opcode);
	}

	unsigned short DisAsm::bchg_r(unsigned short)
	{
		return instructions::BCHG_R;
	}

	unsigned short DisAsm::bchg_i(unsigned short)
	{
		return instructions::BCHG_I;
	}

	unsigned short DisAsm::bclr_r(unsigned short)
	{
		return instructions::BCLR_R;
	}

	unsigned short DisAsm::bclr_i(unsigned short)
	{
		return instructions::BCLR_I;
	}

	unsigned short DisAsm::bset_r(unsigned short)
	{
		return instructions::BSET_R;
	}

	unsigned short DisAsm::bset_i(unsigned short)
	{
		return instructions::BSET_I;
	}

	unsigned short DisAsm::bsr(unsigned short)
	{
		return instructions::BSR;
	}

	unsigned short DisAsm::btst_r(unsigned short)
	{
		return instructions::BTST_R;
	}

	unsigned short DisAsm::btst_i(unsigned short)
	{
		return instructions::BTST_I;
	}

	unsigned short DisAsm::chk(unsigned short)
	{
		return instructions::CHK;
	}

	unsigned short DisAsm::clr(unsigned short)
	{
		return instructions::CLR;
	}


	unsigned short DisAsm::cmp(unsigned short)
	{
		return instructions::CMP;
	}

	unsigned short DisAsm::cmpa(unsigned short)
	{
		return instructions::CMPA;
	}

	unsigned short DisAsm::cmpi(unsigned short opcode)
	{
		return disassembleImmediateInstruction("cmpi", instructions::CMPI, opcode);
	}

	unsigned short DisAsm::cmpm(unsigned short)
	{
		return instructions::CMPM;
	}

	unsigned short DisAsm::dbcc(unsigned short)
	{
		return instructions::DBCC;
	}

	unsigned short DisAsm::divs(unsigned short)
	{
		return instructions::DIVS;
	}

	unsigned short DisAsm::divu(unsigned short)
	{
		return instructions::DIVU;
	}

	unsigned short DisAsm::eor(unsigned short)
	{
		return instructions::EOR;
	}

	unsigned short DisAsm::eori(unsigned short)
	{
		return instructions::EORI;
	}

	unsigned short DisAsm::eori2ccr(unsigned short)
	{
		return instructions::EORI2CCR;
	}

	unsigned short DisAsm::exg(unsigned short)
	{
		return instructions::EXG;
	}

	unsigned short DisAsm::ext(unsigned short)
	{
		return instructions::EXT;
	}

	unsigned short DisAsm::illegal(unsigned short)
	{
		return instructions::ILLEGAL;
	}

	unsigned short DisAsm::jmp(unsigned short)
	{
		return instructions::JMP;
	}

	unsigned short DisAsm::jsr(unsigned short)
	{
		return instructions::JSR;
	}

	unsigned short DisAsm::lea(unsigned short)
	{
		return instructions::LEA;
	}

	unsigned short DisAsm::link(unsigned short opcode)
	{
		disassembly = "link ";
		unsigned short reg = opcode & 7u;
		disassembly += aregisters[reg];
		auto displacement = fetchNextWord();
		disassembly += ",#";
		disassembly += std::to_string(displacement);
		return instructions::LINK;
	}

	unsigned short DisAsm::fetchNextWord()
	{
		pc++;
		return *pc;
	}

	void DisAsm::reset(const unsigned short* mem)
	{
		memory = mem;
		pc = mem;
	}

	unsigned short DisAsm::lsl_memory(unsigned short)
	{
		return instructions::LSL;
	}

	unsigned short DisAsm::lsl_register(unsigned short)
	{
		return instructions::LSL;
	}

	unsigned short DisAsm::lsr_memory(unsigned short)
	{
		return instructions::LSR;
	}

	unsigned short DisAsm::lsr_register(unsigned short)
	{
		return instructions::LSR;
	}

	unsigned short DisAsm::move(unsigned short opcode)
	{
		const char* sizes[4] = { "?", "b", "l", "w" };

		disassembly = "move.";
		disassembly += sizes[opcode >> 12];
		disassembly += " ";

		unsigned short sourceEffectiveAddress = opcode & 0b111111u;
		disassembly += decodeEffectiveAddress(sourceEffectiveAddress);
		disassembly += ",";

		unsigned short destination = (opcode >> 6) & 0b111111u;
		// the destination is inverted: register - mode instead of mode - register
		unsigned short destinationRegister = destination & 0b111u;
		unsigned short destinationMode = destination >> 3;
		unsigned short destinationEffectiveAddress = (destinationRegister << 3) | destinationMode;

		disassembly += decodeEffectiveAddress(destinationEffectiveAddress);

		return instructions::MOVE;
	}

	unsigned short DisAsm::movea(unsigned short opcode)
	{
		disassembly = "movea";
		bool isWordSize = opcode & 0b0001'0000'0000'0000u;
		disassembly += isWordSize ? ".w " : ".l ";
		disassembly += decodeEffectiveAddress(opcode & 0b111111u, !isWordSize);
		disassembly += ",";
		unsigned short reg = (opcode & 0b0000'1110'0000'0000u) >> 9;
		disassembly += aregisters[reg];

		return instructions::MOVEA;
	}

	unsigned short DisAsm::move2ccr(unsigned short)
	{
		return instructions::MOVE2CCR;
	}

	unsigned short DisAsm::movesr(unsigned short)
	{
		return instructions::MOVESR;
	}

	unsigned short DisAsm::movem(unsigned short)
	{
		return instructions::MOVEM;
	}

	unsigned short DisAsm::movep(unsigned short)
	{
		return instructions::MOVEP;
	}

	unsigned short DisAsm::moveq(unsigned short)
	{
		return instructions::MOVEQ;
	}

	unsigned short DisAsm::muls(unsigned short)
	{
		return instructions::MULS;
	}

	unsigned short DisAsm::mulu(unsigned short)
	{
		return instructions::MULU;
	}

	unsigned short DisAsm::nbcd(unsigned short)
	{
		return instructions::NBCD;
	}

	unsigned short DisAsm::neg(unsigned short)
	{
		return instructions::NEG;
	}
	unsigned short DisAsm::negx(unsigned short)
	{
		return instructions::NEGX;
	}
	unsigned short DisAsm::nop(unsigned short)
	{
		return instructions::NOP;
	}
	unsigned short DisAsm::not_(unsigned short)
	{
		return instructions::NOT;
	}

	unsigned short DisAsm::or_(unsigned short)
	{
		return instructions::OR;
	}

	unsigned short DisAsm::ori(unsigned short)
	{
		return instructions::ORI;
	}

	unsigned short DisAsm::ori2ccr(unsigned short)
	{
		return instructions::ORI2CCR;
	}

	unsigned short DisAsm::pea(unsigned short)
	{
		return instructions::PEA;
	}

	unsigned short DisAsm::rol_memory(unsigned short)
	{
		return instructions::ROL;
	}

	unsigned short DisAsm::ror_memory(unsigned short)
	{
		return instructions::ROR;
	}

	unsigned short DisAsm::roxl_memory(unsigned short)
	{
		return instructions::ROXL;
	}

	unsigned short DisAsm::roxr_memory(unsigned short)
	{
		return instructions::ROXR;
	}

	unsigned short DisAsm::rol_register(unsigned short)
	{
		return instructions::ROL;
	}

	unsigned short DisAsm::ror_register(unsigned short)
	{
		return instructions::ROR;
	}

	unsigned short DisAsm::roxl_register(unsigned short)
	{
		return instructions::ROXL;
	}

	unsigned short DisAsm::roxr_register(unsigned short)
	{
		return instructions::ROXR;
	}

	unsigned short DisAsm::sbcd(unsigned short)
	{
		return instructions::SBCD;
	}

	unsigned short DisAsm::rtr(unsigned short)
	{
		return instructions::RTR;
	}

	unsigned short DisAsm::rts(unsigned short)
	{
		disassembly = "rts";
		return instructions::RTS;
	}

	unsigned short DisAsm::scc(unsigned short)
	{
		return instructions::SCC;
	}

	unsigned short DisAsm::sub(unsigned short)
	{
		return instructions::SUB;
	}

	unsigned short DisAsm::subi(unsigned short)
	{
		return instructions::SUBI;
	}

	unsigned short DisAsm::suba(unsigned short)
	{
		return instructions::SUBA;
	}

	unsigned short DisAsm::subq(unsigned short)
	{
		return instructions::SUBQ;
	}

	unsigned short DisAsm::subx(unsigned short)
	{
		return instructions::SUBX;
	}

	unsigned short DisAsm::swap(unsigned short)
	{
		return instructions::SWAP;
	}

	unsigned short DisAsm::tas(unsigned short)
	{
		return instructions::TAS;
	}

	unsigned short DisAsm::trap(unsigned short)
	{
		return instructions::TRAP;
	}

	unsigned short DisAsm::trapv(unsigned short)
	{
		return instructions::TRAPV;
	}

	unsigned short DisAsm::tst(unsigned short)
	{
		return instructions::TST;
	}

	unsigned short DisAsm::unlk(unsigned short opcode)
	{
		disassembly = "unlk a";
		disassembly += toHex(opcode & 0b111u);
		return instructions::UNLK;
	}

	unsigned short DisAsm::unknown(unsigned short)
	{
		return instructions::UNKNOWN;
	}

}