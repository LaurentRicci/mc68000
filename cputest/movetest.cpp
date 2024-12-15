#include <boost/test/unit_test.hpp>
#include "../core/cpu.h"
#include "../core/memory.h"
#include "verifyexecution.h"

using namespace mc68000;

BOOST_AUTO_TEST_SUITE(cpuSuite_move)

BOOST_AUTO_TEST_CASE(move_to_D2_b)
{
	// Arrange
	unsigned char code[] = { 0b0001'0100u, 0b0011'1100u, 0, 0x20, 0x4e, 0x40 }; // move.b #$20,d2

	// Act
	verifyExecution(code, sizeof(code), [](const Cpu& cpu) { BOOST_CHECK_EQUAL(0x20, cpu.d2); });
}

BOOST_AUTO_TEST_CASE(move_to_D4_w)
{
	// Arrange
	unsigned char code[] = { 0b0011'1000u, 0b0011'1100u, 0x12, 0x34, 0x4e, 0x40 }; // move.w #$1234,d4

	// Assert
	verifyExecution(code, sizeof(code), [](const Cpu& cpu)
		{
			BOOST_CHECK_EQUAL(0x1234, cpu.d4);
			validateSR(cpu, -1, 0, 0, 0, 0);
		});
}

BOOST_AUTO_TEST_CASE(move_negativeToD4_w)
{
	// Arrange
	unsigned char code[] = { 0b0011'1000u, 0b0011'1100u, 0x82, 0x34, 0x4e, 0x40 }; // move.w #$1234,d4

	// Assert
	verifyExecution(code, sizeof(code), [](const Cpu& cpu)
		{
			BOOST_CHECK_EQUAL(0x8234, cpu.d4);
			validateSR(cpu, -1, 1, 0, 0, 0);
		});
}

BOOST_AUTO_TEST_CASE(move_to_D7_l)
{
	// Arrange
	unsigned char code[] = { 0b0010'1110u, 0b0011'1100u, 0x12, 0x34, 0x56, 0x78, 0x4e, 0x40 }; // move.w #$1234,d7

	// Assert
	verifyExecution(code, sizeof(code), [](const Cpu& cpu) { BOOST_CHECK_EQUAL(0x12345678, cpu.d7); });
}

BOOST_AUTO_TEST_CASE(move_to_A2_w)
{
	// Arrange
	unsigned char code[] = { 0b0011'0100u, 0b0111'1100u, 0x12, 0x34, 0x4e, 0x40 }; // movea.w #$1234,a2

	// Assert
	verifyExecution(code, sizeof(code), [](const Cpu& cpu) { BOOST_CHECK_EQUAL(0x1234, cpu.a2); });
}

BOOST_AUTO_TEST_CASE(move_negative_to_A2_w)
{
	// Arrange
	unsigned char code[] = { 0b0011'0100u, 0b0111'1100u, 0x87, 0x34, 0x4e, 0x40 }; // movea.w #$8734,a2

	// Assert
	verifyExecution(code, sizeof(code), [](const Cpu& cpu) { BOOST_CHECK_EQUAL(0xffff8734, cpu.a2); });
}



BOOST_AUTO_TEST_CASE(moveq)
{
	unsigned char code[] = {
		0x74, 0xfe,    // moveq.l #-2,d2
		0x76, 0x04,    // moveq.l #4,d3
		0x4e, 0x40,    // trap #0
		0xff, 0xff };
	verifyExecution(code, sizeof(code), [](const Cpu& cpu)
		{
			BOOST_CHECK_EQUAL(-2, cpu.d2);
			BOOST_CHECK_EQUAL(4, cpu.d3);
		});
}

BOOST_AUTO_TEST_CASE(move_to_ccr_1)
{
	unsigned char code[] = {
		0x44, 0xfc, 0x00, 0x15,    // move #21, CCR
		0x4e, 0x40,                // trap #0
		0xff, 0xff };

	verifyExecution(code, sizeof(code), [](const Cpu& cpu)
		{
			BOOST_CHECK_EQUAL(1, cpu.sr.x);
			BOOST_CHECK_EQUAL(0, cpu.sr.n);
			BOOST_CHECK_EQUAL(1, cpu.sr.z);
			BOOST_CHECK_EQUAL(0, cpu.sr.v);
			BOOST_CHECK_EQUAL(1, cpu.sr.c);
		});
}

