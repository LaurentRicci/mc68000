#include <boost/test/unit_test.hpp>
#include "../core/cpu.h"
#include "../core/memory.h"

using namespace mc68000;

BOOST_AUTO_TEST_SUITE(cpuSuite_sub)
// ===================================================
// ADD tests
// ===================================================
BOOST_AUTO_TEST_CASE(sub_to_dregister_b)
{
	unsigned char code[] = {
		0x16, 0x3c, 0x00, 0x32,    //   move.b #$32, d3
		0x38, 0x7c, 0x10, 0x10,    //   move   #value, a4
		0x96, 0x14,                //   sub.b  (a4), d3
		0x4e, 0x40,                //   trap   #0
		0xff, 0xff, 0xff, 0xff,    // 
		0x29, 0xfe };              // value: dc.b $29

	// Arrange
	memory memory(256, 0x1000, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0x1000);

	// Assert
	BOOST_CHECK_EQUAL(0x09, cpu.d3);
	BOOST_CHECK_EQUAL(0x1010, cpu.a4);
	BOOST_CHECK_EQUAL(0, cpu.sr.c);
	BOOST_CHECK_EQUAL(0, cpu.sr.z);
	BOOST_CHECK_EQUAL(0, cpu.sr.n);
	BOOST_CHECK_EQUAL(0, cpu.sr.v);
	BOOST_CHECK_EQUAL(0, cpu.sr.x);
}

BOOST_AUTO_TEST_CASE(sub_from_dregister_b)
{
	unsigned char code[] = {
		0x16, 0x3c, 0x00, 0x32,    //   move.b #$32, d3
		0x38, 0x7c, 0x10, 0x12,    //   move   #value, a4
		0x97, 0x14,                //   sub.b  d3, (a4)
		0x14, 0x14,                //   move.b (a4), d2
		0x4e, 0x40,                //   trap   #0
		0xff, 0xff, 0xff, 0xff,    // 
		0x29, 0xfe };              // value: dc.b $29

	// Arrange
	memory memory(256, 0x1000, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0x1000);

	// Assert
	BOOST_CHECK_EQUAL(0x32, cpu.d3);
	BOOST_CHECK_EQUAL(0x1012, cpu.a4);
	BOOST_CHECK_EQUAL(0xf7, cpu.d2);
	BOOST_CHECK_EQUAL(0, cpu.sr.c);
	BOOST_CHECK_EQUAL(0, cpu.sr.z);
	BOOST_CHECK_EQUAL(1, cpu.sr.n);
	BOOST_CHECK_EQUAL(0, cpu.sr.v);
	BOOST_CHECK_EQUAL(1, cpu.sr.x);
}

BOOST_AUTO_TEST_CASE(sub_to_dregister_w)
{
	unsigned char code[] = {
		0x36, 0x3c, 0x90, 0x90,    //   move.w #$9090, d3
		0x38, 0x7c, 0x10, 0x10,    //   move   #value, a4
		0x96, 0x54,                //   sub.w  (a4), d3
		0x4e, 0x40,                //   trap   #0
		0xff, 0xff, 0xff, 0xff,    // 
		0x81, 0x81,                // value: dc.w $8181
		0xfe, 0xfe };

	// Arrange
	memory memory(256, 0x1000, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0x1000);

	// Assert
	BOOST_CHECK_EQUAL(0x0f0f, cpu.d3);
	BOOST_CHECK_EQUAL(0x1010, cpu.a4);
	BOOST_CHECK_EQUAL(0, cpu.sr.c);
	BOOST_CHECK_EQUAL(0, cpu.sr.z);
	BOOST_CHECK_EQUAL(0, cpu.sr.n);
	BOOST_CHECK_EQUAL(0, cpu.sr.v);
	BOOST_CHECK_EQUAL(0, cpu.sr.x);
}

