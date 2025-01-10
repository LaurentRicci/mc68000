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

BOOST_AUTO_TEST_CASE(addressMode_000)
{
    verifyDisassembly(0x3040, "movea.w d0,a0");
}
BOOST_AUTO_TEST_CASE(addressMode_001)
{
    verifyDisassembly(0x3049, "movea.w a1,a0");
}
BOOST_AUTO_TEST_CASE(addressMode_010)
{
    verifyDisassembly(0x3051, "movea.w (a1),a0");
}
BOOST_AUTO_TEST_CASE(addressMode_011)
{
    verifyDisassembly(0x3059, "movea.w (a1)+,a0");
}
BOOST_AUTO_TEST_CASE(addressMode_100)
{
    verifyDisassembly(0x3061, "movea.w -(a1),a0");
}
BOOST_AUTO_TEST_CASE(addressMode_101)
{
    verifyDisassembly(0x3069, 0x0007, "movea.w 7(a1),a0");
}
BOOST_AUTO_TEST_CASE(addressMode_110)
{
    verifyDisassembly(0x3071, 0x0007, "movea.w 7(a1,d0),a0");
}

BOOST_AUTO_TEST_CASE(addressMode_111_0)
{
    verifyDisassembly(0x3078, 0x007B, "movea.w $7b.w,a0");
}

BOOST_AUTO_TEST_CASE(addressMode_111_1)
{
    verifyDisassembly(0x3079, 0x1234, 0x5678, "movea.w $12345678.l,a0");
}

BOOST_AUTO_TEST_CASE(addressMode_111_2)
{
    verifyDisassembly(0x307A, 0x007B, "movea.w offset_0x7b(pc),a0");
}

BOOST_AUTO_TEST_CASE(addressMode_111_3)
{
    verifyDisassembly(0x307B, 0x007B, "movea.w offset_0x7b(pc,d0),a0");
}

BOOST_AUTO_TEST_CASE(addressMode_111_4w)
{
    verifyDisassembly(0x307C, 0x002A, "movea.w #$2a,a0");
}

BOOST_AUTO_TEST_CASE(addressMode_111_4l)
{
    verifyDisassembly(0x207C, 0x1234, 0x5678, "movea.l #$12345678,a0");
}

// =================================================================================================
// Instructions specific tests
// =================================================================================================
BOOST_AUTO_TEST_CASE(abcd)
{
    verifyDisassembly(0b1100'010'10000'0'101u, "abcd d5,d2");
    verifyDisassembly(0b1100'010'10000'1'101u, "abcd -(a5),-(a2)");
}