BOOST_AUTO_TEST_CASE(move_to_ccr_2)
{
	unsigned char code[] = {
		0x44, 0xfc, 0x00, 0x0a,    // move #12, CCR
		0x4e, 0x40,                // trap #0
		0xff, 0xff };

	verifyExecution(code, sizeof(code), [](const Cpu& cpu)
		{
			BOOST_CHECK_EQUAL(0, cpu.sr.x);
			BOOST_CHECK_EQUAL(1, cpu.sr.n);
			BOOST_CHECK_EQUAL(0, cpu.sr.z);
			BOOST_CHECK_EQUAL(1, cpu.sr.v);
			BOOST_CHECK_EQUAL(0, cpu.sr.c);
		});
}

BOOST_AUTO_TEST_CASE(move_from_sr)
{
	unsigned char code[] = {
		0x44, 0xfc, 0x00, 0x15,    // move #21, CCR
		0x40, 0xc0,                // move sr,d0
		0x44, 0xfc, 0x00, 0x03,    // move #3, CCR
		0x40, 0xc1,                // move sr,d1
		0x44, 0xfc, 0x00, 0x04,    // move #4, CCR
		0x40, 0xc2,                // move sr,d2
		0x4e, 0x40,                // trap #0
		0xff, 0xff };

	verifyExecution(code, sizeof(code), [](const Cpu& cpu)
		{
			BOOST_CHECK_EQUAL(21, cpu.d0);
			BOOST_CHECK_EQUAL(3, cpu.d1);
			BOOST_CHECK_EQUAL(4, cpu.d2);
		});
}

BOOST_AUTO_TEST_CASE(movem_to_memory)
{
	unsigned char code[] = {
		0x47, 0xfa, 0x00, 0x18,    // lea SPACE(PC), a3
		0x70, 0x14,                // moveq.l #20, d0
		0x72, 0x15,                // moveq.l #21, d1
		0x74, 0x28,                // moveq.l #40, d2
		0x20, 0x42,                // move.l   d2, a0
		0x74, 0x2c,                // moveq.l #44, d2
		0x22, 0x42,                // move.l   d2, a1

		0x48, 0xd3, 0x03, 0x03,    // movem.l d0-d1/a0-a1, (a3)
		0x4e, 0x40,                // trap #0
		0xff, 0xff                 //
		                           // SPACE: DS.L 20
	};

	// Arrange
	memory memory(256, 0, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0);

	// Assert
	BOOST_CHECK_EQUAL(20, cpu.d0);
	BOOST_CHECK_EQUAL(21, cpu.d1);
	BOOST_CHECK_EQUAL(40, cpu.a0);
	BOOST_CHECK_EQUAL(44, cpu.a1);

	BOOST_CHECK_EQUAL(0x1a, cpu.a3);
	BOOST_CHECK_EQUAL(20, cpu.mem.get<uint32_t>(0x1a));
}

BOOST_AUTO_TEST_CASE(movem_to_memory_word)
{
	unsigned char code[] = {
		0x47, 0xfa, 0x00, 0x18,    // lea SPACE(PC), a3
		0x70, 0xec,                // moveq.l #-20, d0
		0x72, 0x15,                // moveq.l #21, d1
		0x74, 0x28,                // moveq.l #40, d2
		0x30, 0x42,                // move.w   d2, a0
		0x74, 0x2c,                // moveq.l #44, d2
		0x32, 0x42,                // move.w   d2, a1

		0x48, 0x93, 0x03, 0x03,    // movem.w d0-d1/a0-a1, (a3)
		0x4e, 0x40,                // trap #0
		0xff, 0xff                 //
		// SPACE: DS.L 20
	};

	// Arrange
	memory memory(256, 0, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0);

	// Assert
	BOOST_CHECK_EQUAL(-20, cpu.d0);
	BOOST_CHECK_EQUAL(21, cpu.d1);
	BOOST_CHECK_EQUAL(40, cpu.a0);
	BOOST_CHECK_EQUAL(44, cpu.a1);

	BOOST_CHECK_EQUAL(0x1a, cpu.a3);
	BOOST_CHECK_EQUAL(0xffec, cpu.mem.get<uint16_t>(0x1a));
}



