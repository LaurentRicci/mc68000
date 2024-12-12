#include <boost/test/unit_test.hpp>
#include "../core/cpu.h"
#include "../core/memory.h"

using namespace mc68000;

BOOST_AUTO_TEST_SUITE(cpuSuite_div)

// ===================================================
// DIVS tests
// ===================================================

BOOST_AUTO_TEST_CASE(divs_plusplus)
{
	unsigned char code[] = {
		0x20, 0x3c, 0x00, 0x12, 0xd6, 0x80, // move.l #1234560,d0
		0x32, 0x3c, 0x00, 0x64,             // move.w #100,d1
		0x81, 0xc1,                         // divs d1,d0
		0x4e, 0x40,                         // trap #0
		0xff, 0xff, 0xff, 0xff              // 
	};

	// Arrange
	memory memory(256, 0x1000, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0x1000);

	// Assert
	BOOST_CHECK_EQUAL((60 << 16) + 12345, cpu.d0);
	BOOST_CHECK_EQUAL(100, cpu.d1);
	BOOST_CHECK_EQUAL(0, cpu.sr.n);
	BOOST_CHECK_EQUAL(0, cpu.sr.z);
	BOOST_CHECK_EQUAL(0, cpu.sr.v);
	BOOST_CHECK_EQUAL(0, cpu.sr.c);
}

BOOST_AUTO_TEST_CASE(divs_plusminus)
{
	unsigned char code[] = {
		0x20, 0x3c, 0x00, 0x12, 0xd6, 0x80, // move.l #1234560,d0
		0x32, 0x3c, 0xff, 0x9c,             // move.w #-100,d1
		0x81, 0xc1,                         // divs d1,d0
		0x4e, 0x40,                         // trap #0
		0xff, 0xff, 0xff, 0xff              // 
	};

	// Arrange
	memory memory(256, 0x1000, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0x1000);

	// Assert
	BOOST_CHECK_EQUAL((60 << 16) + (- 12345 & 0xffff), cpu.d0);
	BOOST_CHECK_EQUAL(0xff9c, cpu.d1);
	BOOST_CHECK_EQUAL(1, cpu.sr.n);
	BOOST_CHECK_EQUAL(0, cpu.sr.z);
	BOOST_CHECK_EQUAL(0, cpu.sr.v);
	BOOST_CHECK_EQUAL(0, cpu.sr.c);
}

BOOST_AUTO_TEST_CASE(divs_minusplus)
{
	unsigned char code[] = {
		0x20, 0x3c, 0xff, 0xed, 0x29, 0x80, // move.l #-1234560,d0
		0x32, 0x3c, 0x00, 0x64,             // move.w #100,d1
		0x81, 0xc1,                         // divs d1,d0
		0x4e, 0x40,                         // trap #0
		0xff, 0xff, 0xff, 0xff              // 
	};

	// Arrange
	memory memory(256, 0x1000, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0x1000);

	// Assert
	BOOST_CHECK_EQUAL((-60 << 16) + (-12345 & 0xffff), cpu.d0);
	BOOST_CHECK_EQUAL(100, cpu.d1);
	BOOST_CHECK_EQUAL(1, cpu.sr.n);
	BOOST_CHECK_EQUAL(0, cpu.sr.z);
	BOOST_CHECK_EQUAL(0, cpu.sr.v);
	BOOST_CHECK_EQUAL(0, cpu.sr.c);
}

BOOST_AUTO_TEST_CASE(divs_minusminus)
{
	unsigned char code[] = {
		0x20, 0x3c, 0xff, 0xed, 0x29, 0x80, // move.l #-1234560,d0
		0x32, 0x3c, 0xff, 0x9c,             // move.w #-100,d1
		0x81, 0xc1,                         // divs d1,d0
		0x4e, 0x40,                         // trap #0
		0xff, 0xff, 0xff, 0xff              // 
	};

	// Arrange
	memory memory(256, 0x1000, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0x1000);

	// Assert
	BOOST_CHECK_EQUAL((-60 << 16) + (12345), cpu.d0);
	BOOST_CHECK_EQUAL(0xff9c, cpu.d1);
	BOOST_CHECK_EQUAL(0, cpu.sr.n);
	BOOST_CHECK_EQUAL(0, cpu.sr.z);
	BOOST_CHECK_EQUAL(0, cpu.sr.v);
	BOOST_CHECK_EQUAL(0, cpu.sr.c);
}

BOOST_AUTO_TEST_CASE(divs_overflow)
{
	unsigned char code[] = {
		0x20, 0x3c, 0x12, 0x34, 0x56, 0x78, // move.l #$12345678,d0
		0x32, 0x3c, 0x00, 0x02,             // move.w #2,d1
		0x81, 0xc1,                         // divs d1,d0
		0x4e, 0x40,                         // trap #0
		0xff, 0xff, 0xff, 0xff              // 
	};

	// Arrange
	memory memory(256, 0x1000, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0x1000);

	// Assert
	BOOST_CHECK_EQUAL(0x12345678, cpu.d0);
	BOOST_CHECK_EQUAL(0x2, cpu.d1);
	BOOST_CHECK_EQUAL(0, cpu.sr.n);
	BOOST_CHECK_EQUAL(0, cpu.sr.z);
	BOOST_CHECK_EQUAL(1, cpu.sr.v);
	BOOST_CHECK_EQUAL(0, cpu.sr.c);
}

