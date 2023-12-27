#include <boost/test/unit_test.hpp>
#include "../core/cpu.h"
#include "../core/memory.h"

using namespace mc68000;

BOOST_AUTO_TEST_SUITE(cpuSuite_add)

void verifyAbcdExecution(uint8_t op1, uint8_t op2, uint8_t expected, uint8_t carry, uint8_t zero)
{
	unsigned char code[] = {
	0x70, op1,     // moveq.l #op1,d0
	0x72, op2,     // moveq.l #op2,d1
	0xc3, 0x00,    // abcd d0,d1
	0x4e, 0x40,    // trap #0
	0xff, 0xff };

	// Arrange
	memory memory(256, 0, code, sizeof(code));
	cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0);

	// Assert
	BOOST_CHECK_EQUAL(op1, cpu.d0);
	BOOST_CHECK_EQUAL(expected, cpu.d1);
	BOOST_CHECK_EQUAL(carry, cpu.sr.c);
	BOOST_CHECK_EQUAL(zero, cpu.sr.z);
}

void verifyAbcdExecutionMemory(uint8_t op1, uint8_t op2, uint8_t expected, uint8_t carry, uint8_t zero)
{
	unsigned char code[] = {
		0x32, 0x7c, 0x00,0x11,  // move op1+1, A1
		0x34, 0x7c, 0x00,0x12,  // move op2+1, A2
		0xc5, 0x09,             // abcd -(A1), -(A2)
		0x4e, 0x40,             // trap #0
		0xff, 0xff,
		0xff, 0xff,
		op1,
		op2
	};

	// Arrange
	memory memory(256, 0, code, sizeof(code));
	cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0);

	// Assert
	BOOST_CHECK_EQUAL(0x10, cpu.a1);
	BOOST_CHECK_EQUAL(0x11, cpu.a2);
	BOOST_CHECK_EQUAL(expected, cpu.mem.get<uint8_t>(0x11));
	BOOST_CHECK_EQUAL(carry, cpu.sr.c);
	BOOST_CHECK_EQUAL(zero, cpu.sr.z);
}

BOOST_AUTO_TEST_CASE(a_abcd_register)
{
	verifyAbcdExecution(0x42, 0x31, 0x73, 0, 0);
	verifyAbcdExecution(0x48, 0x34, 0x82, 0, 0);
	verifyAbcdExecution(0x48, 0x62, 0x10, 1, 0);
}

BOOST_AUTO_TEST_CASE(a_abcd_overflow)
{
	unsigned char code[] = {
	0x30, 0x3c, 0x00, 0x48,     // move #$48,d0
	0x32, 0x3c, 0xff, 0x62,     // move #$ff62,d1
	0xc3, 0x00,                 // abcd d0,d1
	0x4e, 0x40,                 // trap #0
	0xff, 0xff };

	// Arrange
	memory memory(256, 0, code, sizeof(code));
	cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0);

	// Assert
	BOOST_CHECK_EQUAL(0x48, cpu.d0);
	BOOST_CHECK_EQUAL(0xff10, cpu.d1);
	BOOST_CHECK_EQUAL(1, cpu.sr.c);
	BOOST_CHECK_EQUAL(0, cpu.sr.z);
}

BOOST_AUTO_TEST_CASE(a_abcd_memory)
{
	verifyAbcdExecutionMemory(0x42, 0x31, 0x73, 0, 0);
	verifyAbcdExecutionMemory(0x48, 0x34, 0x82, 0, 0);
	verifyAbcdExecutionMemory(0x48, 0x62, 0x10, 1, 0);
}
// ===================================================
// ADD tests
// ===================================================
BOOST_AUTO_TEST_CASE(a_add_to_dregister_b)
{
	unsigned char code[] = {
		0x16, 0x3c, 0x00, 0x32,    //   move.b #$32, d3
		0x38, 0x7c, 0x10, 0x0e,    //   move   #value, a4
		0xd6, 0x14,                //   add.b  (a4), d3
		0x4e, 0x40,                //   trap   #0
		0xff, 0xff,                // 
		0x29, 0xfe };              // value dc.b $29

	// Arrange
	memory memory(256, 0x1000, code, sizeof(code));
	cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0x1000);

	// Assert
	BOOST_CHECK_EQUAL(0x5b, cpu.d3);
	BOOST_CHECK_EQUAL(0x100e, cpu.a4);
	BOOST_CHECK_EQUAL(0, cpu.sr.c);
	BOOST_CHECK_EQUAL(0, cpu.sr.z);
	BOOST_CHECK_EQUAL(0, cpu.sr.n);
	BOOST_CHECK_EQUAL(0, cpu.sr.v);
	BOOST_CHECK_EQUAL(0, cpu.sr.x);
}

