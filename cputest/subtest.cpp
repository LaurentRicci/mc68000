#include <boost/test/unit_test.hpp>
#include "../core/cpu.h"
#include "../core/memory.h"
#include "verifyexecution.h"

using namespace mc68000;

BOOST_AUTO_TEST_SUITE(cpuSuite_sub)
// ===================================================
// SUB tests
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
	Memory memory(256, 0x1000, code, sizeof(code));
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
	Memory memory(256, 0x1000, code, sizeof(code));
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
	Memory memory(256, 0x1000, code, sizeof(code));
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
	Memory memory(256, 0x1000, code, sizeof(code));
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
	Memory memory(256, 0x1000, code, sizeof(code));
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
	Memory memory(256, 0x1000, code, sizeof(code));
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

BOOST_AUTO_TEST_CASE(sub)
{
	unsigned char code[] = {

	0x70, 0x9c, //moveq # - 100, d0
	0x72, 0x64, //moveq #100, d1
	0x44, 0xfc, 0x00, 0x00, //move #0, ccr
	0x92, 0x00, //sub.b d0,d1
	0x40, 0xc2, //move sr, d2; 11011 - 201b

	0x70, 0x64, //moveq #100, d0
	0x72, 0x9c, //moveq # - 100, d1
	0x44, 0xfc, 0x00, 0x00, //move #0, ccr
	0x92, 0x00, //sub.b d0,d1
	0x40, 0xc3, //move sr, d3; 00010 - 2002

	0x70, 0x9c, //moveq # - 100, d0
	0x72, 0x64, //moveq #100, d1
	0x44, 0xfc, 0x00, 0x00, //move #0, ccr
	0x92, 0x40, //sub.w d0,d1
	0x40, 0xc4, //move sr, d4; 10001 - 2011

	0x70, 0x64, //moveq #100, d0
	0x72, 0x9c, //moveq # - 100, d1
	0x44, 0xfc, 0x00, 0x00, //move0, ccr
	0x92, 0x40, //sub.w d0,d1
	0x40, 0xc5, //move sr, d5; 01000 - 2008

	0x70, 0x9c, //moveq # - 100, d0
	0x72, 0x64, //moveq #100, d1
	0x44, 0xfc, 0x00, 0x00, //move #0, ccr
	0x92, 0x80, //sub.l d0,d1
	0x40, 0xc6, //move sr, d6; 10001 - 2011

	0x70, 0x64, //moveq #100, d0
	0x72, 0x9c, //moveq # - 100, d1
	0x44, 0xfc, 0x00, 0x00, //move0, ccr
	0x92, 0x80, //sub.l d0,d1
	0x40, 0xc7, //move sr, d7; 01000 - 2008

	0x4e, 0x40, //trap #0
	};

	verifyExecution(code, sizeof(code), [](const Cpu& cpu)
		{
			BOOST_CHECK_EQUAL(0b11011, cpu.d2);
			BOOST_CHECK_EQUAL(0b00010, cpu.d3);
			BOOST_CHECK_EQUAL(0b10001, cpu.d4);
			BOOST_CHECK_EQUAL(0b01000, cpu.d5);
			BOOST_CHECK_EQUAL(0b10001, cpu.d6);
			BOOST_CHECK_EQUAL(0b01000, cpu.d7);
		});
}