BOOST_AUTO_TEST_CASE(add)
{
    verifyDisassembly(0b1101'010'0'00'010'010u, "add.b (a2),d2");
    verifyDisassembly(0b1101'010'1'10'010'010u, "add.l d2,(a2)");
}

BOOST_AUTO_TEST_CASE(adda)
{
    verifyDisassembly(0b1101'110'0'11'010'010u, "adda.w (a2),a6");
    verifyDisassembly(0b1101'110'1'11'000'010u, "adda.l d2,a6");
    verifyDisassembly(0xd2fc, 0x1234, "adda.w #$1234,a1");
    verifyDisassembly(0xd5fc, 0x0045, 0x6789, "adda.l #$456789,a2");
}

BOOST_AUTO_TEST_CASE(addi)
{
    verifyDisassembly(0b0000'0110'00'000'010u, 0x42, "addi.b #$42,d2");
    verifyDisassembly(0b0000'0110'01'011'010u, 0x1234, "addi.w #$1234,(a2)+");
    verifyDisassembly(0b0000'0110'10'100'110u, 0xabcd, 0x1234, "addi.l #$abcd1234,-(a6)");
}

BOOST_AUTO_TEST_CASE(addq)
{
    verifyDisassembly(0b0101'110'0'00'010'010u, "addq.b #6,(a2)");
    verifyDisassembly(0b0101'011'0'01'000'010u, "addq.w #3,d2");
    verifyDisassembly(0b0101'101'0'10'011'010u, "addq.l #5,(a2)+");
}

BOOST_AUTO_TEST_CASE(addx)
{
    verifyDisassembly(0b1101'110'1'00'00'0'010u, "addx.b d2,d6");
    verifyDisassembly(0b1101'011'1'01'00'1'010u, "addx.w -(a2),-(a3)");
    verifyDisassembly(0b1101'101'1'10'00'0'010u, "addx.l d2,d5");
}

BOOST_AUTO_TEST_CASE(and_)
{
    verifyDisassembly(0b1100'011'000'000'010u, 0x42, "and.b d2,d3");
    verifyDisassembly(0b1100'011'001'010'010u, 0x42, "and.w (a2),d3");
    verifyDisassembly(0b1100'011'101'011'010u, 0x42, "and.w d3,(a2)+");
    verifyDisassembly(0b1100'011'110'010'010u, 0x42, "and.l d3,(a2)");
	verifyDisassembly(0xC2BC, 0x005F, 0xFFFF, "and.l #$5fffff,d1");
}

BOOST_AUTO_TEST_CASE(andi)
{
    verifyDisassembly(0b0000'0010'00'000'010u, 0x42, "andi.b #$42,d2");
    verifyDisassembly(0b0000'0010'01'011'010u, 0x1234, "andi.w #$1234,(a2)+");
    verifyDisassembly(0b0000'0010'10'100'110u, 0xabcd, 0x1234, "andi.l #$abcd1234,-(a6)");
}

BOOST_AUTO_TEST_CASE(andi2ccr)
{
	verifyDisassembly(0x023c, 0x0015, "andi #$15,ccr");
    verifyDisassembly(0x023c, 0x0010, "andi #$10,ccr");
}

BOOST_AUTO_TEST_CASE(andi2sr)
{
    verifyDisassembly(0x027c, 0x0015, "andi #$15,sr");
    verifyDisassembly(0x027c, 0x0010, "andi #$10,sr");
}

BOOST_AUTO_TEST_CASE(asl_memory)
{
    verifyDisassembly(0xe1d3, "asl (a3)");
}

BOOST_AUTO_TEST_CASE(asl_b)
{
    verifyDisassembly(0xe300, "asl.b #1,d0");
}

BOOST_AUTO_TEST_CASE(asl_w)
{
    verifyDisassembly(0xe140, "asl.w #8,d0");
}

BOOST_AUTO_TEST_CASE(asl_l)
{
    verifyDisassembly(0xe3a0, "asl.l d1,d0");
}

BOOST_AUTO_TEST_CASE(asr_memory)
{
    verifyDisassembly(0xe0d3, "asr (a3)");
}

BOOST_AUTO_TEST_CASE(asr_b)
{
    verifyDisassembly(0xe200, "asr.b #1,d0");
}

BOOST_AUTO_TEST_CASE(asr_w)
{
    verifyDisassembly(0xe640, "asr.w #3,d0");
}

BOOST_AUTO_TEST_CASE(asr_l)
{
    verifyDisassembly(0xe2a0, "asr.l d1,d0");
}

BOOST_AUTO_TEST_CASE(bcc)
{
    verifyDisassembly(0b0110'0100'0000'0000, 0x3456, "bcc $3458");
    verifyDisassembly(0b0110'0101'0000'1000, "bcs $a");
    verifyDisassembly(0b0110'0111'0000'1000, "beq $a");
    verifyDisassembly(0b0110'0110'0000'1000, "bne $a");
    verifyDisassembly(0b0110'1100'0000'1000, "bge $a");
    verifyDisassembly(0b0110'1110'0000'1000, "bgt $a");
    verifyDisassembly(0b0110'0010'0000'1000, "bhi $a");
    verifyDisassembly(0b0110'1111'0000'1000, "ble $a");
    verifyDisassembly(0b0110'0011'0000'1000, "bls $a");
    verifyDisassembly(0b0110'1101'0000'1000, "blt $a");
    verifyDisassembly(0b0110'1011'0000'1000, "bmi $a");
    verifyDisassembly(0b0110'1010'0000'1000, "bpl $a");
    verifyDisassembly(0b0110'1000'0000'1000, "bvc $a");
    verifyDisassembly(0b0110'1001'0000'1000, "bvs $a");
}

BOOST_AUTO_TEST_CASE(bra)
{
    verifyDisassembly(0x6000, 0x0004, "bra $6");
    verifyDisassembly(0x6008, "bra $a");
    verifyDisassembly(0x60c0, "bra $ffc2");
}

BOOST_AUTO_TEST_CASE(bchg_r)
{
    verifyDisassembly(0x0141, "bchg d0,d1");
    verifyDisassembly(0x0151, "bchg d0,(a1)");
    verifyDisassembly(0x0159, "bchg d0,(a1)+");
}

BOOST_AUTO_TEST_CASE(bchg_i)
{
    verifyDisassembly(0x0853, 0x000c, "bchg #$c,(a3)");
    verifyDisassembly(0x0840, 0x0003, "bchg #$3,d0");
}

BOOST_AUTO_TEST_CASE(bset_r)
{
    verifyDisassembly(0x01c1, "bset d0,d1");
    verifyDisassembly(0x01d1, "bset d0,(a1)");
}

BOOST_AUTO_TEST_CASE(bset_i)
{
    verifyDisassembly(0x08d3, 0x000c, "bset #$c,(a3)");
    verifyDisassembly(0x08c0, 0x0003, "bset #$3,d0");
}

BOOST_AUTO_TEST_CASE(btst)
{
	verifyDisassembly(0x0101, "btst d0,d1");
    verifyDisassembly(0x0711, "btst d3,(a1)");

    verifyDisassembly(0x0801, 0x00a, "btst #$a,d1");
    verifyDisassembly(0x0819, 0x002, "btst #$2,(a1)+");
}

BOOST_AUTO_TEST_CASE(bsr)
{
	verifyDisassembly(0x6100, 0x000e, "bsr $10");
    verifyDisassembly(0x6100, 0x00f8, "bsr $fa");
    verifyDisassembly(0x6108, "bsr $a");
}

BOOST_AUTO_TEST_CASE(chk)
{
	verifyDisassembly(0x4181, "chk d1,d0");
	verifyDisassembly(0x4191, "chk (a1),d0");
}

BOOST_AUTO_TEST_CASE(clr)
{
    verifyDisassembly(0x4200, "clr.b d0");
    verifyDisassembly(0x4251, "clr.w (a1)");
    verifyDisassembly(0x42a6, "clr.l -(a6)");
}

BOOST_AUTO_TEST_CASE(clr_r)
{
    verifyDisassembly(0x0181, "bclr d0,d1");
    verifyDisassembly(0x0191, "bclr d0,(a1)");
	verifyDisassembly(0x0199, "bclr d0,(a1)+");
}

BOOST_AUTO_TEST_CASE(clr_i)
{
    verifyDisassembly(0x0893, 0x000c, "bclr #$c,(a3)");
    verifyDisassembly(0x0880, 0x0003, "bclr #$3,d0");
}

BOOST_AUTO_TEST_CASE(cmp)
{
    verifyDisassembly(0xb641, "cmp.w d1,d3");
	verifyDisassembly(0xbcab, 0x000a, "cmp.l 10(a3),d6");
}

BOOST_AUTO_TEST_CASE(cmpa)
{
    verifyDisassembly(0xb0c1, "cmpa.w d1,a0");
	verifyDisassembly(0xbdfb, 0x40bd, "cmpa.l offset_0xbd(pc,d4),a6");
    verifyDisassembly(0xb2fc, 0x1234, "cmpa.w #$1234,a1");
    verifyDisassembly(0xb5fc, 0x0045, 0x6789, "cmpa.l #$456789,a2");
}

BOOST_AUTO_TEST_CASE(cmpi)
{
    verifyDisassembly(0x0c00, 0x0005, "cmpi.b #$5,d0");
    verifyDisassembly(0x0c01, 0x0075, "cmpi.b #$75,d1");
    verifyDisassembly(0x0c40, 0x0005, "cmpi.w #$5,d0");
    verifyDisassembly(0x0c41, 0x0100, "cmpi.w #$100,d1");
    verifyDisassembly(0x0c80, 0x0001, 0x0005, "cmpi.l #$10005,d0");
    verifyDisassembly(0x0c81, 0x1234, 0x0100, "cmpi.l #$12340100,d1");
}

BOOST_AUTO_TEST_CASE(cmpm)
{
    verifyDisassembly(0xb509, "cmpm.b (a1)+,(a2)+");
    verifyDisassembly(0xb54a, "cmpm.w (a2)+,(a2)+");
    verifyDisassembly(0xb58b, "cmpm.l (a3)+,(a2)+");
}

BOOST_AUTO_TEST_CASE(dbcc)
{
	verifyDisassembly(0x54cf, 0xfffa, "dbcc d7,offset_0xfffa");
	verifyDisassembly(0x55cf, 0xffe6, "dbcs d7,offset_0xffe6");
    verifyDisassembly(0x56cf, 0xffe6, "dbne d7,offset_0xffe6");
    verifyDisassembly(0x57cf, 0xffe6, "dbeq d7,offset_0xffe6");
    verifyDisassembly(0x58cf, 0xffe6, "dbvc d7,offset_0xffe6");
    verifyDisassembly(0x59cf, 0xffe6, "dbvs d7,offset_0xffe6");
    verifyDisassembly(0x5acf, 0xffe6, "dbpl d7,offset_0xffe6");
    verifyDisassembly(0x5bcf, 0xffe6, "dbmi d7,offset_0xffe6");
    verifyDisassembly(0x5ccf, 0xffe6, "dbge d7,offset_0xffe6");
    verifyDisassembly(0x5dcf, 0xffe6, "dblt d7,offset_0xffe6");
    verifyDisassembly(0x5ecf, 0xffe6, "dbgt d7,offset_0xffe6");
    verifyDisassembly(0x5fcf, 0xffe6, "dble d7,offset_0xffe6");
}

BOOST_AUTO_TEST_CASE(divs)
{
	verifyDisassembly(0x81c1, "divs d1,d0");
}

BOOST_AUTO_TEST_CASE(divu)
{
    verifyDisassembly(0x80c1, "divu d1,d0");
}

BOOST_AUTO_TEST_CASE(eor)
{
    verifyDisassembly(0xb714, "eor.b d3,(a4)");
    verifyDisassembly(0xb75c, "eor.w d3,(a4)+");
    verifyDisassembly(0xb3a2, "eor.l d1,-(a2)");
}

BOOST_AUTO_TEST_CASE(eori)
{
    verifyDisassembly(0x0a03, 0x0029, "eori.b #$29,d3");
    verifyDisassembly(0x0a43, 0x8181, "eori.w #$8181,d3");
    verifyDisassembly(0x0a83, 0x7f27, 0x8181, "eori.l #$7f278181,d3");
}

BOOST_AUTO_TEST_CASE(eori2ccr)
{
    verifyDisassembly(0x0a3c, 0x0015, "eori #$15,ccr");
}

BOOST_AUTO_TEST_CASE(eori2sr)
{
    verifyDisassembly(0x0a7c, 0x0023, "eori #$23,sr");
}

BOOST_AUTO_TEST_CASE(exg)
{
    verifyDisassembly(0xc745, "exg d3,d5");
    verifyDisassembly(0xcf49, "exg a7,a1");
    verifyDisassembly(0xcd8b, "exg d6,a3");
}

BOOST_AUTO_TEST_CASE(ext)
{
    verifyDisassembly(0x4885, "ext.w d5");
    verifyDisassembly(0x48c2, "ext.l d2");
}

BOOST_AUTO_TEST_CASE(illegal)
{
    verifyDisassembly(0x4afc, "illegal");
}

BOOST_AUTO_TEST_CASE(jmp)
{
	verifyDisassembly(0x4ef8, 0x100c, "jmp $100c.w");
    verifyDisassembly(0x4ef9, 0x0123, 0x4567, "jmp $1234567.l");
}

BOOST_AUTO_TEST_CASE(jsr)
{
    verifyDisassembly(0x4eb8, 0x100c, "jsr $100c.w");
    verifyDisassembly(0x4eb9, 0x0123, 0x4567, "jsr $1234567.l");
}
BOOST_AUTO_TEST_CASE(lea)
{
    verifyDisassembly(0x47d0, "lea (a0),a3");
    verifyDisassembly(0x45f8, 0x1234, "lea $1234.w,a2");
    verifyDisassembly(0x41fa, 0xfffa, "lea offset_0xfffa(pc),a0");
}

BOOST_AUTO_TEST_CASE(lsl_memory)
{
    verifyDisassembly(0xe2d3, "lsr (a3)");
    verifyDisassembly(0xe3d3, "lsl (a3)");
}

BOOST_AUTO_TEST_CASE(lsl_b)
{
    verifyDisassembly(0xe308, "lsl.b #1,d0");
}

BOOST_AUTO_TEST_CASE(lsl_w)
{
    verifyDisassembly(0xe748, "lsl.w #3,d0");
}

BOOST_AUTO_TEST_CASE(lsl_l)
{
    verifyDisassembly(0xe3a8, "lsl.l d1,d0");
}

BOOST_AUTO_TEST_CASE(lsr_memory)
{
    verifyDisassembly(0xe3d3, "lsl (a3)");
    verifyDisassembly(0xe2d3, "lsr (a3)");
}

BOOST_AUTO_TEST_CASE(lsr_b)
{
    verifyDisassembly(0xe208, "lsr.b #1,d0");
}

BOOST_AUTO_TEST_CASE(lsr_w)
{
    verifyDisassembly(0xe648, "lsr.w #3,d0");
}

BOOST_AUTO_TEST_CASE(lsr_l)
{
    verifyDisassembly(0xe2a8, "lsr.l d1,d0");
}

BOOST_AUTO_TEST_CASE(move)
{
    verifyDisassembly(0x143c, 0x0020, "move.b #$20,d2");
    verifyDisassembly(0x383c, 0x1234, "move.w #$1234,d4");
    verifyDisassembly(0x243c, 0x1234, 0x5678, "move.l #$12345678,d2");
    verifyDisassembly(0x20fc, 0x1234, 0x5678, "move.l #$12345678,(a0)+");
}

BOOST_AUTO_TEST_CASE(moveq)
{
	verifyDisassembly(0x7604, "moveq.l #$4,d3");
}

BOOST_AUTO_TEST_CASE(move_to_ccr)
{
    verifyDisassembly(0x44fc, 0x0015, "move #$15,ccr");
}

BOOST_AUTO_TEST_CASE(move_from_sr)
{
    verifyDisassembly(0x40c1, "move sr,d1");
}

BOOST_AUTO_TEST_CASE(move_to_sr)
{
    verifyDisassembly(0x46db, "move (a3)+,sr");
}

BOOST_AUTO_TEST_CASE(movea)
{
    verifyDisassembly(0x206e, 0x0008, "movea.l 8(a6),a0");
    verifyDisassembly(0x327c, 0x1234, "movea.w #$1234,a1");
    verifyDisassembly(0x347c, 0xfffe, "movea.w #$fffe,a2");
    verifyDisassembly(0x267c, 0x0045, 0x6789, "movea.l #$456789,a3");
}

BOOST_AUTO_TEST_CASE(movem)
{
    verifyDisassembly(0x48d3, 0x0303, "movem.l d0-d1/a0-a1,(a3)");
    verifyDisassembly(0x4893, 0x0303, "movem.w d0-d1/a0-a1,(a3)");
	verifyDisassembly(0x48a3, 0xc0c0, "movem.w d0-d1/a0-a1,-(a3)");

    verifyDisassembly(0x4cd3, 0x315, "movem.l (a3),d0/d2/d4/a0-a1");

    verifyDisassembly(0x4cd3, 0x0303, "movem.l (a3),d0-d1/a0-a1");
    verifyDisassembly(0x4cdb, 0x0303, "movem.l (a3)+,d0-d1/a0-a1");
    verifyDisassembly(0x4cd3, 0x0801, "movem.l (a3),d0/a3");
    verifyDisassembly(0x4cdb, 0x0801, "movem.l (a3)+,d0/a3");
}

BOOST_AUTO_TEST_CASE(movep)
{
	verifyDisassembly(0x078c, 0x0005, "movep.w d3,5(a4)");
    verifyDisassembly(0x0f0c, 0x0005, "movep.w 5(a4),d7");

    verifyDisassembly(0x07cc, 0x0002, "movep.l d3,2(a4)");
    verifyDisassembly(0x0f4c, 0x0002, "movep.l 2(a4),d7");
}

BOOST_AUTO_TEST_CASE(muls)
{
    verifyDisassembly(0xc3c0, "muls d0,d1");
}

BOOST_AUTO_TEST_CASE(mulu)
{
    verifyDisassembly(0xc2c0, "mulu d0,d1");
}

BOOST_AUTO_TEST_CASE(nbcd)
{
    verifyDisassembly(0x4800, "nbcd d0");
}

BOOST_AUTO_TEST_CASE(neg)
{
    verifyDisassembly(0x4400, "neg.b d0");
    verifyDisassembly(0x4441, "neg.w d1");
    verifyDisassembly(0x4482, "neg.l d2");
}

BOOST_AUTO_TEST_CASE(negx)
{
    verifyDisassembly(0x4000, "negx.b d0");
    verifyDisassembly(0x4041, "negx.w d1");
    verifyDisassembly(0x4082, "negx.l d2");
}

BOOST_AUTO_TEST_CASE(nop)
{
	verifyDisassembly(0x4e71, "nop");
}

BOOST_AUTO_TEST_CASE(not_)
{
	verifyDisassembly(0x4600, "not.b d0");
	verifyDisassembly(0x4642, "not.w d2");
	verifyDisassembly(0x4684, "not.l d4");
}

BOOST_AUTO_TEST_CASE(or_)
{
    verifyDisassembly(0x8614, "or.b (a4),d3");
    verifyDisassembly(0x8654, "or.w (a4),d3");
	verifyDisassembly(0x8694, "or.l (a4),d3");

    verifyDisassembly(0x8714, "or.b d3,(a4)");
    verifyDisassembly(0x8754, "or.w d3,(a4)");
    verifyDisassembly(0x8794, "or.l d3,(a4)");
}

BOOST_AUTO_TEST_CASE(ori)
{
    verifyDisassembly(0x003, 0x0029, "ori.b #$29,d3");
    verifyDisassembly(0x043, 0x8187, "ori.w #$8187,d3");
    verifyDisassembly(0x083, 0x7f27, 0x8193, "ori.l #$7f278193,d3");
}

BOOST_AUTO_TEST_CASE(ori2ccr)
{
    verifyDisassembly(0x003c, 0x0023, "ori #$23,ccr");
}

BOOST_AUTO_TEST_CASE(ori2sr)
{
    verifyDisassembly(0x007c, 0x0023, "ori #$23,sr");
}

BOOST_AUTO_TEST_CASE(pea)
{
    verifyDisassembly(0x4850, "pea (a0)");
}

BOOST_AUTO_TEST_CASE(rol)
{
    verifyDisassembly(0xe7d0, "rol (a0)");
    verifyDisassembly(0xe13b, "rol.b d0,d3");
    verifyDisassembly(0xe17b, "rol.w d0,d3");
    verifyDisassembly(0xe1bb, "rol.l d0,d3");
}

BOOST_AUTO_TEST_CASE(roxl)
{
    verifyDisassembly(0xe5d0, "roxl (a0)");
    verifyDisassembly(0xe133, "roxl.b d0,d3");
    verifyDisassembly(0xe952, "roxl.w #4,d2");
    verifyDisassembly(0xe1b7, "roxl.l d0,d7");
}

BOOST_AUTO_TEST_CASE(ror)
{
    verifyDisassembly(0xe6d0, "ror (a0)");
    verifyDisassembly(0xe03b, "ror.b d0,d3");
    verifyDisassembly(0xe07b, "ror.w d0,d3");
    verifyDisassembly(0xe0bb, "ror.l d0,d3");
}

BOOST_AUTO_TEST_CASE(roxr)
{
    verifyDisassembly(0xe4d0, "roxr (a0)");
    verifyDisassembly(0xe033, "roxr.b d0,d3");
    verifyDisassembly(0xe852, "roxr.w #4,d2");
    verifyDisassembly(0xe0b7, "roxr.l d0,d7");
}

BOOST_AUTO_TEST_CASE(rte)
{
    verifyDisassembly(0x4e73, "rte");
}

BOOST_AUTO_TEST_CASE(rtr)
{
    verifyDisassembly(0x4e77, "rtr");
}

BOOST_AUTO_TEST_CASE(scc)
{
    verifyDisassembly(0x54c1, "scc d1");
    verifyDisassembly(0x55c2, "scs d2");
    verifyDisassembly(0x56c3, "sne d3");
    verifyDisassembly(0x57c4, "seq d4");
	verifyDisassembly(0x58c5, "svc d5");
	verifyDisassembly(0x59c6, "svs d6");
    verifyDisassembly(0x5ac7, "spl d7");
    verifyDisassembly(0x5bc0, "smi d0");
	verifyDisassembly(0x5cc1, "sge d1");
	verifyDisassembly(0x5dc2, "slt d2");
	verifyDisassembly(0x5ec3, "sgt d3");
	verifyDisassembly(0x5fc4, "sle d4");
}

BOOST_AUTO_TEST_CASE(sbcd)
{
    verifyDisassembly(0x8300, "sbcd d0,d1");
	verifyDisassembly(0x8509, "sbcd -(a1),-(a2)");
}

BOOST_AUTO_TEST_CASE(stop)
{
	verifyDisassembly(0x4e72, 0x0042, "stop #$42");
}

BOOST_AUTO_TEST_CASE(sub)
{
    verifyDisassembly(0b1001'001'000'000'000u, "sub.b d0,d1");
    verifyDisassembly(0b1001'001'001'000'000u, "sub.w d0,d1");
    verifyDisassembly(0b1001'001'010'000'000u, "sub.l d0,d1");
    verifyDisassembly(0x9614, "sub.b (a4),d3");
    verifyDisassembly(0x9714, "sub.b d3,(a4)");
	verifyDisassembly(0x9cb8, 0x0038, "sub.l $38.w,d6");
    verifyDisassembly(0x96b9, 0x0000,0x0386, "sub.l $386.l,d3");
}

BOOST_AUTO_TEST_CASE(suba)
{
    verifyDisassembly(0x90fc, 0x0020, "suba.w #$20,a0");
    verifyDisassembly(0x92fc, 0xffe0, "suba.w #$ffe0,a1");
	verifyDisassembly(0x95fc, 0x0000, 0x0020, "suba.l #$20,a2");
}

BOOST_AUTO_TEST_CASE(subi)
{
    verifyDisassembly(0x0400, 0x0020, "subi.b #$20,d0");
    verifyDisassembly(0x0401, 0x00e0, "subi.b #$e0,d1");
    verifyDisassembly(0x0440, 0x0232, "subi.w #$232,d0");
    verifyDisassembly(0x0441, 0xf032, "subi.w #$f032,d1");
    verifyDisassembly(0x0480, 0x1000, 0x0678, "subi.l #$10000678,d0");
    verifyDisassembly(0x0481, 0xf123, 0x4567, "subi.l #$f1234567,d1");
}
BOOST_AUTO_TEST_CASE(subq)
{
	verifyDisassembly(0x5d00, "subq.b #6,d0");
    verifyDisassembly(0x5f43, "subq.w #7,d3");
    verifyDisassembly(0x5187, "subq.l #8,d7");
	verifyDisassembly(0x5f88, "subq.l #7,a0");
}

BOOST_AUTO_TEST_CASE(subx)
{
    verifyDisassembly(0b1001'110'1'00'00'0'010u, "subx.b d2,d6");
    verifyDisassembly(0b1001'011'1'01'00'1'010u, "subx.w -(a2),-(a3)");
    verifyDisassembly(0b1001'101'1'10'00'0'010u, "subx.l d2,d5");
}

BOOST_AUTO_TEST_CASE(swap)
{
    verifyDisassembly(0x4846, "swap d6");
    verifyDisassembly(0x4844, "swap d4");
}

BOOST_AUTO_TEST_CASE(tas)
{
    verifyDisassembly(0x4aeb, 0x0005, "tas 5(a3)");
}

BOOST_AUTO_TEST_CASE(trap)
{
    verifyDisassembly(0x4e43, "trap #3");
}

BOOST_AUTO_TEST_CASE(trapv)
{
    verifyDisassembly(0x4e76, "trapv");
}

BOOST_AUTO_TEST_CASE(tst)
{
	verifyDisassembly(0x4a00, "tst.b d0");
    verifyDisassembly(0x4a52, "tst.w (a2)");
	verifyDisassembly(0x4aae, 0x0008, "tst.l 8(a6)");
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
BOOST_AUTO_TEST_CASE(tolower01)
{
    verifyDisassembly(0x4e56, 0x0000, "link a6,#0");
}

BOOST_AUTO_TEST_CASE(tolower02)
{
    verifyDisassembly(0x306e, 0x0008, "movea.w 8(a6),a0");
}

BOOST_AUTO_TEST_CASE(tolower03)
{
    verifyDisassembly(0x326e, 0x000c, "movea.w 12(a6),a1");
}

BOOST_AUTO_TEST_CASE(tolower04)
{
    verifyDisassembly(0x1018, 0x0008, "move.b (a0)+,d0");
}
BOOST_AUTO_TEST_CASE(tolower05)
{
    verifyDisassembly(0x0c40, 0x0041, "cmpi.w #$41,d0");
}
BOOST_AUTO_TEST_CASE(tolower06)
{
    verifyDisassembly(0x6500, 0x000E, "bcs $10");
}

BOOST_AUTO_TEST_CASE(tolower07)
{
    verifyDisassembly(0x0C40, 0x005A, "cmpi.w #$5a,d0");
}

BOOST_AUTO_TEST_CASE(tolower08)
{
verifyDisassembly(0x6200, 0x0006, "bhi $8");
}

BOOST_AUTO_TEST_CASE(tolower09)
{
    verifyDisassembly(0x0640, 0x0020, "addi.w #$20,d0");
}

BOOST_AUTO_TEST_CASE(tolower10)
{
    verifyDisassembly(0x12C0, "move.b d0,(a1)+");
}

BOOST_AUTO_TEST_CASE(tolower11)
{
    verifyDisassembly(0x66E6, "bne $ffe8");
}

BOOST_AUTO_TEST_CASE(tolower12)
{
    verifyDisassembly(0x4E5E, "unlk a6");
}

BOOST_AUTO_TEST_CASE(tolower13)
{
    verifyDisassembly(0x4E75, "rts");
}



BOOST_AUTO_TEST_SUITE_END()
