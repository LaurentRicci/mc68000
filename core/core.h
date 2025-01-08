#pragma once
#include <cstdint>

namespace mc68000
{
	bool isValidAddressingMode(unsigned short ea, unsigned short acceptable);

	template<class T> unsigned short (T::* *setup()) (unsigned short)
	{
		using t_handler = unsigned short (T::*)(unsigned short);

		t_handler* handlers = new t_handler[0x10000];

		for (int i = 0; i < 0x10000; i++)
		{
			handlers[i] = &T::unknown;
		}
		// ABCD and SBCD 
		for (int rx = 0; rx <= 7; rx++)
		{
			for (int ry = 0; ry <= 7; ry++)
			{
				handlers[(rx << 9) + (0 << 3) + ry + 0xc100] = &T::abcd;
				handlers[(rx << 9) + (1 << 3) + ry + 0xc100] = &T::abcd;
				handlers[(rx << 9) + (0 << 3) + ry + 0x8100] = &T::sbcd;
				handlers[(rx << 9) + (1 << 3) + ry + 0x8100] = &T::sbcd;
			}
		}

		// ADD ADDA AND CMP CMPA EOR OR SUB SUBA
		for (int reg = 0; reg <= 7; reg++)
		{
			for (int opmode = 0; opmode <= 2; opmode++)
			{
				for (int ea = 0; ea <= 0x3f; ea++)
				{
					handlers[0xd000 + (reg << 9) + (opmode << 6) + ea] = &T::add;
					handlers[0xb000 + (reg << 9) + (opmode << 6) + ea] = &T::cmp;
					handlers[0x9000 + (reg << 9) + (opmode << 6) + ea] = &T::sub;
					if (isValidAddressingMode(ea, 0b101111111111u))
					{
						handlers[0xc000 + (reg << 9) + (opmode << 6) + ea] = &T::and_;
						handlers[0x8000 + (reg << 9) + (opmode << 6) + ea] = &T::or_;
					}
				}
			}
			for (int opmode = 4; opmode <= 6; opmode++)
			{
				for (int ea = 0; ea <= 0x3f; ea++)
				{
					if (isValidAddressingMode(ea, 0b001111111000u))
					{
						handlers[0xd000 + (reg << 9) + (opmode << 6) + ea] = &T::add;
						handlers[0x9000 + (reg << 9) + (opmode << 6) + ea] = &T::sub;
						handlers[0xc000 + (reg << 9) + (opmode << 6) + ea] = &T::and_;
						handlers[0xb000 + (reg << 9) + (opmode << 6) + ea] = &T::eor;
						handlers[0x8000 + (reg << 9) + (opmode << 6) + ea] = &T::or_;
					}
				}
			}
			for (int ea = 0; ea <= 0x3f; ea++)
			{
				handlers[0xd000 + (reg << 9) + (0b011u << 6) + ea] = &T::adda;
				handlers[0xd000 + (reg << 9) + (0b111u << 6) + ea] = &T::adda;
				handlers[0xb000 + (reg << 9) + (0b011u << 6) + ea] = &T::cmpa;
				handlers[0xb000 + (reg << 9) + (0b111u << 6) + ea] = &T::cmpa;
				handlers[0x9000 + (reg << 9) + (0b011u << 6) + ea] = &T::suba;
				handlers[0x9000 + (reg << 9) + (0b111u << 6) + ea] = &T::suba;
			}
		}

		// ADDI ANDI CMPI EORI ORI SUBI
		for (int size = 0; size <= 2; size++)
		{
			for (unsigned ea = 0; ea <= 0b111'111u; ea++)
			{
				if (isValidAddressingMode(ea, 0b101111111000u))
				{
					handlers[0x0600 + (size << 6) + ea] = &T::addi;
					handlers[0x0200 + (size << 6) + ea] = &T::andi;
					handlers[0x0C00 + (size << 6) + ea] = &T::cmpi;
					handlers[0x0A00 + (size << 6) + ea] = &T::eori;
					handlers[0x0000 + (size << 6) + ea] = &T::ori;
					handlers[0x0400 + (size << 6) + ea] = &T::subi;
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
						handlers[0x5000 + (data << 9) + (0 << 8) + (size << 6) + ea] = &T::addq;
						handlers[0x5000 + (data << 9) + (1 << 8) + (size << 6) + ea] = &T::subq;
					}
				}
			}
		}

		// ADDX SUBX
		for (uint16_t rx = 0; rx <= 7; rx++)
		{
			for (uint16_t size = 0; size <= 2; size++)
			{
				for (uint16_t rm = 0; rm <= 1; rm++)
				{
					for (uint16_t ry = 0; ry <= 7; ry++)
					{
						handlers[0xd000 + (rx << 9) + (1 << 8) + (size << 6) + (rm << 3) + ry] = &T::addx;
						handlers[0x9000 + (rx << 9) + (1 << 8) + (size << 6) + (rm << 3) + ry] = &T::subx;
					}
				}
			}
		}

		// ANDI to CCR, to SR
		handlers[0b0000'0010'0011'1100u] = &T::andi2ccr;
		handlers[0b0000'0010'0111'1100u] = &T::andi2sr;

		// ASL and ASR REGISTER SHIFTS
		for (int reg1 = 0; reg1 <= 7; reg1++)
		{
			for (int size = 0; size <= 2; size++)
			{
				for (int ir = 0; ir <= 1; ir++)
				{
					for (int reg2 = 0; reg2 <= 7; reg2++)
					{
						handlers[0xe008 + (reg1 << 9) + (0 << 8) + (size << 6) + (ir << 5) + reg2] = &T::lsr_register;
						handlers[0xe008 + (reg1 << 9) + (1 << 8) + (size << 6) + (ir << 5) + reg2] = &T::lsl_register;

						handlers[0xe000 + (reg1 << 9) + (0 << 8) + (size << 6) + (ir << 5) + reg2] = &T::asr_register;
						handlers[0xe000 + (reg1 << 9) + (1 << 8) + (size << 6) + (ir << 5) + reg2] = &T::asl_register;
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
			handlers[0xe2c0 + (0 << 8) + mode] = &T::lsr_memory;
			handlers[0xe2c0 + (1 << 8) + mode] = &T::lsl_memory;

			handlers[0xe0c0 + (0 << 8) + mode] = &T::asr_memory;
			handlers[0xe0c0 + (1 << 8) + mode] = &T::asl_memory;
		}

		// BRA , BSR, BCC
		for (int displacement = 0; displacement <= 0xff; displacement++)
		{
			handlers[0x6000 + displacement] = &T::bra;
			handlers[0x6100 + displacement] = &T::bsr;
			handlers[0x6200 + displacement] = &T::bhi;
			handlers[0x6300 + displacement] = &T::bls;
			handlers[0x6400 + displacement] = &T::bcc;
			handlers[0x6500 + displacement] = &T::bcs;
			handlers[0x6600 + displacement] = &T::bne;
			handlers[0x6700 + displacement] = &T::beq;
			handlers[0x6800 + displacement] = &T::bvc;
			handlers[0x6900 + displacement] = &T::bvs;
			handlers[0x6A00 + displacement] = &T::bpl;
			handlers[0x6B00 + displacement] = &T::bmi;
			handlers[0x6C00 + displacement] = &T::bge;
			handlers[0x6D00 + displacement] = &T::blt;
			handlers[0x6E00 + displacement] = &T::bgt;
			handlers[0x6F00 + displacement] = &T::ble;
		}

		// BCHG BCLR BSET register
		for (int reg = 0; reg <= 7; reg++)
		{
			for (int ea = 0; ea <= 0x3f; ea++)
			{
				if (isValidAddressingMode(ea, 0b101111111000))
				{
					handlers[0 + (reg << 9) + (0b101u << 6) + ea] = &T::bchg_r;
					handlers[0 + (reg << 9) + (0b110u << 6) + ea] = &T::bclr_r;
					handlers[0 + (reg << 9) + (0b111u << 6) + ea] = &T::bset_r;
				}
			}
		}

		// BCHG BCLR BSET immediate
		for (int ea = 0; ea <= 0x3f; ea++)
		{
			if (isValidAddressingMode(ea, 0b101111111000))
			{
				handlers[(0b0000100001u << 6) + ea] = &T::bchg_i;
				handlers[(0b0000100010u << 6) + ea] = &T::bclr_i;
				handlers[(0b0000100011u << 6) + ea] = &T::bset_i;
			}
		}

		// BTST
		for (int reg = 0; reg <= 7; reg++)
		{
			for (int ea = 0; ea <= 0x3f; ea++)
			{
				if (isValidAddressingMode(ea, 0b101111111111))
				{
					handlers[0x0100 + (reg << 9) + ea] = &T::btst_r;
				}
			}
		}

		for (int ea = 0; ea <= 0x3f; ea++)
		{
			if (isValidAddressingMode(ea, 0b101111111011))
			{
				handlers[(0b0000100000u << 6) + ea] = &T::btst_i;
			}
		}

		// CHK
		for (int reg = 0; reg <= 7; reg++)
		{
			for (int ea = 0; ea <= 0b111'111u; ea++)
			{
				if (isValidAddressingMode(ea, 0b101111111111))
				{
					handlers[0x4000 + (reg << 9) + (0b11u << 7) + ea] = &T::chk;
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
					handlers[0x4200 + (size << 6) + ea] = &T::clr;
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
					handlers[0xb108 + (regx << 9) + (size << 6) + regy] = &T::cmpm;
				}
			}
		}

		// DBCC
		for (int condition = 0; condition <= 0xF; condition++)
		{
			for (int reg = 0; reg <= 7; reg++)
			{
				handlers[0x50c8 + (condition << 8) + reg] = &T::dbcc;
			}
		}

		// DIVS DIVU MULS MULU
		for (int reg = 0; reg <= 7; reg++)
		{
			for (unsigned ea = 0; ea <= 0b111'111u; ea++)
			{
				if (isValidAddressingMode(ea, 0b101111111111u))
				{
					handlers[0x81c0 + (reg << 9) + ea] = &T::divs;
					handlers[0x80c0 + (reg << 9) + ea] = &T::divu;
					handlers[0xc1c0 + (reg << 9) + ea] = &T::muls;
					handlers[0xc0c0 + (reg << 9) + ea] = &T::mulu;
				}
			}
		}

		// EORI TO CCR
		handlers[0b0000'1010'0011'1100u] = &T::eori2ccr;

		// EORI TO SR
		handlers[0b0000'1010'0111'1100u] = &T::eori2sr;

		// EXG
		for (int regx = 0; regx <= 7; regx++)
		{
			for (int regy = 0; regy <= 7; regy++)
			{
				handlers[0xc100 + (regx << 9) + (0b01000u << 3) + regy] = &T::exg; // Data registers
				handlers[0xc100 + (regx << 9) + (0b01001u << 3) + regy] = &T::exg; // Address registers
				handlers[0xc100 + (regx << 9) + (0b10001u << 3) + regy] = &T::exg; // Data register and address register
			}
		}

		// EXT
		for (int reg = 0; reg <= 7; reg++)
		{
			handlers[0x4800 + (0b010u << 6) + reg] = &T::ext;
			handlers[0x4800 + (0b011u << 6) + reg] = &T::ext;
		}

		handlers[0b0100'1010'1111'1100] = &T::illegal;

		// JMP JSR
		for (unsigned ea = 0; ea <= 0b111'111u; ea++)
		{
			if (isValidAddressingMode(ea, 0b001001111011u))
			{
				handlers[0x4EC0 + ea] = &T::jmp;
				handlers[0x4E80 + ea] = &T::jsr;
			}
		}

		// LEA
		for (int reg = 0; reg <= 7; reg++)
		{
			for (unsigned ea = 0; ea <= 0b111'111u; ea++)
			{
				if (isValidAddressingMode(ea, 0b001001111011u))
				{
					handlers[0x41c0 + (reg << 9) + ea] = &T::lea;
				}
			}
		}

		// LINK
		for (int reg = 0; reg <= 7; reg++)
		{
			handlers[0b0100'1110'0101'0000u + reg] = &T::link;
		}

		// MOVE
		for (unsigned size = 1; size <= 3; size++)
		{
			for (unsigned destination = 0; destination <= 0b111'111u; destination++)
			{
				if (isValidAddressingMode(destination, 0b101111111000u))
				{
					// the destination is inverted: register - mode instead of mode - register
					unsigned short destinationRegister = destination & 0b111u;
					unsigned short destinationMode = destination >> 3;
					unsigned short invertedDestination = (destinationRegister << 3) | destinationMode;
					for (unsigned source = 0; source <= 0b111'111u; source++)
					{
						if (isValidAddressingMode(destination, 0b111111111111u))
						{
							handlers[(size << 12) + (invertedDestination << 6) + source] = &T::move;
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
					handlers[(size << 12) + (reg << 9) + (1 << 6) + source] = &T::movea;
				}
			}
		}

		// MOVE TO CCR
		for (unsigned ea = 0; ea <= 0b111'111u; ea++)
		{
			if (isValidAddressingMode(ea, 0b101111111111u))
			{
				handlers[0x44c0 + ea] = &T::move2ccr;
			}
		}

		// MOVE FROM SR
		for (unsigned ea = 0; ea <= 0b111'111u; ea++)
		{
			if (isValidAddressingMode(ea, 0b101111111000u))
			{
				handlers[0x40c0 + ea] = &T::movesr;
			}
		}

		// MOVE TO SR
		for (unsigned ea = 0; ea <= 0b111'111u; ea++)
		{
			if (isValidAddressingMode(ea, 0b101111111111u))
			{
				handlers[0x46c0 + ea] = &T::move2sr;
			}
		}

		// MOVEM - Register to memory
		for (unsigned ea = 0; ea <= 0b111'111u; ea++)
		{
			if (isValidAddressingMode(ea, 0b001011111000u))
			{
				handlers[0x4880 + (0 << 6) + ea] = &T::movem; // Register to memory size = W
				handlers[0x4880 + (1 << 6) + ea] = &T::movem; // Register to memory size = L
			}
			if (isValidAddressingMode(ea, 0b001101111011u))
			{
				handlers[0x4c80 + (0 << 6) + ea] = &T::movem; // Register to memory size = W
				handlers[0x4c80 + (1 << 6) + ea] = &T::movem; // Register to memory size = L
			}
		}

		// MOVEP
		for (unsigned dataRegister = 0; dataRegister <= 7; dataRegister++)
		{
			for (unsigned opMode = 4; opMode <= 7; opMode++)
			{
				for (unsigned addressRegister = 0; addressRegister <= 7; addressRegister++)
				{
					handlers[(dataRegister << 9) + (opMode << 6) + (1 << 3) + addressRegister] = &T::movep;
				}
			}
		}

		// MOVEQ
		for (unsigned reg = 0; reg <= 7; reg++)
		{
			for (unsigned data = 0; data <= 255; data++)
			{
				handlers[0x7000 + (reg << 9) + data] = &T::moveq;
			}
		}

		// NBCD
		for (unsigned ea = 0; ea <= 0b111'111u; ea++)
		{
			if (isValidAddressingMode(ea, 0b101111111000u))
			{
				handlers[0x4800 + ea] = &T::nbcd;
			}
		}

		// NEG NEGX NOT
		for (unsigned ea = 0; ea <= 0b111'111u; ea++)
		{
			if (isValidAddressingMode(ea, 0b101111111000u))
			{
				for (unsigned size = 0; size <= 2; size++)
				{
					handlers[0x4400 + (size << 6) + ea] = &T::neg;
					handlers[0x4000 + (size << 6) + ea] = &T::negx;
					handlers[0x4600 + (size << 6) + ea] = &T::not_;
				}
			}
		}

		// NOP
		handlers[0b0100'1110'0111'0001u] = &T::nop;

		// ORI to CCR
		handlers[0b0000'0000'0011'1100u] = &T::ori2ccr;

		// ORI to SR
		handlers[0b0000'0000'0111'1100u] = &T::ori2sr;

		// PEA
		for (unsigned ea = 0; ea <= 0b111'111u; ea++)
		{
			if (isValidAddressingMode(ea, 0b001001111011u))
			{
				handlers[0x4840 + ea] = &T::pea;
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
						handlers[0xe018 + (reg1 << 9) + (0 << 8) + (size << 6) + (ir << 5) + reg2] = &T::ror_register;
						handlers[0xe018 + (reg1 << 9) + (1 << 8) + (size << 6) + (ir << 5) + reg2] = &T::rol_register;

						handlers[0xe010 + (reg1 << 9) + (0 << 8) + (size << 6) + (ir << 5) + reg2] = &T::roxr_register;
						handlers[0xe010 + (reg1 << 9) + (1 << 8) + (size << 6) + (ir << 5) + reg2] = &T::roxl_register;
					}
				}
			}
		}

		// ROL ROR ROXL ROXR MEMORY SHIFTS
		for (unsigned ea = 0; ea <= 0b111'111u; ea++)
		{
			if (isValidAddressingMode(ea, 0b001111111000u))
			{
				handlers[0xe6c0 + (0 << 8) + ea] = &T::ror_memory;
				handlers[0xe6c0 + (1 << 8) + ea] = &T::rol_memory;

				handlers[0xe4c0 + (0 << 8) + ea] = &T::roxr_memory;
				handlers[0xe4c0 + (1 << 8) + ea] = &T::roxl_memory;
			}
		}

		// RTE
		handlers[0b0100'1110'0111'0011u] = &T::rte;

		// RTR
		handlers[0b0100'1110'0111'0111u] = &T::rtr;

		// RTS
		handlers[0b0100'1110'0111'0101u] = &T::rts;

		// SCC
		for (unsigned condition = 0; condition <= 16; condition++)
		{
			for (unsigned ea = 0; ea <= 0b111'111u; ea++)
			{
				if (isValidAddressingMode(ea, 0b101111111000u))
				{
					handlers[0x50c0 + (condition << 8) + ea] = &T::scc;
				}
			}
		}

		// STOP
		handlers[0b0100'1110'0111'0010u] = &T::stop;

		// SWAP
		for (unsigned reg = 0; reg <= 7; reg++)
		{
			handlers[0x4840 + reg] = &T::swap;
		}

		// TAS
		for (unsigned ea = 0; ea <= 0b111'111u; ea++)
		{
			if (isValidAddressingMode(ea, 0b101111111000u))
			{
				handlers[0x4Ac0 + ea] = &T::tas;
			}
		}

		// TRAP
		for (unsigned vector = 0; vector <= 15; vector++)
		{
			handlers[0x4e40 + vector] = &T::trap;
		}

		// TRAPV
		handlers[0b0100'1110'0111'0110u] = &T::trapv;

		// TST
		for (int size = 0; size <= 2; size++)
		{
			for (unsigned ea = 0; ea <= 0b111'111u; ea++)
			{
				handlers[0x4A00 + (size << 6) + ea] = &T::tst;
			}
		}

		// SWAP
		for (unsigned reg = 0; reg <= 7; reg++)
		{
			handlers[0x4e58 + reg] = &T::unlk;
		}

		return handlers;
	}
}