// ===================================================
// DIVU tests
// ===================================================
BOOST_AUTO_TEST_CASE(divu_plusplus)
{
	unsigned char code[] = {
		0x20, 0x3c, 0x00, 0x12, 0xd6, 0x80, // move.l #1234560,d0
		0x32, 0x3c, 0x00, 0x64,             // move.w #100,d1
		0x80, 0xc1,                         // divu d1,d0
		0x4e, 0x40,                         // trap #0
		0xff, 0xff, 0xff, 0xff              // 
	};

	// Arrange
	memory memory(256, 0x1000, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0x1000);

	// Assert
	BOOST_CHECK_EQUAL((60 << 16) + 12345, cpu.d0);
	BOOST_CHECK_EQUAL(100, cpu.d1);
	BOOST_CHECK_EQUAL(0, cpu.sr.n);
	BOOST_CHECK_EQUAL(0, cpu.sr.z);
	BOOST_CHECK_EQUAL(0, cpu.sr.v);
	BOOST_CHECK_EQUAL(0, cpu.sr.c);
}

BOOST_AUTO_TEST_CASE(divu_plusminus)
{
	unsigned char code[] = {
		0x20, 0x3c, 0x00, 0x12, 0xd6, 0x80, // move.l #1234560,d0
		0x32, 0x3c, 0xff, 0x9c,             // move.w #-100,d1
		0x80, 0xc1,                         // divu d1,d0
		0x4e, 0x40,                         // trap #0
		0xff, 0xff, 0xff, 0xff              // 
	};

	// Arrange
	memory memory(256, 0x1000, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0x1000);

	// Assert
	BOOST_CHECK_EQUAL(0xdd880012, cpu.d0);
	BOOST_CHECK_EQUAL(0xff9c, cpu.d1);
	BOOST_CHECK_EQUAL(0, cpu.sr.n);
	BOOST_CHECK_EQUAL(0, cpu.sr.z);
	BOOST_CHECK_EQUAL(0, cpu.sr.v);
	BOOST_CHECK_EQUAL(0, cpu.sr.c);
}

BOOST_AUTO_TEST_CASE(divu_minusplus)
{
	unsigned char code[] = {
		0x20, 0x3c, 0xff, 0xed, 0x29, 0x80, // move.l #-1234560,d0
		0x32, 0x3c, 0x00, 0x64,             // move.w #100,d1
		0x80, 0xc1,                         // divu d1,d0
		0x4e, 0x40,                         // trap #0
		0xff, 0xff, 0xff, 0xff              // 
	};

	// Arrange
	memory memory(256, 0x1000, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0x1000);

	// Assert
	BOOST_CHECK_EQUAL(-1234560, cpu.d0);
	BOOST_CHECK_EQUAL(100, cpu.d1);
	BOOST_CHECK_EQUAL(0, cpu.sr.n);
	BOOST_CHECK_EQUAL(0, cpu.sr.z);
	BOOST_CHECK_EQUAL(1, cpu.sr.v);
	BOOST_CHECK_EQUAL(0, cpu.sr.c);
}

BOOST_AUTO_TEST_CASE(divu_minusminus)
{
	unsigned char code[] = {
		0x20, 0x3c, 0xff, 0xed, 0x29, 0x80, // move.l #-1234560,d0
		0x32, 0x3c, 0xff, 0x9c,             // move.w #-100,d1
		0x80, 0xc1,                         // divu d1,d0
		0x4e, 0x40,                         // trap #0
		0xff, 0xff, 0xff, 0xff              // 
	};

	// Arrange
	memory memory(256, 0x1000, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0x1000);

	// Assert
	BOOST_CHECK_EQUAL(-1234560, cpu.d0);
	BOOST_CHECK_EQUAL(0xff9c, cpu.d1);
	BOOST_CHECK_EQUAL(1, cpu.sr.n);
	BOOST_CHECK_EQUAL(0, cpu.sr.z);
	BOOST_CHECK_EQUAL(1, cpu.sr.v);
	BOOST_CHECK_EQUAL(0, cpu.sr.c);
}

BOOST_AUTO_TEST_CASE(divu_overflow)
{
	unsigned char code[] = {
		0x20, 0x3c, 0x12, 0x34, 0x56, 0x78, // move.l #$12345678,d0
		0x32, 0x3c, 0x00, 0x02,             // move.w #2,d1
		0x80, 0xc1,                         // divu d1,d0
		0x4e, 0x40,                         // trap #0
		0xff, 0xff, 0xff, 0xff              // 
	};

	// Arrange
	memory memory(256, 0x1000, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0x1000);

	// Assert
	BOOST_CHECK_EQUAL(0x12345678, cpu.d0);
	BOOST_CHECK_EQUAL(0x2, cpu.d1);
	BOOST_CHECK_EQUAL(0, cpu.sr.n);
	BOOST_CHECK_EQUAL(0, cpu.sr.z);
	BOOST_CHECK_EQUAL(1, cpu.sr.v);
	BOOST_CHECK_EQUAL(0, cpu.sr.c);
}

BOOST_AUTO_TEST_SUITE_END()