#include <boost/test/unit_test.hpp>
#include "../core/disasm.h"

using namespace mc68000;

BOOST_AUTO_TEST_SUITE(Asm)

BOOST_AUTO_TEST_CASE(toto)
{
    BOOST_CHECK_EQUAL(1, 1);

}

void verifyDisassembly(unsigned short memory, const std::string& expected)
{
    // Arrange
    DisAsm d;

    // Act
    std::string actual = d.disassemble(&memory);

    // Assert
    BOOST_CHECK_EQUAL(expected, actual);
}

void verifyDisassembly(unsigned short memory1, unsigned short memory2, const std::string& expected)
{
    // Arrange
    DisAsm d;
    unsigned short memory[2] = { memory1, memory2 };

    // Act
    std::string actual = d.disassemble(memory);

    // Assert
    BOOST_CHECK_EQUAL(expected, actual);
}

void verifyDisassembly(unsigned short memory1, unsigned short memory2, unsigned short memory3, const std::string& expected)
{
    // Arrange
    DisAsm d;
    unsigned short memory[3] = { memory1, memory2, memory3 };

    // Act
    std::string actual = d.disassemble(memory);

    // Assert
    BOOST_CHECK_EQUAL(expected, actual);
}

// =================================================================================================
// Addressing mode tests
// =================================================================================================

BOOST_AUTO_TEST_CASE(d_addressMode_000)
{
    verifyDisassembly(0x3040, "movea.w d0,a0");
}
BOOST_AUTO_TEST_CASE(d_addressMode_001)
{
    verifyDisassembly(0x3049, "movea.w a1,a0");
}
BOOST_AUTO_TEST_CASE(d_addressMode_010)
{
    verifyDisassembly(0x3051, "movea.w (a1),a0");
}
BOOST_AUTO_TEST_CASE(d_addressMode_011)
{
    verifyDisassembly(0x3059, "movea.w (a1)+,a0");
}
BOOST_AUTO_TEST_CASE(d_addressMode_100)
{
    verifyDisassembly(0x3061, "movea.w -(a1),a0");
}
BOOST_AUTO_TEST_CASE(d_addressMode_101)
{
    verifyDisassembly(0x3069, 0x0007, "movea.w 7(a1),a0");
}
BOOST_AUTO_TEST_CASE(d_addressMode_110)
{
    verifyDisassembly(0x3071, 0x0007, "movea.w 7(a1,d0),a0");
}

BOOST_AUTO_TEST_CASE(d_addressMode_111_0)
{
    verifyDisassembly(0x3078, 0x007B, "movea.w $7b.w,a0");
}

BOOST_AUTO_TEST_CASE(d_addressMode_111_1)
{
    verifyDisassembly(0x3079, 0x1234, 0x5678, "movea.w $12345678.l,a0");
}

BOOST_AUTO_TEST_CASE(d_addressMode_111_2)
{
    verifyDisassembly(0x307A, 0x007B, "movea.w 123(pc),a0");
}

BOOST_AUTO_TEST_CASE(d_addressMode_111_3)
{
    verifyDisassembly(0x307B, 0x007B, "movea.w 123(pc,d0),a0");
}

BOOST_AUTO_TEST_CASE(d_addressMode_111_4w)
{
    verifyDisassembly(0x307C, 0x002A, "movea.w #$2a.w,a0");
}

BOOST_AUTO_TEST_CASE(d_addressMode_111_4l)
{
    verifyDisassembly(0x207C, 0x1234, 0x5678, "movea.l #$12345678.l,a0");
}

// =================================================================================================
// Instructions specific tests
// =================================================================================================
BOOST_AUTO_TEST_CASE(d_abcd)
{
    verifyDisassembly(0b1100'010'10000'0'101u, "abcd d5,d2");
    verifyDisassembly(0b1100'010'10000'1'101u, "abcd -(a5),-(a2)");
}

BOOST_AUTO_TEST_CASE(d_add)
{
    verifyDisassembly(0b1101'010'0'00'010'010u, "add.b (a2),d2");
    verifyDisassembly(0b1101'010'1'10'010'010u, "add.l d2,(a2)");
}

BOOST_AUTO_TEST_CASE(d_movea_l)
{
    verifyDisassembly(0x206e, 0x0008, "movea.l 8(a6),a0");
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
BOOST_AUTO_TEST_CASE(d_tolower01)
{
    verifyDisassembly(0x4e56, 0x0000, "link a6,#0");
}

BOOST_AUTO_TEST_CASE(d_tolower02)
{
    verifyDisassembly(0x306e, 0x0008, "movea.w 8(a6),a0");
}

BOOST_AUTO_TEST_CASE(d_tolower03)
{
    verifyDisassembly(0x326e, 0x000c, "movea.w 12(a6),a1");
}

BOOST_AUTO_TEST_CASE(d_tolower04)
{
    verifyDisassembly(0x1018, 0x0008, "move.b (a0)+,d0");
}
BOOST_AUTO_TEST_CASE(d_tolower05)
{
    verifyDisassembly(0x0c40, 0x0041, "cmpi.w #$41,d0");
}
BOOST_AUTO_TEST_CASE(d_tolower06)
{
    verifyDisassembly(0x6500, 0x000E, "bcs $e");
}

BOOST_AUTO_TEST_CASE(d_tolower07)
{
    verifyDisassembly(0x0C40, 0x005A, "cmpi.w #$5a,d0");
}

BOOST_AUTO_TEST_CASE(d_tolower08)
{
verifyDisassembly(0x6200, 0x0006, "bhi $6");
}

BOOST_AUTO_TEST_CASE(d_tolower09)
{
    verifyDisassembly(0x0640, 0x0020, "addi.w #$20,d0");
}

BOOST_AUTO_TEST_CASE(d_tolower10)
{
    verifyDisassembly(0x12C0, "move.b d0,(a1)+");
}

BOOST_AUTO_TEST_CASE(d_tolower11)
{
    verifyDisassembly(0x66E6, "bne $e6");
}

BOOST_AUTO_TEST_CASE(d_tolower12)
{
    verifyDisassembly(0x4E5E, "unlk a6");
}

BOOST_AUTO_TEST_CASE(d_tolower13)
{
    verifyDisassembly(0x4E75, "rts");
}



BOOST_AUTO_TEST_SUITE_END()
