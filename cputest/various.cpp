#include <boost/test/unit_test.hpp>
#include "../core/cpu.h"
#include "../core/memory.h"
#include "verifyexecution.h"

using namespace mc68000;


BOOST_AUTO_TEST_SUITE(various)

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
	unsigned char code[] = {     //             org     $1000          ; Start at 001000
		0x4E, 0x56, 0x00, 0x00,  //             link    a6,#0          ; Set up stack frame
		0x30, 0x7c, 0x10, 0x2e,  //             movea   #src,a0        ; A0 = src
		0x32, 0x7c, 0x10, 0x36,  //             movea   #dst,a1        ; A1 = dst
		0x10, 0x18,              //  loop       move.b(a0) + ,d0       ; Load D0 from(src), incr src
		0x0C, 0x40, 0x00, 0x41,  //             cmpi    #'A',d0        ; If D0 < 'A',
		0x65, 0x00, 0x00, 0x0E,  //             blo     copy           ; skip
		0x0C, 0x40, 0x00, 0x5A,  //             cmpi    #'Z',d0        ; If D0 > 'Z',
		0x62, 0x00, 0x00, 0x06,  //             bhi     copy           ; skip
		0x06, 0x40, 0x00, 0x20,  //             addi    #'a' - 'A',d0  ; D0 = lowercase(D0)
		0x12, 0xC0,              //  copy       move.b  d0,(a1)+       ; Store D0 to(dst), incr dst
		0x66, 0xE6,              //             bne     loop           ; Repeat while D0 <> NUL
		0x4E, 0x5E,              //             unlk    a6             ; Restore stack frame
		0x4E, 0x71,              //             nop                    ; Do nothing, there is nothing on the stack
		0x4e, 0x40,              //             trap #0
		0xff, 0xff,
		'L', 'A', 'U', 'R', 0x00 };


	uint32_t src = 46;
	uint32_t dst = 54;
	uint32_t a6 = 100;
	// Arrange
	BOOST_CHECK_EQUAL('L', code[src]);

	uint32_t base = 0x001000;
	memory memory(1024, base, code, sizeof(code));
	memory.set<uint32_t>(base + a6 + 8, base + src);
	memory.set<uint32_t>(base + a6 + 12, base + dst);

	Cpu cpu(memory);

	cpu.reset();
	cpu.setARegister(6, base + a6);

	// Act
	cpu.start(base, base + 1024);

	// Assert
	auto result = cpu.mem.get<uint8_t>(base + dst);
	BOOST_CHECK_EQUAL('l', result);
}


// =================================================================================================
// Instructions specific tests
// =================================================================================================

BOOST_AUTO_TEST_CASE(clr)
{
	unsigned char code[] = {
		0x20, 0x3c, 0x12, 0x34, 0x56, 0x78,    // move.l #$12345678,d0
		0x42, 0x00,                            // clr.b  d0
		0x22, 0x3c, 0x23, 0x45, 0x67, 0x89,    // move.l #$23456789,d1
		0x42, 0x41,                            // clr.w  d1
		0x24, 0x3c, 0x34, 0x56, 0x78, 0x9A,    // move.l #$3456789A,d2
		0x42, 0x82,                            // clr.l  d2
		0x4e, 0x40,                            // trap #0
		0xff, 0xff };

	// Arrange
	memory memory(256, 0, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0);

	// Assert
	BOOST_CHECK_EQUAL(0x12345600, cpu.d0);
	BOOST_CHECK_EQUAL(0x23450000, cpu.d1);
	BOOST_CHECK_EQUAL(0x00000000, cpu.d2);
	BOOST_CHECK_EQUAL(0, cpu.sr.c);
	BOOST_CHECK_EQUAL(1, cpu.sr.z);
	BOOST_CHECK_EQUAL(0, cpu.sr.n);
	BOOST_CHECK_EQUAL(0, cpu.sr.v);

}

// ===================================================
// EXG tests
// ===================================================
BOOST_AUTO_TEST_CASE(exg)
{
	unsigned char code[] = {
	0x32, 0x3c, 0x03, 0xe8,     // move.l #1000,d1
	0x34, 0x3c, 0x07, 0xd0,     // move.l #2000,d2
	0x32, 0x7c, 0x03, 0xf2,     // move.l #1010,a1
	0x34, 0x7c, 0x07, 0xe4,     // move.l #2020,a2

	0x26, 0x3c, 0x00, 0x05, 0x16, 0x15,    // move.l #$333333,d3
	0x28, 0x7c, 0x00, 0x06, 0xc8, 0x1c,    // move.l #$444444,a4

	0xc3, 0x42,                 // exg d1,d2
	0xc3, 0x4a,                 // exg a1,a2
	0xc7, 0x8c,                 // exg d3,a4
	0x4e, 0x40,                 // trap #0
	0xff, 0xff };

	// Arrange
	memory memory(256, 0, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0);

	// Assert
	BOOST_CHECK_EQUAL(1000, cpu.d2);
	BOOST_CHECK_EQUAL(2000, cpu.d1);

	BOOST_CHECK_EQUAL(1010, cpu.a2);
	BOOST_CHECK_EQUAL(2020, cpu.a1);

	BOOST_CHECK_EQUAL(444444, cpu.d3);
	BOOST_CHECK_EQUAL(333333, cpu.a4);
}

