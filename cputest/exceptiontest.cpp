#include <boost/test/unit_test.hpp>
#include "../core/cpu.h"
#include "../core/memory.h"
#include "verifyexecution.h"

using namespace mc68000;

BOOST_AUTO_TEST_SUITE(cpuSuite_exception)


// ===================================================
// ILLEGAL tests
// ===================================================

BOOST_AUTO_TEST_CASE(illegal)
{
	unsigned char code[] = {
		0x00, 0x00, 0x00, 0x00,             // reset - SSP
		0x00, 0x00, 0x00, 0x00,             // reset - PC
		0x00, 0x00, 0x00, 0x00,             // Bus error
		0x00, 0x00, 0x00, 0x00,             // Address error
		0x00, 0x00, 0x00, 0x1c,             // ILLEGAL

		0x70, 0x2a,                         // moveq.l #42, d0 
		0x4a, 0xfc,                         // Illegal 
		0x70, 0x15,                         // moveq.l #21, d0 
		0x4e, 0x40,                         // trap #0 

	                                        // ILLEGAL:
		0x70, 0x17,                         // moveq.l #23, d0 
		0x4e, 0x40,                         // trap #0 

		0xff, 0xff, 0xff, 0xff              // 
	};

	// Arrange
	Memory memory(128, 0, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0x14, 126, 126);

	// Assert
	BOOST_CHECK_EQUAL(23, cpu.d0);
}

// ===================================================
// RTE tests
// ===================================================
void validateRte(bool supervisorMode)
{
	unsigned char code[] = {
	0x00, 0x00, 0x00, 0x00,             // 0  reset - SSP
	0x00, 0x00, 0x00, 0x00,             // 4  reset - PC
	0x00, 0x00, 0x00, 0x00,             // 8  Bus error
	0x00, 0x00, 0x00, 0x00,             // 12 Address error
	0x00, 0x00, 0x00, 0x00,             // 16 Illegal Instruction
	0x00, 0x00, 0x00, 0x00,             // 20 Integer Divide by Zero
	0x00, 0x00, 0x00, 0x00,             // 24 CHK Instruction
	0x00, 0x00, 0x00, 0x00,             // 28 TRAPV Instruction
	0x00, 0x00, 0x00, 0x32,             // 32 Privilege Violation

	0x70, 0x2a,                         // 36 moveq.l #42, d0
	0x48, 0x7a, 0x00, 0x06,             // 38 pea next(pc)
	0x42, 0x67,                         // 42 clr.w -(a7)
	0x4e, 0x73,                         // 44 rte
	                                    // next:
	0x70, 0x15,                         // 46 moveq.l #21, d0 
	0x4e, 0x40,                         // 48 trap #0 

	                                    // VIOLATION:
	0x70, 0x08,                         // 50 moveq.l #8, d0 
	0x4e, 0x40,                         // 52 trap #0 
	0xff, 0xff, 0xff, 0xff              // 
	                                    // ds.l 16
	                                    // STACK:
	};

	// Arrange
	Memory memory(128, 0, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.setSupervisorMode(supervisorMode);
	cpu.start(36, 0x74, 0x74);

	// Assert
	if (supervisorMode)
	{
		BOOST_CHECK_EQUAL(0x15, cpu.d0);
	}
	else
	{
		BOOST_CHECK_EQUAL(0x8, cpu.d0);
	}
}

BOOST_AUTO_TEST_CASE(rte_user)
{
	validateRte(false);
}

BOOST_AUTO_TEST_CASE(rte_supervisor)
{
	validateRte(true);
}

BOOST_AUTO_TEST_SUITE_END()
