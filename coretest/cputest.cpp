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
// Addressing mode tests
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
BOOST_AUTO_TEST_CASE(a_addressMode_002)
{
	// movea.w #10,a4 ; movea.w (a4),a0 ; trap #0
	unsigned char code[] = { 0x38, 0x7c, 0x00, 0x0A, 0b0011'0000u, 0b01'010'100u, 0x4e, 0x40, 0xff, 0xff, 0x43, 0x21 };
	verifyExecution(code, sizeof(code), [](const cpu& cpu)
		{
			BOOST_CHECK_EQUAL(0xA, cpu.a4);
			BOOST_CHECK_EQUAL(0x4321, cpu.a0);
		});
}




BOOST_AUTO_TEST_CASE(a_sr)
{
	memory memory;
	cpu cpu(memory);

	BOOST_CHECK_EQUAL(0, cpu.sr.c);

}
BOOST_AUTO_TEST_SUITE_END()