#include <boost/test/unit_test.hpp>
#include "../core/cpu.h"
#include "../core/memory.h"

using namespace mc68000;

BOOST_AUTO_TEST_SUITE(cpuSuite)

BOOST_AUTO_TEST_CASE(a_asm)
{
  BOOST_TEST(1 == 1);
}

BOOST_AUTO_TEST_CASE(a_reset)
{
	// Arrange
	memory memory(4, 0);
	cpu cpu(memory);

	// Act
	cpu.reset(memory);

	// Assert
	BOOST_CHECK_EQUAL(0, cpu.d0);
	BOOST_CHECK_EQUAL(0, cpu.d1);
	BOOST_CHECK_EQUAL(0, cpu.d2);
	BOOST_CHECK_EQUAL(0, cpu.d3);
	BOOST_CHECK_EQUAL(0, cpu.d4);
	BOOST_CHECK_EQUAL(0, cpu.d5);
	BOOST_CHECK_EQUAL(0, cpu.d6);
	BOOST_CHECK_EQUAL(0, cpu.d7);

	BOOST_CHECK_EQUAL(0, cpu.a0);
	BOOST_CHECK_EQUAL(0, cpu.a1);
	BOOST_CHECK_EQUAL(0, cpu.a2);
	BOOST_CHECK_EQUAL(0, cpu.a3);
	BOOST_CHECK_EQUAL(0, cpu.a4);
	BOOST_CHECK_EQUAL(0, cpu.a5);
	BOOST_CHECK_EQUAL(0, cpu.a6);
	BOOST_CHECK_EQUAL(0, cpu.a7);
}

BOOST_AUTO_TEST_CASE(a_sr)
{
	memory memory;
	cpu cpu(memory);

	BOOST_CHECK_EQUAL(0, cpu.sr.c);

}

void verifyExecution(const uint8_t* code, uint32_t size, void (*assertFunctor)(const cpu& c))
{
	// Arrange
	memory memory(256, 0, code, size);
	cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0);

	// Assert
	assertFunctor(cpu);
}

void verifyExecution(const uint8_t* code, uint32_t size, uint32_t baseAddress, void (*assertFunctor)(const cpu& c))
{
	// Arrange
	memory memory(256, baseAddress, code, size);
	cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(baseAddress);

	// Assert
	assertFunctor(cpu);
}

void validateSR(const cpu& cpu, int x, int n, int z, int v, int c)
{
	if (x != -1) BOOST_CHECK_EQUAL(x, cpu.sr.x);
	if (n != -1) BOOST_CHECK_EQUAL(n, cpu.sr.n);
	if (z != -1) BOOST_CHECK_EQUAL(z, cpu.sr.z);
	if (v != -1) BOOST_CHECK_EQUAL(v, cpu.sr.v);
	if (c != -1) BOOST_CHECK_EQUAL(c, cpu.sr.c);
}
BOOST_AUTO_TEST_CASE(a_moveToD2_b)
{
	// Arrange
	unsigned char code[] = { 0b0001'0100u, 0b0011'1100u, 0, 0x20, 0x4e, 0x40 }; // move.b #$20,d2

	// Act
	verifyExecution(code, sizeof(code), [](const cpu& cpu) { BOOST_CHECK_EQUAL(0x20, cpu.d2); });
}

BOOST_AUTO_TEST_CASE(a_moveToD4_w)
{
	// Arrange
	unsigned char code[] = { 0b0011'1000u, 0b0011'1100u, 0x12, 0x34, 0x4e, 0x40 }; // move.w #$1234,d4

	// Assert
	verifyExecution(code, sizeof(code), [](const cpu& cpu) 
		{ 
			BOOST_CHECK_EQUAL(0x1234, cpu.d4); 
			validateSR(cpu, -1, 0, 0, 0, 0);
		});
}

BOOST_AUTO_TEST_CASE(a_moveNegativeToD4_w)
{
	// Arrange
	unsigned char code[] = { 0b0011'1000u, 0b0011'1100u, 0x82, 0x34, 0x4e, 0x40 }; // move.w #$1234,d4

	// Assert
	verifyExecution(code, sizeof(code), [](const cpu& cpu) 
		{ 
			BOOST_CHECK_EQUAL(0x8234, cpu.d4);
			validateSR(cpu, -1, 1, 0, 0, 0);
		});
}

