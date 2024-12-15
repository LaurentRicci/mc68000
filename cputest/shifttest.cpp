#include <boost/test/unit_test.hpp>
#include "../core/cpu.h"
#include "../core/memory.h"

using namespace mc68000;

BOOST_AUTO_TEST_SUITE(cpuSuite_shift)
// ===================================================
// ASL tests
// ===================================================
BOOST_AUTO_TEST_CASE(asl_memory)
{
	unsigned char code[] = {
		0x36, 0x7c, 0x10, 0x12,    //   move #data, a3
		0xe0, 0xd3,                //   asr  (a3)
		0x36, 0x13,                //   move (a3), d3
		0xe1, 0xd3,                //   asl  (a3)
		0x34, 0x13,                //   move (a3), d2
		0x4e, 0x40,                //   trap   #0
		0xff, 0xff, 0xff, 0xff,    // 
		0x87, 0xf3 };              // data: dc.w $87f3

	// Arrange
	memory memory(256, 0x1000, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0x1000);

	// Assert
	BOOST_CHECK_EQUAL(0xC3F9, cpu.d3);
	BOOST_CHECK_EQUAL(0x87F2, cpu.d2);
	BOOST_CHECK_EQUAL(0x1012, cpu.a3);
	BOOST_CHECK_EQUAL(0, cpu.sr.c);
	BOOST_CHECK_EQUAL(0, cpu.sr.z);
	BOOST_CHECK_EQUAL(1, cpu.sr.n);
	BOOST_CHECK_EQUAL(0, cpu.sr.v);
	BOOST_CHECK_EQUAL(1, cpu.sr.x);
}

BOOST_AUTO_TEST_CASE(asl_b)
{
	unsigned char code[] = {
		0x20, 0x3c, 0x12, 0x34, 0x56, 0x78,    //   move #$12345678, d0
		0xe3, 0x00,                            //   asl.b #1, d0
		0x4e, 0x40,                            //   trap   #0
		0xff, 0xff, 0xff, 0xff,                // 
		};

	// Arrange
	memory memory(256, 0x1000, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0x1000);

	// Assert
	BOOST_CHECK_EQUAL(0x123456F0, cpu.d0);
	BOOST_CHECK_EQUAL(0, cpu.sr.x);
	BOOST_CHECK_EQUAL(1, cpu.sr.n);
	BOOST_CHECK_EQUAL(0, cpu.sr.z);
	BOOST_CHECK_EQUAL(1, cpu.sr.v);
	BOOST_CHECK_EQUAL(0, cpu.sr.c);
}

BOOST_AUTO_TEST_CASE(asl_w)
{
	unsigned char code[] = {
		0x20, 0x3c, 0x12, 0x34, 0x56, 0x78,    //   move #$12345678, d0
		0xe7, 0x40,                            //   asl.w #3, d0
		0x4e, 0x40,                            //   trap   #0
		0xff, 0xff, 0xff, 0xff,                // 
	};

	// Arrange
	memory memory(256, 0x1000, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0x1000);

	// Assert
	BOOST_CHECK_EQUAL(0x1234B3C0, cpu.d0);
	BOOST_CHECK_EQUAL(0, cpu.sr.x);
	BOOST_CHECK_EQUAL(1, cpu.sr.n);
	BOOST_CHECK_EQUAL(0, cpu.sr.z);
	BOOST_CHECK_EQUAL(1, cpu.sr.v);
	BOOST_CHECK_EQUAL(0, cpu.sr.c);
}

BOOST_AUTO_TEST_CASE(asl_l)
{
	unsigned char code[] = {
		0x20, 0x3c, 0x12, 0x34, 0x56, 0x78,    //   move #$12345678, d0
		0x12, 0x3c, 0x00, 0x07,                //   move.b #7, d1 
		0xe3, 0xa0,                            //   asl.l d1, d0
		0x4e, 0x40,                            //   trap   #0
		0xff, 0xff, 0xff, 0xff,                // 
	};

	// Arrange
	memory memory(256, 0x1000, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0x1000);

	// Assert
	BOOST_CHECK_EQUAL(0x1A2B3C00, cpu.d0);
	BOOST_CHECK_EQUAL(1, cpu.sr.x);
	BOOST_CHECK_EQUAL(0, cpu.sr.n);
	BOOST_CHECK_EQUAL(0, cpu.sr.z);
	BOOST_CHECK_EQUAL(1, cpu.sr.v);
	BOOST_CHECK_EQUAL(1, cpu.sr.c);
}

