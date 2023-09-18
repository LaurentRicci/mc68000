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
	cpu cpu(memory);

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

void verifyExecution(const uint8_t* code, size_t size, void (*assertFunctor)(const cpu& c))
{
	// Arrange
	memory memory(256, 0, code, size);
	cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0);

	// Assert
	assertFunctor(cpu);
}

void verifyExecution(const uint8_t* code, size_t size, uint32_t baseAddress, void (*assertFunctor)(const cpu& c))
{
	// Arrange
	memory memory(256, baseAddress, code, size);
	cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(baseAddress);

	// Assert
	assertFunctor(cpu);
}

void validateSR(const cpu& cpu, int x, int n, int z, int v, int c)
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
	verifyExecution(code, sizeof(code), [](const cpu& cpu) { BOOST_CHECK_EQUAL(0x20, cpu.d2); });
}

BOOST_AUTO_TEST_CASE(a_moveToD4_w)
{
	// Arrange
	unsigned char code[] = { 0b0011'1000u, 0b0011'1100u, 0x12, 0x34, 0x4e, 0x40 }; // move.w #$1234,d4

	// Assert
	verifyExecution(code, sizeof(code), [](const cpu& cpu) 
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
	verifyExecution(code, sizeof(code), [](const cpu& cpu) 
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
	verifyExecution(code, sizeof(code), [](const cpu& cpu) { BOOST_CHECK_EQUAL(0x12345678, cpu.d7); });
}

BOOST_AUTO_TEST_CASE(a_moveToA2_w)
{
	// Arrange
	unsigned char code[] = { 0b0011'0100u, 0b0111'1100u, 0x12, 0x34, 0x4e, 0x40 }; // movea.w #$1234,a2

	// Assert
	verifyExecution(code, sizeof(code), [](const cpu& cpu) { BOOST_CHECK_EQUAL(0x1234, cpu.a2); });
}

BOOST_AUTO_TEST_CASE(a_moveNegativeToA2_w)
{
	// Arrange
	unsigned char code[] = { 0b0011'0100u, 0b0111'1100u, 0x87, 0x34, 0x4e, 0x40 }; // movea.w #$8734,a2

	// Assert
	verifyExecution(code, sizeof(code), [](const cpu& cpu) { BOOST_CHECK_EQUAL(0xffff8734, cpu.a2); });
}

// =================================================================================================
// Addressing mode tests - Read
// =================================================================================================


BOOST_AUTO_TEST_CASE(a_addressMode_000)
{
	// move.w #$1234,d4 ; movea.w d4,a0 ; trap #0
	unsigned char code[] = { 0x38, 0x3c, 0x12, 0x34, 0x30, 0x44, 0x4e, 0x40 };
	verifyExecution(code, sizeof(code), [](const cpu& cpu)
		{
			BOOST_CHECK_EQUAL(0x1234, cpu.d4);
			BOOST_CHECK_EQUAL(0x1234, cpu.a0);
		});
}
BOOST_AUTO_TEST_CASE(a_addressMode_001)
{
	// movea.w #$1234,a4 ; movea.w a4,a0 ; trap #0
	unsigned char code[] = { 0x38, 0x7c, 0x12, 0x34, 0b0011'0000u, 0b01'001'100u, 0x4e, 0x40 };
	verifyExecution(code, sizeof(code), [](const cpu& cpu)
		{
			BOOST_CHECK_EQUAL(0x1234, cpu.a4);
			BOOST_CHECK_EQUAL(0x1234, cpu.a0);
		});
}

BOOST_AUTO_TEST_CASE(a_addressMode_010)
{
	// movea.w #10,a4 ; movea.w (a4),a0 ; trap #0
	unsigned char code[] = { 0x38, 0x7c, 0x00, 0x0A, 0b0011'0000u, 0b01'010'100u, 0x4e, 0x40, 0xff, 0xff, 0x43, 0x21 };
	verifyExecution(code, sizeof(code), [](const cpu& cpu)
		{
			BOOST_CHECK_EQUAL(0x0A, cpu.a4);
			BOOST_CHECK_EQUAL(0x4321, cpu.a0);
		});
}

BOOST_AUTO_TEST_CASE(a_addressMode_011)
{
	// movea.w #10,a4 ; movea.w (a4)+,a0 ; trap #0
	unsigned char code[] = { 0x38, 0x7c, 0x00, 0x0A, 0b0011'0000u, 0b01'011'100u, 0x4e, 0x40, 0xff, 0xff, 0x43, 0x21 };
	verifyExecution(code, sizeof(code), [](const cpu& cpu)
		{
			BOOST_CHECK_EQUAL(0x0C, cpu.a4);
			BOOST_CHECK_EQUAL(0x4321, cpu.a0);
		});
}

BOOST_AUTO_TEST_CASE(a_addressMode_100)
{
	// movea.w #12,a4 ; movea.w -(a4),a0 ; trap #0
	unsigned char code[] = { 0x38, 0x7c, 0x00, 0x0C, 0b0011'0000u, 0b01'100'100u, 0x4e, 0x40, 0xff, 0xff, 0x43, 0x21 };
	verifyExecution(code, sizeof(code), [](const cpu& cpu)
		{
			BOOST_CHECK_EQUAL(0x0A, cpu.a4);
			BOOST_CHECK_EQUAL(0x4321, cpu.a0);
		});
}

BOOST_AUTO_TEST_CASE(a_addressMode_101_positive)
{
	// movea.w #6,a4 ; movea.w 6(a4),a0 ; trap #0
	unsigned char code[] = { 0x38, 0x7c, 0x00, 0x06, 0b0011'0000u, 0b01'101'100u, 0x00, 0x06, 0x4e, 0x40, 0xff, 0xff, 0x43, 0x21 };
	verifyExecution(code, sizeof(code), [](const cpu& cpu)
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
	verifyExecution(code, sizeof(code), [](const cpu& cpu)
		{
			BOOST_CHECK_EQUAL(0x12, cpu.a4);
			BOOST_CHECK_EQUAL(0x4321, cpu.a0);
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
	verifyExecution(code, sizeof(code), [](const cpu& cpu)
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
	verifyExecution(code, sizeof(code), [](const cpu& cpu)
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
	verifyExecution(code, sizeof(code), [](const cpu& cpu)
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
	verifyExecution(code, sizeof(code), 0x32000, [](const cpu& cpu)
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
	verifyExecution(code, sizeof(code), 0x32000, [](const cpu& cpu)
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
	verifyExecution(code, sizeof(code), 0x32000, [](const cpu& cpu)
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
	verifyExecution(code, sizeof(code), [](const cpu& cpu)
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
	verifyExecution(code, sizeof(code), [](const cpu& cpu)
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
	verifyExecution(code, sizeof(code), [](const cpu& cpu)
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
	verifyExecution(code, sizeof(code), [](const cpu& cpu)
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
	verifyExecution(code, sizeof(code), [](const cpu& cpu)
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
	verifyExecution(code, sizeof(code), [](const cpu& cpu)
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
	verifyExecution(code, sizeof(code), [](const cpu& cpu)
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
	verifyExecution(code, sizeof(code), [](const cpu& cpu)
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
	verifyExecution(code, sizeof(code), [](const cpu& cpu)
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
	verifyExecution(code, sizeof(code), 0x32000, [](const cpu& cpu)
		{
			BOOST_CHECK_EQUAL(0x21, cpu.d0);
			BOOST_CHECK_EQUAL(0x21, cpu.mem.get<uint16_t>(0x3200a));
		});
}
BOOST_AUTO_TEST_CASE(a_sr)
{
	memory memory;
	cpu cpu(memory);

	BOOST_CHECK_EQUAL(0, cpu.sr.c);

}

// =================================================================================================
// Instructions specific tests
// =================================================================================================
BOOST_AUTO_TEST_CASE(a_moveq)
{
	unsigned char code[] = { 
		0x74, 0xfe,    // moveq.l #-2,d2
		0x76, 0x04,    // moveq.l #4,d3
		0x4e, 0x40,    // trap #0
		0xff, 0xff};
	verifyExecution(code, sizeof(code), [](const cpu& cpu)
		{
			BOOST_CHECK_EQUAL(-2, cpu.d2);
			BOOST_CHECK_EQUAL(4, cpu.d3);
		});
}

BOOST_AUTO_TEST_SUITE_END()