BOOST_AUTO_TEST_CASE(a_add_from_dregister_b)
{
	unsigned char code[] = {
		0x16, 0x3c, 0x00, 0x32,    //   move.b #$32, d3
		0x38, 0x7c, 0x10, 0x10,    //   move   #value, a4
		0xd7, 0x14,                //   add.b  d3, (a4)
		0x14, 0x14,                 //   move.b (a4), d2
		0x4e, 0x40,                //   trap   #0
		0xff, 0xff,                // 
		0x29, 0xfe };              // value dc.b $29

	// Arrange
	memory memory(256, 0x1000, code, sizeof(code));
	cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0x1000);

	// Assert
	BOOST_CHECK_EQUAL(0x32, cpu.d3);
	BOOST_CHECK_EQUAL(0x1010, cpu.a4);
	BOOST_CHECK_EQUAL(0x5b, cpu.d2);
	BOOST_CHECK_EQUAL(0, cpu.sr.c);
	BOOST_CHECK_EQUAL(0, cpu.sr.z);
	BOOST_CHECK_EQUAL(0, cpu.sr.n);
	BOOST_CHECK_EQUAL(0, cpu.sr.v);
	BOOST_CHECK_EQUAL(0, cpu.sr.x);
}

BOOST_AUTO_TEST_CASE(a_add_to_dregister_w)
{
	unsigned char code[] = {
		0x36, 0x3c, 0x90, 0x90,    //   move.w #$9090, d3
		0x38, 0x7c, 0x10, 0x0e,    //   move   #value, a4
		0xd6, 0x54,                //   add.w  (a4), d3
		0x4e, 0x40,                //   trap   #0
		0xff, 0xff,                // 
		0x81, 0x81,                // value dc.w $8181
		0xfe, 0xfe };

	// Arrange
	memory memory(256, 0x1000, code, sizeof(code));
	cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0x1000);

	// Assert
	BOOST_CHECK_EQUAL(0x1211, cpu.d3);
	BOOST_CHECK_EQUAL(0x100e, cpu.a4);
	BOOST_CHECK_EQUAL(1, cpu.sr.c);
	BOOST_CHECK_EQUAL(0, cpu.sr.z);
	BOOST_CHECK_EQUAL(0, cpu.sr.n);
	BOOST_CHECK_EQUAL(1, cpu.sr.v);
	BOOST_CHECK_EQUAL(1, cpu.sr.x);
}

BOOST_AUTO_TEST_CASE(a_add_from_dregister_w)
{
	unsigned char code[] = {
		0x36, 0x3c, 0x90, 0x90,    //   move.w #$9090, d3
		0x38, 0x7c, 0x10, 0x10,    //   move   #value, a4
		0xd7, 0x54,                //   add.w  d3, (a4)
		0x34, 0x14,                //   move.w (a4), d2
		0x4e, 0x40,                //   trap   #0
		0xff, 0xff,                // 
		0x81, 0x81,                // value dc.w $8181
		0xfe, 0xfe };

	// Arrange
	memory memory(256, 0x1000, code, sizeof(code));
	cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0x1000);

	// Assert
	BOOST_CHECK_EQUAL(0x9090, cpu.d3);
	BOOST_CHECK_EQUAL(0x1010, cpu.a4);
	BOOST_CHECK_EQUAL(0x1211, cpu.d2);
//	BOOST_CHECK_EQUAL(1, cpu.sr.c); the 2nd move is resetting the status register. 
	BOOST_CHECK_EQUAL(0, cpu.sr.z);
	BOOST_CHECK_EQUAL(0, cpu.sr.n);
//	BOOST_CHECK_EQUAL(1, cpu.sr.v);
//	BOOST_CHECK_EQUAL(1, cpu.sr.x);
}

