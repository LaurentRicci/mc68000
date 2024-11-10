#include <boost/test/unit_test.hpp>
#include "../core/cpu.h"
#include "../core/memory.h"

using namespace mc68000;

BOOST_AUTO_TEST_SUITE(cpuSuite_rot)

BOOST_AUTO_TEST_CASE(rot_rolmemory)
{
	unsigned char code[] = {
	0x41, 0xfa, 0x00, 0x0a,    // lea data(pc), a0
	0xe7, 0xd0,                // rol (a0)
	0x4e, 0x40,                // trap #0
	0xff, 0xff, 0xff, 0xff,
	0x00, 0xcd                 // data: dc.w $cd
	};

	// Arrange
	memory memory(256, 0, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0);

	// Assert
	BOOST_CHECK_EQUAL(0x0c, cpu.a0);
	BOOST_CHECK_EQUAL(0x019a, cpu.mem.get<uint16_t>(cpu.a0));
	BOOST_CHECK_EQUAL(0, cpu.sr.c);
	BOOST_CHECK_EQUAL(0, cpu.sr.z);
	BOOST_CHECK_EQUAL(0, cpu.sr.n);
	BOOST_CHECK_EQUAL(0, cpu.sr.v);
	BOOST_CHECK_EQUAL(0, cpu.sr.x);
}

BOOST_AUTO_TEST_CASE(rot_rolmemory2)
{
	unsigned char code[] = {
	0x41, 0xfa, 0x00, 0x0a,    // lea data(pc), a0
	0xe7, 0xd0,                // rol (a0)
	0x4e, 0x40,                // trap #0
	0xff, 0xff, 0xff, 0xff,
	0xd4, 0x56                 // data: dc.w $$d456
	};

	// Arrange
	memory memory(256, 0, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0);

	// Assert
	BOOST_CHECK_EQUAL(0x0c, cpu.a0);
	BOOST_CHECK_EQUAL(0xa8ad, cpu.mem.get<uint16_t>(cpu.a0));
	BOOST_CHECK_EQUAL(1, cpu.sr.c);
	BOOST_CHECK_EQUAL(0, cpu.sr.z);
	BOOST_CHECK_EQUAL(1, cpu.sr.n);
	BOOST_CHECK_EQUAL(0, cpu.sr.v);
	BOOST_CHECK_EQUAL(0, cpu.sr.x);
}

BOOST_AUTO_TEST_CASE(rot_rormemory)
{
	unsigned char code[] = {
	0x41, 0xfa, 0x00, 0x0a,    // lea data(pc), a0
	0xe6, 0xd0,                // rol (a0)
	0x4e, 0x40,                // trap #0
	0xff, 0xff, 0xff, 0xff,
	0x00, 0xcd                 // data: dc.w $cd
	};

	// Arrange
	memory memory(256, 0, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0);

	// Assert
	BOOST_CHECK_EQUAL(0x0c, cpu.a0);
	BOOST_CHECK_EQUAL(0x8066, cpu.mem.get<uint16_t>(cpu.a0));
	BOOST_CHECK_EQUAL(1, cpu.sr.c);
	BOOST_CHECK_EQUAL(0, cpu.sr.z);
	BOOST_CHECK_EQUAL(1, cpu.sr.n);
	BOOST_CHECK_EQUAL(0, cpu.sr.v);
	BOOST_CHECK_EQUAL(0, cpu.sr.x);
}

BOOST_AUTO_TEST_CASE(rot_rormemory2)
{
	unsigned char code[] = {
	0x41, 0xfa, 0x00, 0x0a,    // lea data(pc), a0
	0xe6, 0xd0,                // rol (a0)
	0x4e, 0x40,                // trap #0
	0xff, 0xff, 0xff, 0xff,
	0xd4, 0x56                 // data: dc.w $$d456
	};

	// Arrange
	memory memory(256, 0, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0);

	// Assert
	BOOST_CHECK_EQUAL(0x0c, cpu.a0);
	BOOST_CHECK_EQUAL(0x6a2b, cpu.mem.get<uint16_t>(cpu.a0));
	BOOST_CHECK_EQUAL(0, cpu.sr.c);
	BOOST_CHECK_EQUAL(0, cpu.sr.z);
	BOOST_CHECK_EQUAL(0, cpu.sr.n);
	BOOST_CHECK_EQUAL(0, cpu.sr.v);
	BOOST_CHECK_EQUAL(0, cpu.sr.x);
}

BOOST_AUTO_TEST_CASE(rot_rolregister_byte3)
{
	unsigned char code[] = {
	0x76, 0xcd,     // moveq #$cd,d3
	0x70, 0x03,     // moveq #3,d0
	0xe1, 0x3b,     // rol.b d0,d3
	0x4e, 0x40,     // trap #0
	0xff, 0xff, 0xff, 0xff,
	};

	// Arrange
	memory memory(256, 0, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0);

	// Assert
	BOOST_CHECK_EQUAL(0x3, cpu.d0);
	BOOST_CHECK_EQUAL(0xffffff6e, cpu.d3);
	BOOST_CHECK_EQUAL(0, cpu.sr.c);
	BOOST_CHECK_EQUAL(0, cpu.sr.z);
	BOOST_CHECK_EQUAL(0, cpu.sr.n);
	BOOST_CHECK_EQUAL(0, cpu.sr.v);
	BOOST_CHECK_EQUAL(0, cpu.sr.x);
}

