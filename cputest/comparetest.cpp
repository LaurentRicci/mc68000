#include <boost/test/unit_test.hpp>
#include "../core/cpu.h"
#include "../core/memory.h"

using namespace mc68000;

BOOST_AUTO_TEST_SUITE(cpuSuite_cmp)

void verifyCmpiExecution_b(uint8_t d0, uint8_t cmp, uint8_t n, uint8_t z, uint8_t v, uint8_t c)
{
	unsigned char code[] = {
		0x10, 0x3c, 0x00, d0,    // move.b #10,d0
		0x0c, 0x00, 0x00, cmp,   // cmpi.b #5, d0
		0x4e, 0x40,              // trap #0
		0xff, 0xff };

	// Arrange
	memory memory(256, 0, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0);

	// Assert
	BOOST_CHECK_EQUAL(c, cpu.sr.c);
	BOOST_CHECK_EQUAL(z, cpu.sr.z);
	BOOST_CHECK_EQUAL(n, cpu.sr.n);
	BOOST_CHECK_EQUAL(v, cpu.sr.v);

}
inline uint8_t u8(uint16_t val) { return static_cast<uint8_t>(val); }

void verifyCmpiExecution_w(uint16_t d0, uint16_t cmp, uint8_t n, uint8_t z, uint8_t v, uint8_t c)
{
	unsigned char code[] = {
		0x10, 0x3c, u8(d0 >> 8),  u8(d0 & 0xff),    // move.w #10,d0
		0x0c, 0x00, u8(cmp >> 8), u8(cmp & 0xff),    // cmpi.w #5, d0
		0x4e, 0x40,                                 // trap #0
		0xff, 0xff };

	// Arrange
	memory memory(256, 0, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0);

	// Assert
	BOOST_CHECK_EQUAL(c, cpu.sr.c);
	BOOST_CHECK_EQUAL(z, cpu.sr.z);
	BOOST_CHECK_EQUAL(n, cpu.sr.n);
	BOOST_CHECK_EQUAL(v, cpu.sr.v);

}

void verifyCmpiExecution_l(uint32_t d0, uint32_t cmp, uint8_t n, uint8_t z, uint8_t v, uint8_t c)
{
	unsigned char code[] = {
		0x20, 0x3c,  u8(d0 >> 24),  u8((d0 >> 16) & 0xff),  u8((d0 >> 8) & 0xff),  u8(d0 & 0xff),    // move.l #10,d0
		0x0c, 0x80,  u8(cmp >> 24), u8((cmp >> 16) & 0xff), u8((cmp >> 8) & 0xff), u8(cmp & 0xff),   // cmpi.l #5, d0
		0x4e, 0x40,                                                                                  // trap #0
		0xff, 0xff };

	// Arrange
	memory memory(256, 0, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0);

	// Assert
	BOOST_CHECK_EQUAL(c, cpu.sr.c);
	BOOST_CHECK_EQUAL(z, cpu.sr.z);
	BOOST_CHECK_EQUAL(n, cpu.sr.n);
	BOOST_CHECK_EQUAL(v, cpu.sr.v);

}
BOOST_AUTO_TEST_CASE(cmpi_b)
{
	verifyCmpiExecution_b(10, 5, 0, 0, 0, 0);
	verifyCmpiExecution_b(5, 5, 0, 1, 0, 0);
	verifyCmpiExecution_b(5, 10, 1, 0, 0, 1);
	verifyCmpiExecution_b(0x70, 0x81, 1, 0, 1, 1);
}

BOOST_AUTO_TEST_CASE(cmpi_w)
{
	verifyCmpiExecution_w(0x1010, 0x0505, 0, 0, 0, 0);
	verifyCmpiExecution_w(0x0505, 0x0505, 0, 1, 0, 0);
	verifyCmpiExecution_w(0x0505, 0x1010, 1, 0, 0, 1);
	verifyCmpiExecution_w(0x7070, 0x8181, 1, 0, 1, 1);
}

BOOST_AUTO_TEST_CASE(cmpi_l)
{
	verifyCmpiExecution_l(0x10101010, 0x05050505, 0, 0, 0, 0);
	verifyCmpiExecution_l(0x05050505, 0x05050505, 0, 1, 0, 0);
	verifyCmpiExecution_l(0x05050505, 0x10101010, 1, 0, 0, 1);
	verifyCmpiExecution_l(0x70707070, 0x81818181, 1, 0, 1, 1);
}