// ===================================================
// ASR tests
// ===================================================
BOOST_AUTO_TEST_CASE(asr_memory)
{
	unsigned char code[] = {
		0x36, 0x7c, 0x10, 0x12,    //   move #data, a3
		0xe1, 0xd3,                //   asl  (a3)
		0x36, 0x13,                //   move (a3), d3
		0xe0, 0xd3,                //   asr  (a3)
		0x34, 0x13,                //   move (a3), d2
		0x4e, 0x40,                //   trap   #0
		0xff, 0xff, 0xff, 0xff,    // 
		0x87, 0xf3 };              // data: dc.w $87f3

	// Arrange
	memory memory(256, 0x1000, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0x1000);

	// Assert
	BOOST_CHECK_EQUAL(0x0fe6, cpu.d3);
	BOOST_CHECK_EQUAL(0x07f3, cpu.d2);
	BOOST_CHECK_EQUAL(0x1012, cpu.a3);
	BOOST_CHECK_EQUAL(0, cpu.sr.c);
	BOOST_CHECK_EQUAL(0, cpu.sr.z);
	BOOST_CHECK_EQUAL(0, cpu.sr.n);
	BOOST_CHECK_EQUAL(0, cpu.sr.v);
	BOOST_CHECK_EQUAL(0, cpu.sr.x);
}

BOOST_AUTO_TEST_CASE(asr_b)
{
	unsigned char code[] = {
		0x20, 0x3c, 0x12, 0x34, 0x56, 0x78,    //   move #$12345678, d0
		0xe2, 0x00,                            //   asr.b #1, d0
		0x4e, 0x40,                            //   trap   #0
		0xff, 0xff, 0xff, 0xff,                // 
	};

	// Arrange
	memory memory(256, 0x1000, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0x1000);

	// Assert
	BOOST_CHECK_EQUAL(0x1234563C, cpu.d0);
	BOOST_CHECK_EQUAL(0, cpu.sr.x);
	BOOST_CHECK_EQUAL(0, cpu.sr.n);
	BOOST_CHECK_EQUAL(0, cpu.sr.z);
	BOOST_CHECK_EQUAL(0, cpu.sr.v);
	BOOST_CHECK_EQUAL(0, cpu.sr.c);
}

BOOST_AUTO_TEST_CASE(asr_w)
{
	unsigned char code[] = {
		0x20, 0x3c, 0x12, 0x34, 0x56, 0x78,    //   move #$12345678, d0
		0xe2, 0x40,                            //   asr.w #3, d0
		0x4e, 0x40,                            //   trap   #0
		0xff, 0xff, 0xff, 0xff,                // 
	};

	// Arrange
	memory memory(256, 0x1000, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0x1000);

	// Assert
	BOOST_CHECK_EQUAL(0x12342B3C, cpu.d0);
	BOOST_CHECK_EQUAL(0, cpu.sr.x);
	BOOST_CHECK_EQUAL(0, cpu.sr.n);
	BOOST_CHECK_EQUAL(0, cpu.sr.z);
	BOOST_CHECK_EQUAL(0, cpu.sr.v);
	BOOST_CHECK_EQUAL(0, cpu.sr.c);
}

BOOST_AUTO_TEST_CASE(asr_l)
{
	unsigned char code[] = {
		0x20, 0x3c, 0x12, 0x34, 0x56, 0x78,    //   move #$12345678, d0
		0x12, 0x3c, 0x00, 0x07,                //   move.b #7, d1 
		0xe2, 0xa0,                            //   asr.l d1, d0
		0x4e, 0x40,                            //   trap   #0
		0xff, 0xff, 0xff, 0xff,                // 
	};

	// Arrange
	memory memory(256, 0x1000, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0x1000);

	// Assert
	BOOST_CHECK_EQUAL(0x002468AC, cpu.d0);
	BOOST_CHECK_EQUAL(1, cpu.sr.x);
	BOOST_CHECK_EQUAL(0, cpu.sr.n);
	BOOST_CHECK_EQUAL(0, cpu.sr.z);
	BOOST_CHECK_EQUAL(0, cpu.sr.v);
	BOOST_CHECK_EQUAL(1, cpu.sr.c);
}

