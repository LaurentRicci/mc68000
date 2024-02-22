#include <boost/test/unit_test.hpp>
#include "../core/cpu.h"
#include "../core/memory.h"

using namespace mc68000;

BOOST_AUTO_TEST_SUITE(cpuSuite)

BOOST_AUTO_TEST_CASE(a_asm)
{
  BOOST_TEST(1 == 1);
}

BOOST_AUTO_TEST_CASE(a_reset)
{
	// Arrange
	memory memory(4, 0);
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

BOOST_AUTO_TEST_CASE(a_sr)
{
	memory memory;
	Cpu cpu(memory);

	BOOST_CHECK_EQUAL(0, cpu.sr.c);

}

BOOST_AUTO_TEST_CASE(sr_init)
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

BOOST_AUTO_TEST_CASE(sr_from_int1)
{
	StatusRegister sr;

	sr = (int16_t) 0b1010'0110'0001'0110;

	BOOST_CHECK_EQUAL(0, sr.c);
	BOOST_CHECK_EQUAL(1, sr.v);
	BOOST_CHECK_EQUAL(1, sr.z);
	BOOST_CHECK_EQUAL(0, sr.n);
	BOOST_CHECK_EQUAL(1, sr.x);
	BOOST_CHECK_EQUAL(6, sr.i);
	BOOST_CHECK_EQUAL(1, sr.s);
	BOOST_CHECK_EQUAL(1, sr.t);
}

BOOST_AUTO_TEST_CASE(sr_from_int2)
{
	StatusRegister sr;

	sr = (int16_t)0b0101'1001'1110'1001;

	BOOST_CHECK_EQUAL(1, sr.c);
	BOOST_CHECK_EQUAL(0, sr.v);
	BOOST_CHECK_EQUAL(0, sr.z);
	BOOST_CHECK_EQUAL(1, sr.n);
	BOOST_CHECK_EQUAL(0, sr.x);
	BOOST_CHECK_EQUAL(1, sr.i);
	BOOST_CHECK_EQUAL(0, sr.s);
	BOOST_CHECK_EQUAL(0, sr.t);
}

BOOST_AUTO_TEST_CASE(sr_to_int1)
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

BOOST_AUTO_TEST_CASE(sr_to_int2)
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

void verifyExecution(const uint8_t* code, uint32_t size, void (*assertFunctor)(const Cpu& c))
{
	// Arrange
	memory memory(256, 0, code, size);
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0);

	// Assert
	assertFunctor(cpu);
}

void verifyExecution(const uint8_t* code, uint32_t size, uint32_t baseAddress, void (*assertFunctor)(const Cpu& c))
{
	// Arrange
	memory memory(256, baseAddress, code, size);
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(baseAddress);

	// Assert
	assertFunctor(cpu);
}

void validateSR(const Cpu& cpu, int x, int n, int z, int v, int c)
{
	if (x != -1) BOOST_CHECK_EQUAL(x, cpu.sr.x);
	if (n != -1) BOOST_CHECK_EQUAL(n, cpu.sr.n);
	if (z != -1) BOOST_CHECK_EQUAL(z, cpu.sr.z);
	if (v != -1) BOOST_CHECK_EQUAL(v, cpu.sr.v);
	if (c != -1) BOOST_CHECK_EQUAL(c, cpu.sr.c);
}
BOOST_AUTO_TEST_CASE(a_moveToD2_b)
{
	// Arrange
	unsigned char code[] = { 0b0001'0100u, 0b0011'1100u, 0, 0x20, 0x4e, 0x40 }; // move.b #$20,d2

	// Act
	verifyExecution(code, sizeof(code), [](const Cpu& cpu) { BOOST_CHECK_EQUAL(0x20, cpu.d2); });
}

BOOST_AUTO_TEST_CASE(a_moveToD4_w)
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

BOOST_AUTO_TEST_CASE(a_moveNegativeToD4_w)
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