BOOST_AUTO_TEST_CASE(a_moveToD7_l)
{
	// Arrange
	unsigned char code[] = { 0b0010'1110u, 0b0011'1100u, 0x12, 0x34, 0x56, 0x78, 0x4e, 0x40 }; // move.w #$1234,d7

	// Assert
	verifyExecution(code, sizeof(code), [](const cpu& cpu) { BOOST_CHECK_EQUAL(0x12345678, cpu.d7); });
}

BOOST_AUTO_TEST_CASE(a_moveToA2_w)
{
	// Arrange
	unsigned char code[] = { 0b0011'0100u, 0b0111'1100u, 0x12, 0x34, 0x4e, 0x40 }; // movea.w #$1234,a2

	// Assert
	verifyExecution(code, sizeof(code), [](const cpu& cpu) { BOOST_CHECK_EQUAL(0x1234, cpu.a2); });
}

BOOST_AUTO_TEST_CASE(a_moveNegativeToA2_w)
{
	// Arrange
	unsigned char code[] = { 0b0011'0100u, 0b0111'1100u, 0x87, 0x34, 0x4e, 0x40 }; // movea.w #$8734,a2

	// Assert
	verifyExecution(code, sizeof(code), [](const cpu& cpu) { BOOST_CHECK_EQUAL(0xffff8734, cpu.a2); });
}

// =================================================================================================
// Addressing mode tests - Read
// =================================================================================================


