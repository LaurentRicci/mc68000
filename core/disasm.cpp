#include <string>
#include <iostream>
#include <sstream>

#include "core.h"
#include "instructions.h"
#include "disasm.h"

namespace mc68000
{
	extern const char* const dregisters[8] = { "d0", "d1", "d2", "d3", "d4", "d5", "d6", "d7" };
	extern const char* const aregisters[8] = { "a0", "a1", "a2", "a3", "a4", "a5", "a6", "a7" };
	extern const char* const AddressRegisterIndirect[8] = { "(a0)", "(a1)", "(a2)", "(a3)", "(a4)", "(a5)", "(a6)", "(a7)" };
	extern const char* const AddressRegisterIndirectPost[8] = { "(a0)+", "(a1)+", "(a2)+", "(a3)+", "(a4)+", "(a5)+", "(a6)+", "(a7)+" };
	extern const char* const AddressRegisterIndirectPre[8] = { "-(a0)", "-(a1)", "-(a2)", "-(a3)", "-(a4)", "-(a5)", "-(a6)", "-(a7)" };
	extern const char* const SizesForImmediateInstructions[] = { ".b #$", ".w #$", ".l #$" };
	extern const char* const Sizes[] = { ".b ", ".w ", ".l " };


	DisAsm::DisAsm() : pc(nullptr), memory(nullptr)
	{
		handlers = setup<DisAsm>();
	}

	DisAsm::~DisAsm()
	{
		delete handlers;
	}

	std::string DisAsm::disassemble(const uint16_t* code)
	{
		reset(code);

		auto resultCode = (this->*handlers[*code])(*code);
		return disassembly;
	}

