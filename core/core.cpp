// core.cpp
#include "core.h"
#include "instructions.h"

bool core::initialized = false;
core::t_handler core::handlers[] = {};

core::core()
{
	if (!core::initialized)
	{
		setup();
		core::initialized = true;
	}
}

int core::operator()(unsigned short opcode)
{
	return (this->*handlers[opcode])(opcode);
}

bool core::isValidAddressingMode(unsigned short ea, unsigned short acceptable)
{
	static const unsigned masks[] = {
		1 << 11, 1 << 10, 1 << 9, 1 << 8, 1 << 7, 1 << 6, 1 << 5, // mode 000 to 110
		1 << 4, 1 << 3, 1 << 2, 1 << 1, 1                         // mode 111 
	};
	unsigned mode = ea >> 3;
	if (mode != 7)
	{
		return (acceptable & masks[mode]) == masks[mode];
	}

	static const unsigned offsets[] = { 7, 8, 10, 11, 9 }; // 000, 001, 100, 010, 011
	mode = ea & 7;
	if (mode > 4)
	{
		return false;
	}
	auto offset = offsets[mode];
	return (acceptable & masks[offset]) == masks[offset];
}

void core::setup()
{
	for (int i = 0; i < 0x10000; i++)
	{
		handlers[i] = &core::illegal;
	}

	// ABCD and SBCD 
	for (int rx = 0; rx <= 7; rx++)
	{
		for (int ry = 0; ry <= 7; ry++)
		{
			handlers[(rx << 9) + (0 << 3) + ry + 0xc100] = &core::abcd;
			handlers[(rx << 9) + (1 << 3) + ry + 0xc100] = &core::abcd;
			handlers[(rx << 9) + (0 << 3) + ry + 0x8100] = &core::sbcd;
			handlers[(rx << 9) + (1 << 3) + ry + 0x8100] = &core::sbcd;
		}
	}

	// ADD ADDA AND CMP CMPA EOR OR SUB SUBA
	for (int reg = 0; reg <= 7; reg++)
	{
		for (int opmode = 0; opmode <= 2; opmode++)
		{
			for (int ea = 0; ea <= 0x3f; ea++)
			{
				handlers[0xd000 + (reg << 9) + (opmode << 6) + ea] = &core::add;
				handlers[0xb000 + (reg << 9) + (opmode << 6) + ea] = &core::cmp;
				handlers[0x9000 + (reg << 9) + (opmode << 6) + ea] = &core::sub;
				if (isValidAddressingMode(ea, 0b101111111111u))
				{
					handlers[0xc000 + (reg << 9) + (opmode << 6) + ea] = &core::and_;
					handlers[0x8000 + (reg << 9) + (opmode << 6) + ea] = &core::or_;
				}
			}
		}
		for (int opmode = 4; opmode <= 6; opmode++)
		{
			for (int ea = 0; ea <= 0x3f; ea++)
			{
				if (isValidAddressingMode(ea, 0b001111111000u))
				{
					handlers[0xd000 + (reg << 9) + (opmode << 6) + ea] = &core::add;
					handlers[0x9000 + (reg << 9) + (opmode << 6) + ea] = &core::sub;
					handlers[0xc000 + (reg << 9) + (opmode << 6) + ea] = &core::and_;
					handlers[0xb000 + (reg << 9) + (opmode << 6) + ea] = &core::eor;
					handlers[0x8000 + (reg << 9) + (opmode << 6) + ea] = &core::or_;
				}
			}
		}
		for (int ea = 0; ea <= 0x3f; ea++)
		{
			handlers[0xd000 + (reg << 9) + (0b011u << 6) + ea] = &core::adda;
			handlers[0xd000 + (reg << 9) + (0b111u << 6) + ea] = &core::adda;
			handlers[0xb000 + (reg << 9) + (0b011u << 6) + ea] = &core::cmpa;
			handlers[0xb000 + (reg << 9) + (0b111u << 6) + ea] = &core::cmpa;
			handlers[0x9000 + (reg << 9) + (0b011u << 6) + ea] = &core::suba;
			handlers[0x9000 + (reg << 9) + (0b111u << 6) + ea] = &core::suba;
		}
	}

	// ADDI ANDI CMPI EORI ORI SUBI
	for (int size = 0; size <= 2; size++)
	{
		for (unsigned ea = 0; ea <= 0b111'111u; ea++)
		{
			if (isValidAddressingMode(ea, 0b101111111000u))
			{
				handlers[0x0600 + (size << 6) + ea] = &core::addi;
				handlers[0x0200 + (size << 6) + ea] = &core::andi;
				handlers[0x0C00 + (size << 6) + ea] = &core::cmpi;
				handlers[0x0A00 + (size << 6) + ea] = &core::eori;
				handlers[0x0000 + (size << 6) + ea] = &core::ori;
				handlers[0x0400 + (size << 6) + ea] = &core::subi;
			}
		}
	}

	// ADDQ SUBQ
	for (int data = 0; data <= 7; data++)
	{
		for (int size = 0; size <= 2; size++)
		{
			for (int ea = 0; ea <= 0x3f; ea++)
			{
				if (isValidAddressingMode(ea, 0b111111111000u))
				{
					handlers[0x5000 + (data << 9) + (0 << 8) + (size << 6) + ea] = &core::addq;
					handlers[0x5000 + (data << 9) + (1 << 8) + (size << 6) + ea] = &core::subq;
				}
			}
		}
	}

	// ADDX SUBX
	for (int rx = 0; rx <= 7; rx++)
	{
		for (int size = 0; size <= 2; size++)
		{
			for (int rm = 0; rm <= 1; rm++)
			{
				for (int ry = 0; ry <= 7; ry++)
				{
					handlers[0xd000 + (rx << 9) + (1 << 8) + (size << 6) + (rm << 3) + rx] = &core::addx;
					handlers[0x9000 + (rx << 9) + (1 << 8) + (size << 6) + (rm << 3) + rx] = &core::subx;
				}
			}
		}
	}

	// ANDI to CCR, to SR
	handlers[0b0000'0010'0011'1100u] = &core::andi2ccr;
	handlers[0b0000'0010'0111'1100u] = &core::andi2sr;

	// ASL and ASR REGISTER SHIFTS
	for (int reg1 = 0; reg1 <= 7; reg1++)
	{
		for (int size = 0; size <= 2; size++)
		{
			for (int ir = 0; ir <= 1; ir++)
			{
				for (int reg2 = 0; reg2 <= 7; reg2++)
				{
					handlers[0xe008 + (reg1 << 9) + (0 << 8) + (size << 6) + (ir << 5) + reg2] = &core::lsr_register;
					handlers[0xe008 + (reg1 << 9) + (1 << 8) + (size << 6) + (ir << 5) + reg2] = &core::lsl_register;

					handlers[0xe000 + (reg1 << 9) + (0 << 8) + (size << 6) + (ir << 5) + reg2] = &core::asr_register;
					handlers[0xe000 + (reg1 << 9) + (1 << 8) + (size << 6) + (ir << 5) + reg2] = &core::asl_register;
				}
			}
		}
	}

	// ASL and ASR MEMORY SHIFTS
	for (unsigned mode = 0; mode <= 0b111'111u; mode++)
	{
		if (!isValidAddressingMode(mode, 0b001111111000u))
		{
			continue;
		}
		handlers[0xe2c0 + (0 << 8) + mode] = &core::lsr_memory;
		handlers[0xe2c0 + (1 << 8) + mode] = &core::lsl_memory;

		handlers[0xe0c0 + (0 << 8) + mode] = &core::asr_memory;
		handlers[0xe0c0 + (1 << 8) + mode] = &core::asl_memory;
	}

	// BRA , BSR, BCC
	for (int displacement = 0; displacement <= 0xff; displacement++)
	{
		handlers[0x6000 + displacement] = &core::bra;
		handlers[0x6100 + displacement] = &core::bsr;
		handlers[0x6200 + displacement] = &core::bhi;
		handlers[0x6300 + displacement] = &core::bls;
		handlers[0x6400 + displacement] = &core::bcc;
		handlers[0x6500 + displacement] = &core::bcs;
		handlers[0x6600 + displacement] = &core::bne;
		handlers[0x6700 + displacement] = &core::beq;
		handlers[0x6800 + displacement] = &core::bvc;
		handlers[0x6900 + displacement] = &core::bvs;
		handlers[0x6A00 + displacement] = &core::bpl;
		handlers[0x6B00 + displacement] = &core::bmi;
		handlers[0x6C00 + displacement] = &core::bge;
		handlers[0x6D00 + displacement] = &core::blt;
		handlers[0x6E00 + displacement] = &core::bgt;
		handlers[0x6F00 + displacement] = &core::ble;
	}

	// BCHG BCLR BSET register
	for (int reg = 0; reg <= 7; reg++)
	{
		for (int ea = 0; ea <= 0x3f; ea++)
		{
			if (isValidAddressingMode(ea, 0b101111111000))
			{
				handlers[0 + (reg << 9) + (0b101u << 6) + ea] = &core::bchg_r;
				handlers[0 + (reg << 9) + (0b110u << 6) + ea] = &core::bclr_r;
				handlers[0 + (reg << 9) + (0b111u << 6) + ea] = &core::bset_r;
			}
		}
	}

	// BCHG BCLR BSET immediate
	for (int ea = 0; ea <= 0x3f; ea++)
	{
		if (isValidAddressingMode(ea, 0b101111111000))
		{
			handlers[(0b0000100001u << 6) + ea] = &core::bchg_i;
			handlers[(0b0000100010u << 6) + ea] = &core::bclr_i;
			handlers[(0b0000100011u << 6) + ea] = &core::bset_i;
		}
	}

	// BTST
	for (int reg = 0; reg <= 7; reg++)
	{
		for (int ea = 0; ea <= 0x3f; ea++)
		{
			if (isValidAddressingMode(ea, 0b101111111111))
			{
				handlers[0x0100 + (reg << 9) + ea] = &core::btst_r;
			}
		}
	}

	for (int ea = 0; ea <= 0x3f; ea++)
	{
		if (isValidAddressingMode(ea, 0b101111111011))
		{
			handlers[(0b0000100000u << 6) + ea] = &core::btst_i;
		}
	}

	// CHK
	for (int reg = 0; reg <= 7; reg++)
	{
		for (int ea = 0; ea <= 0b111'111u; ea++)
		{
			if (isValidAddressingMode(ea, 0b101111111111))
			{
				handlers[0x4000 + (reg << 9) + (0b11u << 7) + ea] = &core::chk;
			}
		}
	}

	// CLR
	for (int size = 0; size <= 2; size++)
	{
		for (unsigned ea = 0; ea <= 0b111'111u; ea++)
		{
			if (isValidAddressingMode(ea, 0b101111111000u))
			{
				handlers[0x4200 + (size << 6) + ea] = &core::clr;
			}
		}
	}

	// CMPM
	for (int regx = 0; regx <= 7; regx++)
	{
		for (int size = 0; size <= 2; size++)
		{
			for (int regy = 0; regy <= 7; regy++)
			{
				handlers[0xb108 + (regx << 9) + (size << 6) + regy] = &core::cmpm;
			}
		}
	}

	// DBCC
	for (int condition = 0; condition <= 0xF; condition++)
	{
		for (int reg = 0; reg <= 7; reg++)
		{
			handlers[0x50c8 + (condition << 8) + reg] = &core::dbcc;
		}
	}

	// DIVS DIVU MULS MULU
	for (int reg = 0; reg <= 7; reg++)
	{
		for (unsigned ea = 0; ea <= 0b111'111u; ea++)
		{
			if (isValidAddressingMode(ea, 0b101111111111u))
			{
				handlers[0x81c0 + (reg << 9) + ea] = &core::divs;
				handlers[0x80c0 + (reg << 9) + ea] = &core::divu;
				handlers[0xc1c0 + (reg << 9) + ea] = &core::muls;
				handlers[0xc0c0 + (reg << 9) + ea] = &core::mulu;
			}
		}
	}

	// EORI TO CCR
	handlers[0b0000'1010'0011'1100u] = &core::eori2ccr;

	// EXG
	for (int regx = 0; regx <= 7; regx++)
	{
		for (int regy = 0; regy <= 7; regy++)
		{
			handlers[0xc100 + (regx << 9) + (0b01000u << 3) + regy] = &core::exg; // Data registers
			handlers[0xc100 + (regx << 9) + (0b01001u << 3) + regy] = &core::exg; // Address registers
			handlers[0xc100 + (regx << 9) + (0b10001u << 3) + regy] = &core::exg; // Data register and address register
		}
	}

	// EXT
	for (int reg = 0; reg <= 7; reg++)
	{
		handlers[0x4800 + (0b010u << 6) + reg] = &core::ext;
		handlers[0x4800 + (0b011u << 6) + reg] = &core::ext;
	}

	// JMP JSR
	for (unsigned ea = 0; ea <= 0b111'111u; ea++)
	{
		if (isValidAddressingMode(ea, 0b001001111011u))
		{
			handlers[0x4EC0 + ea] = &core::jmp;
			handlers[0x4E80 + ea] = &core::jsr;
		}
	}

	// LEA
	for (int reg = 0; reg <= 7; reg++)
	{
		for (unsigned ea = 0; ea <= 0b111'111u; ea++)
		{
			if (isValidAddressingMode(ea, 0b001001111011u))
			{
				handlers[0x41c0 + (reg << 9) + ea] = &core::lea;
			}
		}
	}

	// LINK
	for (int reg = 0; reg <= 7; reg++)
	{
		handlers[0b0100'1110'0101'0000u + reg] = &core::link;
	}

	// MOVE
	for (unsigned size = 1; size <= 3; size++)
	{
		for (unsigned destination = 0; destination <= 0b111'111u; destination++)
		{
			if (isValidAddressingMode(destination, 0b101111111000u))
			{
				for (unsigned source = 0; source <= 0b111'111u; source++)
				{
					if (isValidAddressingMode(destination, 0b111111111111u))
					{
						handlers[(size << 12) + (destination << 6) + source] = &core::move;
					}
				}
			}
		}
	}

	// MOVEA
	for (unsigned size = 2; size <= 3; size++)
	{
		for (unsigned reg = 0; reg <= 7; reg++)
		{
			for (unsigned source = 0; source <= 0b111'111u; source++)
			{
				handlers[(size << 12) + (reg << 9) + (1 << 6) + source] = &core::movea;
			}
		}
	}

	// MOVE TO CCR
	for (unsigned ea = 0; ea <= 0b111'111u; ea++)
	{
		if (isValidAddressingMode(ea, 0b101111111111u))
		{
			handlers[0x44c0 + ea] = &core::move2ccr;
		}
	}

	// MOVE FROM SR
	for (unsigned ea = 0; ea <= 0b111'111u; ea++)
	{
		if (isValidAddressingMode(ea, 0b101111111000u))
		{
			handlers[0x40c0 + ea] = &core::movesr;
		}
	}

	// MOVEM - Register to memory
	for (unsigned ea = 0; ea <= 0b111'111u; ea++)
	{
		if (isValidAddressingMode(ea, 0b001011111000u))
		{
			handlers[0x4880 + (0 << 6) + ea] = &core::movem; // Register to memory size = W
			handlers[0x4880 + (1 << 6) + ea] = &core::movem; // Register to memory size = L
		}
		if (isValidAddressingMode(ea, 0b001101111011u))
		{
			handlers[0x4c80 + (0 << 6) + ea] = &core::movem; // Register to memory size = W
			handlers[0x4c80 + (1 << 6) + ea] = &core::movem; // Register to memory size = L
		}
	}

	// MOVEP
	for (unsigned dataRegister = 0; dataRegister <= 7; dataRegister++)
	{
		for (unsigned opMode = 4; opMode <= 7; opMode++)
		{
			for (unsigned addressRegister = 0; addressRegister <= 7; addressRegister++)
			{
				handlers[(dataRegister << 9) + (opMode << 6) + (1 << 3) + addressRegister] = &core::movep;
			}
		}
	}

	// MOVEQ
	for (unsigned reg = 0; reg <= 7; reg++)
	{
		for (unsigned data = 0; data <= 255; data++)
		{
			handlers[0x7000 + (reg << 9) + data] = &core::moveq;
		}
	}

	// NBCD
	for (unsigned ea = 0; ea <= 0b111'111u; ea++)
	{
		if (isValidAddressingMode(ea, 0b101111111000u))
		{
			handlers[0x4800 + ea] = &core::nbcd;
		}
	}

	// NEG NEGX NOT
	for (unsigned ea = 0; ea <= 0b111'111u; ea++)
	{
		if (isValidAddressingMode(ea, 0b101111111000u))
		{
			for (unsigned size = 0; size <= 2; size++)
			{
				handlers[0x4400 + (size << 6) + ea] = &core::neg;
				handlers[0x4000 + (size << 6) + ea] = &core::negx;
				handlers[0x4600 + (size << 6) + ea] = &core::not_;
			}
		}
	}

	// NOP
	handlers[0b0100'1110'0111'0001u] = &core::nop;

	// ORI to CCR
	handlers[0b0000'0000'0011'1100u] = &core::ori2ccr;

	// PEA
	for (unsigned ea = 0; ea <= 0b111'111u; ea++)
	{
		if (isValidAddressingMode(ea, 0b001001111011u))
		{
			handlers[0x4840 + ea] = &core::pea;
		}
	}

	// ROL ROR ROXL ROXR REGISTER SHIFTS
	for (int reg1 = 0; reg1 <= 7; reg1++)
	{
		for (int size = 0; size <= 2; size++)
		{
			for (int ir = 0; ir <= 1; ir++)
			{
				for (int reg2 = 0; reg2 <= 7; reg2++)
				{
					handlers[0xe018 + (reg1 << 9) + (0 << 8) + (size << 6) + (ir << 5) + reg2] = &core::ror_register;
					handlers[0xe018 + (reg1 << 9) + (1 << 8) + (size << 6) + (ir << 5) + reg2] = &core::rol_register;

					handlers[0xe010 + (reg1 << 9) + (0 << 8) + (size << 6) + (ir << 5) + reg2] = &core::roxr_register;
					handlers[0xe010 + (reg1 << 9) + (1 << 8) + (size << 6) + (ir << 5) + reg2] = &core::roxl_register;
				}
			}
		}
	}

	// ROL ROR ROXL ROXR MEMORY SHIFTS
	for (unsigned ea = 0; ea <= 0b111'111u; ea++)
	{
		if (isValidAddressingMode(ea, 0b001111111000u))
		{
			handlers[0xe6c0 + (0 << 8) + ea] = &core::ror_memory;
			handlers[0xe6c0 + (1 << 8) + ea] = &core::rol_memory;

			handlers[0xe4c0 + (0 << 8) + ea] = &core::roxr_memory;
			handlers[0xe4c0 + (1 << 8) + ea] = &core::roxl_memory;
		}
	}

	// RTR
	handlers[0b0100'1110'0111'0111u] = &core::rtr;

	// RTS
	handlers[0b0100'1110'0111'0101u] = &core::rts;

	// SCC
	for (unsigned condition = 0; condition <= 16; condition++)
	{
		for (unsigned ea = 0; ea <= 0b111'111u; ea++)
		{
			if (isValidAddressingMode(ea, 0b101111111000u))
			{
				handlers[0x50c0 + (condition << 8) + ea] = &core::scc;
			}
		}
	}

	// SWAP
	for (unsigned reg = 0; reg <= 7; reg++)
	{
		handlers[0x4840 + reg] = &core::swap;
	}

	// TAS
	for (unsigned ea = 0; ea <= 0b111'111u; ea++)
	{
		if (isValidAddressingMode(ea, 0b101111111000u))
		{
			handlers[0x4Ac0 + ea] = &core::tas;
		}
	}

	// TRAP
	for (unsigned vector = 0; vector <= 7; vector++)
	{
		handlers[0x4e40 + vector] = &core::trap;
	}

	// TRAPV
	handlers[0b0100'1110'0111'0110u] = &core::trapv;

	// TST
	for (int size = 0; size <= 2; size++)
	{
		for (unsigned ea = 0; ea <= 0b111'111u; ea++)
		{
			handlers[0x4A00 + (size << 6) + ea] = &core::tst;
		}
	}

	// SWAP
	for (unsigned reg = 0; reg <= 7; reg++)
	{
		handlers[0x4e58 + reg] = &core::unlk;
	}

}

unsigned short core::abcd(unsigned short)
{
	return instructions::ABCD;
}

unsigned short core::add(unsigned short)
{
	return instructions::ADD;
}

unsigned short core::adda(unsigned short)
{
	return instructions::ADDA;
}

unsigned short core::addi(unsigned short)
{
	return instructions::ADDI;
}

unsigned short core::addq(unsigned short)
{
	return instructions::ADDQ;
}

unsigned short core::addx(unsigned short)
{
	return instructions::ADDX;
}

unsigned short core::and_(unsigned short)
{
	return instructions::AND;
}

unsigned short core::andi(unsigned short)
{
	return instructions::ANDI;
}

unsigned short core::andi2ccr(unsigned short)
{
	return instructions::ANDI2CCR;
}
unsigned short core::andi2sr(unsigned short)
{
	return instructions::ANDI2SR;
}
unsigned short core::asl_memory(unsigned short)
{
	return instructions::ASL;
}

unsigned short core::asl_register(unsigned short)
{
	return instructions::ASL;
}

unsigned short core::asr_memory(unsigned short)
{
	return instructions::ASR;
}

unsigned short core::asr_register(unsigned short)
{
	return instructions::ASR;
}

unsigned short core::bra(unsigned short)
{
	return instructions::BRA;
}
unsigned short core::bhi(unsigned short)
{
	return instructions::BHI;
}
unsigned short core::bls(unsigned short)
{
	return instructions::BLS;
}
unsigned short core::bcc(unsigned short)
{
	return instructions::BCC;
}
unsigned short core::bcs(unsigned short)
{
	return instructions::BCS;
}
unsigned short core::bne(unsigned short)
{
	return instructions::BNE;
}
unsigned short core::beq(unsigned short)
{
	return instructions::BEQ;
}
unsigned short core::bvc(unsigned short)
{
	return instructions::BVC;
}
unsigned short core::bvs(unsigned short)
{
	return instructions::BVS;
}
unsigned short core::bpl(unsigned short)
{
	return instructions::BPL;
}
unsigned short core::bmi(unsigned short)
{
	return instructions::BMI;
}
unsigned short core::bge(unsigned short)
{
	return instructions::BGE;
}
unsigned short core::blt(unsigned short)
{
	return instructions::BLT;
}
unsigned short core::bgt(unsigned short)
{
	return instructions::BGT;
}
unsigned short core::ble(unsigned short)
{
	return instructions::BLE;
}

unsigned short core::bchg_r(unsigned short)
{
	return instructions::BCHG_R;
}

unsigned short core::bchg_i(unsigned short)
{
	return instructions::BCHG_I;
}

unsigned short core::bclr_r(unsigned short)
{
	return instructions::BCLR_R;
}

unsigned short core::bclr_i(unsigned short)
{
	return instructions::BCLR_I;
}

unsigned short core::bset_r(unsigned short)
{
	return instructions::BSET_R;
}

unsigned short core::bset_i(unsigned short)
{
	return instructions::BSET_I;
}

unsigned short core::bsr(unsigned short)
{
	return instructions::BSR;
}

unsigned short core::btst_r(unsigned short)
{
	return instructions::BTST_R;
}

unsigned short core::btst_i(unsigned short)
{
	return instructions::BTST_I;
}

unsigned short core::chk(unsigned short)
{
	return instructions::CHK;
}

unsigned short core::clr(unsigned short)
{
	return instructions::CLR;
}


unsigned short core::cmp(unsigned short)
{
	return instructions::CMP;
}

unsigned short core::cmpa(unsigned short)
{
	return instructions::CMPA;
}

unsigned short core::cmpi(unsigned short)
{
	return instructions::CMPI;
}

unsigned short core::cmpm(unsigned short)
{
	return instructions::CMPM;
}

unsigned short core::dbcc(unsigned short)
{
	return instructions::DBCC;
}

unsigned short core::divs(unsigned short)
{
	return instructions::DIVS;
}

unsigned short core::divu(unsigned short)
{
	return instructions::DIVU;
}

unsigned short core::eor(unsigned short)
{
	return instructions::EOR;
}

unsigned short core::eori(unsigned short)
{
	return instructions::EORI;
}

unsigned short core::eori2ccr(unsigned short)
{
	return instructions::EORI2CCR;
}

unsigned short core::exg(unsigned short)
{
	return instructions::EXG;
}

unsigned short core::ext(unsigned short)
{
	return instructions::EXT;
}

unsigned short core::illegal(unsigned short)
{
	return instructions::ILLEGAL;
}

unsigned short core::jmp(unsigned short)
{
	return instructions::JMP;
}

unsigned short core::jsr(unsigned short)
{
	return instructions::JSR;
}

unsigned short core::lea(unsigned short)
{
	return instructions::LEA;
}

unsigned short core::link(unsigned short)
{
	return instructions::LINK;
}

unsigned short core::lsl_memory(unsigned short)
{
	return instructions::LSL;
}

unsigned short core::lsl_register(unsigned short)
{
	return instructions::LSL;
}

unsigned short core::lsr_memory(unsigned short)
{
	return instructions::LSR;
}

unsigned short core::lsr_register(unsigned short)
{
	return instructions::LSR;
}

unsigned short core::move(unsigned short)
{
	return instructions::MOVE;
}

unsigned short core::movea(unsigned short)
{
	return instructions::MOVEA;
}

unsigned short core::move2ccr(unsigned short)
{
	return instructions::MOVE2CCR;
}

unsigned short core::movesr(unsigned short)
{
	return instructions::MOVESR;
}

unsigned short core::movem(unsigned short)
{
	return instructions::MOVEM;
}

unsigned short core::movep(unsigned short)
{
	return instructions::MOVEP;
}

unsigned short core::moveq(unsigned short)
{
	return instructions::MOVEQ;
}

unsigned short core::muls(unsigned short)
{
	return instructions::MULS;
}

unsigned short core::mulu(unsigned short)
{
	return instructions::MULU;
}

unsigned short core::nbcd(unsigned short)
{
	return instructions::NBCD;
}

unsigned short core::neg(unsigned short)
{
	return instructions::NEG;
}
unsigned short core::negx(unsigned short)
{
	return instructions::NEGX;
}
unsigned short core::nop(unsigned short)
{
	return instructions::NOP;
}
unsigned short core::not_(unsigned short)
{
	return instructions::NOT;
}

unsigned short core::or_(unsigned short)
{
	return instructions::OR;
}

unsigned short core::ori(unsigned short)
{
	return instructions::ORI;
}

unsigned short core::ori2ccr(unsigned short)
{
	return instructions::ORI2CCR;
}

unsigned short core::pea(unsigned short)
{
	return instructions::PEA;
}

unsigned short core::rol_memory(unsigned short)
{
	return instructions::ROL;
}

unsigned short core::ror_memory(unsigned short)
{
	return instructions::ROR;
}

unsigned short core::roxl_memory(unsigned short)
{
	return instructions::ROXL;
}

unsigned short core::roxr_memory(unsigned short)
{
	return instructions::ROXR;
}

unsigned short core::rol_register(unsigned short)
{
	return instructions::ROL;
}

unsigned short core::ror_register(unsigned short)
{
	return instructions::ROR;
}

unsigned short core::roxl_register(unsigned short)
{
	return instructions::ROXL;
}

unsigned short core::roxr_register(unsigned short)
{
	return instructions::ROXR;
}

unsigned short core::sbcd(unsigned short)
{
	return instructions::SBCD;
}

unsigned short core::rtr(unsigned short)
{
	return instructions::RTR;
}

unsigned short core::rts(unsigned short)
{
	return instructions::RTS;
}

unsigned short core::scc(unsigned short)
{
	return instructions::SCC;
}

unsigned short core::sub(unsigned short)
{
	return instructions::SUB;
}

unsigned short core::subi(unsigned short)
{
	return instructions::SUBI;
}

unsigned short core::suba(unsigned short)
{
	return instructions::SUBA;
}

unsigned short core::subq(unsigned short)
{
	return instructions::SUBQ;
}

unsigned short core::subx(unsigned short)
{
	return instructions::SUBX;
}

unsigned short core::swap(unsigned short)
{
	return instructions::SWAP;
}

unsigned short core::tas(unsigned short)
{
	return instructions::TAS;
}

unsigned short core::trap(unsigned short)
{
	return instructions::TRAP;
}

unsigned short core::trapv(unsigned short)
{
	return instructions::TRAPV;
}

unsigned short core::tst(unsigned short)
{
	return instructions::TST;
}

unsigned short core::unlk(unsigned short)
{
	return instructions::UNLK;
}

