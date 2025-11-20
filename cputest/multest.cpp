#include <boost/test/unit_test.hpp>
#include "../core/cpu.h"
#include "../core/memory.h"

using namespace mc68000;

BOOST_AUTO_TEST_SUITE(cpuSuite_mul)

// ===================================================
// MULS tests
// ===================================================

BOOST_AUTO_TEST_CASE(muls_plusplus)
{
	unsigned char code[] = {
	0x70, 0x7b,            // moveq #123,d0
	0x72, 0x2d,            // moveq #45,d1
	0xc3, 0xc0,            // muls d0,d1
	0x4e, 0x40,            // trap #0
	0xff, 0xff, 0xff, 0xff // 
	};

	// Arrange
	Memory memory(256, 0x1000, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0x1000);

	// Assert
	BOOST_CHECK_EQUAL(123, cpu.d0);
	BOOST_CHECK_EQUAL(123*45, cpu.d1);
	BOOST_CHECK_EQUAL(0, cpu.sr.n);
	BOOST_CHECK_EQUAL(0, cpu.sr.z);
	BOOST_CHECK_EQUAL(0, cpu.sr.v);
	BOOST_CHECK_EQUAL(0, cpu.sr.c);
}

BOOST_AUTO_TEST_CASE(muls_minusplus)
{
	unsigned char code[] = {
	0x70, 0x7b,            // moveq #123,d0
	0x72, 0xd3,            // moveq #-45,d1
	0xc3, 0xc0,            // muls d0,d1
	0x4e, 0x40,            // trap #0
	0xff, 0xff, 0xff, 0xff // 
	};

	// Arrange
	Memory memory(256, 0x1000, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0x1000);

	// Assert
	BOOST_CHECK_EQUAL(123, cpu.d0);
	BOOST_CHECK_EQUAL(-(123 * 45), cpu.d1);
	BOOST_CHECK_EQUAL(1, cpu.sr.n);
	BOOST_CHECK_EQUAL(0, cpu.sr.z);
	BOOST_CHECK_EQUAL(0, cpu.sr.v);
	BOOST_CHECK_EQUAL(0, cpu.sr.c);
}

BOOST_AUTO_TEST_CASE(muls_plusminus)
{
	unsigned char code[] = {
	0x70, 0x85,            // moveq #-123,d0
	0x72, 0x2d,            // moveq #45,d1
	0xc3, 0xc0,            // muls d0,d1
	0x4e, 0x40,            // trap #0
	0xff, 0xff, 0xff, 0xff // 
	};

	// Arrange
	Memory memory(256, 0x1000, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0x1000);

	// Assert
	BOOST_CHECK_EQUAL(-123, cpu.d0);
	BOOST_CHECK_EQUAL(-(123 * 45), cpu.d1);
	BOOST_CHECK_EQUAL(1, cpu.sr.n);
	BOOST_CHECK_EQUAL(0, cpu.sr.z);
	BOOST_CHECK_EQUAL(0, cpu.sr.v);
	BOOST_CHECK_EQUAL(0, cpu.sr.c);
}

BOOST_AUTO_TEST_CASE(muls_minusminus)
{
	unsigned char code[] = {
	0x70, 0x85,            // moveq #-123,d0
	0x72, 0xd3,            // moveq #-45,d1
	0xc3, 0xc0,            // muls d0,d1
	0x4e, 0x40,            // trap #0
	0xff, 0xff, 0xff, 0xff // 
	};

	// Arrange
	Memory memory(256, 0x1000, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0x1000);

	// Assert
	BOOST_CHECK_EQUAL(-123, cpu.d0);
	BOOST_CHECK_EQUAL(123 * 45, cpu.d1);
	BOOST_CHECK_EQUAL(0, cpu.sr.n);
	BOOST_CHECK_EQUAL(0, cpu.sr.z);
	BOOST_CHECK_EQUAL(0, cpu.sr.v);
	BOOST_CHECK_EQUAL(0, cpu.sr.c);
}

// ===================================================
// MULU tests
// ===================================================

BOOST_AUTO_TEST_CASE(mulu_plusplus)
{
	unsigned char code[] = {
	0x70, 0x7b,            // moveq #123,d0
	0x72, 0x2d,            // moveq #45,d1
	0xc2, 0xc0,            // mulu d0,d1
	0x4e, 0x40,            // trap #0
	0xff, 0xff, 0xff, 0xff // 
	};

	// Arrange
	Memory memory(256, 0x1000, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0x1000);

	// Assert
	BOOST_CHECK_EQUAL(123, cpu.d0);
	BOOST_CHECK_EQUAL(123 * 45, cpu.d1);
	BOOST_CHECK_EQUAL(0, cpu.sr.n);
	BOOST_CHECK_EQUAL(0, cpu.sr.z);
	BOOST_CHECK_EQUAL(0, cpu.sr.v);
	BOOST_CHECK_EQUAL(0, cpu.sr.c);
}

BOOST_AUTO_TEST_CASE(mulu_minusplus)
{
	unsigned char code[] = {
	0x70, 0x7b,            // moveq #123,d0
	0x72, 0xd3,            // moveq #-45,d1
	0xc2, 0xc0,            // mulu d0,d1
	0x4e, 0x40,            // trap #0
	0xff, 0xff, 0xff, 0xff // 
	};

	// Arrange
	Memory memory(256, 0x1000, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0x1000);

	// Assert
	BOOST_CHECK_EQUAL(123, cpu.d0);
	BOOST_CHECK_EQUAL(123 * 0xffd3, cpu.d1);
	BOOST_CHECK_EQUAL(0, cpu.sr.n);
	BOOST_CHECK_EQUAL(0, cpu.sr.z);
	BOOST_CHECK_EQUAL(0, cpu.sr.v);
	BOOST_CHECK_EQUAL(0, cpu.sr.c);
}

BOOST_AUTO_TEST_CASE(mulu_plusminus)
{
	unsigned char code[] = {
	0x70, 0x85,            // moveq #-123,d0
	0x72, 0x2d,            // moveq #45,d1
	0xc2, 0xc0,            // mulu d0,d1
	0x4e, 0x40,            // trap #0
	0xff, 0xff, 0xff, 0xff // 
	};

	// Arrange
	Memory memory(256, 0x1000, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0x1000);

	// Assert
	BOOST_CHECK_EQUAL(-123, cpu.d0);
	BOOST_CHECK_EQUAL(0xff85 * 45, cpu.d1);
	BOOST_CHECK_EQUAL(0, cpu.sr.n);
	BOOST_CHECK_EQUAL(0, cpu.sr.z);
	BOOST_CHECK_EQUAL(0, cpu.sr.v);
	BOOST_CHECK_EQUAL(0, cpu.sr.c);
}
BOOST_AUTO_TEST_SUITE_END()