BOOST_AUTO_TEST_CASE(a_addressMode_000)
{
	// move.w #$1234,d4 ; movea.w d4,a0 ; trap #0
	unsigned char code[] = { 0x38, 0x3c, 0x12, 0x34, 0x30, 0x44, 0x4e, 0x40 };
	verifyExecution(code, sizeof(code), [](const cpu& cpu)
		{
			BOOST_CHECK_EQUAL(0x1234, cpu.d4);
			BOOST_CHECK_EQUAL(0x1234, cpu.a0);
		});
}
BOOST_AUTO_TEST_CASE(a_addressMode_001)
{
	// movea.w #$1234,a4 ; movea.w a4,a0 ; trap #0
	unsigned char code[] = { 0x38, 0x7c, 0x12, 0x34, 0b0011'0000u, 0b01'001'100u, 0x4e, 0x40 };
	verifyExecution(code, sizeof(code), [](const cpu& cpu)
		{
			BOOST_CHECK_EQUAL(0x1234, cpu.a4);
			BOOST_CHECK_EQUAL(0x1234, cpu.a0);
		});
}

BOOST_AUTO_TEST_CASE(a_addressMode_010)
{
	// movea.w #10,a4 ; movea.w (a4),a0 ; trap #0
	unsigned char code[] = { 0x38, 0x7c, 0x00, 0x0A, 0b0011'0000u, 0b01'010'100u, 0x4e, 0x40, 0xff, 0xff, 0x43, 0x21 };
	verifyExecution(code, sizeof(code), [](const cpu& cpu)
		{
			BOOST_CHECK_EQUAL(0x0A, cpu.a4);
			BOOST_CHECK_EQUAL(0x4321, cpu.a0);
		});
}

BOOST_AUTO_TEST_CASE(a_addressMode_011)
{
	// movea.w #10,a4 ; movea.w (a4)+,a0 ; trap #0
	unsigned char code[] = { 0x38, 0x7c, 0x00, 0x0A, 0b0011'0000u, 0b01'011'100u, 0x4e, 0x40, 0xff, 0xff, 0x43, 0x21 };
	verifyExecution(code, sizeof(code), [](const cpu& cpu)
		{
			BOOST_CHECK_EQUAL(0x0C, cpu.a4);
			BOOST_CHECK_EQUAL(0x4321, cpu.a0);
		});
}

BOOST_AUTO_TEST_CASE(a_addressMode_100)
{
	// movea.w #12,a4 ; movea.w -(a4),a0 ; trap #0
	unsigned char code[] = { 0x38, 0x7c, 0x00, 0x0C, 0b0011'0000u, 0b01'100'100u, 0x4e, 0x40, 0xff, 0xff, 0x43, 0x21 };
	verifyExecution(code, sizeof(code), [](const cpu& cpu)
		{
			BOOST_CHECK_EQUAL(0x0A, cpu.a4);
			BOOST_CHECK_EQUAL(0x4321, cpu.a0);
		});
}

BOOST_AUTO_TEST_CASE(a_addressMode_101_positive)
{
	// movea.w #6,a4 ; movea.w 6(a4),a0 ; trap #0
	unsigned char code[] = { 0x38, 0x7c, 0x00, 0x06, 0b0011'0000u, 0b01'101'100u, 0x00, 0x06, 0x4e, 0x40, 0xff, 0xff, 0x43, 0x21 };
	verifyExecution(code, sizeof(code), [](const cpu& cpu)
		{
			BOOST_CHECK_EQUAL(0x06, cpu.a4);
			BOOST_CHECK_EQUAL(0x4321, cpu.a0);
		});
}

BOOST_AUTO_TEST_CASE(a_addressMode_101_negative)
{
	unsigned char code[] = {
		0x38, 0x7c, 0x00, 0x12,                  // movea.w #18,a4
		0b0011'0000u, 0b01'101'100u, 0xff, 0xfa, // movea.w -6(a4),a0
		0x4e, 0x40,                              // trap #0
		0xff, 0xff,
		0x43, 0x21 };
	verifyExecution(code, sizeof(code), [](const cpu& cpu)
		{
			BOOST_CHECK_EQUAL(0x12, cpu.a4);
			BOOST_CHECK_EQUAL(0x4321, cpu.a0);
		});
}

BOOST_AUTO_TEST_CASE(a_addressMode_101_long)
{
	unsigned char code[] = { 
		0x38, 0x7c, 0x00, 0x06,                  // movea.w #6,a4
		0x20, 0x6c, 0x00, 0x06,                  // movea.l 6(a4),a0
		0x4e, 0x40,                              // trap #0
		0xff, 0xff, 
		0x43, 0x21, 0x12, 0x34 };
	verifyExecution(code, sizeof(code), [](const cpu& cpu)
		{
			BOOST_CHECK_EQUAL(0x6, cpu.a4);
			BOOST_CHECK_EQUAL(0x43211234, cpu.a0);
		});
}


BOOST_AUTO_TEST_CASE(a_addressMode_110_positive)
{
	unsigned char code[] = { 
		0x38, 0x7c, 0x00, 0x06,                   // movea.w #6,a4 ;
		0b0011'010'0u, 0b00'111'100u, 0x00, 0x02, // move.w #2,d2 ;
		0b0011'0000u, 0b01'110'100u, 0x20, 0x08,  // movea.w 8(a4,d2),a0 ;
		0x4e, 0x40,                               // trap #0
		0xff, 0xff, 
		0x43, 0x21 };
	verifyExecution(code, sizeof(code), [](const cpu& cpu)
		{
			BOOST_CHECK_EQUAL(0x06, cpu.a4);
			BOOST_CHECK_EQUAL(0x02, cpu.d2);
			BOOST_CHECK_EQUAL(0x4321, cpu.a0);
		});
}

BOOST_AUTO_TEST_CASE(a_addressMode_110_negative)
{
	unsigned char code[] = {
		0x38, 0x7c, 0x00, 0x0A,  // movea.w #10,a4 ;
		0x74, 0xfe,              // moveq.l #-2,d2 ;
		0x30, 0x74, 0x20, 0x08,  // movea.w 8(a4,d2.l),a0 ;
		0x4e, 0x40,              // trap #0
		0xff, 0xff,
		0xff, 0xff,
		0x43, 0x21 };
	verifyExecution(code, sizeof(code), [](const cpu& cpu)
		{
			BOOST_CHECK_EQUAL(10, cpu.a4);
			BOOST_CHECK_EQUAL(0xfffffffe, cpu.d2);
			BOOST_CHECK_EQUAL(0x4321, cpu.a0);
		});
}

BOOST_AUTO_TEST_CASE(a_addressMode_111_000)
{
	unsigned char code[] = {
		0x38, 0x78, 0x00, 0x0A,  // movea.w $10,a4 ;
		0x4e, 0x40,              // trap #0
		0xff, 0xff,
		0xff, 0xff,
		0x43, 0x21 };
	verifyExecution(code, sizeof(code), [](const cpu& cpu)
		{
			BOOST_CHECK_EQUAL(0x4321, cpu.a4);
		});
}

BOOST_AUTO_TEST_CASE(a_addressMode_111_001)
{
	unsigned char code[] = {
		0x38, 0x79, 0x00, 0x03, 0x20, 0x0A,  // movea.w $3200a,a4 ;
		0x4e, 0x40,                          // trap #0
		0xff, 0xff,
		0x43, 0x21 };
	verifyExecution(code, sizeof(code), 0x32000, [](const cpu& cpu)
		{
			BOOST_CHECK_EQUAL(0x4321, cpu.a4);
		});
}

BOOST_AUTO_TEST_CASE(a_addressMode_111_010)
{
	unsigned char code[] = {
		0x32, 0x3A, 0x00, 0x08,  // movea.w data,d1 ;
		0x4e, 0x40,              // trap #0
		0xff, 0xff,
		0xff, 0xff,
		0x43, 0x21 };            // data
	verifyExecution(code, sizeof(code), 0x32000, [](const cpu& cpu)
		{
			BOOST_CHECK_EQUAL(0x4321, cpu.d1);
		});
}

BOOST_AUTO_TEST_CASE(a_addressMode_111_011)
{
	unsigned char code[] = {
		0x74, 0xfe,              // moveq.l #-2,d2 ;

		0x32, 0x3b, 0x20, 0x0A,  // movea.w -2(pc,d2),d1 ;
		0x4e, 0x40,              // trap #0
		0xff, 0xff,
		0xff, 0xff,
		0x43, 0x21 };            // data
	verifyExecution(code, sizeof(code), 0x32000, [](const cpu& cpu)
		{
			BOOST_CHECK_EQUAL(-2, cpu.d2);
			BOOST_CHECK_EQUAL(0x4321, cpu.d1);
		});
}

// =================================================================================================
// Addressing mode tests - Write
// =================================================================================================

BOOST_AUTO_TEST_CASE(a_addressMode_w000)
{
	unsigned char code[] = 
	{ 
		0x70, 0x21,    // moveq #$21, d0
		0x32, 0x00,    // move d0, d1
		0x4e, 0x40 };  // trap #0
	verifyExecution(code, sizeof(code), [](const cpu& cpu)
		{
			BOOST_CHECK_EQUAL(0x21, cpu.d0);
			BOOST_CHECK_EQUAL(0x21, cpu.d1);
		});
}

BOOST_AUTO_TEST_CASE(a_addressMode_w010)
{
	unsigned char code[] =
	{
		0x70, 0x21,             // moveq #$21, d0
		0x32, 0x7c, 0x00, 0x0A, // movea #10, a1
		0x32, 0x80,             // move  d0, (a1)
		0x4e, 0x40,             // trap #0
		0xff, 0xff };
	verifyExecution(code, sizeof(code), [](const cpu& cpu)
		{
			BOOST_CHECK_EQUAL(0x21, cpu.d0);
			BOOST_CHECK_EQUAL(10, cpu.a1);
			BOOST_CHECK_EQUAL(0x21, cpu.mem.get<uint16_t>(10));
		});
}

BOOST_AUTO_TEST_CASE(a_addressMode_w011)
{
	unsigned char code[] =
	{
		0x70, 0x21,             // moveq #$21, d0
		0x32, 0x7c, 0x00, 0x0A, // movea #10, a1
		0x32, 0xC0,             // move  d0, (a1)+
		0x4e, 0x40,             // trap #0
		0xff, 0xff };
	verifyExecution(code, sizeof(code), [](const cpu& cpu)
		{
			BOOST_CHECK_EQUAL(0x21, cpu.d0);
			BOOST_CHECK_EQUAL(12, cpu.a1);
			BOOST_CHECK_EQUAL(0x21, cpu.mem.get<uint16_t>(10));
		});
}

BOOST_AUTO_TEST_CASE(a_addressMode_w100)
{
	unsigned char code[] =
	{
		0x70, 0x21,             // moveq #$21, d0
		0x32, 0x7c, 0x00, 0x0C, // movea #10, a1
		0x33, 0x00,             // move  d0, (a1)+
		0x4e, 0x40,             // trap #0
		0xff, 0xff };
	verifyExecution(code, sizeof(code), [](const cpu& cpu)
		{
			BOOST_CHECK_EQUAL(0x21, cpu.d0);
			BOOST_CHECK_EQUAL(10, cpu.a1);
			BOOST_CHECK_EQUAL(0x21, cpu.mem.get<uint16_t>(10));
		});
}

BOOST_AUTO_TEST_CASE(a_addressMode_w101_p)
{
	unsigned char code[] =
	{
		0x70, 0x21,             // moveq #$21, d0
		0x32, 0x7c, 0x00, 0x06, // movea #6 a1
		0x33, 0x40, 0x00, 0x06, // move  d0, 6(a1)
		0x4e, 0x40,             // trap #0
		0xff, 0xff };
	verifyExecution(code, sizeof(code), [](const cpu& cpu)
		{
			BOOST_CHECK_EQUAL(0x21, cpu.d0);
			BOOST_CHECK_EQUAL(6, cpu.a1);
			BOOST_CHECK_EQUAL(0x21, cpu.mem.get<uint16_t>(12));
		});
}

BOOST_AUTO_TEST_CASE(a_addressMode_w101_n)
{
	unsigned char code[] =
	{
		0x70, 0x21,             // moveq #$21, d0
		0x32, 0x7c, 0x00, 0x10, // movea #16 a1
		0x33, 0x40, 0xff, 0xfc, // move  d0, -4(a1)
		0x4e, 0x40,             // trap #0
		0xff, 0xff };
	verifyExecution(code, sizeof(code), [](const cpu& cpu)
		{
			BOOST_CHECK_EQUAL(0x21, cpu.d0);
			BOOST_CHECK_EQUAL(16, cpu.a1);
			BOOST_CHECK_EQUAL(0x21, cpu.mem.get<uint16_t>(12));
		});
}

BOOST_AUTO_TEST_CASE(a_addressMode_w110_p)
{
	unsigned char code[] =
	{
		0x70, 0x21,             // moveq #$21, d0
		0x74, 0x02,             // moveq #2, d2
		0x32, 0x7c, 0x00, 0x0A, // movea #10 a1
		0x33, 0x80, 0x20, 0x04, // move  d0, 4(a1,d2)
		0x4e, 0x40,             // trap #0
		0xff, 0xff, 0xff, 0xff };
	verifyExecution(code, sizeof(code), [](const cpu& cpu)
		{
			BOOST_CHECK_EQUAL(0x21, cpu.d0);
			BOOST_CHECK_EQUAL(0x2, cpu.d2);
			BOOST_CHECK_EQUAL(10, cpu.a1);
			BOOST_CHECK_EQUAL(0x21, cpu.mem.get<uint16_t>(16));
		});
}

BOOST_AUTO_TEST_CASE(a_addressMode_w110_n)
{
	unsigned char code[] =
	{
		0x70, 0x21,             // moveq #$21, d0
		0x74, 0xFE,             // moveq #-2, d2
		0x32, 0x7c, 0x00, 0x0C, // movea #12 a1
		0x33, 0x80, 0x20, 0x04, // move  d0, 4(a1,d2)
		0x4e, 0x40,             // trap #0
		0xff, 0xff, 0xff, 0xff };
	verifyExecution(code, sizeof(code), [](const cpu& cpu)
		{
			BOOST_CHECK_EQUAL(0x21, cpu.d0);
			BOOST_CHECK_EQUAL(-2, cpu.d2);
			BOOST_CHECK_EQUAL(12, cpu.a1);
			BOOST_CHECK_EQUAL(0x21, cpu.mem.get<uint16_t>(14));
		});
}

BOOST_AUTO_TEST_CASE(a_addressMode_w111_000)
{
	unsigned char code[] = {
		0x70, 0x21,             // moveq #$21, d0
		0x31, 0xC0, 0x00, 0x0a, // move  d0, $10.w
		0x4e, 0x40,             // trap #0
		0xff, 0xff,
		0xff, 0xff,
		0xff, 0xff };
	verifyExecution(code, sizeof(code), [](const cpu& cpu)
		{
			BOOST_CHECK_EQUAL(0x21, cpu.d0);
			BOOST_CHECK_EQUAL(0x21, cpu.mem.get<uint16_t>(10));
		});
}

BOOST_AUTO_TEST_CASE(a_addressMode_w111_001)
{
	unsigned char code[] = {
		0x70, 0x21,                          // moveq #$21, d0
		0x33, 0xC0, 0x00, 0x03, 0x20, 0x0A,  // movea.w d0,$3200a
		0x4e, 0x40,                          // trap #0
		0xff, 0xff,
		0x43, 0x21 };
	verifyExecution(code, sizeof(code), 0x32000, [](const cpu& cpu)
		{
			BOOST_CHECK_EQUAL(0x21, cpu.d0);
			BOOST_CHECK_EQUAL(0x21, cpu.mem.get<uint16_t>(0x3200a));
		});
}
/*=================================================================================================
									org     $00100000       ;Start at 00100000
					strtolower      public
00100000  4E56 0000                     link    a6,#0           ;Set up stack frame
00100004  306E 0008                     movea   8(a6),a0        ;A0 = src, from stack
00100008  326E 000C                     movea   12(a6),a1       ;A1 = dst, from stack
0010000C  1018          loop            move.b  (a0)+,d0        ;Load D0 from (src), incr src
0010000E  0C40 0041                     cmpi    #'A',d0         ;If D0 < 'A',
00100012  6500 000E                     blo     copy            ;skip
00100016  0C40 005A                     cmpi    #'Z',d0         ;If D0 > 'Z',
0010001A  6200 0006                     bhi     copy            ;skip
0010001E  0640 0020                     addi    #'a'-'A',d0     ;D0 = lowercase(D0)
00100022  12C0          copy            move.b  d0,(a1)+        ;Store D0 to (dst), incr dst
00100024  66E6                          bne     loop            ;Repeat while D0 <> NUL
00100026  4E5E                          unlk    a6              ;Restore stack frame
00100028  4E75                          rts                     ;Return
0010002A                                end

=================================================================================================*/
BOOST_AUTO_TEST_CASE(a_toLowerCase)
{
	unsigned char code[] = {     //             org     $00100000      ; Start at 00100000
		0x4E, 0x56, 0x00, 0x00,  //             link    a6,#0          ; Set up stack frame
		0x20, 0x6E, 0x00, 0x08,  //             movea.l   8(a6),a0       ; A0 = src, from stack
		0x22, 0x6E, 0x00, 0x0C,  //             movea.l   12(a6),a1      ; A1 = dst, from stack
		0x10, 0x18,              //  loop       move.b(a0) + ,d0       ; Load D0 from(src), incr src
		0x0C, 0x40, 0x00, 0x41,  //             cmpi    #'A',d0        ; If D0 < 'A',
		0x65, 0x00, 0x00, 0x0E,  //             blo     copy           ; skip
		0x0C, 0x40, 0x00, 0x5A,  //             cmpi    #'Z',d0        ; If D0 > 'Z',
		0x62, 0x00, 0x00, 0x06,  //             bhi     copy           ; skip
		0x06, 0x40, 0x00, 0x20,  //             addi    #'a' - 'A',d0  ; D0 = lowercase(D0)
		0x12, 0xC0,              //  copy       move.b  d0,(a1)+       ; Store D0 to(dst), incr dst
		0x66, 0xE6,              //             bne     loop           ; Repeat while D0 <> NUL
		0x4E, 0x5E,              //             unlk    a6             ; Restore stack frame
		0x4E, 0x75,              //             rts                    ; Return
		0x4e, 0x40,              //             trap #0
		0xff, 0xff,
		'L', 'A', 'U', 'R', 0x00 };


	uint32_t src = 46;
	uint32_t dst = 56;
	uint32_t a6 = 100;
	// Arrange
	BOOST_CHECK_EQUAL('L', code[src]);

	uint32_t base = 0x00100000;
	memory memory(1024, base, code, sizeof(code));
	memory.set<uint32_t>(base + a6 + 8, base + src);
	memory.set<uint32_t>(base + a6 + 12, base + dst);

	cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.setARegister(6, base + a6);
	cpu.start(base);

	// Assert
	// TODO: add missing assert
}


// =================================================================================================
// Instructions specific tests
// =================================================================================================
void verifyAbcdExecution(uint8_t op1, uint8_t op2, uint8_t expected, uint8_t carry, uint8_t zero)
{
	unsigned char code[] = {
	0x70, op1,     // moveq.l #op1,d0
	0x72, op2,     // moveq.l #op2,d1
	0xc3, 0x00,    // abcd d0,d1
	0x4e, 0x40,    // trap #0
	0xff, 0xff };

	// Arrange
	memory memory(256, 0, code, sizeof(code));
	cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0);

	// Assert
	BOOST_CHECK_EQUAL(op1, cpu.d0);
	BOOST_CHECK_EQUAL(expected, cpu.d1);
	BOOST_CHECK_EQUAL(carry, cpu.sr.c);
	BOOST_CHECK_EQUAL(zero, cpu.sr.z);
}

void verifyAbcdExecutionMemory(uint8_t op1, uint8_t op2, uint8_t expected, uint8_t carry, uint8_t zero)
{
	unsigned char code[] = {
		0x32, 0x7c, 0x00,0x11,  // move op1+1, A1
		0x34, 0x7c, 0x00,0x12,  // move op2+1, A2
		0xc5, 0x09,             // abcd -(A1), -(A2)
		0x4e, 0x40,             // trap #0
		0xff, 0xff,
		0xff, 0xff,
		op1,
		op2
	};

	// Arrange
	memory memory(256, 0, code, sizeof(code));
	cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0);

	// Assert
	BOOST_CHECK_EQUAL(0x10, cpu.a1);
	BOOST_CHECK_EQUAL(0x11, cpu.a2);
	BOOST_CHECK_EQUAL(expected, cpu.mem.get<uint8_t>(0x11));
	BOOST_CHECK_EQUAL(carry, cpu.sr.c);
	BOOST_CHECK_EQUAL(zero, cpu.sr.z);
}

BOOST_AUTO_TEST_CASE(a_abcd_register)
{
	verifyAbcdExecution(0x42, 0x31, 0x73, 0, 0);
	verifyAbcdExecution(0x48, 0x34, 0x82, 0, 0);
	verifyAbcdExecution(0x48, 0x62, 0x10, 1, 0);
}

BOOST_AUTO_TEST_CASE(a_abcd_overflow)
{
	unsigned char code[] = {
	0x30, 0x3c, 0x00, 0x48,     // move #$48,d0
	0x32, 0x3c, 0xff, 0x62,     // move #$ff62,d1
	0xc3, 0x00,                 // abcd d0,d1
	0x4e, 0x40,                 // trap #0
	0xff, 0xff };

	// Arrange
	memory memory(256, 0, code, sizeof(code));
	cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0);

	// Assert
	BOOST_CHECK_EQUAL(0x48, cpu.d0);
	BOOST_CHECK_EQUAL(0xff10, cpu.d1);
	BOOST_CHECK_EQUAL(1, cpu.sr.c);
	BOOST_CHECK_EQUAL(0, cpu.sr.z);
}

BOOST_AUTO_TEST_CASE(a_abcd_memory)
{
	verifyAbcdExecutionMemory(0x42, 0x31, 0x73, 0, 0);
	verifyAbcdExecutionMemory(0x48, 0x34, 0x82, 0, 0);
	verifyAbcdExecutionMemory(0x48, 0x62, 0x10, 1, 0);
}

BOOST_AUTO_TEST_CASE(a_adda_1)
{
	unsigned char code[] = {
	0x30, 0x7c, 0x00, 0x64,     // move #100,a0
	0xd0, 0xfc, 0x00, 0x20,     // add  #32,a0
	0x32, 0x7c, 0x00, 0xc8,     // move #200,a1
	0xd2, 0xfc, 0xff, 0xe0,     // add  #-32,a1
	0x4e, 0x40,                 // trap #0
	0xff, 0xff };

	// Arrange
	memory memory(256, 0, code, sizeof(code));
	cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0);

	// Assert
	BOOST_CHECK_EQUAL(132, cpu.a0);
	BOOST_CHECK_EQUAL(168, cpu.a1);
}

BOOST_AUTO_TEST_CASE(a_adda_2)
{
	unsigned char code[] = {
	0x30, 0x7c, 0x01, 0x00,             // move   #$100,a0
	0xd1, 0xfc, 0x12, 0x34, 0x12, 0x34, // add.l  #$12341234,a0
	0x32, 0x7c, 0x02, 0x00,		        // move   #$200,a1
	0x22, 0x3c, 0x12, 0x34, 0x00, 0x00, // move.l #$1234000,d1
	0xd3, 0xc1,                         // add.l  d1,a1
	0x4e, 0x40,                         // trap   #0
	0xff, 0xff };

	// Arrange
	memory memory(256, 0, code, sizeof(code));
	cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0);

	// Assert
	BOOST_CHECK_EQUAL(0x12341334, cpu.a0);
	BOOST_CHECK_EQUAL(0x12340200, cpu.a1);
}
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

	verifyExecution(code, sizeof(code), [](const cpu& cpu)
		{
			BOOST_CHECK_EQUAL(42, cpu.d0);
		});
}
BOOST_AUTO_TEST_CASE(a_bcc)
{
	verifyBccExecution(0x24, 0x64); // C=0
}