BOOST_AUTO_TEST_CASE(movem_to_from_memory)
{
	unsigned char code[] = {
		0x47, 0xfa, 0x00, 0x24,    // lea SPACE(PC), a3
		0x70, 0x14,                // moveq.l #20, d0
		0x72, 0x15,                // moveq.l #21, d1
		0x74, 0x28,                // moveq.l #40, d2
		0x20, 0x42,                // move.l   d2, a0
		0x74, 0x2c,                // moveq.l #44, d2
		0x22, 0x42,                // move.l   d2, a1

		0x48, 0xd3, 0x03, 0x03,    // movem.l d0-d1/a0-a1, (a3)

		0x70, 0x00,                // moveq.l #0, d0
		0x22, 0x00,                // move.l  d0, d1
		0x20, 0x40,                // move.l  d0, a0
		0x22, 0x40,                // move.l  d0, a1

		0x4c, 0xd3, 0x03, 0x03,    // movem.l (a3), d0-d1/a0-a1

		0x4e, 0x40,                // trap #0
		0xff, 0xff                 //
		                           // SPACE: DS.L 20
	};

	// Arrange
	memory memory(256, 0, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0);

	// Assert
	BOOST_CHECK_EQUAL(20, cpu.d0);
	BOOST_CHECK_EQUAL(21, cpu.d1);
	BOOST_CHECK_EQUAL(40, cpu.a0);
	BOOST_CHECK_EQUAL(44, cpu.a1);

	BOOST_CHECK_EQUAL(0x26, cpu.a3);
	BOOST_CHECK_EQUAL(20, cpu.mem.get<uint32_t>(0x26));
}

BOOST_AUTO_TEST_CASE(movem_to_from_memory_word)
{
	unsigned char code[] = {
		0x47, 0xfa, 0x00, 0x24,    // lea SPACE(PC), a3
		0x70, 0xec,                // moveq.l #-20, d0
		0x72, 0x15,                // moveq.l #21, d1
		0x74, 0x28,                // moveq.l #40, d2
		0x30, 0x42,                // move.w   d2, a0
		0x74, 0x2c,                // moveq.l #44, d2
		0x32, 0x42,                // move.w   d2, a1

		0x48, 0x93, 0x03, 0x03,    // movem.w d0-d1/a0-a1, (a3)

		0x70, 0x00,                // moveq.l #0, d0
		0x22, 0x00,                // move.l  d0, d1
		0x20, 0x40,                // move.l  d0, a0
		0x22, 0x40,                // move.l  d0, a1

		0x4c, 0x93, 0x03, 0x03,    // movem.l (a3), d0-d1/a0-a1

		0x4e, 0x40,                // trap #0
		0xff, 0xff                 //
		                           // SPACE: DS.L 20
	};

	// Arrange
	memory memory(256, 0, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0);

	// Assert
	BOOST_CHECK_EQUAL(0xffffffec, cpu.d0);
	BOOST_CHECK_EQUAL(21, cpu.d1);
	BOOST_CHECK_EQUAL(40, cpu.a0);
	BOOST_CHECK_EQUAL(44, cpu.a1);

	BOOST_CHECK_EQUAL(0x26, cpu.a3);
	BOOST_CHECK_EQUAL(0xffec, cpu.mem.get<uint16_t>(0x26));
}

