#include <boost/test/unit_test.hpp>
#include "../core/disasm.h"

using namespace mc68000;

BOOST_AUTO_TEST_SUITE(Offset)

BOOST_AUTO_TEST_CASE(offset)
{
	// Arrange
	uint16_t code[] = {
		0x4e71,                   //00001000  4E71                      12      nop
		0x4ef9, 0x0000, 0x100e,   //00001002  4EF9 0000100E             13      jmp L1
		0x6000, 0x0008,           //00001008  6000 0008                 14      bra L2
		0x4e71,                   //0000100C  4E71                      15      nop
		                          //0000100E                            16  L1
		0x4e71,                   //0000100E  4E71                      17      nop
		0x4e71,                   //00001010  4E71                      18      nop
		                          //00001012                            19  L2
		0x4e71,                   //00001012  4E71                      20      nop
		0x65f8,                   //00001014  65F8                      21      bcs L1
		                          //00001016                            22  L3
		0x4e71,                   //00001016  4E71                      23      nop
		0x4e71,                   //00001018  4E71                      24      nop
		0x4e71,                   //0000101A  4E71                      25      nop
		0x6000, 0x0002,           //0000101C  6000 0002                 26      bra L4
		                          //00001020                            27  L4
		0x64f4,                   //00001020  64F4                      28      bcc L3
		0xffff, 0xffff            //00001022  FFFF FFFF                 29      SIMHALT;

	};
	std::string expected =
		"nop\n"
		"jmp $100e.l\n"
		"bra $1012\n"
		"nop\n"
		"nop\n"
		"nop\n"
		"nop\n"
		"bcs $100e\n"
		"nop\n"
		"nop\n"
		"nop\n"
		"bra $1020\n"
		"bcc $1016\n"
		"end\n";

	// Act
	DisAsm d;
	auto result = d.dasm(code, 0x1000);

	// Assert
	BOOST_CHECK_EQUAL(expected.c_str(), result.c_str());

}
BOOST_AUTO_TEST_SUITE_END()