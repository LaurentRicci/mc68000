#include <boost/test/unit_test.hpp>
#include "../core/cpu.h"
#include "../core/memory.h"

using namespace mc68000;

BOOST_AUTO_TEST_SUITE(cpuSuite_rot)

BOOST_AUTO_TEST_CASE(rol_memory)
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

BOOST_AUTO_TEST_CASE(rol_memory2)
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

BOOST_AUTO_TEST_CASE(ror_memory)
{
	unsigned char code[] = {
	0x41, 0xfa, 0x00, 0x0a,    // lea data(pc), a0
	0xe6, 0xd0,                // ror (a0)
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

BOOST_AUTO_TEST_CASE(ror_memory2)
{
	unsigned char code[] = {
	0x41, 0xfa, 0x00, 0x0a,    // lea data(pc), a0
	0xe6, 0xd0,                // ror (a0)
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

BOOST_AUTO_TEST_CASE(rol_register_byte3)
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

BOOST_AUTO_TEST_CASE(rol_register_word12)
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

BOOST_AUTO_TEST_CASE(rol_register_long72)
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

BOOST_AUTO_TEST_CASE(ror_register_byte3)
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

BOOST_AUTO_TEST_CASE(ror_register_word12)
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
BOOST_AUTO_TEST_CASE(ror_register_long72)
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

enum size { BYTE = 0x33, WORD = 0x73, LONG = 0xb3 };

void verifyRoxlExecution(uint8_t value, uint8_t rotation, uint8_t ccr, uint32_t expected, uint8_t ccrExpected, size sz=BYTE )
{
	unsigned char code[] = {
		0x76, value,           // moveq VALUE,d3
		0x70, rotation,        // moveq ROTATION,d0
		0x44, 0xfc, 0x00, ccr, // move CCR, ccr
		0xe1, (uint8_t)sz,     // roxl.b d0,d3
		0x4e, 0x40,            // trap #0
		0xff, 0xff, 0xff, 0xff,
	};

	// Arrange
	memory memory(256, 0, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0);

	// Assert
	BOOST_CHECK_EQUAL(rotation, cpu.d0);
	BOOST_CHECK_EQUAL(expected, cpu.d3);
	BOOST_CHECK_EQUAL(ccrExpected, (uint8_t) cpu.sr);

}

BOOST_AUTO_TEST_CASE(roxl_register_byte3)
{
	verifyRoxlExecution(0xcd, 0x03, 0x00, 0xffffff6b, 0x00);
}

BOOST_AUTO_TEST_CASE(roxl_register_byte0)
{
	verifyRoxlExecution(0xcd, 0x00, 0x00, 0xffffffcd, 0x08);
}

BOOST_AUTO_TEST_CASE(roxl_register_byte3x)
{
	verifyRoxlExecution(0xcd, 0x03, 0x10, 0xffffff6f, 0x00);
}

BOOST_AUTO_TEST_CASE(roxl_register_byte0x)
{
	verifyRoxlExecution(0xcd, 0x00, 0x10, 0xffffffcd, 0x19);
}

BOOST_AUTO_TEST_CASE(roxl_register_byte8x)
{
	verifyRoxlExecution(0xcd, 0x08, 0x10, 0xffffffe6, 0x19);
}

BOOST_AUTO_TEST_CASE(roxl_register_byte18x)
{
	verifyRoxlExecution(0xcd, 0x12, 0x10, 0xffffffcd, 0x19);
}

BOOST_AUTO_TEST_CASE(roxl_register_byte24x)
{
	verifyRoxlExecution(0xcd, 0x18, 0x10, 0xffffff79, 0x11);
}

BOOST_AUTO_TEST_CASE(roxl_register_word23x)
{
	verifyRoxlExecution(0xcd, 0x17, 0x10, 0xfffff37f, 0x19, WORD);
}

BOOST_AUTO_TEST_CASE(roxl_register_long42x)
{
	verifyRoxlExecution(0xcd, 0x2a, 0x10, 0xffff9bff, 0x19, LONG);
}

void verifyRoxrExecution(uint8_t value, uint8_t rotation, uint8_t ccr, uint32_t expected, uint8_t ccrExpected, size sz=BYTE)
{
	unsigned char code[] = {
		0x76, value,           // moveq VALUE,d3
		0x70, rotation,        // moveq ROTATION,d0
		0x44, 0xfc, 0x00, ccr, // move CCR, ccr
		0xe0, (uint8_t)sz,     // roxl.b d0,d3
		0x4e, 0x40,            // trap #0
		0xff, 0xff, 0xff, 0xff,
	};

	// Arrange
	memory memory(256, 0, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0);

	// Assert
	BOOST_CHECK_EQUAL(rotation, cpu.d0);
	BOOST_CHECK_EQUAL(expected, cpu.d3);
	BOOST_CHECK_EQUAL(ccrExpected, (uint8_t)cpu.sr);

}
BOOST_AUTO_TEST_CASE(roxr_register_byte3)
{
	verifyRoxrExecution(0xcd, 0x03, 0x00, 0xffffff59, 0x11);
}

BOOST_AUTO_TEST_CASE(roxr_register_byte0)
{
	verifyRoxrExecution(0xcd, 0x00, 0x00, 0xffffffcd, 0x08);
}

BOOST_AUTO_TEST_CASE(roxr_register_byte3x)
{
	verifyRoxrExecution(0xcd, 0x03, 0x10, 0xffffff79, 0x11);
}

BOOST_AUTO_TEST_CASE(roxr_register_byte0x)
{
	verifyRoxrExecution(0xcd, 0x00, 0x10, 0xffffffcd, 0x19);
}

BOOST_AUTO_TEST_CASE(roxr_register_byte8x)
{
	verifyRoxrExecution(0xcd, 0x08, 0x10, 0xffffff9b, 0x19);
}

BOOST_AUTO_TEST_CASE(roxr_register_byte18x)
{
	verifyRoxrExecution(0xcd, 0x12, 0x10, 0xffffffcd, 0x19);
}

BOOST_AUTO_TEST_CASE(roxr_register_byte24x)
{
	verifyRoxrExecution(0xcd, 0x18, 0x10, 0xffffff6f, 0x00);
}

BOOST_AUTO_TEST_CASE(roxr_register_word23x)
{
	verifyRoxrExecution(0x75, 0x17, 0x10, 0xac01, 0x19, WORD);
}

BOOST_AUTO_TEST_CASE(roxr_register_long42x)
{
	verifyRoxrExecution(0xcd, 0x2a, 0x10, 0xcdffffff, 0x19, LONG);
}


enum direction { LEFT = 0xe5, RIGHT = 0xe4 };
void verifyRotateMemoryExecution(direction dir, uint16_t value, uint8_t ccr, uint16_t expected, uint8_t ccrExpected)
{
	unsigned char code[] = {
	0x41, 0xfa, 0x00, 0x0e,    // lea data(pc), a0
	0x44, 0xfc, 0x00, ccr,     // move CCR, ccr
	(uint8_t)dir, 0xd0,        // roxDIR (a0)
	0x4e, 0x40,                // trap #0
	0xff, 0xff, 0xff, 0xff,
	(uint8_t)(value >> 8),     // data: dc.w VALUE
	(uint8_t)(value & 0xff)   
	};

	// Arrange
	memory memory(256, 0, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0);

	// Assert
	BOOST_CHECK_EQUAL(0x10, cpu.a0);
	BOOST_CHECK_EQUAL(expected, cpu.mem.get<uint16_t>(cpu.a0));
	BOOST_CHECK_EQUAL(ccrExpected, (uint8_t)cpu.sr);
}

BOOST_AUTO_TEST_CASE(roxl_memory)
{
	verifyRotateMemoryExecution(LEFT, 0xcd, 0x00, 0x19a, 0x00);
}

BOOST_AUTO_TEST_CASE(roxl_memoryX)
{
	verifyRotateMemoryExecution(LEFT, 0x7654, 0x10, 0xeca9, 0x08);
}

BOOST_AUTO_TEST_CASE(roxr_memory)
{
	verifyRotateMemoryExecution(RIGHT, 0xcd, 0x00, 0x66, 0x11);
}

BOOST_AUTO_TEST_CASE(roxr_memoryX)
{
	verifyRotateMemoryExecution(RIGHT, 0x7654, 0x10, 0xbb2a, 0x08);
}
BOOST_AUTO_TEST_SUITE_END()