BOOST_AUTO_TEST_CASE(a_moveToD7_l)
{
	// Arrange
	unsigned char code[] = { 0b0010'1110u, 0b0011'1100u, 0x12, 0x34, 0x56, 0x78, 0x4e, 0x40 }; // move.w #$1234,d7

	// Assert
	verifyExecution(code, sizeof(code), [](const Cpu& cpu) { BOOST_CHECK_EQUAL(0x12345678, cpu.d7); });
}

BOOST_AUTO_TEST_CASE(a_moveToA2_w)
{
	// Arrange
	unsigned char code[] = { 0b0011'0100u, 0b0111'1100u, 0x12, 0x34, 0x4e, 0x40 }; // movea.w #$1234,a2

	// Assert
	verifyExecution(code, sizeof(code), [](const Cpu& cpu) { BOOST_CHECK_EQUAL(0x1234, cpu.a2); });
}

BOOST_AUTO_TEST_CASE(a_moveNegativeToA2_w)
{
	// Arrange
	unsigned char code[] = { 0b0011'0100u, 0b0111'1100u, 0x87, 0x34, 0x4e, 0x40 }; // movea.w #$8734,a2

	// Assert
	verifyExecution(code, sizeof(code), [](const Cpu& cpu) { BOOST_CHECK_EQUAL(0xffff8734, cpu.a2); });
}

// =================================================================================================
// Addressing mode tests - Read
// =================================================================================================


BOOST_AUTO_TEST_CASE(a_addressMode_000)
{
	// move.w #$1234,d4 ; movea.w d4,a0 ; trap #0
	unsigned char code[] = { 0x38, 0x3c, 0x12, 0x34, 0x30, 0x44, 0x4e, 0x40 };
	verifyExecution(code, sizeof(code), [](const Cpu& cpu)
		{
			BOOST_CHECK_EQUAL(0x1234, cpu.d4);
			BOOST_CHECK_EQUAL(0x1234, cpu.a0);
		});
}

BOOST_AUTO_TEST_CASE(a_addressMode_000_value)
{
	unsigned char code[] = { 
		0x30, 0x3C, 0x21, 0x21,             // move.w #$2121,d0
		0x10, 0x3C, 0x00, 0x42,             // move.b #$42,d0
		0x22, 0x3C, 0x10, 0x20, 0x30, 0x40, // move.l #$10203040, d1
		0x22, 0x3C, 0x40, 0x30, 0x20, 0x10, // move.l #$10203040, d1
		0x4E, 0x40,				            // trap #0
		0xFF, 0xFF };

	// Arrange
	memory memory(256, 0, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0);

	// Assert
	BOOST_CHECK_EQUAL(0x2142, cpu.d0);
	BOOST_CHECK_EQUAL(0x40302010, cpu.d1);
}

