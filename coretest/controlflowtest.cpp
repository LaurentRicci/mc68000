#include <boost/test/unit_test.hpp>
#include "../core/cpu.h"
#include "../core/memory.h"
#include "coretest.h"

using namespace mc68000;

BOOST_AUTO_TEST_SUITE(cpuSuite_control_flow)

BOOST_AUTO_TEST_CASE(control_flow_bsr)
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

BOOST_AUTO_TEST_SUITE_END()