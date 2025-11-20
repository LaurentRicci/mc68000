#include <boost/test/unit_test.hpp>
#include "../core/cpu.h"
#include "../core/memory.h"
#include "verifyexecution.h"

using namespace mc68000;


BOOST_AUTO_TEST_SUITE(cpuSuite)

BOOST_AUTO_TEST_CASE(cpu_reset)
{
	// Arrange
	Memory memory(4, 0);
	Cpu cpu(memory);

	// Act
	cpu.reset(memory);

	// Assert
	BOOST_CHECK_EQUAL(0, cpu.d0);
	BOOST_CHECK_EQUAL(0, cpu.d1);
	BOOST_CHECK_EQUAL(0, cpu.d2);
	BOOST_CHECK_EQUAL(0, cpu.d3);
	BOOST_CHECK_EQUAL(0, cpu.d4);
	BOOST_CHECK_EQUAL(0, cpu.d5);
	BOOST_CHECK_EQUAL(0, cpu.d6);
	BOOST_CHECK_EQUAL(0, cpu.d7);

	BOOST_CHECK_EQUAL(0, cpu.a0);
	BOOST_CHECK_EQUAL(0, cpu.a1);
	BOOST_CHECK_EQUAL(0, cpu.a2);
	BOOST_CHECK_EQUAL(0, cpu.a3);
	BOOST_CHECK_EQUAL(0, cpu.a4);
	BOOST_CHECK_EQUAL(0, cpu.a5);
	BOOST_CHECK_EQUAL(0, cpu.a6);
	BOOST_CHECK_EQUAL(0, cpu.a7);
}

BOOST_AUTO_TEST_CASE(cpu_reset_sr)
{
	Memory memory;
	Cpu cpu(memory);

	BOOST_CHECK_EQUAL(0, cpu.sr.c);

}

BOOST_AUTO_TEST_CASE(statusRegister_init)
{
	StatusRegister sr;

	BOOST_CHECK_EQUAL(0, sr.c);
	BOOST_CHECK_EQUAL(0, sr.v);
	BOOST_CHECK_EQUAL(0, sr.z);
	BOOST_CHECK_EQUAL(0, sr.n);
	BOOST_CHECK_EQUAL(0, sr.x);
	BOOST_CHECK_EQUAL(0, sr.i);
	BOOST_CHECK_EQUAL(0, sr.s);
	BOOST_CHECK_EQUAL(0, sr.t);

}

BOOST_AUTO_TEST_CASE(statusRegister_from_int1)
{
	StatusRegister sr;

	sr = (uint16_t) 0b1010'0110'0001'0110;

	BOOST_CHECK_EQUAL(0, sr.c);
	BOOST_CHECK_EQUAL(1, sr.v);
	BOOST_CHECK_EQUAL(1, sr.z);
	BOOST_CHECK_EQUAL(0, sr.n);
	BOOST_CHECK_EQUAL(1, sr.x);
	BOOST_CHECK_EQUAL(6, sr.i);
	BOOST_CHECK_EQUAL(1, sr.s);
	BOOST_CHECK_EQUAL(1, sr.t);
}

BOOST_AUTO_TEST_CASE(statusRegister_from_int2)
{
	StatusRegister sr;

	sr = (uint16_t)0b0101'1001'1110'1001;

	BOOST_CHECK_EQUAL(1, sr.c);
	BOOST_CHECK_EQUAL(0, sr.v);
	BOOST_CHECK_EQUAL(0, sr.z);
	BOOST_CHECK_EQUAL(1, sr.n);
	BOOST_CHECK_EQUAL(0, sr.x);
	BOOST_CHECK_EQUAL(1, sr.i);
	BOOST_CHECK_EQUAL(0, sr.s);
	BOOST_CHECK_EQUAL(0, sr.t);
}