// ===================================================
// LSL tests
// ===================================================
BOOST_AUTO_TEST_CASE(lsl_memory)
{
	unsigned char code[] = {
		0x36, 0x7c, 0x10, 0x12,    //   move #data, a3
		0xe2, 0xd3,                //   lsr  (a3)
		0x36, 0x13,                //   move (a3), d3
		0xe3, 0xd3,                //   lsl  (a3)
		0x34, 0x13,                //   move (a3), d2
		0x4e, 0x40,                //   trap   #0
		0xff, 0xff, 0xff, 0xff,    // 
		0x87, 0xf3 };              // data: dc.w $87f3

	// Arrange
	memory memory(256, 0x1000, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0x1000);

	// Assert
	BOOST_CHECK_EQUAL(0x43F9, cpu.d3);
	BOOST_CHECK_EQUAL(0x87F2, cpu.d2);
	BOOST_CHECK_EQUAL(0x1012, cpu.a3);
	BOOST_CHECK_EQUAL(0, cpu.sr.c);
	BOOST_CHECK_EQUAL(0, cpu.sr.z);
	BOOST_CHECK_EQUAL(1, cpu.sr.n);
	BOOST_CHECK_EQUAL(0, cpu.sr.v);
	BOOST_CHECK_EQUAL(0, cpu.sr.x);
}

BOOST_AUTO_TEST_CASE(lsl_b)
{
	unsigned char code[] = {
		0x20, 0x3c, 0x12, 0x34, 0x56, 0x78,    //   move.l #$12345678, d0
		0xe3, 0x08,                            //   lsl.b #1, d0
		0x4e, 0x40,                            //   trap   #0
		0xff, 0xff, 0xff, 0xff,                // 
	};

	// Arrange
	memory memory(256, 0x1000, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0x1000);

	// Assert
	BOOST_CHECK_EQUAL(0x123456F0, cpu.d0);
	BOOST_CHECK_EQUAL(0, cpu.sr.x);
	BOOST_CHECK_EQUAL(1, cpu.sr.n);
	BOOST_CHECK_EQUAL(0, cpu.sr.z);
	BOOST_CHECK_EQUAL(1, cpu.sr.v);
	BOOST_CHECK_EQUAL(0, cpu.sr.c);
}

BOOST_AUTO_TEST_CASE(lsl_w)
{
	unsigned char code[] = {
		0x20, 0x3c, 0x12, 0x34, 0x56, 0x78,    //   move.l #$12345678, d0
		0xe7, 0x48,                            //   lsl.w #3, d0
		0x4e, 0x40,                            //   trap   #0
		0xff, 0xff, 0xff, 0xff,                // 
	};

	// Arrange
	memory memory(256, 0x1000, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0x1000);

	// Assert
	BOOST_CHECK_EQUAL(0x1234B3C0, cpu.d0);
	BOOST_CHECK_EQUAL(0, cpu.sr.x);
	BOOST_CHECK_EQUAL(1, cpu.sr.n);
	BOOST_CHECK_EQUAL(0, cpu.sr.z);
	BOOST_CHECK_EQUAL(1, cpu.sr.v);
	BOOST_CHECK_EQUAL(0, cpu.sr.c);
}

BOOST_AUTO_TEST_CASE(lsl_l)
{
	unsigned char code[] = {
		0x20, 0x3c, 0x12, 0x34, 0x56, 0x78,    //   move.l #$12345678, d0
		0x12, 0x3c, 0x00, 0x07,                //   move.b #7, d1 
		0xe3, 0xa8,                            //   lsl.l d1, d0
		0x4e, 0x40,                            //   trap   #0
		0xff, 0xff, 0xff, 0xff,                // 
	};

	// Arrange
	memory memory(256, 0x1000, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0x1000);

	// Assert
	BOOST_CHECK_EQUAL(0x1A2B3C00, cpu.d0);
	BOOST_CHECK_EQUAL(1, cpu.sr.x);
	BOOST_CHECK_EQUAL(0, cpu.sr.n);
	BOOST_CHECK_EQUAL(0, cpu.sr.z);
	BOOST_CHECK_EQUAL(1, cpu.sr.v);
	BOOST_CHECK_EQUAL(1, cpu.sr.c);
}