	/// <summary>
	/// ABCD: Add Binary Coded Decimal
	/// </summary>
	uint16_t DisAsm::abcd(uint16_t opcode)
	{
		disassembly = "abcd ";
		uint16_t registerX = (opcode >> 9) & 7;
		uint16_t registerY = opcode & 7;
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

	/// <summary>
	/// ADD: Add
	/// </summary>
	uint16_t DisAsm::add(uint16_t opcode)
	{
		disassembly = "add";
		uint16_t reg = (opcode >> 9) & 7;
		bool isMemoryDestination = opcode & 0x100;
		uint16_t size = (opcode >> 6) & 3;

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

	/// <summary>
	/// ADDA: Add Address
	/// </summary>
	uint16_t DisAsm::adda(uint16_t opcode)
	{
		disassembly = "adda";
		uint16_t reg = (opcode >> 9) & 0b111;
		uint16_t size = (opcode >> 8) & 1;
		disassembly += Sizes[size+1];
		disassembly += decodeEffectiveAddress(opcode & 0b111111u, size);
		disassembly += ",";
		disassembly += aregisters[reg];

		return instructions::ADDA;
	}

	/// <summary>
	/// ADDI: Add Immediate
	/// </summary>
	uint16_t DisAsm::addi(uint16_t opcode)
	{
		return disassembleImmediateInstruction("addi", instructions::ADDI, opcode);
	}

	/// <summary>
	/// ADDQ: Add Quick
	/// </summary>
	uint16_t DisAsm::addq(uint16_t opcode)
	{
		disassembly = "addq";
		uint16_t size = (opcode >> 6) & 0b11;
		uint16_t data = (opcode >> 9) & 0b111;
		disassembly += Sizes[size];
		disassembly += "#" + std::to_string(data);
		disassembly += ",";
		disassembly += decodeEffectiveAddress(opcode & 0b111111u);
		return instructions::ADDQ;
	}

	/// <summary>
	/// ADDX: Add extended
	/// </summary>
	uint16_t DisAsm::addx(uint16_t opcode)
	{
		return disassembleAddxSubx("addx", instructions::ADDX, opcode);
	}

	/// <summary>
	/// AND: And logical
	/// </summary>
	uint16_t DisAsm::and_(uint16_t opcode)
	{
		disassembly = "and";
		uint16_t size = (opcode >> 6) & 0b11;
		disassembly += Sizes[size];

		uint16_t reg = (opcode >> 9) & 0b111;
		bool fromRegister = opcode & (1 << 8);
		if (fromRegister)
		{
			disassembly += dregisters[reg];
			disassembly += ",";
			disassembly += decodeEffectiveAddress(opcode & 0b111'111);
		}
		else
		{
			disassembly += decodeEffectiveAddress(opcode & 0b111'111);
			disassembly += ",";
			disassembly += dregisters[reg];
		}
		return instructions::AND;
	}

	/// <summary>
	/// ANDI: And immediate
	/// </summary>
	uint16_t DisAsm::andi(uint16_t opcode)
	{
		return disassembleImmediateInstruction("andi", instructions::ANDI, opcode);
	}

	uint16_t DisAsm::andi2ccr(uint16_t)
	{
		return instructions::ANDI2CCR;
	}
	uint16_t DisAsm::andi2sr(uint16_t)
	{
		return instructions::ANDI2SR;
	}
	uint16_t DisAsm::asl_memory(uint16_t)
	{
		return instructions::ASL;
	}

	uint16_t DisAsm::asl_register(uint16_t)
	{
		return instructions::ASL;
	}

	uint16_t DisAsm::asr_memory(uint16_t)
	{
		return instructions::ASR;
	}

	uint16_t DisAsm::asr_register(uint16_t)
	{
		return instructions::ASR;
	}

	uint16_t DisAsm::bra(uint16_t)
	{
		return instructions::BRA;
	}
	uint16_t DisAsm::bhi(uint16_t opcode)
	{
		return disassembleBccInstruction("bhi", instructions::BHI, opcode);
	}
	uint16_t DisAsm::bls(uint16_t opcode)
	{
		return disassembleBccInstruction("bls", instructions::BLS, opcode);
	}
	uint16_t DisAsm::bcc(uint16_t opcode)
	{
		return disassembleBccInstruction("bcc", instructions::BCC, opcode);
	}
	uint16_t DisAsm::bcs(uint16_t opcode)
	{
		return disassembleBccInstruction("bcs", instructions::BCS, opcode);
	}
	uint16_t DisAsm::bne(uint16_t opcode)
	{
		return disassembleBccInstruction("bne", instructions::BNE, opcode);
	}
	uint16_t DisAsm::beq(uint16_t opcode)
	{
		return disassembleBccInstruction("beq", instructions::BEQ, opcode);
	}
	uint16_t DisAsm::bvc(uint16_t opcode)
	{
		return disassembleBccInstruction("bvc", instructions::BVC, opcode);
	}
	uint16_t DisAsm::bvs(uint16_t opcode)
	{
		return disassembleBccInstruction("bvs", instructions::BVS, opcode);
	}
	uint16_t DisAsm::bpl(uint16_t opcode)
	{
		return disassembleBccInstruction("bpl", instructions::BPL, opcode);
	}
	uint16_t DisAsm::bmi(uint16_t opcode)
	{
		return disassembleBccInstruction("bmi", instructions::BMI, opcode);
	}
	uint16_t DisAsm::bge(uint16_t opcode)
	{
		return disassembleBccInstruction("bge", instructions::BGE, opcode);
	}
	uint16_t DisAsm::blt(uint16_t opcode)
	{
		return disassembleBccInstruction("blt", instructions::BLT, opcode);
	}
	uint16_t DisAsm::bgt(uint16_t opcode)
	{
		return disassembleBccInstruction("bgt", instructions::BGT, opcode);
	}
	uint16_t DisAsm::ble(uint16_t opcode)
	{
		return disassembleBccInstruction("ble", instructions::BLE, opcode);
	}

	uint16_t DisAsm::bchg_r(uint16_t opcode)
	{
		disassembly = "bchg ";
		uint16_t reg = (opcode >> 9) & 0b111;
		disassembly += dregisters[reg];
		disassembly += ",";
		disassembly += decodeEffectiveAddress(opcode & 0b111'111);

		return instructions::BCHG_R;
	}

	uint16_t DisAsm::bchg_i(uint16_t opcode)
	{
		disassembly = "bchg #";
		uint16_t bit = fetchNextWord();
		disassembly += toHexDollar(bit);
		disassembly += ",";
		disassembly += decodeEffectiveAddress(opcode & 0b111'111);

		return instructions::BCHG_I;
	}

	uint16_t DisAsm::bclr_r(uint16_t)
	{
		return instructions::BCLR_R;
	}

	uint16_t DisAsm::bclr_i(uint16_t)
	{
		return instructions::BCLR_I;
	}

	uint16_t DisAsm::bset_r(uint16_t)
	{
		return instructions::BSET_R;
	}

	uint16_t DisAsm::bset_i(uint16_t)
	{
		return instructions::BSET_I;
	}

	uint16_t DisAsm::bsr(uint16_t)
	{
		return instructions::BSR;
	}

	uint16_t DisAsm::btst_r(uint16_t)
	{
		return instructions::BTST_R;
	}

	uint16_t DisAsm::btst_i(uint16_t)
	{
		return instructions::BTST_I;
	}

	uint16_t DisAsm::chk(uint16_t)
	{
		return instructions::CHK;
	}

	uint16_t DisAsm::clr(uint16_t)
	{
		return instructions::CLR;
	}


	uint16_t DisAsm::cmp(uint16_t)
	{
		return instructions::CMP;
	}

	uint16_t DisAsm::cmpa(uint16_t)
	{
		return instructions::CMPA;
	}

	uint16_t DisAsm::cmpi(uint16_t opcode)
	{
		return disassembleImmediateInstruction("cmpi", instructions::CMPI, opcode);
	}

	uint16_t DisAsm::cmpm(uint16_t)
	{
		return instructions::CMPM;
	}

	uint16_t DisAsm::dbcc(uint16_t)
	{
		return instructions::DBCC;
	}

	uint16_t DisAsm::divs(uint16_t)
	{
		return instructions::DIVS;
	}

	uint16_t DisAsm::divu(uint16_t)
	{
		return instructions::DIVU;
	}

	uint16_t DisAsm::eor(uint16_t)
	{
		return instructions::EOR;
	}

	uint16_t DisAsm::eori(uint16_t)
	{
		return instructions::EORI;
	}

	uint16_t DisAsm::eori2ccr(uint16_t)
	{
		return instructions::EORI2CCR;
	}

	uint16_t DisAsm::exg(uint16_t)
	{
		return instructions::EXG;
	}

	uint16_t DisAsm::ext(uint16_t)
	{
		return instructions::EXT;
	}

	uint16_t DisAsm::illegal(uint16_t)
	{
		return instructions::ILLEGAL;
	}

	uint16_t DisAsm::jmp(uint16_t)
	{
		return instructions::JMP;
	}

	uint16_t DisAsm::jsr(uint16_t)
	{
		return instructions::JSR;
	}

	uint16_t DisAsm::lea(uint16_t)
	{
		return instructions::LEA;
	}

	uint16_t DisAsm::link(uint16_t opcode)
	{
		disassembly = "link ";
		uint16_t reg = opcode & 7u;
		disassembly += aregisters[reg];
		auto displacement = fetchNextWord();
		disassembly += ",#";
		disassembly += std::to_string(displacement);
		return instructions::LINK;
	}

	uint16_t DisAsm::fetchNextWord()
	{
		pc++;
		return *pc;
	}

	void DisAsm::reset(const uint16_t* mem)
	{
		memory = mem;
		pc = mem;
	}

	uint16_t DisAsm::lsl_memory(uint16_t)
	{
		return instructions::LSL;
	}

	uint16_t DisAsm::lsl_register(uint16_t)
	{
		return instructions::LSL;
	}

	uint16_t DisAsm::lsr_memory(uint16_t)
	{
		return instructions::LSR;
	}

	uint16_t DisAsm::lsr_register(uint16_t)
	{
		return instructions::LSR;
	}

	uint16_t DisAsm::move(uint16_t opcode)
	{
		const char* sizes[4] = { "?", "b", "l", "w" };

		disassembly = "move.";
		disassembly += sizes[opcode >> 12];
		disassembly += " ";

		uint16_t sourceEffectiveAddress = opcode & 0b111111u;
		disassembly += decodeEffectiveAddress(sourceEffectiveAddress);
		disassembly += ",";

		uint16_t destination = (opcode >> 6) & 0b111111u;
		// the destination is inverted: register - mode instead of mode - register
		uint16_t destinationRegister = destination & 0b111u;
		uint16_t destinationMode = destination >> 3;
		uint16_t destinationEffectiveAddress = (destinationRegister << 3) | destinationMode;

		disassembly += decodeEffectiveAddress(destinationEffectiveAddress);

		return instructions::MOVE;
	}

	uint16_t DisAsm::movea(uint16_t opcode)
	{
		disassembly = "movea";
		bool isWordSize = opcode & 0b0001'0000'0000'0000u;
		disassembly += isWordSize ? ".w " : ".l ";
		disassembly += decodeEffectiveAddress(opcode & 0b111111u, !isWordSize);
		disassembly += ",";
		uint16_t reg = (opcode & 0b0000'1110'0000'0000u) >> 9;
		disassembly += aregisters[reg];

		return instructions::MOVEA;
	}

	uint16_t DisAsm::move2ccr(uint16_t)
	{
		return instructions::MOVE2CCR;
	}

	uint16_t DisAsm::movesr(uint16_t)
	{
		return instructions::MOVESR;
	}

	uint16_t DisAsm::movem(uint16_t)
	{
		return instructions::MOVEM;
	}

	uint16_t DisAsm::movep(uint16_t)
	{
		return instructions::MOVEP;
	}

	uint16_t DisAsm::moveq(uint16_t)
	{
		return instructions::MOVEQ;
	}

	uint16_t DisAsm::muls(uint16_t)
	{
		return instructions::MULS;
	}

	uint16_t DisAsm::mulu(uint16_t)
	{
		return instructions::MULU;
	}

	uint16_t DisAsm::nbcd(uint16_t)
	{
		return instructions::NBCD;
	}

	uint16_t DisAsm::neg(uint16_t)
	{
		return instructions::NEG;
	}
	uint16_t DisAsm::negx(uint16_t)
	{
		return instructions::NEGX;
	}
	uint16_t DisAsm::nop(uint16_t)
	{
		return instructions::NOP;
	}
	uint16_t DisAsm::not_(uint16_t)
	{
		return instructions::NOT;
	}

	uint16_t DisAsm::or_(uint16_t)
	{
		return instructions::OR;
	}

	uint16_t DisAsm::ori(uint16_t)
	{
		return instructions::ORI;
	}

	uint16_t DisAsm::ori2ccr(uint16_t)
	{
		return instructions::ORI2CCR;
	}

	uint16_t DisAsm::pea(uint16_t)
	{
		return instructions::PEA;
	}

	uint16_t DisAsm::rol_memory(uint16_t)
	{
		return instructions::ROL;
	}

	uint16_t DisAsm::ror_memory(uint16_t)
	{
		return instructions::ROR;
	}

	uint16_t DisAsm::roxl_memory(uint16_t)
	{
		return instructions::ROXL;
	}

	uint16_t DisAsm::roxr_memory(uint16_t)
	{
		return instructions::ROXR;
	}

	uint16_t DisAsm::rol_register(uint16_t)
	{
		return instructions::ROL;
	}

	uint16_t DisAsm::ror_register(uint16_t)
	{
		return instructions::ROR;
	}

	uint16_t DisAsm::roxl_register(uint16_t)
	{
		return instructions::ROXL;
	}

	uint16_t DisAsm::roxr_register(uint16_t)
	{
		return instructions::ROXR;
	}

	uint16_t DisAsm::sbcd(uint16_t)
	{
		return instructions::SBCD;
	}

	uint16_t DisAsm::rtr(uint16_t)
	{
		return instructions::RTR;
	}

	uint16_t DisAsm::rts(uint16_t)
	{
		disassembly = "rts";
		return instructions::RTS;
	}

	uint16_t DisAsm::scc(uint16_t)
	{
		return instructions::SCC;
	}

	uint16_t DisAsm::sub(uint16_t)
	{
		return instructions::SUB;
	}

	uint16_t DisAsm::subi(uint16_t)
	{
		return instructions::SUBI;
	}

	uint16_t DisAsm::suba(uint16_t)
	{
		return instructions::SUBA;
	}

	uint16_t DisAsm::subq(uint16_t)
	{
		return instructions::SUBQ;
	}

	/// <summary>
	/// SUBX: Sub extended
	/// </summary>
	uint16_t DisAsm::subx(uint16_t opcode)
	{
		return disassembleAddxSubx("subx", instructions::SUBX, opcode);
	}

	uint16_t DisAsm::swap(uint16_t)
	{
		return instructions::SWAP;
	}

	uint16_t DisAsm::tas(uint16_t)
	{
		return instructions::TAS;
	}

	uint16_t DisAsm::trap(uint16_t)
	{
		return instructions::TRAP;
	}

	uint16_t DisAsm::trapv(uint16_t)
	{
		return instructions::TRAPV;
	}

	uint16_t DisAsm::tst(uint16_t)
	{
		return instructions::TST;
	}

	uint16_t DisAsm::unlk(uint16_t opcode)
	{
		disassembly = "unlk a";
		disassembly += toHex(opcode & 0b111u);
		return instructions::UNLK;
	}

	uint16_t DisAsm::unknown(uint16_t)
	{
		return instructions::UNKNOWN;
	}

}