BOOST_AUTO_TEST_CASE(a_add_to_dregister_l)
{
	unsigned char code[] = {
		0x26, 0x3c, 0x90, 0x90, 0x90, 0x90,    //   move.l #$90909090, d3
		0x38, 0x7c, 0x10, 0x10,                //   move   #value, a4
		0xd6, 0x94,                            //   add.l  (a4), d3
		0x4e, 0x40,                            //   trap   #0
		0xff, 0xff,                            // 
		0x81, 0x81, 0x81, 0x81,                // value dc.l $81818181
		0xfe, 0xfe, 0xfe, 0xfe };

	// Arrange
	memory memory(256, 0x1000, code, sizeof(code));
	cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0x1000);

	// Assert
	BOOST_CHECK_EQUAL(0x12121211, cpu.d3);
	BOOST_CHECK_EQUAL(0x1010, cpu.a4);
	BOOST_CHECK_EQUAL(1, cpu.sr.c);
	BOOST_CHECK_EQUAL(0, cpu.sr.z);
	BOOST_CHECK_EQUAL(0, cpu.sr.n);
	BOOST_CHECK_EQUAL(1, cpu.sr.v);
	BOOST_CHECK_EQUAL(1, cpu.sr.x);
}

BOOST_AUTO_TEST_CASE(a_add_from_dregister_l)
{
	unsigned char code[] = {
		0x26, 0x3c, 0x90, 0x90, 0x90, 0x90,    //   move.l #$90909090, d3
		0x38, 0x7c, 0x10, 0x12,                //   move   #value, a4
		0xd7, 0x94,                            //   add.l  d3, (a4)
		0x24, 0x14,                            //   move.l (a4), d2
		0x4e, 0x40,                            //   trap   #0
		0xff, 0xff,                            // 
		0x81, 0x81, 0x81, 0x81,                // value dc.l $81818181
		0xfe, 0xfe, 0xfe, 0xfe };

	// Arrange
	memory memory(256, 0x1000, code, sizeof(code));
	cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0x1000);

	// Assert
	BOOST_CHECK_EQUAL(0x90909090, cpu.d3);
	BOOST_CHECK_EQUAL(0x1012, cpu.a4);
	BOOST_CHECK_EQUAL(0x12121211, cpu.d2);
	//	BOOST_CHECK_EQUAL(1, cpu.sr.c);
	BOOST_CHECK_EQUAL(0, cpu.sr.z);
	BOOST_CHECK_EQUAL(0, cpu.sr.n);
	//	BOOST_CHECK_EQUAL(1, cpu.sr.v);
	//	BOOST_CHECK_EQUAL(1, cpu.sr.x);
}

// ===================================================
// ADDA tests
// ===================================================

BOOST_AUTO_TEST_CASE(a_adda_1)
{
	unsigned char code[] = {
	0x30, 0x7c, 0x00, 0x64,     // move #100,a0
	0xd0, 0xfc, 0x00, 0x20,     // add  #32,a0
	0x32, 0x7c, 0x00, 0xc8,     // move #200,a1
	0xd2, 0xfc, 0xff, 0xe0,     // add  #-32,a1
	0x4e, 0x40,                 // trap #0
	0xff, 0xff };

	// Arrange
	memory memory(256, 0, code, sizeof(code));
	cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0);

	// Assert
	BOOST_CHECK_EQUAL(132, cpu.a0);
	BOOST_CHECK_EQUAL(168, cpu.a1);
}

BOOST_AUTO_TEST_CASE(a_adda_2)
{
	unsigned char code[] = {
	0x30, 0x7c, 0x01, 0x00,             // move   #$100,a0
	0xd1, 0xfc, 0x12, 0x34, 0x12, 0x34, // add.l  #$12341234,a0
	0x32, 0x7c, 0x02, 0x00,		        // move   #$200,a1
	0x22, 0x3c, 0x12, 0x34, 0x00, 0x00, // move.l #$1234000,d1
	0xd3, 0xc1,                         // add.l  d1,a1
	0x4e, 0x40,                         // trap   #0
	0xff, 0xff };

	// Arrange
	memory memory(256, 0, code, sizeof(code));
	cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0);

	// Assert
	BOOST_CHECK_EQUAL(0x12341334, cpu.a0);
	BOOST_CHECK_EQUAL(0x12340200, cpu.a1);
}

// ===================================================
// ADDI tests
// ===================================================