BOOST_AUTO_TEST_CASE(cmp)
{
	unsigned char code[] = {
		0x36, 0x3c,  0x70, 0x70,    // move.w #$7070,d3
		0x32, 0x3c,  0x81, 0x81,    // move.w #$8181,d1
		0xb6, 0x41,                 // cmp.w d1, d3
		0x4e, 0x40,                 // trap #0
		0xff, 0xff };

	// Arrange
	memory memory(256, 100, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(100);

	// Assert
	BOOST_CHECK_EQUAL(1, cpu.sr.c);
	BOOST_CHECK_EQUAL(0, cpu.sr.z);
	BOOST_CHECK_EQUAL(1, cpu.sr.n);
	BOOST_CHECK_EQUAL(1, cpu.sr.v);

}

BOOST_AUTO_TEST_CASE(cmpa)
{
	unsigned char code[] = {
		0x30, 0x7c,  0x70, 0x70,    // move.w #$7070,a0
		0x32, 0x3c,  0x81, 0x81,    // move.w #$8181,d1
		0xb0, 0xc1,                 // cmpa.w d1, a0
		0x4e, 0x40,                 // trap #0
		0xff, 0xff };

	// Arrange
	memory memory(256, 100, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(100);

	// Assert
	BOOST_CHECK_EQUAL(1, cpu.sr.c);
	BOOST_CHECK_EQUAL(0, cpu.sr.z);
	BOOST_CHECK_EQUAL(1, cpu.sr.n);
	BOOST_CHECK_EQUAL(1, cpu.sr.v);

}

BOOST_AUTO_TEST_CASE(cmpm_b)
{
	unsigned char code[] = {
		0x43, 0xfa, 0x00, 0x0e,    // lea data1(pc), a1
		0x45, 0xfa, 0x00, 0x0e,    // lea data2(pc), a2
		0xb5, 0x09,                 // cmpm.b (a1)+, (a2)+
		0x4e, 0x40,                 // trap #0
		0xff, 0xff, 0xff, 0xff,
		0x12, 0x34, 0x56, 0x78,    // data1: dc.b $12,$34,$56,$78
		0xfe, 0xdc, 0xba, 0x98     // data2: dc.b $fe,$dc,$ba,$98
	};

	// Arrange
	memory memory(256, 100, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(100);

	// Assert
	BOOST_CHECK_EQUAL(1, cpu.sr.n);
	BOOST_CHECK_EQUAL(0, cpu.sr.z);
	BOOST_CHECK_EQUAL(0, cpu.sr.v);
	BOOST_CHECK_EQUAL(0, cpu.sr.c);
	BOOST_CHECK_EQUAL(117, cpu.a1);
	BOOST_CHECK_EQUAL(121, cpu.a2);
}

BOOST_AUTO_TEST_CASE(cmpm_w)
{
	unsigned char code[] = {
		0x43, 0xfa, 0x00, 0x0e,    // lea data1(pc), a1
		0x45, 0xfa, 0x00, 0x0e,    // lea data2(pc), a2
		0xb5, 0x49,                 // cmpm.b (a1)+, (a2)+
		0x4e, 0x40,                 // trap #0
		0xff, 0xff, 0xff, 0xff,
		0x12, 0x34, 0x56, 0x78,    // data1: dc.b $12,$34,$56,$78
		0xfe, 0xdc, 0xba, 0x98     // data2: dc.b $fe,$dc,$ba,$98
	};

	// Arrange
	memory memory(256, 100, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(100);

	// Assert
	BOOST_CHECK_EQUAL(1, cpu.sr.n);
	BOOST_CHECK_EQUAL(0, cpu.sr.z);
	BOOST_CHECK_EQUAL(0, cpu.sr.v);
	BOOST_CHECK_EQUAL(0, cpu.sr.c);
	BOOST_CHECK_EQUAL(118, cpu.a1);
	BOOST_CHECK_EQUAL(122, cpu.a2);
}

BOOST_AUTO_TEST_CASE(cmpm_l)
{
	unsigned char code[] = {
		0x43, 0xfa, 0x00, 0x12,    // lea data1(pc), a1
		0x45, 0xfa, 0x00, 0x0a,    // lea data2(pc), a2
		0xb5, 0x89,                 // cmpm.b (a1)+, (a2)+
		0x4e, 0x40,                 // trap #0
		0xff, 0xff, 0xff, 0xff,
		0x12, 0x34, 0x56, 0x78,    // data1: dc.b $12,$34,$56,$78
		0xfe, 0xdc, 0xba, 0x98     // data2: dc.b $fe,$dc,$ba,$98
	};

	// Arrange
	memory memory(256, 100, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(100);

	// Assert
	BOOST_CHECK_EQUAL(0, cpu.sr.n);
	BOOST_CHECK_EQUAL(0, cpu.sr.z);
	BOOST_CHECK_EQUAL(0, cpu.sr.v);
	BOOST_CHECK_EQUAL(1, cpu.sr.c);
	BOOST_CHECK_EQUAL(124, cpu.a1);
	BOOST_CHECK_EQUAL(120, cpu.a2);
}

BOOST_AUTO_TEST_SUITE_END()
