#include <boost/test/unit_test.hpp>
#include "../core/cpu.h"
#include "../core/memory.h"

using namespace mc68000;

BOOST_AUTO_TEST_SUITE(cpuSuite_exception)


// ===================================================
// ILLEGAL tests
// ===================================================

BOOST_AUTO_TEST_CASE(exception_illegal)
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



BOOST_AUTO_TEST_SUITE_END()
