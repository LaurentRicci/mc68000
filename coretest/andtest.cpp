#include <boost/test/unit_test.hpp>
#include "../core/cpu.h"
#include "../core/memory.h"

using namespace mc68000;

BOOST_AUTO_TEST_SUITE(cpuSuite_and)

// ===================================================
// AND tests
// ===================================================
BOOST_AUTO_TEST_CASE(a_and_to_dregister_b)
{
	unsigned char code[] = {
		0x16, 0x3c, 0x00, 0x32,    //   move.b #$32, d3
		0x38, 0x7c, 0x10, 0x10,    //   move   #value, a4
		0xc6, 0x14,                //   and.b  (a4), d3
		0x4e, 0x40,                //   trap   #0
		0xff, 0xff,                //
		0xff, 0xff,                // 
		0x29, 0xfe };              // value dc.b $29

	// Arrange
	memory memory(256, 0x1000, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0x1000);

	// Assert
	BOOST_CHECK_EQUAL(0x20, cpu.d3);
	BOOST_CHECK_EQUAL(0x1010, cpu.a4);
	BOOST_CHECK_EQUAL(0, cpu.sr.c);
	BOOST_CHECK_EQUAL(0, cpu.sr.z);
	BOOST_CHECK_EQUAL(0, cpu.sr.n);
	BOOST_CHECK_EQUAL(0, cpu.sr.v);
	BOOST_CHECK_EQUAL(0, cpu.sr.x);
}

BOOST_AUTO_TEST_CASE(a_and_to_dregister_w)
{
	unsigned char code[] = {
		0x36, 0x3c, 0x8a, 0x8f, //   move.w #$8a8f,d3
		0x38, 0x7c, 0x10, 0x10, //   move   #data, a4
		0xc6, 0x54,             //   and.w(a4), d3
		0x4e, 0x40,             //   trap #0
		0xff, 0xff,             //
		0xff, 0xff,             // 
		                        // data :
		0x81, 0x81, 0xfe, 0xfe  //   dc.w $8181, $fefe
	};

	// Arrange
	memory memory(256, 0x1000, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0x1000);

	// Assert
	BOOST_CHECK_EQUAL(0x8081, cpu.d3);
	BOOST_CHECK_EQUAL(0x1010, cpu.a4);
	BOOST_CHECK_EQUAL(0, cpu.sr.c);
	BOOST_CHECK_EQUAL(0, cpu.sr.z);
	BOOST_CHECK_EQUAL(1, cpu.sr.n);
	BOOST_CHECK_EQUAL(0, cpu.sr.v);
	BOOST_CHECK_EQUAL(0, cpu.sr.x);
}

BOOST_AUTO_TEST_CASE(a_and_to_dregister_l)
{
	unsigned char code[] = {
		0x26, 0x3c, 0x95, 0x36, 0x8a, 0xbf, //   move.l #$95368a8f,d3
		0x38, 0x7c, 0x10, 0x12,             //   move   #data, a4
		0xc6, 0x94,                         //   and.w(a4), d3
		0x4e, 0x40,                         //   trap #0
		0xff, 0xff, 0xff, 0xff,             // 
		// data :
		0x7f, 0x27, 0x81, 0x81,             //   dc.l $7f278181, $fefefefe
		0xfe, 0xfe, 0xfe, 0xfe
	};

	// Arrange
	memory memory(256, 0x1000, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0x1000);

	// Assert
	BOOST_CHECK_EQUAL(0x15268081, cpu.d3);
	BOOST_CHECK_EQUAL(0x1012, cpu.a4);
	BOOST_CHECK_EQUAL(0, cpu.sr.c);
	BOOST_CHECK_EQUAL(0, cpu.sr.z);
	BOOST_CHECK_EQUAL(0, cpu.sr.n);
	BOOST_CHECK_EQUAL(0, cpu.sr.v);
	BOOST_CHECK_EQUAL(0, cpu.sr.x);
}