// ===================================================
// EXT tests
// ===================================================
BOOST_AUTO_TEST_CASE(ext)
{
	unsigned char code[] = {
		0x30, 0x3C, 0x12, 0x34,             // move #$1234, d0
		0x48, 0x80,                         // ext d0
		0x32, 0x3C, 0x56, 0xFE,             // move #$56fe, d1
		0x48, 0x81,                         // ext d1

		0x24, 0x3C, 0x12, 0x34, 0x56, 0x78, // move.l #$12345678, d2
		0x48, 0xC2,                         // ext.l d2
		0x26, 0x3C, 0x34, 0x56, 0xFE, 0x23, // move.l #$3456fe23, d3
		0x48, 0xC3,							// ext.l d3
		0x4e, 0x40,                         // trap #0
		0xff, 0xff
	};

	// Arrange
	memory memory(256, 0, code, sizeof(code));
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0);

	// Assert
	BOOST_CHECK_EQUAL(0x34, cpu.d0);
	BOOST_CHECK_EQUAL(0xfffe, cpu.d1);

	BOOST_CHECK_EQUAL(0x5678, cpu.d2);
	BOOST_CHECK_EQUAL(0xfffffe23, cpu.d3);
}
// ===================================================
// LEA tests
// ===================================================
BOOST_AUTO_TEST_CASE(lea)
{
	unsigned char code[] = {
		0x41, 0xf9, 0x00, 0x00, 0x10, 0x24,    // lea data, a0
		0x43, 0xf8, 0x10, 0x24,                // lea data.w, a1
		0x45, 0xd1,                            // lea (a1), a2
		0x47, 0xea, 0x00, 0x04,                // lea 4(a2), a3
		0x76, 0x08,                            // moveq #8, d3
		0x49, 0xf2, 0x30, 0x04,                // lea 4(a2,d3), a4
		0x4b, 0xfa, 0x00, 0x0c,                // lea data(pc), a5
		0x4d, 0xfb, 0x30, 0x08,                // lea data(pc,d3), a6
		0x4e, 0x40,                            // trap #0
		0xff, 0xff, 0xff, 0xff,
		0x10, 0x00 };                          // data dc.b $1000    

	verifyExecution(code, sizeof(code), 0x1000, [](const Cpu& cpu)
		{
			BOOST_CHECK_EQUAL(0x1024, cpu.a0);
			BOOST_CHECK_EQUAL(0x1024, cpu.a1);
			BOOST_CHECK_EQUAL(0x1024, cpu.a2);
			BOOST_CHECK_EQUAL(0x1028, cpu.a3);
			BOOST_CHECK_EQUAL(0x1030, cpu.a4);
			BOOST_CHECK_EQUAL(0x1024, cpu.a5);
			BOOST_CHECK_EQUAL(0x102C, cpu.a6);
		});
}

// ===================================================
// LINK tests
// ===================================================
BOOST_AUTO_TEST_CASE(link)
{
	unsigned char code[] = {
		0x4f, 0xf9, 0x00, 0x00, 0x10, 0x44, // lea stack,a7
		0x3c, 0x7c, 0x12, 0x34,             // move #$1234, a6
		0x3a, 0x7c, 0x56, 0x78,             // move #$5678, a5
		0x38, 0x7c, 0x9a, 0xbc,             // move #$9abc, a4

		0x4e, 0x56, 0x00, 0x00,             // link a6,#0
		0x4e, 0x55, 0xff, 0xf8,             // link a5,#-8
		0x4e, 0x54, 0x00, 0x08,             // link a4,#8

		0x4e, 0x40,                         // trap #0
		0xff, 0xff, 0xff, 0xff,
		0xee, 0xee, 0xee, 0xee,             // ds.w 16
	};                                      // stack:

	verifyExecution(code, sizeof(code), 0x1000, [](const Cpu& cpu)
		{
			BOOST_CHECK_EQUAL(0x1030, cpu.a4);
			BOOST_CHECK_EQUAL(0x103c, cpu.a5);
			BOOST_CHECK_EQUAL(0x1040, cpu.a6);
			BOOST_CHECK_EQUAL(0x1038, cpu.a7);
			BOOST_CHECK_EQUAL(0x1234, cpu.mem.get<uint32_t>(cpu.a6));
			BOOST_CHECK_EQUAL(0x5678, cpu.mem.get<uint32_t>(cpu.a5));
			BOOST_CHECK_EQUAL(0xffff9abc, cpu.mem.get<uint32_t>(cpu.a4));
		});
}

// ===================================================
// NOP tests
// ===================================================
BOOST_AUTO_TEST_CASE(nop)
{
	unsigned char code[] = {
		0x4e, 0x71, // nop
		0x4e, 0x40, // trap #0
		0xff, 0xff
	};

	verifyExecution(code, sizeof(code), [](const Cpu& cpu)
		{
			BOOST_CHECK_EQUAL(0, cpu.sr.c);
			BOOST_CHECK_EQUAL(0, cpu.sr.z);
			BOOST_CHECK_EQUAL(0, cpu.sr.n);
			BOOST_CHECK_EQUAL(0, cpu.sr.v);
		});
}

// ===================================================
// STOP
// ===================================================
BOOST_AUTO_TEST_CASE(stop)
{
	unsigned char code[] = {
		0x4e, 0x72, 0x00, 0xff, // stop #$ff
		0x4e, 0x40,             // trap #0
		0xff, 0xff
	};

	verifyExecution(code, sizeof(code), [](const Cpu& cpu)
		{
			BOOST_CHECK_EQUAL(0, cpu.sr.c);
			BOOST_CHECK_EQUAL(0, cpu.sr.z);
			BOOST_CHECK_EQUAL(0, cpu.sr.n);
			BOOST_CHECK_EQUAL(0, cpu.sr.v);
		});
}

BOOST_AUTO_TEST_SUITE_END()