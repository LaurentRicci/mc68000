#include <boost/test/unit_test.hpp>
#include "../core/cpu.h"
#include "../core/memory.h"

using namespace mc68000;

BOOST_AUTO_TEST_SUITE(cpuSuite_and)

// ===================================================
// AND tests
// ===================================================
BOOST_AUTO_TEST_CASE(and_to_dregister_b)
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

BOOST_AUTO_TEST_CASE(and_to_dregister_w)
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

BOOST_AUTO_TEST_CASE(and_to_dregister_l)
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

BOOST_AUTO_TEST_CASE(and_from_dregister_b)
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

BOOST_AUTO_TEST_CASE(and_from_dregister_w)
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

BOOST_AUTO_TEST_CASE(and_from_dregister_l)
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

// ===================================================
// ANDI tests
// ===================================================
BOOST_AUTO_TEST_CASE(andi_b)
{
	unsigned char code[] = {
		0x16, 0x3c, 0x00, 0x32, //   move.b #$32,d3
		0x02, 0x03, 0x00, 0x29, //   andi.b  #$29,d3
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
	BOOST_CHECK_EQUAL(0x20, cpu.d3);
}

BOOST_AUTO_TEST_CASE(andi_w)
{
	unsigned char code[] = {
		0x36, 0x3c, 0x8a, 0x8f, //   move.w #$8a8f,d3
		0x02, 0x43, 0x81, 0x81, //   andi.w #$8181,d3
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
	BOOST_CHECK_EQUAL(0x8081, cpu.d3);
}

BOOST_AUTO_TEST_CASE(andi_l)
{
	unsigned char code[] = {
		0x26, 0x3c, 0x95, 0x36, 0x8a, 0x8f, //   move.l #$95368a8f,d3
		0x02, 0x83, 0x7f, 0x27, 0x81, 0x81, //   andi.l #$7f278181,d3
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
	BOOST_CHECK_EQUAL(0x15268081, cpu.d3);
}

// ===================================================
// ANDI tests
// ===================================================
BOOST_AUTO_TEST_CASE(andi2ccr_1)
{
	unsigned char code[] = {
		0x44, 0xfc, 0x00, 0x1f, // move #31,ccr
		0x02, 0x3c, 0x00, 0x15, // andi #21,ccr
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
	BOOST_CHECK_EQUAL(0, cpu.sr.n);
	BOOST_CHECK_EQUAL(1, cpu.sr.z);
	BOOST_CHECK_EQUAL(0, cpu.sr.v);
	BOOST_CHECK_EQUAL(1, cpu.sr.c);
}
BOOST_AUTO_TEST_CASE(andi2ccr_2)
{
	unsigned char code[] = {
		0x44, 0xfc, 0x00, 0x1f, // move #31,ccr
		0x02, 0x3c, 0x00, 0x0a, // andi #10,ccr
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

// ===================================================
// ANDI2SR tests
// ===================================================
uint32_t validateAndi2sr(bool supervisorMode)
{
	unsigned char code[] = {
		0x00, 0x00, 0x00, 0x00,             // reset - SSP
		0x00, 0x00, 0x00, 0x00,             // reset - PC
		0x00, 0x00, 0x00, 0x00,             // Bus error
		0x00, 0x00, 0x00, 0x00,             // Address error
		0x00, 0x00, 0x00, 0x00,             // Illegal Instruction
		0x00, 0x00, 0x00, 0x00,             // Integer Divide by Zero
		0x00, 0x00, 0x00, 0x00,             // CHK Instruction
		0x00, 0x00, 0x00, 0x00,             // TRAPV Instruction
		0x00, 0x00, 0x00, 0x30,             // Privilege Violation

		0x70, 0x2a,                         // moveq.l #42, d0 
		0x02, 0x7c, 0xff, 0xff,             // andi #$ffff, sr 
		0x40, 0xc7,                         // move.w sr, d7
		0x70, 0x15,                         // moveq.l #21, d0 
		0x4e, 0x40,                         // trap #0 

		// VIOLATION:
		0x70, 0x08,                         // moveq.l #8, d0 
		0x4e, 0x40,                         // trap #0 

		0xff, 0xff, 0xff, 0xff              // 
		                                    // ds.l 16
		                                    // 0x74 STACK:
	};

	// Arrange
	memory memory(128, 0, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.setSupervisorMode(supervisorMode);
	cpu.start(0x24, 0x74, 0x74);

	// Assert
	if (supervisorMode)
	{
		BOOST_CHECK_EQUAL(0x15, cpu.d0);
	}
	else
	{
		BOOST_CHECK_EQUAL(0x8, cpu.d0);
	}
	return cpu.d7;
}

BOOST_AUTO_TEST_CASE(andi2sr_user)
{
	validateAndi2sr(false);
}

BOOST_AUTO_TEST_CASE(andi2sr_supervisor)
{
	uint32_t sr = validateAndi2sr(true);
	BOOST_CHECK_EQUAL(0x2000, sr);
}
BOOST_AUTO_TEST_SUITE_END()