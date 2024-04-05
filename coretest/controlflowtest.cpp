#include <boost/test/unit_test.hpp>
#include "../core/cpu.h"
#include "../core/memory.h"
#include "coretest.h"

using namespace mc68000;

BOOST_AUTO_TEST_SUITE(cpuSuite_controlFlow)

BOOST_AUTO_TEST_CASE(controlFlow_bsr)
{
	unsigned char code[] =
	{
		                         // start:
		0x4f, 0xf8, 0x10, 0x64,  // lea start + 100, a7
		0x61, 0x00, 0x00, 0x0e,  // bsr sub1
		0x76, 0x03,              // moveq #3, d3
		0x4e, 0x40,              // trap #0
		0xff, 0xff, 0xff, 0xff,  //
		                         // sub2:
		0x74, 0x02,	             // moveq #2, d2
		0x4e, 0x75,              // rts
		                         // sub1:
		0x72, 0x01,              // moveq #1, d1
		0x61, 0xf8,              // bsr sub2
		0x4e, 0x75,              // rts
	};

	// Arrange
	memory memory(256, 0x1000, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0x1000);

	BOOST_CHECK_EQUAL(0x1, cpu.d1);
	BOOST_CHECK_EQUAL(0x2, cpu.d2);
	BOOST_CHECK_EQUAL(0x3, cpu.d3);
}

BOOST_AUTO_TEST_CASE(controlFlow_chk_ok)
{
	unsigned char code[] =
	{
		// start:
		0x30, 0x3c, 0x12, 0x34,  // move    #$1234, d0
		0x32, 0x3c, 0x20, 0x00,  // move    #$2000, d1
		0x41, 0x81,              // chk     d1, d0
		0x34, 0x3c, 0x00, 0x01,  // move    #1, d2
		0x4e, 0x40,              // trap #0
		0xff, 0xff, 0xff, 0xff,  //
	};

	// Arrange
	memory memory(256, 0x1000, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0x1000);

	BOOST_CHECK_EQUAL(0x1, cpu.d2);
}

BOOST_AUTO_TEST_CASE(controlFlow_chk_below)
{
	unsigned char code[] =
	{
		// start:
		0x30, 0x3c, 0xff, 0xf6,  // move    #-10, d0
		0x32, 0x3c, 0x20, 0x00,  // move    #$2000, d1
		0x41, 0x81,              // chk     d1, d0
		0x34, 0x3c, 0x00, 0x01,  // move    #1, d2
		0x4e, 0x40,              // trap #0
		0xff, 0xff, 0xff, 0xff,  //
	};

	// Arrange
	memory memory(256, 0x1000, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0x1000);

	BOOST_CHECK_EQUAL(0x0, cpu.d2);
	BOOST_CHECK_EQUAL(1, cpu.sr.n);
}

BOOST_AUTO_TEST_CASE(controlFlow_chk_above)
{
	unsigned char code[] =
	{
		// start:
		0x30, 0x3c, 0x21, 0x00,  // move    #$2100, d0
		0x32, 0x3c, 0x20, 0x00,  // move    #$2000, d1
		0x41, 0x81,              // chk     d1, d0
		0x34, 0x3c, 0x00, 0x01,  // move    #1, d2
		0x4e, 0x40,              // trap #0
		0xff, 0xff, 0xff, 0xff,  //
	};

	// Arrange
	memory memory(256, 0x1000, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0x1000);

	BOOST_CHECK_EQUAL(0x0, cpu.d2);
	BOOST_CHECK_EQUAL(0, cpu.sr.n);
}



BOOST_AUTO_TEST_SUITE_END()