BOOST_AUTO_TEST_CASE(a_and_from_dregister_b)
{
	unsigned char code[] = {
		0x16, 0x3c, 0x00, 0x32,    //   move.b #$32, d3
		0x38, 0x7c, 0x10, 0x12,    //   move   #value, a4
		0xc7, 0x14,                //   and.b  d3,(a4)
		0x14, 0x14,                //   move.b (a4), d2
		0x4e, 0x40,                //   trap   #0
		0xff, 0xff,                //
		0xff, 0xff,                // 
		0x29, 0xfe };              // value dc.b $29

	// Arrange
	memory memory(256, 0x1000, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0x1000);

	// Assert
	BOOST_CHECK_EQUAL(0x32, cpu.d3);
	BOOST_CHECK_EQUAL(0x20, cpu.d2);
	BOOST_CHECK_EQUAL(0x1012, cpu.a4);
	BOOST_CHECK_EQUAL(0, cpu.sr.c);
	BOOST_CHECK_EQUAL(0, cpu.sr.z);
	BOOST_CHECK_EQUAL(0, cpu.sr.n);
	BOOST_CHECK_EQUAL(0, cpu.sr.v);
	BOOST_CHECK_EQUAL(0, cpu.sr.x);
}

BOOST_AUTO_TEST_CASE(a_and_from_dregister_w)
{
	unsigned char code[] = {
		0x36, 0x3c, 0x8a, 0x8f, //   move.w #$8a8f,d3
		0x38, 0x7c, 0x10, 0x12, //   move   #data, a4
		0xc7, 0x54,             //   and.w  (a4), d3
		0x34, 0x14,             //   move.w (a4), d2
		0x4e, 0x40,             //   trap #0
		0xff, 0xff,             //
		0xff, 0xff,             // 
		                        // data :
		0x81, 0x81, 0xfe, 0xfe  //   dc.w $8181, $fefe
	};

	// Arrange
	memory memory(256, 0x1000, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0x1000);

	// Assert
	BOOST_CHECK_EQUAL(0x8a8f, cpu.d3);
	BOOST_CHECK_EQUAL(0x8081, cpu.d2);
	BOOST_CHECK_EQUAL(0x1012, cpu.a4);
	BOOST_CHECK_EQUAL(0, cpu.sr.c);
	BOOST_CHECK_EQUAL(0, cpu.sr.z);
	BOOST_CHECK_EQUAL(1, cpu.sr.n);
	BOOST_CHECK_EQUAL(0, cpu.sr.v);
	BOOST_CHECK_EQUAL(0, cpu.sr.x);
}

BOOST_AUTO_TEST_CASE(a_and_from_dregister_l)
{
	unsigned char code[] = {
		0x26, 0x3c, 0x95, 0x36, 0x8a, 0x8f, //   move.l #$95368a8f,d3
		0x38, 0x7c, 0x10, 0x14,             //   move   #data, a4
		0xc7, 0x94,                         //   and.l  d3, (a4)
		0x24, 0x14,                         //   move.l (a4), d2
		0x4e, 0x40,                         //   trap #0
		0xff, 0xff, 0xff, 0xff,             // 
		// data :
		0x7f, 0x27, 0x81, 0x81,             //   dc.l $7f278181, $fefefefe
		0xfe, 0xfe, 0xfe, 0xfe
	};

	// Arrange
	memory memory(256, 0x1000, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0x1000);

	// Assert
	BOOST_CHECK_EQUAL(0x95368a8f, cpu.d3);
	BOOST_CHECK_EQUAL(0x15268081, cpu.d2);
	BOOST_CHECK_EQUAL(0x1014, cpu.a4);
	BOOST_CHECK_EQUAL(0, cpu.sr.c);
	BOOST_CHECK_EQUAL(0, cpu.sr.z);
	BOOST_CHECK_EQUAL(0, cpu.sr.n);
	BOOST_CHECK_EQUAL(0, cpu.sr.v);
	BOOST_CHECK_EQUAL(0, cpu.sr.x);
}

BOOST_AUTO_TEST_SUITE_END()