BOOST_AUTO_TEST_CASE(a_addressMode_001)
{
	// movea.w #$1234,a4 ; movea.w a4,a0 ; trap #0
	unsigned char code[] = { 0x38, 0x7c, 0x12, 0x34, 0b0011'0000u, 0b01'001'100u, 0x4e, 0x40 };
	verifyExecution(code, sizeof(code), [](const Cpu& cpu)
		{
			BOOST_CHECK_EQUAL(0x1234, cpu.a4);
			BOOST_CHECK_EQUAL(0x1234, cpu.a0);
		});
}

BOOST_AUTO_TEST_CASE(a_addressMode_010)
{
	// movea.w #10,a4 ; movea.w (a4),a0 ; trap #0
	unsigned char code[] = { 0x38, 0x7c, 0x00, 0x0A, 0b0011'0000u, 0b01'010'100u, 0x4e, 0x40, 0xff, 0xff, 0x43, 0x21 };
	verifyExecution(code, sizeof(code), [](const Cpu& cpu)
		{
			BOOST_CHECK_EQUAL(0x0A, cpu.a4);
			BOOST_CHECK_EQUAL(0x4321, cpu.a0);
		});
}

BOOST_AUTO_TEST_CASE(a_addressMode_011)
{
	// movea.w #10,a4 ; movea.w (a4)+,a0 ; trap #0
	unsigned char code[] = { 0x38, 0x7c, 0x00, 0x0A, 0b0011'0000u, 0b01'011'100u, 0x4e, 0x40, 0xff, 0xff, 0x43, 0x21 };
	verifyExecution(code, sizeof(code), [](const Cpu& cpu)
		{
			BOOST_CHECK_EQUAL(0x0C, cpu.a4);
			BOOST_CHECK_EQUAL(0x4321, cpu.a0);
		});
}

BOOST_AUTO_TEST_CASE(a_addressMode_100)
{
	// movea.w #12,a4 ; movea.w -(a4),a0 ; trap #0
	unsigned char code[] = { 0x38, 0x7c, 0x00, 0x0C, 0b0011'0000u, 0b01'100'100u, 0x4e, 0x40, 0xff, 0xff, 0x43, 0x21 };
	verifyExecution(code, sizeof(code), [](const Cpu& cpu)
		{
			BOOST_CHECK_EQUAL(0x0A, cpu.a4);
			BOOST_CHECK_EQUAL(0x4321, cpu.a0);
		});
}

BOOST_AUTO_TEST_CASE(a_addressMode_101_positive)
{
	// movea.w #6,a4 ; movea.w 6(a4),a0 ; trap #0
	unsigned char code[] = { 0x38, 0x7c, 0x00, 0x06, 0b0011'0000u, 0b01'101'100u, 0x00, 0x06, 0x4e, 0x40, 0xff, 0xff, 0x43, 0x21 };
	verifyExecution(code, sizeof(code), [](const Cpu& cpu)
		{
			BOOST_CHECK_EQUAL(0x06, cpu.a4);
			BOOST_CHECK_EQUAL(0x4321, cpu.a0);
		});
}

BOOST_AUTO_TEST_CASE(a_addressMode_101_negative)
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

BOOST_AUTO_TEST_CASE(a_addressMode_101_long)
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


BOOST_AUTO_TEST_CASE(a_addressMode_110_positive)
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

BOOST_AUTO_TEST_CASE(a_addressMode_110_negative)
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

BOOST_AUTO_TEST_CASE(a_addressMode_111_000)
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

BOOST_AUTO_TEST_CASE(a_addressMode_111_001)
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

BOOST_AUTO_TEST_CASE(a_addressMode_111_010)
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

BOOST_AUTO_TEST_CASE(a_addressMode_111_011)
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

BOOST_AUTO_TEST_CASE(a_addressMode_w000)
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

BOOST_AUTO_TEST_CASE(a_addressMode_w010)
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

BOOST_AUTO_TEST_CASE(a_addressMode_w011)
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

BOOST_AUTO_TEST_CASE(a_addressMode_w100)
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

BOOST_AUTO_TEST_CASE(a_addressMode_w101_p)
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

BOOST_AUTO_TEST_CASE(a_addressMode_w101_n)
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

BOOST_AUTO_TEST_CASE(a_addressMode_w110_p)
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

BOOST_AUTO_TEST_CASE(a_addressMode_w110_n)
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

BOOST_AUTO_TEST_CASE(a_addressMode_w111_000)
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

BOOST_AUTO_TEST_CASE(a_addressMode_w111_001)
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
/*=================================================================================================
									org     $00100000       ;Start at 00100000
					strtolower      public
00100000  4E56 0000                     link    a6,#0           ;Set up stack frame
00100004  306E 0008                     movea   8(a6),a0        ;A0 = src, from stack
00100008  326E 000C                     movea   12(a6),a1       ;A1 = dst, from stack
0010000C  1018          loop            move.b  (a0)+,d0        ;Load D0 from (src), incr src
0010000E  0C40 0041                     cmpi    #'A',d0         ;If D0 < 'A',
00100012  6500 000E                     blo     copy            ;skip
00100016  0C40 005A                     cmpi    #'Z',d0         ;If D0 > 'Z',
0010001A  6200 0006                     bhi     copy            ;skip
0010001E  0640 0020                     addi    #'a'-'A',d0     ;D0 = lowercase(D0)
00100022  12C0          copy            move.b  d0,(a1)+        ;Store D0 to (dst), incr dst
00100024  66E6                          bne     loop            ;Repeat while D0 <> NUL
00100026  4E5E                          unlk    a6              ;Restore stack frame
00100028  4E75                          rts                     ;Return
0010002A                                end

=================================================================================================*/
BOOST_AUTO_TEST_CASE(a_toLowerCase)
{
	unsigned char code[] = {     //             org     $1000          ; Start at 001000
		0x4E, 0x56, 0x00, 0x00,  //             link    a6,#0          ; Set up stack frame
		0x30, 0x7c, 0x10, 0x2e,  //             movea   #src,a0        ; A0 = src
		0x32, 0x7c, 0x10, 0x36,  //             movea   #dst,a1        ; A1 = dst
		0x10, 0x18,              //  loop       move.b(a0) + ,d0       ; Load D0 from(src), incr src
		0x0C, 0x40, 0x00, 0x41,  //             cmpi    #'A',d0        ; If D0 < 'A',
		0x65, 0x00, 0x00, 0x0E,  //             blo     copy           ; skip
		0x0C, 0x40, 0x00, 0x5A,  //             cmpi    #'Z',d0        ; If D0 > 'Z',
		0x62, 0x00, 0x00, 0x06,  //             bhi     copy           ; skip
		0x06, 0x40, 0x00, 0x20,  //             addi    #'a' - 'A',d0  ; D0 = lowercase(D0)
		0x12, 0xC0,              //  copy       move.b  d0,(a1)+       ; Store D0 to(dst), incr dst
		0x66, 0xE6,              //             bne     loop           ; Repeat while D0 <> NUL
		0x4E, 0x5E,              //             unlk    a6             ; Restore stack frame
		0x4E, 0x75,              //             rts                    ; Return
		0x4e, 0x40,              //             trap #0
		0xff, 0xff,
		'L', 'A', 'U', 'R', 0x00 };


	uint32_t src = 46;
	uint32_t dst = 54;
	uint32_t a6 = 100;
	// Arrange
	BOOST_CHECK_EQUAL('L', code[src]);

	uint32_t base = 0x001000;
	memory memory(1024, base, code, sizeof(code));
	memory.set<uint32_t>(base + a6 + 8, base + src);
	memory.set<uint32_t>(base + a6 + 12, base + dst);

	Cpu cpu(memory);

	cpu.reset();
	cpu.setARegister(6, base + a6);

	// Act
	cpu.start(base, base+1024);

	// Assert
	auto result = cpu.mem.get<uint8_t>(base + dst);
	BOOST_CHECK_EQUAL('l', result);
}


// =================================================================================================
// Instructions specific tests
// =================================================================================================

void verifyBccExecution(uint8_t ccr, uint8_t bccOp)
{
	//uint8_t ccr = 24;
	//uint8_t bccOp = 64;

	unsigned char code[] = {
		0x44, 0xfc, 0x00, ccr,     //       move #24,ccr
		bccOp, 0x00, 0x00, 0x04,   //       bcc pass
		0x4e, 0x40,                //       trap #0
		0x70, 0x2a,                // pass: moveq #42,d0
		0x4e, 0x40,                //       trap #0
		0xff, 0xff };

	verifyExecution(code, sizeof(code), [](const Cpu& cpu)
		{
			BOOST_CHECK_EQUAL(42, cpu.d0);
		});
}
BOOST_AUTO_TEST_CASE(a_bcc)
{
	verifyBccExecution(0x24, 0x64); // C=0
}

BOOST_AUTO_TEST_CASE(a_bcs)
{
	verifyBccExecution(0x19, 0x65); // C=1
}

BOOST_AUTO_TEST_CASE(a_beq)
{
	verifyBccExecution(0x04, 0x67); // Z=1
}

BOOST_AUTO_TEST_CASE(a_bne)
{
	verifyBccExecution(0x08, 0x66); // Z=0
}

BOOST_AUTO_TEST_CASE(a_bge)
{
	verifyBccExecution(0x02, 0x6c); // N=0 V=1
}

BOOST_AUTO_TEST_CASE(a_bgt)
{
	verifyBccExecution(0x08, 0x6e); // N=1 Z=0 V=0
}

BOOST_AUTO_TEST_CASE(a_bhi)
{
	verifyBccExecution(0x18, 0x62); // C=0 Z=0
}

BOOST_AUTO_TEST_CASE(a_ble)
{
	verifyBccExecution(0x0a, 0x6f);  // N=1 Z=0 V=1
}

BOOST_AUTO_TEST_CASE(a_bls)
{
	verifyBccExecution(0x01, 0x63); // Z=0 C=1
}

BOOST_AUTO_TEST_CASE(a_blt)
{
	verifyBccExecution(0x0b, 0x6d); // N=1 V=1
}

BOOST_AUTO_TEST_CASE(a_bmi)
{
	verifyBccExecution(0x08, 0x6b); // N=1
}

BOOST_AUTO_TEST_CASE(a_bpl)
{
	verifyBccExecution(0x07, 0x6a); // N=0
}

BOOST_AUTO_TEST_CASE(a_bvc)
{
	verifyBccExecution(0x05, 0x68); // V=0
}

BOOST_AUTO_TEST_CASE(a_bvs)
{
	verifyBccExecution(0x07, 0x69); // V=1
}

BOOST_AUTO_TEST_CASE(a_clr)
{
	unsigned char code[] = {
		0x20, 0x3c, 0x12, 0x34, 0x56, 0x78,    // move.l #$12345678,d0
		0x42, 0x00,                            // clr.b  d0
		0x22, 0x3c, 0x23, 0x45, 0x67, 0x89,    // move.l #$23456789,d1
		0x42, 0x41,                            // clr.w  d1
		0x24, 0x3c, 0x34, 0x56, 0x78, 0x9A,    // move.l #$3456789A,d2
		0x42, 0x82,                            // clr.l  d2
		0x4e, 0x40,                            // trap #0
		0xff, 0xff };

	// Arrange
	memory memory(256, 0, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0);

	// Assert
	BOOST_CHECK_EQUAL(0x12345600, cpu.d0);
	BOOST_CHECK_EQUAL(0x23450000, cpu.d1);
	BOOST_CHECK_EQUAL(0x00000000, cpu.d2);
	BOOST_CHECK_EQUAL(0, cpu.sr.c);
	BOOST_CHECK_EQUAL(1, cpu.sr.z);
	BOOST_CHECK_EQUAL(0, cpu.sr.n);
	BOOST_CHECK_EQUAL(0, cpu.sr.v);

}

void verifyCmpiExecution_b(uint8_t d0, uint8_t cmp, uint8_t n, uint8_t z, uint8_t v, uint8_t c)
{
	unsigned char code[] = {
		0x10, 0x3c, 0x00, d0,    // move.b #10,d0
		0x0c, 0x00, 0x00, cmp,   // cmpi.b #5, d0
		0x4e, 0x40,              // trap #0
		0xff, 0xff };

	// Arrange
	memory memory(256, 0, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0);

	// Assert
	BOOST_CHECK_EQUAL(c, cpu.sr.c);
	BOOST_CHECK_EQUAL(z, cpu.sr.z);
	BOOST_CHECK_EQUAL(n, cpu.sr.n);
	BOOST_CHECK_EQUAL(v, cpu.sr.v);

}
inline uint8_t u8(uint16_t val) { return static_cast<uint8_t>(val); }

void verifyCmpiExecution_w(uint16_t d0, uint16_t cmp, uint8_t n, uint8_t z, uint8_t v, uint8_t c)
{
	unsigned char code[] = {
		0x10, 0x3c, u8(d0 >> 8),  u8(d0 & 0xff),    // move.w #10,d0
		0x0c, 0x00, u8(cmp >> 8), u8(cmp &0xff),    // cmpi.w #5, d0
		0x4e, 0x40,                                 // trap #0
		0xff, 0xff };

	// Arrange
	memory memory(256, 0, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0);

	// Assert
	BOOST_CHECK_EQUAL(c, cpu.sr.c);
	BOOST_CHECK_EQUAL(z, cpu.sr.z);
	BOOST_CHECK_EQUAL(n, cpu.sr.n);
	BOOST_CHECK_EQUAL(v, cpu.sr.v);

}

void verifyCmpiExecution_l(uint32_t d0, uint32_t cmp, uint8_t n, uint8_t z, uint8_t v, uint8_t c)
{
	unsigned char code[] = {
		0x20, 0x3c,  u8(d0 >> 24),  u8((d0 >> 16) & 0xff),  u8((d0 >> 8) & 0xff),  u8(d0 & 0xff),    // move.l #10,d0
		0x0c, 0x80,  u8(cmp >> 24), u8((cmp >> 16) & 0xff), u8((cmp >> 8) & 0xff), u8(cmp & 0xff),   // cmpi.l #5, d0
		0x4e, 0x40,                                                                                  // trap #0
		0xff, 0xff };

	// Arrange
	memory memory(256, 0, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0);

	// Assert
	BOOST_CHECK_EQUAL(c, cpu.sr.c);
	BOOST_CHECK_EQUAL(z, cpu.sr.z);
	BOOST_CHECK_EQUAL(n, cpu.sr.n);
	BOOST_CHECK_EQUAL(v, cpu.sr.v);

}
BOOST_AUTO_TEST_CASE(a_cmpi_b)
{
	verifyCmpiExecution_b(10, 5, 0, 0, 0, 0);
	verifyCmpiExecution_b(5,  5, 0, 1, 0, 0);
	verifyCmpiExecution_b(5, 10, 1, 0, 0, 1);
	verifyCmpiExecution_b(0x70, 0x81, 1, 0, 1, 1);
}

BOOST_AUTO_TEST_CASE(a_cmpi_w)
{
	verifyCmpiExecution_w(0x1010, 0x0505, 0, 0, 0, 0);
	verifyCmpiExecution_w(0x0505, 0x0505, 0, 1, 0, 0);
	verifyCmpiExecution_w(0x0505, 0x1010, 1, 0, 0, 1);
	verifyCmpiExecution_w(0x7070, 0x8181, 1, 0, 1, 1);
}

BOOST_AUTO_TEST_CASE(a_cmpi_l)
{
	verifyCmpiExecution_l(0x10101010, 0x05050505, 0, 0, 0, 0);
	verifyCmpiExecution_l(0x05050505, 0x05050505, 0, 1, 0, 0);
	verifyCmpiExecution_l(0x05050505, 0x10101010, 1, 0, 0, 1);
	verifyCmpiExecution_l(0x70707070, 0x81818181, 1, 0, 1, 1);
}

BOOST_AUTO_TEST_CASE(a_cmp)
{
	unsigned char code[] = {
		0x36, 0x3c,  0x70, 0x70,    // move.w #$7070,d3
		0x32, 0x3c,  0x81, 0x81,    // move.w #$8181,d1
		0xb6, 0x41,                 // cmp.w d1, d3
		0x4e, 0x40,                 // trap #0
		0xff, 0xff };

	// Arrange
	memory memory(256, 100, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(100);

	// Assert
	BOOST_CHECK_EQUAL(1, cpu.sr.c);
	BOOST_CHECK_EQUAL(0, cpu.sr.z);
	BOOST_CHECK_EQUAL(1, cpu.sr.n);
	BOOST_CHECK_EQUAL(1, cpu.sr.v);

}

BOOST_AUTO_TEST_CASE(a_cmpa)
{
	unsigned char code[] = {
		0x30, 0x7c,  0x70, 0x70,    // move.w #$7070,a0
		0x32, 0x3c,  0x81, 0x81,    // move.w #$8181,d1
		0xb0, 0xc1,                 // cmpa.w d1, a0
		0x4e, 0x40,                 // trap #0
		0xff, 0xff };

	// Arrange
	memory memory(256, 100, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(100);

	// Assert
	BOOST_CHECK_EQUAL(1, cpu.sr.c);
	BOOST_CHECK_EQUAL(0, cpu.sr.z);
	BOOST_CHECK_EQUAL(1, cpu.sr.n);
	BOOST_CHECK_EQUAL(1, cpu.sr.v);

}
// ===================================================
// LEA tests
// ===================================================
BOOST_AUTO_TEST_CASE(a_lea)
{
	unsigned char code[] = {
		0x41, 0xf9, 0x00, 0x00, 0x10, 0x24,    // lea data, a0
		0x43, 0xf8, 0x10, 0x24,                // lea data.w, a1
		0x45, 0xd1,                            // lea (a1), a2
		0x47, 0xea, 0x00, 0x04,                // lea 4(a2), a3
		0x76, 0x08,                            // moveq #8, d3
		0x49, 0xf2, 0x30, 0x04,                // lea 4(a2,d3), a4
		0x4b, 0xfa, 0x00, 0x0c,                // lea data(pc), a5
		0x4d, 0xfb, 0x30, 0x08,                // lea data(pc,d3), a6
		0x4e, 0x40,                            // trap #0
		0xff, 0xff, 0xff, 0xff,
	    0x10, 0x00 };                          // data dc.b $1000    

	verifyExecution(code, sizeof(code), 0x1000, [](const Cpu& cpu)
		{
			BOOST_CHECK_EQUAL(0x1024, cpu.a0);
			BOOST_CHECK_EQUAL(0x1024, cpu.a1);
			BOOST_CHECK_EQUAL(0x1024, cpu.a2);
			BOOST_CHECK_EQUAL(0x1028, cpu.a3);
			BOOST_CHECK_EQUAL(0x1030, cpu.a4);
			BOOST_CHECK_EQUAL(0x1024, cpu.a5);
			BOOST_CHECK_EQUAL(0x102C, cpu.a6);
		});
}

// ===================================================
// LINK tests
// ===================================================
BOOST_AUTO_TEST_CASE(a_link)
{
	unsigned char code[] = {
		0x4f, 0xf9, 0x00, 0x00, 0x10, 0x44, // lea stack,a7
		0x3c, 0x7c, 0x12, 0x34,             // move #$1234, a6
		0x3a, 0x7c, 0x56, 0x78,             // move #$5678, a5
		0x38, 0x7c, 0x9a, 0xbc,             // move #$9abc, a4

		0x4e, 0x56, 0x00, 0x00,             // link a6,#0
		0x4e, 0x55, 0xff, 0xf8,             // link a5,#-8
		0x4e, 0x54, 0x00, 0x08,             // link a4,#8

		0x4e, 0x40,                         // trap #0
		0xff, 0xff, 0xff, 0xff,
		0xee, 0xee, 0xee, 0xee,             // ds.w 16
	};                                      // stack:

	verifyExecution(code, sizeof(code), 0x1000, [](const Cpu& cpu)
		{
			BOOST_CHECK_EQUAL(0x1030, cpu.a4);
			BOOST_CHECK_EQUAL(0x103c, cpu.a5);
			BOOST_CHECK_EQUAL(0x1040, cpu.a6);
			BOOST_CHECK_EQUAL(0x1038, cpu.a7);
			BOOST_CHECK_EQUAL(0x1234, cpu.mem.get<uint32_t>(cpu.a6));
			BOOST_CHECK_EQUAL(0x5678, cpu.mem.get<uint32_t>(cpu.a5));
			BOOST_CHECK_EQUAL(0xffff9abc, cpu.mem.get<uint32_t>(cpu.a4));
		});
}

BOOST_AUTO_TEST_CASE(a_moveq)
{
	unsigned char code[] = { 
		0x74, 0xfe,    // moveq.l #-2,d2
		0x76, 0x04,    // moveq.l #4,d3
		0x4e, 0x40,    // trap #0
		0xff, 0xff};
	verifyExecution(code, sizeof(code), [](const Cpu& cpu)
		{
			BOOST_CHECK_EQUAL(-2, cpu.d2);
			BOOST_CHECK_EQUAL(4, cpu.d3);
		});
}

BOOST_AUTO_TEST_CASE(a_move2ccr_1)
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

BOOST_AUTO_TEST_CASE(a_move2ccr_2)
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

BOOST_AUTO_TEST_CASE(sr_movefromsr)
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
BOOST_AUTO_TEST_SUITE_END()