BOOST_AUTO_TEST_CASE(a_bcs)
{
	verifyBccExecution(0x19, 0x65); // C=1
}

BOOST_AUTO_TEST_CASE(a_beq)
{
	verifyBccExecution(0x04, 0x67); // Z=1
}

BOOST_AUTO_TEST_CASE(a_bne)
{
	verifyBccExecution(0x08, 0x66); // Z=0
}

BOOST_AUTO_TEST_CASE(a_bge)
{
	verifyBccExecution(0x02, 0x6c); // N=0 V=1
}

BOOST_AUTO_TEST_CASE(a_bgt)
{
	verifyBccExecution(0x08, 0x6e); // N=1 Z=0 V=0
}

BOOST_AUTO_TEST_CASE(a_bhi)
{
	verifyBccExecution(0x18, 0x62); // C=0 Z=0
}

BOOST_AUTO_TEST_CASE(a_ble)
{
	verifyBccExecution(0x0a, 0x6f);  // N=1 Z=0 V=1
}

BOOST_AUTO_TEST_CASE(a_bls)
{
	verifyBccExecution(0x01, 0x63); // Z=0 C=1
}

BOOST_AUTO_TEST_CASE(a_blt)
{
	verifyBccExecution(0x0b, 0x6d); // N=1 V=1
}