BOOST_AUTO_TEST_CASE(movem_incremental_word)
{
	unsigned char code[] = {
		0x47, 0xfa, 0x00, 0x44,    // lea SPACE+32(PC), a3
		0x70, 0xec,                // moveq.l #-20, d0
		0x72, 0x15,                // moveq.l #21, d1
		0x74, 0x28,                // moveq.l #40, d2
		0x30, 0x42,                // move.w   d2, a0
		0x74, 0x2c,                // moveq.l #44, d2
		0x32, 0x42,                // move.w   d2, a1

		0x48, 0xa3, 0xc0, 0xc0,    // movem.w d0-d1/a0-a1, -(a3)

		0x70, 0x00,                // moveq.l #0, d0
		0x22, 0x00,                // move.l  d0, d1
		0x20, 0x40,                // move.l  d0, a0
		0x22, 0x40,                // move.l  d0, a1

		0x4c, 0x9b, 0x03, 0x03,    // movem.w (a3)+, d0-d1/a0-a1

		0x4e, 0x40,                // trap #0
		0xff, 0xff                 //
		// SPACE: DS.L 32
	};

	// Arrange
	memory memory(256, 0, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0);

	// Assert
	BOOST_CHECK_EQUAL(0xffffffec, cpu.d0);
	BOOST_CHECK_EQUAL(21, cpu.d1);
	BOOST_CHECK_EQUAL(40, cpu.a0);
	BOOST_CHECK_EQUAL(44, cpu.a1);

	BOOST_CHECK_EQUAL(0x46, cpu.a3);
	// A1 is the first register to be transfered
	BOOST_CHECK_EQUAL(44, cpu.mem.get<uint16_t>(0x44));
}

BOOST_AUTO_TEST_CASE(movem_incremental_long)
{
	unsigned char code[] = {
		0x47, 0xfa, 0x00, 0x44,    // lea SPACE+32(PC), a3
		0x70, 0xec,                // moveq.l #-20, d0
		0x72, 0x15,                // moveq.l #21, d1
		0x74, 0x28,                // moveq.l #40, d2
		0x30, 0x42,                // move.w   d2, a0
		0x74, 0x2c,                // moveq.l #44, d2
		0x32, 0x42,                // move.w   d2, a1

		0x48, 0xe3, 0xc0, 0xc0,    // movem.l d0-d1/a0-a1, -(a3)

		0x70, 0x00,                // moveq.l #0, d0
		0x22, 0x00,                // move.l  d0, d1
		0x20, 0x40,                // move.l  d0, a0
		0x22, 0x40,                // move.l  d0, a1

		0x4c, 0xdb, 0x03, 0x03,    // movem.l (a3)+, d0-d1/a0-a1

		0x4e, 0x40,                // trap #0
		0xff, 0xff                 //
		                           // SPACE: DS.L 32
	};

	// Arrange
	memory memory(256, 0, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0);

	// Assert
	BOOST_CHECK_EQUAL(0xffffffec, cpu.d0);
	BOOST_CHECK_EQUAL(21, cpu.d1);
	BOOST_CHECK_EQUAL(40, cpu.a0);
	BOOST_CHECK_EQUAL(44, cpu.a1);

	BOOST_CHECK_EQUAL(0x46, cpu.a3);
	// A1 is the first register to be transfered
	BOOST_CHECK_EQUAL(44, cpu.mem.get<uint32_t>(0x42));
}

BOOST_AUTO_TEST_CASE(movem_with_register)
{
	unsigned char code[] = {
		0x47, 0xfa, 0x00, 0x34,    // lea SPACE+32(PC), a3
		0x70, 0x42,                // moveq.l #$42, d0

		0x48, 0x93, 0x08, 0x01,    // movem.w d0/a3, (a3)

		0x70, 0x00,                // moveq.l #0, d0

		0x4c, 0x93, 0x08, 0x01,    // movem.l (a3), d0/a3

		0x4e, 0x40,                // trap #0
		0xff, 0xff                 //
		                           // SPACE: DS.L 32
	};

	// Arrange
	memory memory(256, 0, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0);

	// Assert
	BOOST_CHECK_EQUAL(0x42, cpu.d0);
	BOOST_CHECK_EQUAL(0x36, cpu.a3);
}