void verifyAddiExecution_b(uint8_t destination, uint8_t source, uint8_t expected, uint8_t x, uint8_t n, uint8_t z, uint8_t v, uint8_t c)
{
	unsigned char code[] = {
		0x16, 0x3c, 0x00, destination,    // move.b #destination, d3
		0x06, 0x03, 0x00, source,         // addi.b #source, d3
		0x4e, 0x40,                       // trap #0
		0xff, 0xff };

	// Arrange
	memory memory(256, 0, code, sizeof(code));
	cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0);

	// Assert
	BOOST_CHECK_EQUAL(expected, cpu.d3);

	BOOST_CHECK_EQUAL(c, cpu.sr.c);
	BOOST_CHECK_EQUAL(z, cpu.sr.z);
	BOOST_CHECK_EQUAL(n, cpu.sr.n);
	BOOST_CHECK_EQUAL(v, cpu.sr.v);
	BOOST_CHECK_EQUAL(x, cpu.sr.x);

}

void verifyAddiExecution_w(uint16_t destination, uint16_t source, uint16_t expected, uint8_t x, uint8_t n, uint8_t z, uint8_t v, uint8_t c)
{
	unsigned char code[] = {
		0x36, 0x3c, destination >> 8, destination & 0xff,    // move.w #destination, d3
		0x06, 0x43, source >> 8 , source & 0xff,             // addi.w #source, d3
		0x4e, 0x40,                                          // trap #0
		0xff, 0xff };

	// Arrange
	memory memory(256, 0, code, sizeof(code));
	cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0);

	// Assert
	BOOST_CHECK_EQUAL(expected, cpu.d3);

	BOOST_CHECK_EQUAL(c, cpu.sr.c);
	BOOST_CHECK_EQUAL(z, cpu.sr.z);
	BOOST_CHECK_EQUAL(n, cpu.sr.n);
	BOOST_CHECK_EQUAL(v, cpu.sr.v);
	BOOST_CHECK_EQUAL(x, cpu.sr.x);

}

void verifyAddiExecution_l(uint32_t destination, uint32_t source, uint32_t expected, uint8_t x, uint8_t n, uint8_t z, uint8_t v, uint8_t c)
{
	unsigned char code[] = {
		0x26, 0x3c, (destination >> 24), (destination >> 16) & 0xff, (destination >> 8) & 0xff, destination & 0xff,    // move.l #destination, d3
		0x06, 0x83, (source >> 24), (source >> 16) & 0xff, (source >> 8) & 0xff, source & 0xff,                        // addi.l #source, d3
		0x4e, 0x40,                                                                                                    // trap #0
		0xff, 0xff };

	// Arrange
	memory memory(256, 0, code, sizeof(code));
	cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0);

	// Assert
	BOOST_CHECK_EQUAL(expected, cpu.d3);

	BOOST_CHECK_EQUAL(c, cpu.sr.c);
	BOOST_CHECK_EQUAL(z, cpu.sr.z);
	BOOST_CHECK_EQUAL(n, cpu.sr.n);
	BOOST_CHECK_EQUAL(v, cpu.sr.v);
	BOOST_CHECK_EQUAL(x, cpu.sr.x);

}


BOOST_AUTO_TEST_CASE(a_addi_b)
{
	verifyAddiExecution_b(  10,    5,   15, 0, 0, 0, 0, 0); // 10 + 5
	verifyAddiExecution_b(  10, 0xf6,    0, 1, 0, 1, 0, 1); // 10 + -10
	verifyAddiExecution_b(  10, 0xec, 0xf6, 0, 1, 0, 0, 0); // 10 + -20
	verifyAddiExecution_b(0x90, 0x81, 0x11, 1, 0, 0, 1, 1); // -112 + -127

}

BOOST_AUTO_TEST_CASE(a_addi_w)
{
	verifyAddiExecution_w(0x1010, 0x0505, 0x1515, 0, 0, 0, 0, 0); 
	verifyAddiExecution_w(0x1010, 0xf6f6, 0x0706, 1, 0, 0, 0, 1); 
	verifyAddiExecution_w(0x1010, 0xecec, 0xfcfc, 0, 1, 0, 0, 0); 
	verifyAddiExecution_w(0x9090, 0x8181, 0x1211, 1, 0, 0, 1, 1); 
}

BOOST_AUTO_TEST_CASE(a_addi_l)
{
	verifyAddiExecution_l(0x10101010, 0x05050505, 0x15151515, 0, 0, 0, 0, 0);
	verifyAddiExecution_l(0x10101010, 0xf6f6f6f6, 0x07070706, 1, 0, 0, 0, 1);
	verifyAddiExecution_l(0x10101010, 0xecececec, 0xfcfcfcfc, 0, 1, 0, 0, 0);
	verifyAddiExecution_l(0x90909090, 0x81818181, 0x12121211, 1, 0, 0, 1, 1);
}

BOOST_AUTO_TEST_SUITE_END()