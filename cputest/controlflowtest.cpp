#include <boost/test/unit_test.hpp>
#include "../core/cpu.h"
#include "../core/memory.h"
#include "verifyexecution.h"

using namespace mc68000;

BOOST_AUTO_TEST_SUITE(cpuSuite_controlFlow)

// ===================================================
// BCC tests
// ===================================================
void verifyBccExecution(uint8_t ccr, uint8_t bccOp)
{
	//uint8_t ccr = 24;
	//uint8_t bccOp = 64;

	unsigned char code[] = {
		0x44, 0xfc, 0x00, ccr,     //       move #24,ccr
		bccOp, 0x00, 0x00, 0x04,   //       bcc pass
		0x4e, 0x40,                //       trap #0
		0x70, 0x2a,                // pass: moveq #42,d0
		0x4e, 0x40,                //       trap #0
		0xff, 0xff };

	verifyExecution(code, sizeof(code), [](const Cpu& cpu)
		{
			BOOST_CHECK_EQUAL(42, cpu.d0);
		});
}

BOOST_AUTO_TEST_CASE(bcc)
{
	//                   XNZVC
	verifyBccExecution(0b11000, 0x64); // C=0
}

BOOST_AUTO_TEST_CASE(bcs)
{
	//                   XNZVC
	verifyBccExecution(0b11001, 0x65); // C=1
}

BOOST_AUTO_TEST_CASE(beq)
{
	//                   XNZVC
	verifyBccExecution(0b00100, 0x67); // Z=1
}

BOOST_AUTO_TEST_CASE(bne)
{
	//                   XNZVC
	verifyBccExecution(0b01000, 0x66); // Z=0
}

BOOST_AUTO_TEST_CASE(bge)
{
	//                   XNZVC
	verifyBccExecution(0b10101, 0x6c); // N=0 V=0
}

BOOST_AUTO_TEST_CASE(bgt)
{
	//                   XNZVC
	verifyBccExecution(0b11011, 0x6e); // N=1 Z=0 V=1
}

BOOST_AUTO_TEST_CASE(bhi)
{
	//                   XNZVC
	verifyBccExecution(0b11000, 0x62); // C=0 Z=0
}

BOOST_AUTO_TEST_CASE(ble)
{
	//                   XNZVC
	verifyBccExecution(0b11001, 0x6f); // N=1 Z=0 V=0
}

BOOST_AUTO_TEST_CASE(bls)
{
	//                   XNZVC
	verifyBccExecution(0b00001, 0x63); // Z=0 C=1
}

BOOST_AUTO_TEST_CASE(blt)
{
	//                   XNZVC
	verifyBccExecution(0b01011, 0x6d); // N=1 V=1
}

BOOST_AUTO_TEST_CASE(bmi)
{
	//                   XNZVC
	verifyBccExecution(0b01000, 0x6b); // N=1
}

BOOST_AUTO_TEST_CASE(bpl)
{
	//                   XNZVC
	verifyBccExecution(0b00111, 0x6a); // N=0
}

BOOST_AUTO_TEST_CASE(bvc)
{
	//                   XNZVC
	verifyBccExecution(0b00101, 0x68); // V=0
}

BOOST_AUTO_TEST_CASE(bvs)
{
	//                   XNZVC
	verifyBccExecution(0b00111, 0x69); // V=1
}

BOOST_AUTO_TEST_CASE(bra)
{
	verifyBccExecution(0, 0x60);
}
// ===================================================
// BSR tests
// ===================================================
BOOST_AUTO_TEST_CASE(bsr)
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

// ===================================================
// CHK tests
// ===================================================
BOOST_AUTO_TEST_CASE(chk_ok)
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

BOOST_AUTO_TEST_CASE(chk_below)
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

BOOST_AUTO_TEST_CASE(chk_above)
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

// ===================================================
// DBCC tests
// ===================================================
BOOST_AUTO_TEST_CASE(dbf)
{
	unsigned char code[] =
	{
		// start:
		0x76, 0x07,              // moveq    #7, d3
		                         // loop:
		0x52, 0x41,              //    addq #1,d1
		0x51, 0xcb, 0xff, 0xfc,  //	   dbra d3, loop
		0x4e, 0x40,              // trap #0
		0xff, 0xff, 0xff, 0xff,  //
	};

	// Arrange
	memory memory(256, 0x1000, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0x1000);

	BOOST_CHECK_EQUAL(0xffff, cpu.d3);
	BOOST_CHECK_EQUAL(8, cpu.d1);
}

BOOST_AUTO_TEST_CASE(dbt)
{
	unsigned char code[] =
	{
		// start:
		0x76, 0x07,              // moveq    #7, d3
		                         // loop:
		0x52, 0x41,              //    addq #1,d1
		0x50, 0xcb, 0xff, 0xfc,  //	   dbt d3, loop
		0x4e, 0x40,              // trap #0
		0xff, 0xff, 0xff, 0xff,  //
	};

	// Arrange
	memory memory(256, 0x1000, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0x1000);

	BOOST_CHECK_EQUAL(0x7, cpu.d3);
	BOOST_CHECK_EQUAL(1, cpu.d1);
}