BOOST_AUTO_TEST_CASE(movem_with_register_incremental)
{
	unsigned char code[] = {
		0x47, 0xfa, 0x00, 0x34,    // lea SPACE+32(PC), a3
		0x70, 0x42,                // moveq.l #$42, d0

		0x48, 0xa3, 0x80, 0x10,    // movem.w d0/a3, -(a3)

		0x70, 0x00,                // moveq.l #0, d0

		0x4c, 0x9b, 0x08, 0x01,    // movem.l (a3)+, d0/a3

		0x4e, 0x40,                // trap #0
		0xff, 0xff                 //
		                           // SPACE: DS.L 32
	};

	// Arrange
	memory memory(256, 0, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0);

	// Assert
	BOOST_CHECK_EQUAL(0x42, cpu.d0);
	BOOST_CHECK_EQUAL(0x36, cpu.a3);

}

BOOST_AUTO_TEST_CASE(movep_from_reg_word)
{
	unsigned char code[] = {
		0x36, 0x3c, 0x12, 0x34,    // move.w #$1234, d3
		0x49, 0xfa, 0x00, 0x10,    // lea    SPACE(pc), a4
		0x07, 0x8c, 0x00, 0x05,    // movep.w d3, (5,a4)
		0x0f, 0x0c, 0x00, 0x05,    // movep.w (5,a4), d7
		0x4e, 0x40,                // trap #0
		0xff, 0xff, 0xff, 0xff     //
		                           // SPACE: DS.L 32
	};

	// Arrange
	memory memory(256, 0, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0);

	// Assert
	BOOST_CHECK_EQUAL(0x1234, cpu.d3);
	BOOST_CHECK_EQUAL(0x1234, cpu.d7);
	BOOST_CHECK_EQUAL(0x16, cpu.a4);
	BOOST_CHECK_EQUAL(0x12, cpu.mem.get<uint8_t>(0x1b));
	BOOST_CHECK_EQUAL(0x34, cpu.mem.get<uint8_t>(0x1d));

}

BOOST_AUTO_TEST_CASE(movep_from_reg_long)
{
	unsigned char code[] = {
		0x26, 0x3c, 0x12, 0x34, 0x56, 0x78,    // move.l #$12345678, d3
		0x49, 0xfa, 0x00, 0x10,                // lea    SPACE(pc), a4
		0x07, 0xcc, 0x00, 0x02,                // movep.l d3, (2,a4)
		0x0f, 0x4c, 0x00, 0x02,                // movep.l (2,a4), d7
		0x4e, 0x40,                            // trap #0
		0xff, 0xff, 0xff, 0xff                 //
		                                       // SPACE: DS.L 32
	};

	// Arrange
	memory memory(256, 0, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0);

	// Assert
	BOOST_CHECK_EQUAL(0x12345678, cpu.d3);
	BOOST_CHECK_EQUAL(0x12345678, cpu.d7);
	BOOST_CHECK_EQUAL(0x18, cpu.a4);
	BOOST_CHECK_EQUAL(0x12, cpu.mem.get<uint8_t>(0x1a));
	BOOST_CHECK_EQUAL(0x34, cpu.mem.get<uint8_t>(0x1c));
	BOOST_CHECK_EQUAL(0x56, cpu.mem.get<uint8_t>(0x1e));
	BOOST_CHECK_EQUAL(0x78, cpu.mem.get<uint8_t>(0x20));
}

BOOST_AUTO_TEST_CASE(pea)
{
	unsigned char code[] = {
		0x4F, 0xFA, 0x01, 0xA0, //   lea stack(pc),a7
		0x41, 0xfa, 0x00, 0x0A, //   lea data(pc),a0
		0x48, 0x50,             //   pea (a0)
		0x4E, 0x40,             //   trap #0
		0xFF, 0xFF, 0xFF, 0xFF, //   
		0x42, 0x42              //   data: dc.w $4242
		                        //   memory : ds.l 100
		                        //   stack :
	};

	// Arrange
	memory memory(1024, 0, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0);

	// Assert
	BOOST_CHECK_EQUAL(0x10, cpu.a0);
	BOOST_CHECK_EQUAL(0x19e, cpu.a7);
	BOOST_CHECK_EQUAL(cpu.a0, cpu.mem.get<uint32_t>(cpu.a7));
}



BOOST_AUTO_TEST_SUITE_END()