BOOST_AUTO_TEST_CASE(nbcd_17)
{
	unsigned char code[] = {
	0x70, 0x17,            // moveq #$17,d0
	0x48, 0x00,            // nbcd  d0
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
	BOOST_CHECK_EQUAL(0x83, cpu.d0);
	BOOST_CHECK_EQUAL(1, cpu.sr.x);
	BOOST_CHECK_EQUAL(0, cpu.sr.z);
	BOOST_CHECK_EQUAL(1, cpu.sr.c);
}

BOOST_AUTO_TEST_CASE(nbcd_83)
{
	unsigned char code[] = {
	0x70, 0x83,            // moveq #$83,d0
	0x48, 0x00,            // nbcd  d0
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
	BOOST_CHECK_EQUAL(0xffffff17, cpu.d0);
	BOOST_CHECK_EQUAL(1, cpu.sr.x);
	BOOST_CHECK_EQUAL(0, cpu.sr.z);
	BOOST_CHECK_EQUAL(1, cpu.sr.c);
}

BOOST_AUTO_TEST_CASE(nbcd_0)
{
	unsigned char code[] = {
	0x70, 0x00,            // moveq #$0,d0
	0x48, 0x00,            // nbcd  d0
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
	BOOST_CHECK_EQUAL(0x0, cpu.d0);
	BOOST_CHECK_EQUAL(0, cpu.sr.x);
	BOOST_CHECK_EQUAL(1, cpu.sr.z);
	BOOST_CHECK_EQUAL(0, cpu.sr.c);
}

BOOST_AUTO_TEST_CASE(nbcd_17x)
{
	unsigned char code[] = {
	0x44, 0xfc, 0x00, 0x10, // move  #$10, ccr
	0x70, 0x17,             // moveq #$17,d0
	0x48, 0x00,             // nbcd  d0
	0x4e, 0x40,             // trap #0
	0xff, 0xff, 0xff, 0xff  // 
	};

	// Arrange
	Memory memory(256, 0x1000, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0x1000);

	// Assert
	BOOST_CHECK_EQUAL(0x82, cpu.d0);
	BOOST_CHECK_EQUAL(1, cpu.sr.x);
	BOOST_CHECK_EQUAL(0, cpu.sr.z);
	BOOST_CHECK_EQUAL(1, cpu.sr.c);
}

BOOST_AUTO_TEST_CASE(nbcd_83x)
{
	unsigned char code[] = {
	0x44, 0xfc, 0x00, 0x10, // move  #$10, ccr
	0x70, 0x83,             // moveq #$83,d0
	0x48, 0x00,             // nbcd  d0
	0x4e, 0x40,             // trap #0
	0xff, 0xff, 0xff, 0xff  // 
	};

	// Arrange
	Memory memory(256, 0x1000, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0x1000);

	// Assert
	BOOST_CHECK_EQUAL(0xffffff16, cpu.d0);
	BOOST_CHECK_EQUAL(1, cpu.sr.x);
	BOOST_CHECK_EQUAL(0, cpu.sr.z);
	BOOST_CHECK_EQUAL(1, cpu.sr.c);
}

BOOST_AUTO_TEST_CASE(nbcd_0x)
{
	unsigned char code[] = {
	0x44, 0xfc, 0x00, 0x10, // move  #$10, ccr
	0x70, 0x00,             // moveq #$0,d0
	0x48, 0x00,             // nbcd  d0
	0x4e, 0x40,             // trap #0
	0xff, 0xff, 0xff, 0xff  // 
	};

	// Arrange
	Memory memory(256, 0x1000, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0x1000);

	// Assert
	BOOST_CHECK_EQUAL(0x99, cpu.d0);
	BOOST_CHECK_EQUAL(1, cpu.sr.x);
	BOOST_CHECK_EQUAL(0, cpu.sr.z);
	BOOST_CHECK_EQUAL(1, cpu.sr.c);
}

BOOST_AUTO_TEST_CASE(neg)
{
	unsigned char code[] = {
	0x70, 0x2a,             // moveq #42, d0
	0x72, 0x24,             // moveq #36, d1
	0x74, 0x4a,             // moveq #74, d2
	0x44, 0x00,             // neg.b d0
	0x44, 0x41,             // neg.w d1
	0x44, 0x82,             // neg.l d2
	0x4e, 0x40,             // trap #0
	0xff, 0xff, 0xff, 0xff  // 
	};

	// Arrange
	Memory memory(256, 0x1000, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0x1000);

	// Assert
	BOOST_CHECK_EQUAL(0xd6, cpu.d0);
	BOOST_CHECK_EQUAL(0xffdc, cpu.d1);
	BOOST_CHECK_EQUAL(0xffffffb6, cpu.d2);
	BOOST_CHECK_EQUAL(1, cpu.sr.x);
	BOOST_CHECK_EQUAL(1, cpu.sr.n);
	BOOST_CHECK_EQUAL(0, cpu.sr.z);
	BOOST_CHECK_EQUAL(0, cpu.sr.v);
	BOOST_CHECK_EQUAL(1, cpu.sr.c);
}

BOOST_AUTO_TEST_CASE(neg_neg)
{
	unsigned char code[] = {
	0x70, 0xd6,             // moveq #-42, d0
	0x72, 0xdc,             // moveq #-36, d1
	0x74, 0xb6,             // moveq #-74, d2
	0x44, 0x00,             // neg.b d0
	0x44, 0x41,             // neg.w d1
	0x44, 0x82,             // neg.l d2
	0x4e, 0x40,             // trap #0
	0xff, 0xff, 0xff, 0xff  // 
	};

	// Arrange
	Memory memory(256, 0x1000, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0x1000);

	// Assert
	BOOST_CHECK_EQUAL(42, (cpu.d0 & 0xff));
	BOOST_CHECK_EQUAL(36, (cpu.d1 & 0xffff));
	BOOST_CHECK_EQUAL(74, cpu.d2);
	BOOST_CHECK_EQUAL(1, cpu.sr.x);
	BOOST_CHECK_EQUAL(0, cpu.sr.n);
	BOOST_CHECK_EQUAL(0, cpu.sr.z);
	BOOST_CHECK_EQUAL(0, cpu.sr.v);
	BOOST_CHECK_EQUAL(1, cpu.sr.c);
}

BOOST_AUTO_TEST_CASE(negx)
{
	unsigned char code[] = {
	0x70, 0x2a,             // moveq #42, d0
	0x72, 0x24,             // moveq #36, d1
	0x74, 0x4a,             // moveq #74, d2
	0x44, 0xfc, 0x00, 0x10, // move  #$10, ccr
	0x40, 0x00,             // negx.b d0
	0x40, 0x41,             // negx.w d1
	0x40, 0x82,             // negx.l d2
	0x4e, 0x40,             // trap #0
	0xff, 0xff, 0xff, 0xff  // 
	};

	// Arrange
	Memory memory(256, 0x1000, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0x1000);

	// Assert
	BOOST_CHECK_EQUAL(0xd5, cpu.d0);
	BOOST_CHECK_EQUAL(0xffdb, cpu.d1);
	BOOST_CHECK_EQUAL(0xffffffb5, cpu.d2);
	BOOST_CHECK_EQUAL(1, cpu.sr.x);
	BOOST_CHECK_EQUAL(1, cpu.sr.n);
	BOOST_CHECK_EQUAL(0, cpu.sr.z);
	BOOST_CHECK_EQUAL(0, cpu.sr.v);
	BOOST_CHECK_EQUAL(1, cpu.sr.c);
}

BOOST_AUTO_TEST_CASE(negx_neg)
{
	unsigned char code[] = {
	0x70, 0xd6,             // moveq #-42, d0
	0x72, 0xdc,             // moveq #-36, d1
	0x74, 0xb6,             // moveq #-74, d2
	0x44, 0xfc, 0x00, 0x10, // move  #$10, ccr
	0x40, 0x00,             // negx.b d0
	0x40, 0x41,             // negx.w d1
	0x40, 0x82,             // negx.l d2
	0x4e, 0x40,             // trap #0
	0xff, 0xff, 0xff, 0xff  // 
	};

	// Arrange
	Memory memory(256, 0x1000, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0x1000);

	// Assert
	BOOST_CHECK_EQUAL(41, (cpu.d0 & 0xff));
	BOOST_CHECK_EQUAL(35, (cpu.d1 & 0xffff));
	BOOST_CHECK_EQUAL(73, cpu.d2);
	BOOST_CHECK_EQUAL(1, cpu.sr.x);
	BOOST_CHECK_EQUAL(0, cpu.sr.n);
	BOOST_CHECK_EQUAL(0, cpu.sr.z);
	BOOST_CHECK_EQUAL(0, cpu.sr.v);
	BOOST_CHECK_EQUAL(1, cpu.sr.c);
}

BOOST_AUTO_TEST_CASE(negx_zero)
{
	unsigned char code[] = {
		0x70, 0x00,             //  moveq #0,d0
		0x44, 0xfc, 0x00, 0x00, //  move #$00,ccr
		0x40, 0x00,             //  negx.b d0
		0x40, 0xc1,             //  move sr, d1
		                        //  
		0x74, 0x00,             //  moveq #0,d2
		0x44, 0xfc, 0x00, 0x10, //  move #$10,ccr
		0x40, 0x02,             //  negx.b d2
		0x40, 0xc3,             //  move sr,d3
		0x4e, 0x40,             //  trap #0
		0xff, 0xff, 0xff, 0xff  //  
	};

	// Arrange
	Memory memory(256, 0x1000, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0x1000);

	// Assert
	BOOST_CHECK_EQUAL(0x00, cpu.d0);
	BOOST_CHECK_EQUAL(0x00, cpu.d1);
	BOOST_CHECK_EQUAL(0xff, cpu.d2);
	BOOST_CHECK_EQUAL(0x19, cpu.d3);
	BOOST_CHECK_EQUAL(1, cpu.sr.x);
	BOOST_CHECK_EQUAL(1, cpu.sr.n);
	BOOST_CHECK_EQUAL(0, cpu.sr.z);
	BOOST_CHECK_EQUAL(0, cpu.sr.v);
	BOOST_CHECK_EQUAL(1, cpu.sr.c);
}

void verifySbcdExecution(uint8_t op1, uint8_t op2, uint8_t ccr, uint8_t expected, uint8_t carry, uint8_t zero)
{
	unsigned char code[] = {
	0x70, op1,             // moveq.l #op1,d0
	0x72, op2,             // moveq.l #op2,d1
	0x44, 0xfc, 0x00, ccr, // move #ccr, ccr
	0x83, 0x00,            // sbcd d0,d1
	0x4e, 0x40,            // trap #0
	0xff, 0xff };

	// Arrange
	Memory memory(256, 0, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0);

	// Assert
	BOOST_CHECK_EQUAL(op1, (uint8_t)cpu.d0);
	BOOST_CHECK_EQUAL(expected, (uint8_t)cpu.d1);
	BOOST_CHECK_EQUAL(carry, cpu.sr.c);
	BOOST_CHECK_EQUAL(zero, cpu.sr.z);
}

BOOST_AUTO_TEST_CASE(sbcd_register_x0)
{
	verifySbcdExecution(0x31, 0x42, 0, 0x11, 0, 0);
	verifySbcdExecution(0x34, 0x48, 0, 0x14, 0, 0);
	verifySbcdExecution(0x62, 0x48, 0, 0x86, 1, 0);
}

BOOST_AUTO_TEST_CASE(sbcd_register_x1)
{
	verifySbcdExecution(0x31, 0x42, 0x10, 0x10, 0, 0);
	verifySbcdExecution(0x34, 0x48, 0x10, 0x13, 0, 0);
	verifySbcdExecution(0x62, 0x48, 0x10, 0x85, 1, 0);
}

void verifySbcdExecutionMemory(uint8_t op1, uint8_t op2, uint8_t ccr, uint8_t expected, uint8_t carry, uint8_t zero)
{
	unsigned char code[] = {
		0x43, 0xfa, 0x00,0x13,  // lea op1+1(pc), A1
		0x45, 0xfa, 0x00,0x10,  // lea op2+1(pc), A2
		0x44, 0xfc, 0x00, ccr,  // move #ccr, ccr
		0x85, 0x09,             // sbcd -(A1), -(A2)
		0x4e, 0x40,             // trap #0
		0xff, 0xff,
		0xff, 0xff,
		op1,
		op2
	};

	// Arrange
	Memory memory(256, 0, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0);

	// Assert
	BOOST_CHECK_EQUAL(0x14, cpu.a1);
	BOOST_CHECK_EQUAL(0x15, cpu.a2);
	BOOST_CHECK_EQUAL(expected, cpu.mem.get<uint8_t>(0x15));
	BOOST_CHECK_EQUAL(carry, cpu.sr.c);
	BOOST_CHECK_EQUAL(zero, cpu.sr.z);
}

BOOST_AUTO_TEST_CASE(sbcd_memory_x0)
{
	verifySbcdExecutionMemory(0x31, 0x42, 0, 0x11, 0, 0);
	verifySbcdExecutionMemory(0x34, 0x48, 0, 0x14, 0, 0);
	verifySbcdExecutionMemory(0x62, 0x48, 0, 0x86, 1, 0);
}

BOOST_AUTO_TEST_CASE(sbcd_memory_x1)
{
	verifySbcdExecutionMemory(0x31, 0x42, 0x10, 0x10, 0, 0);
	verifySbcdExecutionMemory(0x34, 0x48, 0x10, 0x13, 0, 0);
	verifySbcdExecutionMemory(0x62, 0x48, 0x10, 0x85, 1, 0);
}

BOOST_AUTO_TEST_CASE(sbcd_overflow)
{
	unsigned char code[] = {
	0x30, 0x3c, 0xff, 0x62,     // move #$ff62,d0
	0x32, 0x3c, 0xff, 0x48,     // move #$ff48,d1
	0x83, 0x00,                 // sbcd d0,d1
	0x4e, 0x40,                 // trap #0
	0xff, 0xff };

	// Arrange
	Memory memory(256, 0, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0);

	// Assert
	BOOST_CHECK_EQUAL(0xff62, cpu.d0);
	BOOST_CHECK_EQUAL(0xff86, cpu.d1);
	BOOST_CHECK_EQUAL(1, cpu.sr.c);
	BOOST_CHECK_EQUAL(1, cpu.sr.x);
	BOOST_CHECK_EQUAL(0, cpu.sr.z);
}

// ===================================================
// SUBA tests
// ===================================================
BOOST_AUTO_TEST_CASE(suba)
{
	unsigned char code[] = {
	0x30, 0x7c, 0x00, 0x64,                 // move.w #100,a0
	0x90, 0xfc, 0x00, 0x20,                 // suba.w #32,a0
	0x32, 0x7c, 0x00, 0xc8,                 // move.w #200,a1
	0x92, 0xfc, 0xff, 0xe0,                 // suba.w #-32,a1

	0x34, 0x7c, 0x01, 0x2c,                 // move.w #300,a2
	0x95, 0xfc, 0x00, 0x00, 0x00, 0x20,     // suba.l #32,a2
	0x36, 0x7c, 0x01, 0x90,                 // move.w #400,a3
	0x97, 0xfc, 0xff, 0xff, 0xff, 0xe0,     // suba.w #-32,a1

	0x4e, 0x40,                             // trap #0
	0xff, 0xff };

	// Arrange
	Memory memory(256, 0, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0);

	// Assert
	BOOST_CHECK_EQUAL(68, cpu.a0);
	BOOST_CHECK_EQUAL(232, cpu.a1);
	BOOST_CHECK_EQUAL(268, cpu.a2);
	BOOST_CHECK_EQUAL(432, cpu.a3);
}

// ===================================================
// SUBI tests
// ===================================================
BOOST_AUTO_TEST_CASE(subi_b)
{
	unsigned char code[] = {
	0x70, 0x64,                 // moveq #100,d0
	0x04, 0x00, 0x00, 0x20,     // subi.b #32,d0
	0x72, 0x32,                 // move.w #50,d1
	0x04, 0x01, 0x00, 0xe0,     // subi.b #-32,d1

	0x4e, 0x40,                 // trap #0
	0xff, 0xff };

	// Arrange
	Memory memory(256, 0, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0);

	// Assert
	BOOST_CHECK_EQUAL(68, cpu.d0);
	BOOST_CHECK_EQUAL(82, cpu.d1);
	BOOST_CHECK_EQUAL(1, cpu.sr.c);
	BOOST_CHECK_EQUAL(1, cpu.sr.x);
}

BOOST_AUTO_TEST_CASE(subi_w)
{
	unsigned char code[] = {
	0x30, 0x3c, 0x12, 0x34,     // move.w #$1234,d0
	0x04, 0x40, 0x02, 0x32,     // subi.w #$232,d0
	0x32, 0x3c, 0x45, 0x67,     // move.w #$4567,d1
	0x04, 0x41, 0xf0, 0x32,     // subi.w #$F032,d1

	0x4e, 0x40,                 // trap #0
	0xff, 0xff };

	// Arrange
	Memory memory(256, 0, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0);

	// Assert
	BOOST_CHECK_EQUAL(0x1002, cpu.d0);
	BOOST_CHECK_EQUAL(0x5535, cpu.d1);
	BOOST_CHECK_EQUAL(1, cpu.sr.c);
	BOOST_CHECK_EQUAL(1, cpu.sr.x);
}

BOOST_AUTO_TEST_CASE(subi_l)
{
	unsigned char code[] = {
	0x20, 0x3c, 0x12, 0x34, 0x56, 0x78,     // move.l #$12345678, d0
	0x04, 0x80, 0x10, 0x00, 0x06, 0x78,     // subi.l #$10000678, d0
	0x22, 0x3c, 0x87, 0x65, 0x43, 0x21,     // move.l #$87654321, d1
	0x04, 0x81, 0xf1, 0x23, 0x45, 0x67,     // subi.l #$F1234567, d1

	0x4e, 0x40,                             // trap #0
	0xff, 0xff };

	// Arrange
	Memory memory(256, 0, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0);

	// Assert
	BOOST_CHECK_EQUAL(0x02345000, cpu.d0);
	BOOST_CHECK_EQUAL(0x9641fdba, cpu.d1);
	BOOST_CHECK_EQUAL(1, cpu.sr.c);
	BOOST_CHECK_EQUAL(1, cpu.sr.x);
	BOOST_CHECK_EQUAL(1, cpu.sr.n);
}

// ===================================================
// SUBQ tests
// ===================================================
BOOST_AUTO_TEST_CASE(subq_dregister_b)
{
	unsigned char code[] = {
		0x70, 0x15,             // moveq #21,d0
		0x5d, 0x40,             // subq.b #6,d0
		0x4e, 0x40,             // trap #0
		0xff, 0xff };

	verifyExecution(code, sizeof(code), 0x1000, [](const Cpu& cpu)
	{
		BOOST_CHECK_EQUAL(15, cpu.d0);
		BOOST_CHECK_EQUAL(0, cpu.sr.c);
		BOOST_CHECK_EQUAL(0, cpu.sr.x);
		BOOST_CHECK_EQUAL(0, cpu.sr.n);
	});
}

BOOST_AUTO_TEST_CASE(subq_dregister_w)
{
	unsigned char code[] = {
		0x30, 0x3c, 0x11, 0xd7, // move   #4567, d0
		0x5f, 0x40,             // subq.b #7,d0
		0x4e, 0x40,             // trap #0
		0xff, 0xff };

	verifyExecution(code, sizeof(code), 0x1000, [](const Cpu& cpu)
	{
		BOOST_CHECK_EQUAL(4560, cpu.d0);
		BOOST_CHECK_EQUAL(0, cpu.sr.c);
		BOOST_CHECK_EQUAL(0, cpu.sr.x);
		BOOST_CHECK_EQUAL(0, cpu.sr.n);
	});
}

BOOST_AUTO_TEST_CASE(subq_dregister_l)
{
	unsigned char code[] = {
		0x70, 0xe8,  // moveq.l #-24, d0
		0x5f, 0x80,  // subq.b  #7,d0
		0x4e, 0x40,  // trap #0
		0xff, 0xff };

	verifyExecution(code, sizeof(code), 0x1000, [](const Cpu& cpu)
		{
			BOOST_CHECK_EQUAL(0xFFFFFFE1, cpu.d0);
			BOOST_CHECK_EQUAL(0, cpu.sr.c);
			BOOST_CHECK_EQUAL(0, cpu.sr.x);
			BOOST_CHECK_EQUAL(1, cpu.sr.n);
		});
}

BOOST_AUTO_TEST_CASE(subq_aregister)
{
	unsigned char code[] = {
		0x30, 0x7c, 0x00, 0x64,  // move.w #100, a0
		0x44, 0xfc, 0x00, 0xff,  // move #255, ccr
		0x5f, 0x88,              // subq.l  #7,a0
		0x4e, 0x40,              // trap #0
		0xff, 0xff };

	verifyExecution(code, sizeof(code), 0x1000, [](const Cpu& cpu)
		{
			BOOST_CHECK_EQUAL(93, cpu.a0);
			BOOST_CHECK_EQUAL(1, cpu.sr.c);
			BOOST_CHECK_EQUAL(1, cpu.sr.x);
			BOOST_CHECK_EQUAL(1, cpu.sr.n);
			BOOST_CHECK_EQUAL(1, cpu.sr.z);
			BOOST_CHECK_EQUAL(1, cpu.sr.v);
		});
}

// ===================================================
// SUBX tests
// ===================================================
BOOST_AUTO_TEST_CASE(subx_register_b)
{
	unsigned char code[] = {
		0x70, 0x15,             // moveq #21,d0
		0x72, 0x16,             // moveq #22,d1
		0x44, 0xfc, 0x00, 0x14, // move #20, ccr
		0x93, 0x00,             // subx.b d0,d1
		0x4e, 0x40,             // trap #0
		0xff, 0xff };

	verifyExecution(code, sizeof(code), 0x1000, [](const Cpu& cpu)
		{
			BOOST_CHECK_EQUAL(21, cpu.d0);
			BOOST_CHECK_EQUAL(0, cpu.d1);
			BOOST_CHECK_EQUAL(0, cpu.sr.x);
			BOOST_CHECK_EQUAL(0, cpu.sr.n);
			BOOST_CHECK_EQUAL(1, cpu.sr.z);
			BOOST_CHECK_EQUAL(0, cpu.sr.v);
			BOOST_CHECK_EQUAL(0, cpu.sr.c);
		});
}

BOOST_AUTO_TEST_CASE(subx_register_w)
{
	unsigned char code[] = {
		0x30, 0x3c, 0x82, 0x83,    // move.w #$8283, d0
		0x32, 0x3c, 0x67, 0x34,    // move.w #$6734, d1
		0x44, 0xfc, 0x00, 0x10,    // move #16, ccr
		0x93, 0x40,                // subx.w d0,d1
		0x4e, 0x40,                // trap #0
	};

	verifyExecution(code, sizeof(code), [](const Cpu& cpu)
		{
			BOOST_CHECK_EQUAL(0x8283, cpu.d0);
			BOOST_CHECK_EQUAL(0xe4b0, cpu.d1);
			BOOST_CHECK_EQUAL(1, cpu.sr.x);
			BOOST_CHECK_EQUAL(1, cpu.sr.n);
			BOOST_CHECK_EQUAL(0, cpu.sr.z);
			BOOST_CHECK_EQUAL(1, cpu.sr.v);
			BOOST_CHECK_EQUAL(1, cpu.sr.c);
		});
}

BOOST_AUTO_TEST_CASE(subx_register_l)
{
	unsigned char code[] = {
		0x20, 0x3c, 0x76, 0x54, 0x82, 0x83,    // move.l #$76548283, d0
		0x22, 0x3c, 0xba, 0xd0, 0x67, 0x34,    // move.l #$bad06734, d1
		0x44, 0xfc, 0x00, 0x10,                // move #16, ccr
		0x93, 0x80,                            // subx.l d0,d1
		0x4e, 0x40,                            // trap #0
	};

	verifyExecution(code, sizeof(code), [](const Cpu& cpu)
		{
			BOOST_CHECK_EQUAL(0x76548283, cpu.d0);
			BOOST_CHECK_EQUAL(0x447BE4B0, cpu.d1);
			BOOST_CHECK_EQUAL(0, cpu.sr.x);
			BOOST_CHECK_EQUAL(0, cpu.sr.n);
			BOOST_CHECK_EQUAL(0, cpu.sr.z);
			BOOST_CHECK_EQUAL(1, cpu.sr.v); 
			BOOST_CHECK_EQUAL(0, cpu.sr.c);
		});
}

BOOST_AUTO_TEST_CASE(subx_register)
{
	unsigned char code[] = {
		
	0x70, 0x9c, //moveq # - 100, d0
	0x72, 0x64, //moveq #100, d1
	0x44, 0xfc, 0x00, 0x00, //move #0, ccr
	0x93, 0x00, //subx.b d0,d1
	0x40, 0xc2, //move sr, d2; 11011 - 201b

	0x70, 0x64, //moveq #100, d0
	0x72, 0x9c, //moveq # - 100, d1
	0x44, 0xfc, 0x00, 0x00, //move #0, ccr
	0x93, 0x00, //subx.b d0,d1
	0x40, 0xc3, //move sr, d3; 00010 - 2002

	0x70, 0x9c, //moveq # - 100, d0
	0x72, 0x64, //moveq #100, d1
	0x44, 0xfc, 0x00, 0x00, //move #0, ccr
	0x93, 0x40, //subx.w d0,d1
	0x40, 0xc4, //move sr, d4; 10001 - 2011

	0x70, 0x64, //moveq #100, d0
	0x72, 0x9c, //moveq # - 100, d1
	0x44, 0xfc, 0x00, 0x00, //move0, ccr
	0x93, 0x40, //subx.w d0,d1
	0x40, 0xc5, //move sr, d5; 01000 - 2008
	
	0x70, 0x9c, //moveq # - 100, d0
	0x72, 0x64, //moveq #100, d1
	0x44, 0xfc, 0x00, 0x00, //move #0, ccr
	0x93, 0x80, //subx.l d0,d1
	0x40, 0xc6, //move sr, d6; 10001 - 2011

	0x70, 0x64, //moveq #100, d0
	0x72, 0x9c, //moveq # - 100, d1
	0x44, 0xfc, 0x00, 0x00, //move0, ccr
	0x93, 0x80, //subx.l d0,d1
	0x40, 0xc7, //move sr, d7; 01000 - 2008

	0x4e, 0x40, //trap #0
	};

	verifyExecution(code, sizeof(code), [](const Cpu& cpu)
		{
			BOOST_CHECK_EQUAL(0b11011, cpu.d2);
			BOOST_CHECK_EQUAL(0b00010, cpu.d3);
			BOOST_CHECK_EQUAL(0b10001, cpu.d4);
			BOOST_CHECK_EQUAL(0b01000, cpu.d5);
			BOOST_CHECK_EQUAL(0b10001, cpu.d6);
			BOOST_CHECK_EQUAL(0b01000, cpu.d7);
		});
}
BOOST_AUTO_TEST_SUITE_END()