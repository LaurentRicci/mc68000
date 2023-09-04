#include <boost/test/unit_test.hpp>
#include "../core/cpu.h"
#include "../core/memory.h"

using namespace mc68000;

BOOST_AUTO_TEST_SUITE(Asm)

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

void verifyExecution(const uint8_t* code, size_t size, void (*asert)(const cpu& c))
{
	// Arrange
	memory memory(256, 0, code, sizeof(code));
	cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0);

	// Assert
	asert(cpu);
}

BOOST_AUTO_TEST_CASE(a_moveToD0_b)
{
	// Arrange
	unsigned char code[] = { 0b0001'0000u, 0b0011'1100u, 0, 0x20 }; // move.b #$20,d0

	// Act
	verifyExecution(code, sizeof(code), [](const cpu& cpu) { BOOST_CHECK_EQUAL(0x20, cpu.d0); });
}

BOOST_AUTO_TEST_CASE(a_moveToD0_w)
{
	// Arrange
	unsigned char code[] = { 0b0011'0000u, 0b0011'1100u, 0x12, 0x34 }; // move.w #$1234,d0

	// Assert
	verifyExecution(code, sizeof(code), [](const cpu& cpu) { BOOST_CHECK_EQUAL(0x1234, cpu.d0); });
}

BOOST_AUTO_TEST_CASE(a_moveToD0_l)
{
	// Arrange
	unsigned char code[] = { 0b0010'0000u, 0b0011'1100u, 0x12, 0x34, 0x56, 0x78 }; // move.w #$1234,d0

	// Assert
	verifyExecution(code, sizeof(code), [](const cpu& cpu) { BOOST_CHECK_EQUAL(0x12345678, cpu.d0); });
}

BOOST_AUTO_TEST_SUITE_END()