BOOST_AUTO_TEST_CASE(sub_from_dregister_w)
{
	unsigned char code[] = {
		0x36, 0x3c, 0x90, 0x90,    //   move.w #$9090, d3
		0x38, 0x7c, 0x10, 0x12,    //   move   #value, a4
		0x97, 0x54,                //   sub.w  d3, (a4)
		0x34, 0x14,                //   move.w (a4), d2
		0x4e, 0x40,                //   trap   #0
		0xff, 0xff, 0xff, 0xff,    // 
		0x81, 0x81,                // value dc.w $8181
		0xfe, 0xfe };

	// Arrange
	memory memory(256, 0x1000, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0x1000);

	// Assert
	BOOST_CHECK_EQUAL(0x9090, cpu.d3);
	BOOST_CHECK_EQUAL(0x1012, cpu.a4);
	BOOST_CHECK_EQUAL(0xf0f1, cpu.d2);
	//	BOOST_CHECK_EQUAL(1, cpu.sr.c); the 2nd move is resetting the status register. 
	BOOST_CHECK_EQUAL(0, cpu.sr.z);
	BOOST_CHECK_EQUAL(1, cpu.sr.n);
	//	BOOST_CHECK_EQUAL(1, cpu.sr.v); the 2nd move is resetting the status register. 
	BOOST_CHECK_EQUAL(1, cpu.sr.x);
}

BOOST_AUTO_TEST_CASE(sub_to_dregister_l)
{
	unsigned char code[] = {
		0x26, 0x3c, 0x90, 0x90, 0x90, 0x90,    //   move.l #$90909090, d3
		0x38, 0x7c, 0x10, 0x12,                //   move   #value, a4
		0x96, 0x94,                            //   sub.l  (a4), d3
		0x4e, 0x40,                            //   trap   #0
		0xff, 0xff, 0xff, 0xff,                // 
		0x81, 0x81, 0x81, 0x81,                // value dc.l $81818181
		0xfe, 0xfe, 0xfe, 0xfe };

	// Arrange
	memory memory(256, 0x1000, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0x1000);

	// Assert
	BOOST_CHECK_EQUAL(0x0f0f0f0f, cpu.d3);
	BOOST_CHECK_EQUAL(0x1012, cpu.a4);
	BOOST_CHECK_EQUAL(0, cpu.sr.c);
	BOOST_CHECK_EQUAL(0, cpu.sr.z);
	BOOST_CHECK_EQUAL(0, cpu.sr.n);
	BOOST_CHECK_EQUAL(0, cpu.sr.v);
	BOOST_CHECK_EQUAL(0, cpu.sr.x);
}

BOOST_AUTO_TEST_CASE(sub_from_dregister_l)
{
	unsigned char code[] = {
		0x26, 0x3c, 0x90, 0x90, 0x90, 0x90,    //   move.l #$90909090, d3
		0x38, 0x7c, 0x10, 0x14,                //   move   #value, a4
		0x97, 0x94,                            //   sub.l  d3, (a4)
		0x24, 0x14,                            //   move.l (a4), d2
		0x4e, 0x40,                            //   trap   #0
		0xff, 0xff, 0xff, 0xff,                // 
		0x81, 0x81, 0x81, 0x81,                // value dc.l $81818181
		0xfe, 0xfe, 0xfe, 0xfe };

	// Arrange
	memory memory(256, 0x1000, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0x1000);

	// Assert
	BOOST_CHECK_EQUAL(0x90909090, cpu.d3);
	BOOST_CHECK_EQUAL(0x1014, cpu.a4);
	BOOST_CHECK_EQUAL(0xf0f0f0f1, cpu.d2);
	//	BOOST_CHECK_EQUAL(1, cpu.sr.c);
	BOOST_CHECK_EQUAL(0, cpu.sr.z);
	BOOST_CHECK_EQUAL(1, cpu.sr.n);
	//	BOOST_CHECK_EQUAL(1, cpu.sr.v);
	BOOST_CHECK_EQUAL(1, cpu.sr.x);
}



BOOST_AUTO_TEST_SUITE_END()