// ===================================================
// LSR tests
// ===================================================
BOOST_AUTO_TEST_CASE(lsr_memory)
{
	unsigned char code[] = {
		0x36, 0x7c, 0x10, 0x12,    //   move #data, a3
		0xe3, 0xd3,                //   lsl  (a3)
		0x36, 0x13,                //   move (a3), d3
		0xe2, 0xd3,                //   lsr  (a3)
		0x34, 0x13,                //   move (a3), d2
		0x4e, 0x40,                //   trap   #0
		0xff, 0xff, 0xff, 0xff,    // 
		0x87, 0xf3 };              // data: dc.w $87f3

	// Arrange
	memory memory(256, 0x1000, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0x1000);

	// Assert
	BOOST_CHECK_EQUAL(0x0fe6, cpu.d3);
	BOOST_CHECK_EQUAL(0x07f3, cpu.d2);
	BOOST_CHECK_EQUAL(0x1012, cpu.a3);
	BOOST_CHECK_EQUAL(0, cpu.sr.c);
	BOOST_CHECK_EQUAL(0, cpu.sr.z);
	BOOST_CHECK_EQUAL(0, cpu.sr.n);
	BOOST_CHECK_EQUAL(0, cpu.sr.v);
	BOOST_CHECK_EQUAL(0, cpu.sr.x);
}

BOOST_AUTO_TEST_CASE(lsr_b)
{
	unsigned char code[] = {
		0x20, 0x3c, 0x12, 0x34, 0x56, 0x9A,    //   move.l #$1234569A, d0
		0xe2, 0x08,                            //   lsr.b #1, d0
		0x4e, 0x40,                            //   trap   #0
		0xff, 0xff, 0xff, 0xff,                // 
	};

	// Arrange
	memory memory(256, 0x1000, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0x1000);

	// Assert
	BOOST_CHECK_EQUAL(0x1234564D, cpu.d0);
	BOOST_CHECK_EQUAL(0, cpu.sr.x);
	BOOST_CHECK_EQUAL(0, cpu.sr.n);
	BOOST_CHECK_EQUAL(0, cpu.sr.z);
	BOOST_CHECK_EQUAL(0, cpu.sr.v);
	BOOST_CHECK_EQUAL(0, cpu.sr.c);
}

BOOST_AUTO_TEST_CASE(lsr_w)
{
	unsigned char code[] = {
		0x20, 0x3c, 0x12, 0x34, 0x9A, 0xBC,    //   move.l #$12349ABC, d0
		0xe6, 0x48,                            //   lsr.w #3, d0
		0x4e, 0x40,                            //   trap   #0
		0xff, 0xff, 0xff, 0xff,                // 
	};

	// Arrange
	memory memory(256, 0x1000, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0x1000);

	// Assert
	BOOST_CHECK_EQUAL(0x12341357, cpu.d0);
	BOOST_CHECK_EQUAL(1, cpu.sr.x);
	BOOST_CHECK_EQUAL(0, cpu.sr.n);
	BOOST_CHECK_EQUAL(0, cpu.sr.z);
	BOOST_CHECK_EQUAL(0, cpu.sr.v);
	BOOST_CHECK_EQUAL(1, cpu.sr.c);
}

BOOST_AUTO_TEST_CASE(lsr_l)
{
	unsigned char code[] = {
		0x20, 0x3c, 0x9A, 0xBC, 0x12, 0x34,    //   move.l #$9ABC1234, d0
		0x12, 0x3c, 0x00, 0x07,                //   move.b #7, d1 
		0xe2, 0xa8,                            //   lsr.l d1, d0
		0x4e, 0x40,                            //   trap   #0
		0xff, 0xff, 0xff, 0xff,                // 
	};

	// Arrange
	memory memory(256, 0x1000, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0x1000);

	// Assert
	BOOST_CHECK_EQUAL(0x01357824, cpu.d0);
	BOOST_CHECK_EQUAL(0, cpu.sr.x);
	BOOST_CHECK_EQUAL(0, cpu.sr.n);
	BOOST_CHECK_EQUAL(0, cpu.sr.z);
	BOOST_CHECK_EQUAL(0, cpu.sr.v);
	BOOST_CHECK_EQUAL(0, cpu.sr.c);
}



BOOST_AUTO_TEST_SUITE_END()