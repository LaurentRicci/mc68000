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

BOOST_AUTO_TEST_CASE(a_moveToD0)
{
	// Arrange
	unsigned char code[] = { 0b0001'0000u, 0b0011'1100u, 0, 0x20 }; // move.w #$20,d0
	memory memory(256, 0, code, sizeof(code));
	cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0);

	// Assert
	BOOST_CHECK_EQUAL(0x20, cpu.d0);

}

BOOST_AUTO_TEST_SUITE_END()