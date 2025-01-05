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
	memory memory(128, 0, code, sizeof(code));
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
BOOST_AUTO_TEST_CASE(rte_user)
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
		0x00, 0x00, 0x00, 0x2c,             // Privilege Violation

		0x70, 0x2a,                         // 0x24 moveq.l #42, d0 
		0x4e, 0x73,                         // rte 
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
	cpu.start(0x24, 0x74, 0x74);

	// Assert
	BOOST_CHECK_EQUAL(0x8, cpu.d0);

}

BOOST_AUTO_TEST_SUITE_END()