BOOST_AUTO_TEST_CASE(a_bmi)
{
	verifyBccExecution(0x08, 0x6b); // N=1
}

BOOST_AUTO_TEST_CASE(a_bpl)
{
	verifyBccExecution(0x07, 0x6a); // N=0
}

BOOST_AUTO_TEST_CASE(a_bvc)
{
	verifyBccExecution(0x05, 0x68); // V=0
}

BOOST_AUTO_TEST_CASE(a_bvs)
{
	verifyBccExecution(0x07, 0x69); // V=1
}


BOOST_AUTO_TEST_CASE(a_moveq)
{
	unsigned char code[] = { 
		0x74, 0xfe,    // moveq.l #-2,d2
		0x76, 0x04,    // moveq.l #4,d3
		0x4e, 0x40,    // trap #0
		0xff, 0xff};
	verifyExecution(code, sizeof(code), [](const cpu& cpu)
		{
			BOOST_CHECK_EQUAL(-2, cpu.d2);
			BOOST_CHECK_EQUAL(4, cpu.d3);
		});
}

BOOST_AUTO_TEST_CASE(a_move2ccr_1)
{
	unsigned char code[] = {
		0x44, 0xfc, 0x00, 0x15,    // move #21, CCR
		0x4e, 0x40,                // trap #0
		0xff, 0xff };

	verifyExecution(code, sizeof(code), [](const cpu& cpu)
		{
			BOOST_CHECK_EQUAL(1, cpu.sr.x);
			BOOST_CHECK_EQUAL(0, cpu.sr.n);
			BOOST_CHECK_EQUAL(1, cpu.sr.z);
			BOOST_CHECK_EQUAL(0, cpu.sr.v);
			BOOST_CHECK_EQUAL(1, cpu.sr.c);
		});
}

BOOST_AUTO_TEST_CASE(a_move2ccr_2)
{
	unsigned char code[] = {
		0x44, 0xfc, 0x00, 0x0a,    // move #12, CCR
		0x4e, 0x40,                // trap #0
		0xff, 0xff };

	verifyExecution(code, sizeof(code), [](const cpu& cpu)
		{
			BOOST_CHECK_EQUAL(0, cpu.sr.x);
			BOOST_CHECK_EQUAL(1, cpu.sr.n);
			BOOST_CHECK_EQUAL(0, cpu.sr.z);
			BOOST_CHECK_EQUAL(1, cpu.sr.v);
			BOOST_CHECK_EQUAL(0, cpu.sr.c);
		});
}
BOOST_AUTO_TEST_SUITE_END()