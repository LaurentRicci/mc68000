#include <boost/test/unit_test.hpp>
#include "../core/cpu.h"
#include "../core/memory.h"

using namespace mc68000;

BOOST_AUTO_TEST_SUITE(cpuSuite_or)

// ===================================================
// OR tests
// ===================================================
BOOST_AUTO_TEST_CASE(or_to_dregister_b)
{
	unsigned char code[] = {
		0x16, 0x3c, 0x00, 0x32,    //   move.b #$32, d3
		0x38, 0x7c, 0x10, 0x10,    //   move   #value, a4
		0x86, 0x14,                //   or.b  (a4), d3
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
	BOOST_CHECK_EQUAL(0x3b, cpu.d3);
	BOOST_CHECK_EQUAL(0x1010, cpu.a4);
	BOOST_CHECK_EQUAL(0, cpu.sr.c);
	BOOST_CHECK_EQUAL(0, cpu.sr.z);
	BOOST_CHECK_EQUAL(0, cpu.sr.n);
	BOOST_CHECK_EQUAL(0, cpu.sr.v);
	BOOST_CHECK_EQUAL(0, cpu.sr.x);
}

BOOST_AUTO_TEST_CASE(or_to_dregister_w)
{
	unsigned char code[] = {
		0x36, 0x3c, 0x8a, 0x8f, //   move.w #$8a8f,d3
		0x38, 0x7c, 0x10, 0x10, //   move   #data, a4
		0x86, 0x54,             //   or.w(a4), d3
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
	BOOST_CHECK_EQUAL(0x8B8f, cpu.d3);
	BOOST_CHECK_EQUAL(0x1010, cpu.a4);
	BOOST_CHECK_EQUAL(0, cpu.sr.c);
	BOOST_CHECK_EQUAL(0, cpu.sr.z);
	BOOST_CHECK_EQUAL(1, cpu.sr.n);
	BOOST_CHECK_EQUAL(0, cpu.sr.v);
	BOOST_CHECK_EQUAL(0, cpu.sr.x);
}

BOOST_AUTO_TEST_CASE(or_to_dregister_l)
{
	unsigned char code[] = {
		0x26, 0x3c, 0x95, 0x36, 0x8a, 0x8f, //   move.l #$95368a8f,d3
		0x38, 0x7c, 0x10, 0x12,             //   move   #data, a4
		0x86, 0x94,                         //   or.w(a4), d3
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
	BOOST_CHECK_EQUAL(0xff378b8f, cpu.d3);
	BOOST_CHECK_EQUAL(0x1012, cpu.a4);
	BOOST_CHECK_EQUAL(0, cpu.sr.c);
	BOOST_CHECK_EQUAL(0, cpu.sr.z);
	BOOST_CHECK_EQUAL(1, cpu.sr.n);
	BOOST_CHECK_EQUAL(0, cpu.sr.v);
	BOOST_CHECK_EQUAL(0, cpu.sr.x);
}

BOOST_AUTO_TEST_CASE(or_from_dregister_b)
{
	unsigned char code[] = {
		0x16, 0x3c, 0x00, 0x32,    //   move.b #$32, d3
		0x38, 0x7c, 0x10, 0x12,    //   move   #value, a4
		0x87, 0x14,                //   or.b  d3,(a4)
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
	BOOST_CHECK_EQUAL(0x3b, cpu.d2);
	BOOST_CHECK_EQUAL(0x1012, cpu.a4);
	BOOST_CHECK_EQUAL(0, cpu.sr.c);
	BOOST_CHECK_EQUAL(0, cpu.sr.z);
	BOOST_CHECK_EQUAL(0, cpu.sr.n);
	BOOST_CHECK_EQUAL(0, cpu.sr.v);
	BOOST_CHECK_EQUAL(0, cpu.sr.x);
}

BOOST_AUTO_TEST_CASE(or_from_dregister_w)
{
	unsigned char code[] = {
		0x36, 0x3c, 0x8a, 0x8f, //   move.w #$8a8f,d3
		0x38, 0x7c, 0x10, 0x12, //   move   #data, a4
		0x87, 0x54,             //   or.w  (a4), d3
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
	BOOST_CHECK_EQUAL(0x8B8f, cpu.d2);
	BOOST_CHECK_EQUAL(0x1012, cpu.a4);
	BOOST_CHECK_EQUAL(0, cpu.sr.c);
	BOOST_CHECK_EQUAL(0, cpu.sr.z);
	BOOST_CHECK_EQUAL(1, cpu.sr.n);
	BOOST_CHECK_EQUAL(0, cpu.sr.v);
	BOOST_CHECK_EQUAL(0, cpu.sr.x);
}

BOOST_AUTO_TEST_CASE(or_from_dregister_l)
{
	unsigned char code[] = {
		0x26, 0x3c, 0x95, 0x36, 0x8a, 0x8f, //   move.l #$95368a8f,d3
		0x38, 0x7c, 0x10, 0x14,             //   move   #data, a4
		0x87, 0x94,                         //   or.l  d3, (a4)
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
	BOOST_CHECK_EQUAL(0xff378b8f, cpu.d2);
	BOOST_CHECK_EQUAL(0x1014, cpu.a4);
	BOOST_CHECK_EQUAL(0, cpu.sr.c);
	BOOST_CHECK_EQUAL(0, cpu.sr.z);
	BOOST_CHECK_EQUAL(1, cpu.sr.n);
	BOOST_CHECK_EQUAL(0, cpu.sr.v);
	BOOST_CHECK_EQUAL(0, cpu.sr.x);
}

// ===================================================
// ORI tests
// ===================================================
BOOST_AUTO_TEST_CASE(ori_b)
{
	unsigned char code[] = {
		0x16, 0x3c, 0x00, 0x32, //   move.b #$32,d3
		0x00, 0x03, 0x00, 0x29, //   ori.b  #$29,d3
		0x4e, 0x40,             //   trap #0
		0xff, 0xff, 0xff, 0xff  // 
	};

	// Arrange
	memory memory(256, 0x1000, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0x1000);

	// Assert
	BOOST_CHECK_EQUAL(0x3b, cpu.d3);
}

BOOST_AUTO_TEST_CASE(ori_w)
{
	unsigned char code[] = {
		0x36, 0x3c, 0x8a, 0x8f, //   move.w #$8a8f,d3
		0x00, 0x43, 0x81, 0x81, //   ori.w #$8181,d3
		0x4e, 0x40,             //   trap #0
		0xff, 0xff, 0xff, 0xff  // 
	};

	// Arrange
	memory memory(256, 0x1000, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0x1000);

	// Assert
	BOOST_CHECK_EQUAL(0x8B8f, cpu.d3);
}

BOOST_AUTO_TEST_CASE(ori_l)
{
	unsigned char code[] = {
		0x26, 0x3c, 0x95, 0x36, 0x8a, 0x8f, //   move.l #$95368a8f,d3
		0x00, 0x83, 0x7f, 0x27, 0x81, 0x81, //   ori.l #$7f278181,d3
		0x4e, 0x40,                         //   trap #0
		0xff, 0xff, 0xff, 0xff              // 
	};

	// Arrange
	memory memory(256, 0x1000, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0x1000);

	// Assert
	BOOST_CHECK_EQUAL(0xff378b8f, cpu.d3);
}

// ===================================================
// ORI to CCR tests
// ===================================================
BOOST_AUTO_TEST_CASE(ori2ccr_1)
{
	unsigned char code[] = {
		0x44, 0xfc, 0x00, 0x1f, // move #31,ccr
		0x00, 0x3c, 0x00, 0x15, // ori #21,ccr
		0x4e, 0x40,             // trap #0
		0xff, 0xff, 0xff, 0xff  // 
	};

	// Arrange
	memory memory(256, 0x1000, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0x1000);

	// Assert
	BOOST_CHECK_EQUAL(1, cpu.sr.x);
	BOOST_CHECK_EQUAL(1, cpu.sr.n);
	BOOST_CHECK_EQUAL(1, cpu.sr.z);
	BOOST_CHECK_EQUAL(1, cpu.sr.v);
	BOOST_CHECK_EQUAL(1, cpu.sr.c);
}
BOOST_AUTO_TEST_CASE(ori2ccr_2)
{
	unsigned char code[] = {
		0x44, 0xfc, 0x00, 0x00, // move #0,ccr
		0x00, 0x3c, 0x00, 0x0a, // ori #10,ccr
		0x4e, 0x40,             // trap #0
		0xff, 0xff, 0xff, 0xff  // 
	};

	// Arrange
	memory memory(256, 0x1000, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0x1000);

	// Assert
	BOOST_CHECK_EQUAL(0, cpu.sr.x);
	BOOST_CHECK_EQUAL(1, cpu.sr.n);
	BOOST_CHECK_EQUAL(0, cpu.sr.z);
	BOOST_CHECK_EQUAL(1, cpu.sr.v);
	BOOST_CHECK_EQUAL(0, cpu.sr.c);
}

BOOST_AUTO_TEST_SUITE_END()