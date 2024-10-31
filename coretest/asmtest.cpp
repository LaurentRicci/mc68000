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

BOOST_AUTO_TEST_CASE(d_adda)
{
    verifyDisassembly(0b1101'110'0'11'010'010u, "adda.w (a2),a6");
    verifyDisassembly(0b1101'110'1'11'000'010u, "adda.l d2,a6");
}

BOOST_AUTO_TEST_CASE(d_addi)
{
    verifyDisassembly(0b0000'0110'00'000'010u, 0x42, "addi.b #$42,d2");
    verifyDisassembly(0b0000'0110'01'011'010u, 0x1234, "addi.w #$1234,(a2)+");
    verifyDisassembly(0b0000'0110'10'100'110u, 0xabcd, 0x1234, "addi.l #$abcd1234,-(a6)");
}

BOOST_AUTO_TEST_CASE(d_addq)
{
    verifyDisassembly(0b0101'110'0'00'010'010u, "addq.b #6,(a2)");
    verifyDisassembly(0b0101'011'0'01'000'010u, "addq.w #3,d2");
    verifyDisassembly(0b0101'101'0'10'011'010u, "addq.l #5,(a2)+");
}

BOOST_AUTO_TEST_CASE(d_addx)
{
    verifyDisassembly(0b1101'110'1'00'00'0'010u, "addx.b d2,d6");
    verifyDisassembly(0b1101'011'1'01'00'1'010u, "addx.w -(a2),-(a3)");
    verifyDisassembly(0b1101'101'1'10'00'0'010u, "addx.l d2,d5");
}

BOOST_AUTO_TEST_CASE(d_and)
{
    verifyDisassembly(0b1100'011'000'000'010u, 0x42, "and.b d2,d3");
    verifyDisassembly(0b1100'011'001'010'010u, 0x42, "and.w (a2),d3");
    verifyDisassembly(0b1100'011'101'011'010u, 0x42, "and.w d3,(a2)+");
    verifyDisassembly(0b1100'011'110'010'010u, 0x42, "and.l d3,(a2)");
}

BOOST_AUTO_TEST_CASE(d_andi)
{
    verifyDisassembly(0b0000'0010'00'000'010u, 0x42, "andi.b #$42,d2");
    verifyDisassembly(0b0000'0010'01'011'010u, 0x1234, "andi.w #$1234,(a2)+");
    verifyDisassembly(0b0000'0010'10'100'110u, 0xabcd, 0x1234, "andi.l #$abcd1234,-(a6)");
}

BOOST_AUTO_TEST_CASE(d_bcc)
{
    verifyDisassembly(0b0110'0100'0000'0000, 0x3456, "bcc $3456");
    verifyDisassembly(0b0110'0101'0000'1000, "bcs $8");
    verifyDisassembly(0b0110'0111'0000'1000, "beq $8");
    verifyDisassembly(0b0110'0110'0000'1000, "bne $8");
    verifyDisassembly(0b0110'1100'0000'1000, "bge $8");
    verifyDisassembly(0b0110'1110'0000'1000, "bgt $8");
    verifyDisassembly(0b0110'0010'0000'1000, "bhi $8");
    verifyDisassembly(0b0110'1111'0000'1000, "ble $8");
    verifyDisassembly(0b0110'0011'0000'1000, "bls $8");
    verifyDisassembly(0b0110'1101'0000'1000, "blt $8");
    verifyDisassembly(0b0110'1011'0000'1000, "bmi $8");
    verifyDisassembly(0b0110'1010'0000'1000, "bpl $8");
    verifyDisassembly(0b0110'1000'0000'1000, "bvc $8");
    verifyDisassembly(0b0110'1001'0000'1000, "bvs $8");
//    verifyDisassembly(0b0110'0000'0000'1000, "bt $8");
//    verifyDisassembly(0b0110'0001'0000'1000, "bf $8");
}

BOOST_AUTO_TEST_CASE(d_bchg_r)
{
    verifyDisassembly(0x0141, "bchg d0,d1");
}

BOOST_AUTO_TEST_CASE(d_bchg_i)
{
    verifyDisassembly(0x0853, 0x000c, "bchg #$c,(a3)");
}

BOOST_AUTO_TEST_CASE(d_movea_l)
{
    verifyDisassembly(0x206e, 0x0008, "movea.l 8(a6),a0");
}

BOOST_AUTO_TEST_CASE(d_subx)
{
    verifyDisassembly(0b1001'110'1'00'00'0'010u, "subx.b d2,d6");
    verifyDisassembly(0b1001'011'1'01'00'1'010u, "subx.w -(a2),-(a3)");
    verifyDisassembly(0b1001'101'1'10'00'0'010u, "subx.l d2,d5");
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