void verifyDbccExecution(uint8_t ccr, uint8_t dbcc)
{
	//uint8_t CCR = 0x18;
	//uint8_t DBCCOP = 0x57;

	unsigned char code[] =
	{
		// start:
		0x76, 0x07,              // moveq    #7, d3
		0x74, 0x04,              // moveq    #4, d2
		                         // loop:
		0x52, 0x41,              //    addq #1,d1
		0x44, 0xfc, 0x00, ccr,   //    moveq #CCR,ccr
		dbcc, 0xcb, 0xff, 0xf8,  //	   dbeq d3, loop
		0x4e, 0x40,              // trap #0
		0xff, 0xff, 0xff, 0xff,  //
	};

	// Arrange
	memory memory(256, 0x1000, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0x1000);

	BOOST_CHECK_EQUAL(0xffff, cpu.d3);
	BOOST_CHECK_EQUAL(8, cpu.d1);
}

BOOST_AUTO_TEST_CASE(dbcc)
{
	//                    XNZVC
	verifyDbccExecution(0b11111, 0x54); // C=1
}

BOOST_AUTO_TEST_CASE(dbcs)
{
	//                    XNZVC
	verifyDbccExecution(0b11110, 0x55); // C=0
}

BOOST_AUTO_TEST_CASE(dbeq)
{
	//                    XNZVC
	verifyDbccExecution(0b11011, 0x57); // Z=0
}

BOOST_AUTO_TEST_CASE(dbne)
{
	//                    XNZVC
	verifyDbccExecution(0b11111, 0x56); // Z=1
}

BOOST_AUTO_TEST_CASE(dbge)
{
	//                    XNZVC
	verifyDbccExecution(0b11101, 0x5c); // N=1 V=0
}

BOOST_AUTO_TEST_CASE(dbgt)
{
	//                    XNZVC
	verifyDbccExecution(0b10111, 0x5e); // N=0 Z=1 V=1
}

BOOST_AUTO_TEST_CASE(dbhi)
{
	//                    XNZVC
	verifyDbccExecution(0b11001, 0x52); // C=1 Z=0
}

BOOST_AUTO_TEST_CASE(dble)
{
	//                    XNZVC
	verifyDbccExecution(0b10001, 0x5f); // Z=0 N=0 V=0
}

BOOST_AUTO_TEST_CASE(dbls)
{
	//                    XNZVC
	verifyDbccExecution(0b11010, 0x53); // C=0 Z=0
}

BOOST_AUTO_TEST_CASE(dblt)
{
	//                    XNZVC
	verifyDbccExecution(0b10101, 0x5d); // N=0 V=0
}

BOOST_AUTO_TEST_CASE(dbmi)
{
	//                    XNZVC
	verifyDbccExecution(0b10111, 0x5b); // N=0
}

BOOST_AUTO_TEST_CASE(dbpl)
{
	//                    XNZVC
	verifyDbccExecution(0b11001, 0x5a); // N=1
}

BOOST_AUTO_TEST_CASE(dbvc)
{
	//                    XNZVC
	verifyDbccExecution(0b11111, 0x58); // V=1
}

BOOST_AUTO_TEST_CASE(dbvs)
{
	//                    XNZVC
	verifyDbccExecution(0b10001, 0x59); // V=0
}

// ===================================================
// JMP tests
// ===================================================
BOOST_AUTO_TEST_CASE(jmp)
{
	unsigned char code[] = {
		0x70, 0x15,                           //   moveq #21, d0
		0x4e, 0xf9, 0x00, 0x00, 0x10, 0x0c,   //   jmp label
		0x70, 0x2a,                           //   moveq #42,d0
		0x4e, 0x40,                           //   trap #0
                                              // label:
		0x70, 0x40,                           //   moveq #64,d0
		0x4e, 0x40,                           //   trap #0
		0xff, 0xff };


	// Arrange
	memory memory(256, 0x1000, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0x1000);

	// Arrange
	BOOST_CHECK_EQUAL(64, cpu.d0);
}

// ===================================================
// JSR tests
// ===================================================
BOOST_AUTO_TEST_CASE(jsr)
{
	unsigned char code[] =
	{
		// start:
		0x4f, 0xf8, 0x10, 0x64,              // lea (start + 100), a7
		0x4e, 0xb9, 0x00, 0x00, 0x10, 0x16,  // bsr sub1
		0x76, 0x03,                          // moveq #3, d3
		0x4e, 0x40,                          // trap #0
		0xff, 0xff, 0xff, 0xff,              //
 		                                     // sub2:
		0x74, 0x02,	                         // moveq #2, d2
		0x4e, 0x75,                          // rts
		                                     // sub1:
		0x72, 0x01,                          // moveq #1, d1
		0x4e, 0xb8, 0x10, 0x12,              // bsr sub2
		0x4e, 0x75,                          // rts
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