BOOST_AUTO_TEST_CASE(rot_rolregister_word12)
{
	unsigned char code[] = {
	0x76, 0xcd,     // moveq #$cd,d3
	0x70, 0x0c,     // moveq #12,d0
	0xe1, 0x7b,     // rol.w d0,d3
	0x4e, 0x40,     // trap #0
	0xff, 0xff, 0xff, 0xff,
	};

	// Arrange
	memory memory(256, 0, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0);

	// Assert
	BOOST_CHECK_EQUAL(0x0c, cpu.d0);
	BOOST_CHECK_EQUAL(0xffffdffc, cpu.d3);
	BOOST_CHECK_EQUAL(0, cpu.sr.c);
	BOOST_CHECK_EQUAL(0, cpu.sr.z);
	BOOST_CHECK_EQUAL(1, cpu.sr.n);
	BOOST_CHECK_EQUAL(0, cpu.sr.v);
	BOOST_CHECK_EQUAL(0, cpu.sr.x);
}

BOOST_AUTO_TEST_CASE(rot_rolregister_long72)
{
	unsigned char code[] = {
	0x76, 0xcd,     // moveq #$cd,d3
	0x70, 0x48,     // moveq #72,d0
	0xe1, 0xbb,     // rol.l d0,d3
	0x4e, 0x40,     // trap #0
	0xff, 0xff, 0xff, 0xff,
	};

	// Arrange
	memory memory(256, 0, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0);

	// Assert
	BOOST_CHECK_EQUAL(0x48, cpu.d0);
	BOOST_CHECK_EQUAL(0xffffcdff, cpu.d3);
	BOOST_CHECK_EQUAL(1, cpu.sr.c);
	BOOST_CHECK_EQUAL(0, cpu.sr.z);
	BOOST_CHECK_EQUAL(1, cpu.sr.n);
	BOOST_CHECK_EQUAL(0, cpu.sr.v);
	BOOST_CHECK_EQUAL(0, cpu.sr.x);
}

BOOST_AUTO_TEST_CASE(rot_rorregister_byte3)
{
	unsigned char code[] = {
	0x76, 0xcd,     // moveq #$cd,d3
	0x70, 0x03,     // moveq #3,d0
	0xe0, 0x3b,     // rol.b d0,d3
	0x4e, 0x40,     // trap #0
	0xff, 0xff, 0xff, 0xff,
	};

	// Arrange
	memory memory(256, 0, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0);

	// Assert
	BOOST_CHECK_EQUAL(0x3, cpu.d0);
	BOOST_CHECK_EQUAL(0xffffffb9, cpu.d3);
	BOOST_CHECK_EQUAL(1, cpu.sr.c);
	BOOST_CHECK_EQUAL(0, cpu.sr.z);
	BOOST_CHECK_EQUAL(1, cpu.sr.n);
	BOOST_CHECK_EQUAL(0, cpu.sr.v);
	BOOST_CHECK_EQUAL(0, cpu.sr.x);
}

BOOST_AUTO_TEST_CASE(rot_rorregister_word12)
{
	unsigned char code[] = {
	0x76, 0xcd,     // moveq #$cd,d3
	0x70, 0x0c,     // moveq #12,d0
	0xe0, 0x7b,     // rol.w d0,d3
	0x4e, 0x40,     // trap #0
	0xff, 0xff, 0xff, 0xff,
	};

	// Arrange
	memory memory(256, 0, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0);

	// Assert
	BOOST_CHECK_EQUAL(0x0c, cpu.d0);
	BOOST_CHECK_EQUAL(0xfffffcdf, cpu.d3);
	BOOST_CHECK_EQUAL(1, cpu.sr.c);
	BOOST_CHECK_EQUAL(0, cpu.sr.z);
	BOOST_CHECK_EQUAL(1, cpu.sr.n);
	BOOST_CHECK_EQUAL(0, cpu.sr.v);
	BOOST_CHECK_EQUAL(0, cpu.sr.x);
}
BOOST_AUTO_TEST_CASE(rot_rorregister_long72)
{
	unsigned char code[] = {
	0x76, 0xcd,     // moveq #$cd,d3
	0x70, 0x48,     // moveq #72,d0
	0xe0, 0xbb,     // rol.l d0,d3
	0x4e, 0x40,     // trap #0
	0xff, 0xff, 0xff, 0xff,
	};

	// Arrange
	memory memory(256, 0, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0);

	// Assert
	BOOST_CHECK_EQUAL(0x48, cpu.d0);
	BOOST_CHECK_EQUAL(0xcdffffff, cpu.d3);
	BOOST_CHECK_EQUAL(1, cpu.sr.c);
	BOOST_CHECK_EQUAL(0, cpu.sr.z);
	BOOST_CHECK_EQUAL(1, cpu.sr.n);
	BOOST_CHECK_EQUAL(0, cpu.sr.v);
	BOOST_CHECK_EQUAL(0, cpu.sr.x);
}

BOOST_AUTO_TEST_SUITE_END()