BOOST_AUTO_TEST_CASE(statusRegister_to_int1)
{
	StatusRegister sr;

	sr.c = 0;
	sr.v = 1;
	sr.z = 1;
	sr.n = 0;
	sr.x = 1;
	sr.i = 6;
	sr.s = 1;
	sr.t = 1;

	uint16_t value = sr;
	BOOST_CHECK_EQUAL(0b1010'0110'0001'0110, value);
}

BOOST_AUTO_TEST_CASE(statusRegister_to_int2)
{
	StatusRegister sr;

	sr.c = 1;
	sr.v = 0;
	sr.z = 0;
	sr.n = 1;
	sr.x = 0;
	sr.i = 1;
	sr.s = 0;
	sr.t = 0;

	uint16_t value = sr;
	BOOST_CHECK_EQUAL(0b0000'0001'0000'1001, value);
}



// =================================================================================================
// Addressing mode tests - Read
// =================================================================================================


BOOST_AUTO_TEST_CASE(addressMode_000)
{
	// move.w #$1234,d4 ; movea.w d4,a0 ; trap #0
	unsigned char code[] = { 0x38, 0x3c, 0x12, 0x34, 0x30, 0x44, 0x4e, 0x40 };
	verifyExecution(code, sizeof(code), [](const Cpu& cpu)
		{
			BOOST_CHECK_EQUAL(0x1234, cpu.d4);
			BOOST_CHECK_EQUAL(0x1234, cpu.a0);
		});
}

BOOST_AUTO_TEST_CASE(addressMode_000_value)
{
	unsigned char code[] = { 
		0x30, 0x3C, 0x21, 0x21,             // move.w #$2121,d0
		0x10, 0x3C, 0x00, 0x42,             // move.b #$42,d0
		0x22, 0x3C, 0x10, 0x20, 0x30, 0x40, // move.l #$10203040, d1
		0x22, 0x3C, 0x40, 0x30, 0x20, 0x10, // move.l #$10203040, d1
		0x4E, 0x40,				            // trap #0
		0xFF, 0xFF };

	// Arrange
	Memory memory(256, 0, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0);

	// Assert
	BOOST_CHECK_EQUAL(0x2142, cpu.d0);
	BOOST_CHECK_EQUAL(0x40302010, cpu.d1);
}

BOOST_AUTO_TEST_CASE(addressMode_001)
{
	// movea.w #$1234,a4 ; movea.w a4,a0 ; trap #0
	unsigned char code[] = { 0x38, 0x7c, 0x12, 0x34, 0b0011'0000u, 0b01'001'100u, 0x4e, 0x40 };
	verifyExecution(code, sizeof(code), [](const Cpu& cpu)
		{
			BOOST_CHECK_EQUAL(0x1234, cpu.a4);
			BOOST_CHECK_EQUAL(0x1234, cpu.a0);
		});
}

BOOST_AUTO_TEST_CASE(addressMode_010)
{
	// movea.w #10,a4 ; movea.w (a4),a0 ; trap #0
	unsigned char code[] = { 0x38, 0x7c, 0x00, 0x0A, 0b0011'0000u, 0b01'010'100u, 0x4e, 0x40, 0xff, 0xff, 0x43, 0x21 };
	verifyExecution(code, sizeof(code), [](const Cpu& cpu)
		{
			BOOST_CHECK_EQUAL(0x0A, cpu.a4);
			BOOST_CHECK_EQUAL(0x4321, cpu.a0);
		});
}

BOOST_AUTO_TEST_CASE(addressMode_011)
{
	// movea.w #10,a4 ; movea.w (a4)+,a0 ; trap #0
	unsigned char code[] = { 0x38, 0x7c, 0x00, 0x0A, 0b0011'0000u, 0b01'011'100u, 0x4e, 0x40, 0xff, 0xff, 0x43, 0x21 };
	verifyExecution(code, sizeof(code), [](const Cpu& cpu)
		{
			BOOST_CHECK_EQUAL(0x0C, cpu.a4);
			BOOST_CHECK_EQUAL(0x4321, cpu.a0);
		});
}

BOOST_AUTO_TEST_CASE(addressMode_100)
{
	// movea.w #12,a4 ; movea.w -(a4),a0 ; trap #0
	unsigned char code[] = { 0x38, 0x7c, 0x00, 0x0C, 0b0011'0000u, 0b01'100'100u, 0x4e, 0x40, 0xff, 0xff, 0x43, 0x21 };
	verifyExecution(code, sizeof(code), [](const Cpu& cpu)
		{
			BOOST_CHECK_EQUAL(0x0A, cpu.a4);
			BOOST_CHECK_EQUAL(0x4321, cpu.a0);
		});
}

BOOST_AUTO_TEST_CASE(addressMode_101_positive)
{
	// movea.w #6,a4 ; movea.w 6(a4),a0 ; trap #0
	unsigned char code[] = { 0x38, 0x7c, 0x00, 0x06, 0b0011'0000u, 0b01'101'100u, 0x00, 0x06, 0x4e, 0x40, 0xff, 0xff, 0x43, 0x21 };
	verifyExecution(code, sizeof(code), [](const Cpu& cpu)
		{
			BOOST_CHECK_EQUAL(0x06, cpu.a4);
			BOOST_CHECK_EQUAL(0x4321, cpu.a0);
		});
}

BOOST_AUTO_TEST_CASE(addressMode_101_negative)
{
	unsigned char code[] = {
		0x38, 0x7c, 0x00, 0x12,                  // movea.w #18,a4
		0b0011'0000u, 0b01'101'100u, 0xff, 0xfa, // movea.w -6(a4),a0
		0x4e, 0x40,                              // trap #0
		0xff, 0xff,
		0x43, 0x21 };
	verifyExecution(code, sizeof(code), [](const Cpu& cpu)
		{
			BOOST_CHECK_EQUAL(0x12, cpu.a4);
			BOOST_CHECK_EQUAL(0x4321, cpu.a0);
		});
}

BOOST_AUTO_TEST_CASE(addressMode_101_long)
{
	unsigned char code[] = { 
		0x38, 0x7c, 0x00, 0x06,                  // movea.w #6,a4
		0x20, 0x6c, 0x00, 0x06,                  // movea.l 6(a4),a0
		0x4e, 0x40,                              // trap #0
		0xff, 0xff, 
		0x43, 0x21, 0x12, 0x34 };
	verifyExecution(code, sizeof(code), [](const Cpu& cpu)
		{
			BOOST_CHECK_EQUAL(0x6, cpu.a4);
			BOOST_CHECK_EQUAL(0x43211234, cpu.a0);
		});
}


BOOST_AUTO_TEST_CASE(addressMode_110_positive)
{
	unsigned char code[] = { 
		0x38, 0x7c, 0x00, 0x06,                   // movea.w #6,a4 ;
		0b0011'010'0u, 0b00'111'100u, 0x00, 0x02, // move.w #2,d2 ;
		0b0011'0000u, 0b01'110'100u, 0x20, 0x08,  // movea.w 8(a4,d2),a0 ;
		0x4e, 0x40,                               // trap #0
		0xff, 0xff, 
		0x43, 0x21 };
	verifyExecution(code, sizeof(code), [](const Cpu& cpu)
		{
			BOOST_CHECK_EQUAL(0x06, cpu.a4);
			BOOST_CHECK_EQUAL(0x02, cpu.d2);
			BOOST_CHECK_EQUAL(0x4321, cpu.a0);
		});
}

BOOST_AUTO_TEST_CASE(addressMode_110_negative)
{
	unsigned char code[] = {
		0x38, 0x7c, 0x00, 0x0A,  // movea.w #10,a4 ;
		0x74, 0xfe,              // moveq.l #-2,d2 ;
		0x30, 0x74, 0x20, 0x08,  // movea.w 8(a4,d2.l),a0 ;
		0x4e, 0x40,              // trap #0
		0xff, 0xff,
		0xff, 0xff,
		0x43, 0x21 };
	verifyExecution(code, sizeof(code), [](const Cpu& cpu)
		{
			BOOST_CHECK_EQUAL(10, cpu.a4);
			BOOST_CHECK_EQUAL(0xfffffffe, cpu.d2);
			BOOST_CHECK_EQUAL(0x4321, cpu.a0);
		});
}

BOOST_AUTO_TEST_CASE(addressMode_111_000)
{
	unsigned char code[] = {
		0x38, 0x78, 0x00, 0x0A,  // movea.w $10,a4 ;
		0x4e, 0x40,              // trap #0
		0xff, 0xff,
		0xff, 0xff,
		0x43, 0x21 };
	verifyExecution(code, sizeof(code), [](const Cpu& cpu)
		{
			BOOST_CHECK_EQUAL(0x4321, cpu.a4);
		});
}

BOOST_AUTO_TEST_CASE(addressMode_111_001)
{
	unsigned char code[] = {
		0x38, 0x79, 0x00, 0x03, 0x20, 0x0A,  // movea.w $3200a,a4 ;
		0x4e, 0x40,                          // trap #0
		0xff, 0xff,
		0x43, 0x21 };
	verifyExecution(code, sizeof(code), 0x32000, [](const Cpu& cpu)
		{
			BOOST_CHECK_EQUAL(0x4321, cpu.a4);
		});
}

BOOST_AUTO_TEST_CASE(addressMode_111_010)
{
	unsigned char code[] = {
		0x32, 0x3A, 0x00, 0x08,  // movea.w data,d1 ;
		0x4e, 0x40,              // trap #0
		0xff, 0xff,
		0xff, 0xff,
		0x43, 0x21 };            // data
	verifyExecution(code, sizeof(code), 0x32000, [](const Cpu& cpu)
		{
			BOOST_CHECK_EQUAL(0x4321, cpu.d1);
		});
}

BOOST_AUTO_TEST_CASE(addressMode_111_011)
{
	unsigned char code[] = {
		0x74, 0xfe,              // moveq.l #-2,d2 ;

		0x32, 0x3b, 0x20, 0x0A,  // movea.w -2(pc,d2),d1 ;
		0x4e, 0x40,              // trap #0
		0xff, 0xff,
		0xff, 0xff,
		0x43, 0x21 };            // data
	verifyExecution(code, sizeof(code), 0x32000, [](const Cpu& cpu)
		{
			BOOST_CHECK_EQUAL(-2, cpu.d2);
			BOOST_CHECK_EQUAL(0x4321, cpu.d1);
		});
}

// =================================================================================================
// Addressing mode tests - Write
// =================================================================================================

BOOST_AUTO_TEST_CASE(addressMode_w000)
{
	unsigned char code[] = 
	{ 
		0x70, 0x21,    // moveq #$21, d0
		0x32, 0x00,    // move d0, d1
		0x4e, 0x40 };  // trap #0
	verifyExecution(code, sizeof(code), [](const Cpu& cpu)
		{
			BOOST_CHECK_EQUAL(0x21, cpu.d0);
			BOOST_CHECK_EQUAL(0x21, cpu.d1);
		});
}

BOOST_AUTO_TEST_CASE(addressMode_w010)
{
	unsigned char code[] =
	{
		0x70, 0x21,             // moveq #$21, d0
		0x32, 0x7c, 0x00, 0x0A, // movea #10, a1
		0x32, 0x80,             // move  d0, (a1)
		0x4e, 0x40,             // trap #0
		0xff, 0xff };
	verifyExecution(code, sizeof(code), [](const Cpu& cpu)
		{
			BOOST_CHECK_EQUAL(0x21, cpu.d0);
			BOOST_CHECK_EQUAL(10, cpu.a1);
			BOOST_CHECK_EQUAL(0x21, cpu.mem.get<uint16_t>(10));
		});
}

BOOST_AUTO_TEST_CASE(addressMode_w011)
{
	unsigned char code[] =
	{
		0x70, 0x21,             // moveq #$21, d0
		0x32, 0x7c, 0x00, 0x0A, // movea #10, a1
		0x32, 0xC0,             // move  d0, (a1)+
		0x4e, 0x40,             // trap #0
		0xff, 0xff };
	verifyExecution(code, sizeof(code), [](const Cpu& cpu)
		{
			BOOST_CHECK_EQUAL(0x21, cpu.d0);
			BOOST_CHECK_EQUAL(12, cpu.a1);
			BOOST_CHECK_EQUAL(0x21, cpu.mem.get<uint16_t>(10));
		});
}

BOOST_AUTO_TEST_CASE(addressMode_w100)
{
	unsigned char code[] =
	{
		0x70, 0x21,             // moveq #$21, d0
		0x32, 0x7c, 0x00, 0x0C, // movea #10, a1
		0x33, 0x00,             // move  d0, (a1)+
		0x4e, 0x40,             // trap #0
		0xff, 0xff };
	verifyExecution(code, sizeof(code), [](const Cpu& cpu)
		{
			BOOST_CHECK_EQUAL(0x21, cpu.d0);
			BOOST_CHECK_EQUAL(10, cpu.a1);
			BOOST_CHECK_EQUAL(0x21, cpu.mem.get<uint16_t>(10));
		});
}

BOOST_AUTO_TEST_CASE(addressMode_w101_p)
{
	unsigned char code[] =
	{
		0x70, 0x21,             // moveq #$21, d0
		0x32, 0x7c, 0x00, 0x06, // movea #6 a1
		0x33, 0x40, 0x00, 0x06, // move  d0, 6(a1)
		0x4e, 0x40,             // trap #0
		0xff, 0xff };
	verifyExecution(code, sizeof(code), [](const Cpu& cpu)
		{
			BOOST_CHECK_EQUAL(0x21, cpu.d0);
			BOOST_CHECK_EQUAL(6, cpu.a1);
			BOOST_CHECK_EQUAL(0x21, cpu.mem.get<uint16_t>(12));
		});
}

BOOST_AUTO_TEST_CASE(addressMode_w101_n)
{
	unsigned char code[] =
	{
		0x70, 0x21,             // moveq #$21, d0
		0x32, 0x7c, 0x00, 0x10, // movea #16 a1
		0x33, 0x40, 0xff, 0xfc, // move  d0, -4(a1)
		0x4e, 0x40,             // trap #0
		0xff, 0xff };
	verifyExecution(code, sizeof(code), [](const Cpu& cpu)
		{
			BOOST_CHECK_EQUAL(0x21, cpu.d0);
			BOOST_CHECK_EQUAL(16, cpu.a1);
			BOOST_CHECK_EQUAL(0x21, cpu.mem.get<uint16_t>(12));
		});
}

BOOST_AUTO_TEST_CASE(addressMode_w110_p)
{
	unsigned char code[] =
	{
		0x70, 0x21,             // moveq #$21, d0
		0x74, 0x02,             // moveq #2, d2
		0x32, 0x7c, 0x00, 0x0A, // movea #10 a1
		0x33, 0x80, 0x20, 0x04, // move  d0, 4(a1,d2)
		0x4e, 0x40,             // trap #0
		0xff, 0xff, 0xff, 0xff };
	verifyExecution(code, sizeof(code), [](const Cpu& cpu)
		{
			BOOST_CHECK_EQUAL(0x21, cpu.d0);
			BOOST_CHECK_EQUAL(0x2, cpu.d2);
			BOOST_CHECK_EQUAL(10, cpu.a1);
			BOOST_CHECK_EQUAL(0x21, cpu.mem.get<uint16_t>(16));
		});
}

BOOST_AUTO_TEST_CASE(addressMode_w110_n)
{
	unsigned char code[] =
	{
		0x70, 0x21,             // moveq #$21, d0
		0x74, 0xFE,             // moveq #-2, d2
		0x32, 0x7c, 0x00, 0x0C, // movea #12 a1
		0x33, 0x80, 0x20, 0x04, // move  d0, 4(a1,d2)
		0x4e, 0x40,             // trap #0
		0xff, 0xff, 0xff, 0xff };
	verifyExecution(code, sizeof(code), [](const Cpu& cpu)
		{
			BOOST_CHECK_EQUAL(0x21, cpu.d0);
			BOOST_CHECK_EQUAL(-2, cpu.d2);
			BOOST_CHECK_EQUAL(12, cpu.a1);
			BOOST_CHECK_EQUAL(0x21, cpu.mem.get<uint16_t>(14));
		});
}

BOOST_AUTO_TEST_CASE(addressMode_w111_000)
{
	unsigned char code[] = {
		0x70, 0x21,             // moveq #$21, d0
		0x31, 0xC0, 0x00, 0x0a, // move  d0, $10.w
		0x4e, 0x40,             // trap #0
		0xff, 0xff,
		0xff, 0xff,
		0xff, 0xff };
	verifyExecution(code, sizeof(code), [](const Cpu& cpu)
		{
			BOOST_CHECK_EQUAL(0x21, cpu.d0);
			BOOST_CHECK_EQUAL(0x21, cpu.mem.get<uint16_t>(10));
		});
}

BOOST_AUTO_TEST_CASE(addressMode_w111_001)
{
	unsigned char code[] = {
		0x70, 0x21,                          // moveq #$21, d0
		0x33, 0xC0, 0x00, 0x03, 0x20, 0x0A,  // movea.w d0,$3200a
		0x4e, 0x40,                          // trap #0
		0xff, 0xff,
		0x43, 0x21 };
	verifyExecution(code, sizeof(code), 0x32000, [](const Cpu& cpu)
		{
			BOOST_CHECK_EQUAL(0x21, cpu.d0);
			BOOST_CHECK_EQUAL(0x21, cpu.mem.get<uint16_t>(0x3200a));
		});
}
BOOST_AUTO_TEST_SUITE_END()
