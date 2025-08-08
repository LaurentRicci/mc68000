#include <boost/test/unit_test.hpp>
#include "asmparser.h"
#include "util.h"
BOOST_AUTO_TEST_SUITE(asmSuite)

bool hasFailed(std::any result)
{
	try
	{
		bool ok = std::any_cast<bool>(result);
		return !ok;
	}
	catch (const std::bad_any_cast&)
	{
		// The parsrer returned something so the parsing was correct and hasn't failed
		return false;
	}
}


BOOST_AUTO_TEST_CASE(start)
{
	BOOST_CHECK(1 == 1);
}

BOOST_AUTO_TEST_CASE(move)
{
	asmparser parser;
	auto error = parser.checkSyntax("   move.l #1,d0\n");
	BOOST_CHECK_EQUAL(0, error);
}

BOOST_AUTO_TEST_CASE(error)
{
	asmparser parser;
	auto error = parser.checkSyntax("bad.l #1, d0");
	BOOST_CHECK_EQUAL(1, error);
}

BOOST_AUTO_TEST_CASE(label1)
{
	asmparser parser;
	auto error = parser.checkSyntax("label move #1,d0\n");
	BOOST_CHECK_EQUAL(0, error);
}

BOOST_AUTO_TEST_CASE(label2)
{
	asmparser parser;
	auto error = parser.checkSyntax("label: move #1,d0\n");
	BOOST_CHECK_EQUAL(0, error);
}

BOOST_AUTO_TEST_CASE(label3)
{
	asmparser parser;
	auto error = parser.checkSyntax(" label: move #1,d0\n");
	BOOST_CHECK_EQUAL(0, error);
}

BOOST_AUTO_TEST_CASE(label4)
{
	asmparser parser;
	auto error = parser.checkSyntax(" label:\n move #1,d0\n");
	BOOST_CHECK_EQUAL(0, error);
}

BOOST_AUTO_TEST_CASE(comment)
{
	asmparser parser;
	auto error = parser.checkSyntax("* comment \n move #1,d0\n");
	BOOST_CHECK_EQUAL(0, error);
}

BOOST_AUTO_TEST_CASE(label_duplicate)
{
	asmparser parser;
	auto opcode = parser.parseText("here:\n  abcd d2,d4\nhere:\n nop\n");
	BOOST_CHECK_EQUAL(1, parser.getErrors().get().size());
}

// ====================================================================================================
// ABCD
// ====================================================================================================
BOOST_AUTO_TEST_CASE(abcd_register)
{
	asmparser parser;
	auto opcode = parser.parseText("  abcd d2,d4\n");
	validate_hasValue<uint16_t>(0b1100'100'10000'0'010, opcode);
}

BOOST_AUTO_TEST_CASE(abcd_decrement)
{
	asmparser parser;
	auto opcode = parser.parseText("  ABCD -(a3), -(a4)\n");
	validate_hasValue<uint16_t>(0b1100'100'10000'1'011, opcode);
}

BOOST_AUTO_TEST_CASE(abcd_fail)
{
	asmparser parser;
	auto error = parser.checkSyntax("  abcd d2,(a4)\n");
	BOOST_CHECK_EQUAL(1, error);
}

// ====================================================================================================
// ADD
// ====================================================================================================
BOOST_AUTO_TEST_CASE(add_from)
{
	asmparser parser;
	auto opcode = parser.parseText("  add d2,(a4)\n");
	validate_hasValue<uint16_t>(0b1101'010'101'010'100, opcode);
}

BOOST_AUTO_TEST_CASE(add_from_long)
{
	asmparser parser;
	auto opcode = parser.parseText("  add.l d2,(a4)\n");
	validate_hasValue<uint16_t>(0b1101'010'110'010'100, opcode);
}

BOOST_AUTO_TEST_CASE(add_to)
{
	asmparser parser;
	auto opcode = parser.parseText("  add (a5)+, d4\n");
	validate_hasValue<uint16_t>(0b1101'100'001'011'101, opcode);
}

BOOST_AUTO_TEST_CASE(add_to_byte)
{
	asmparser parser;
	auto opcode = parser.parseText("  add.b (a2), d0\n");
	validate_hasValue<uint16_t>(0b1101'000'000'010'010, opcode);
}
// ====================================================================================================
// ADDA
// ====================================================================================================
BOOST_AUTO_TEST_CASE(adda_word_default)
{
	asmparser parser;
	auto opcode = parser.parseText("  adda d2,A4\n");
	validate_hasValue<uint16_t>(0b1101'100'011'000'010, opcode);
}
BOOST_AUTO_TEST_CASE(adda_byte)
{
	asmparser parser;
	auto opcode = parser.parseText("  adda.b a2,A4\n");
	BOOST_CHECK_EQUAL(1, parser.getErrors().get().size());
}
BOOST_AUTO_TEST_CASE(adda_word)
{
	asmparser parser;
	auto opcode = parser.parseText("  adda.w a2,A4\n");
	validate_hasValue<uint16_t>(0b1101'100'011'001'010, opcode);
}
BOOST_AUTO_TEST_CASE(adda_long)
{
	asmparser parser;
	auto opcode = parser.parseText("  adda.l (a2),A4\n");
	validate_hasValue<uint16_t>(0b1101'100'111'010'010, opcode);
}
// ====================================================================================================
// ADDI
// ====================================================================================================
BOOST_AUTO_TEST_CASE(addi_byte)
{
	asmparser parser;
	auto opcode = parser.parseText("  addi.b #$34,(a2)\n");
	validate_hasValue<uint16_t>(0b0000'0110'00'010'010, opcode);
	const std::vector<uint16_t>& code = parser.getCode();
	BOOST_CHECK_EQUAL(2, code.size());
	BOOST_CHECK_EQUAL(0x34, code[1]);
}

BOOST_AUTO_TEST_CASE(addi_word)
{
	asmparser parser;
	auto opcode = parser.parseText("  addi.w #$1234,d2\n");
	validate_hasValue<uint16_t>(0b0000'0110'01'000'010, opcode);
	const std::vector<uint16_t>& code = parser.getCode();
	BOOST_CHECK_EQUAL(2, code.size());
	BOOST_CHECK_EQUAL(0x1234, code[1]);
}

BOOST_AUTO_TEST_CASE(addi_word_default)
{
	asmparser parser;
	auto opcode = parser.parseText("  addi #$1234,d2\n");
	validate_hasValue<uint16_t>(0b0000'0110'01'000'010, opcode);
	const std::vector<uint16_t>& code = parser.getCode();
	BOOST_CHECK_EQUAL(2, code.size());
	BOOST_CHECK_EQUAL(0x1234, code[1]);
}

BOOST_AUTO_TEST_CASE(addi_extended)
{
	asmparser parser;
	auto opcode = parser.parseText("   addi.w #$3456, $78(A4)\n");
	const std::vector<uint16_t>& code = parser.getCode();
	BOOST_CHECK_EQUAL(3, code.size());
	BOOST_CHECK_EQUAL(0x066c, code[0]);
	BOOST_CHECK_EQUAL(0x3456, code[1]);
	BOOST_CHECK_EQUAL(0x0078, code[2]);
}


BOOST_AUTO_TEST_CASE(addi_long)
{
	asmparser parser;
	auto opcode = parser.parseText("  addi.l #$12345678,d2\n");
	validate_hasValue<uint16_t>(0b0000'0110'10'000'010, opcode);
	const std::vector<uint16_t>& code = parser.getCode();
	BOOST_CHECK_EQUAL(3, code.size());
	BOOST_CHECK_EQUAL(0x1234, code[1]);
	BOOST_CHECK_EQUAL(0x5678, code[2]);
}

BOOST_AUTO_TEST_CASE(addi_invalid)
{
	asmparser parser;
	auto opcode = parser.parseText("  addi #1,4(PC)\n  addi #2,4(PC,D0)\n  addi #3,#4\n");
	BOOST_CHECK_EQUAL(3, parser.getErrors().get().size());
}
// ====================================================================================================
// ADDQ
// ====================================================================================================
BOOST_AUTO_TEST_CASE(addq)
{
	asmparser parser;
	auto opcode = parser.parseText("  addq #4,(a2)\n");
	validate_hasValue<uint16_t>(0b0101'100'0'01'010'010, opcode);
	const std::vector<uint16_t>& code = parser.getCode();
	BOOST_CHECK_EQUAL(1, code.size());
}

BOOST_AUTO_TEST_CASE(addq_byte)
{
	asmparser parser;
	auto opcode = parser.parseText("  addq.b #4,(a2)\n");
	validate_hasValue<uint16_t>(0b0101'100'0'00'010'010, opcode);
	const std::vector<uint16_t>& code = parser.getCode();
	BOOST_CHECK_EQUAL(1, code.size());
}

BOOST_AUTO_TEST_CASE(addq_long)
{
	asmparser parser;
	auto opcode = parser.parseText("  addq.l #8,(a2)\n");
	validate_hasValue<uint16_t>(0b0101'000'0'10'010'010, opcode);
	const std::vector<uint16_t>& code = parser.getCode();
	BOOST_CHECK_EQUAL(1, code.size());
}

BOOST_AUTO_TEST_CASE(addq_byte_error)
{
	asmparser parser;
	auto opcode = parser.parseText("  addq.b #4,a2\n");
	BOOST_CHECK_EQUAL(1, parser.getErrors().get().size());
}

BOOST_AUTO_TEST_CASE(addq_invalid_data)
{
	asmparser parser;
	auto opcode = parser.parseText("  addq.b #9,d2\n");
	BOOST_CHECK_EQUAL(1, parser.getErrors().get().size());
}

BOOST_AUTO_TEST_CASE(addq_invalid)
{
	asmparser parser;
	auto opcode = parser.parseText("  addq #1,4(PC)\n  addq #2,4(PC,D0)\n  addq #3,#4\n");
	BOOST_CHECK_EQUAL(3, parser.getErrors().get().size());
}

// ====================================================================================================
// ADDX
// ====================================================================================================
BOOST_AUTO_TEST_CASE(addx_register_word)
{
    asmparser parser;
    auto opcode = parser.parseText("  addx.w d2,d4\n");
    validate_hasValue<uint16_t>(0b1101'100'1'01'00'0'010, opcode);
}

BOOST_AUTO_TEST_CASE(addx_register_byte)
{
    asmparser parser;
    auto opcode = parser.parseText("  addx.b d2,d4\n");
    validate_hasValue<uint16_t>(0b1101'100'1'00'00'0'010, opcode);
}

BOOST_AUTO_TEST_CASE(addx_memory_word)
{
    asmparser parser;
    auto opcode = parser.parseText("  addx.w -(a2),-(a4)\n");
    validate_hasValue<uint16_t>(0b1101'100'1'01'00'1'010, opcode);
}

BOOST_AUTO_TEST_CASE(addx_memory_long)
{
    asmparser parser;
    auto opcode = parser.parseText("  addx.l -(a2),-(a4)\n");
    validate_hasValue<uint16_t>(0b1101'100'1'10'00'1'010, opcode);
}

// ====================================================================================================
// AND
// ====================================================================================================

// AND: Dn,<ea> (Dn is source, <ea> is destination)
BOOST_AUTO_TEST_CASE(and_from)
{
	asmparser parser;
	auto opcode = parser.parseText("  and d2,-(a3)\n");
	validate_hasValue<uint16_t>(0b1100'010'101'100'011, opcode);
}

BOOST_AUTO_TEST_CASE(and_from_word)
{
	asmparser parser;
	auto opcode = parser.parseText("  and.w d2,(a3)\n");
	validate_hasValue<uint16_t>(0b1100'010'101'010'011, opcode);
}

BOOST_AUTO_TEST_CASE(and_from_byte)
{
    asmparser parser;	
    auto opcode = parser.parseText("  and.b d1,(a2)\n");
    validate_hasValue<uint16_t>(0b1100'001'100'010'010, opcode);
}

BOOST_AUTO_TEST_CASE(and_from_long)
{
    asmparser parser;
    auto opcode = parser.parseText("  and.l d3,(a4)\n");
    validate_hasValue<uint16_t>(0b1100'011'110'010'100, opcode);
}

// AND: <ea>,Dn (<ea> is source, Dn is destination)
BOOST_AUTO_TEST_CASE(and_to)
{
	asmparser parser;
	auto opcode = parser.parseText("  and (a3)+,d2\n");
	validate_hasValue<uint16_t>(0b1100'010'001'011'011, opcode);
}

BOOST_AUTO_TEST_CASE(and_to_word)
{
    asmparser parser;
    auto opcode = parser.parseText("  and.w (a3),d2\n");
    validate_hasValue<uint16_t>(0b1100'010'001'010'011, opcode);
}

BOOST_AUTO_TEST_CASE(and_to_byte)
{
    asmparser parser;
    auto opcode = parser.parseText("  and.b (a2),d1\n");
    validate_hasValue<uint16_t>(0b1100'001'000'010'010, opcode);
}

BOOST_AUTO_TEST_CASE(and_to_long)
{
    asmparser parser;
    auto opcode = parser.parseText("  and.l (a4),d3\n");
    validate_hasValue<uint16_t>(0b1100'011'010'010'100, opcode);
}

BOOST_AUTO_TEST_CASE(and_from_invalid)
{
    asmparser parser;
    auto opcode = parser.parseText("  and.w d0,a1\n");
    BOOST_CHECK_EQUAL(1, parser.getErrors().get().size());
}

// Invalid addressing mode: AND from address register to Dn
BOOST_AUTO_TEST_CASE(and_to_invalid)
{
    asmparser parser;
    auto opcode = parser.parseText("  and.w a1,d0\n");
    BOOST_CHECK_EQUAL(1, parser.getErrors().get().size());
}
// ====================================================================================================
// ANDI
// ====================================================================================================
BOOST_AUTO_TEST_CASE(andi_byte)
{
	asmparser parser;
	auto opcode = parser.parseText("  andi.b #$34,(a2)\n");
	validate_hasValue<uint16_t>(0b0000'0010'00'010'010, opcode);
	const std::vector<uint16_t>& code = parser.getCode();
	BOOST_CHECK_EQUAL(2, code.size());
	BOOST_CHECK_EQUAL(0x34, code[1]);
}

BOOST_AUTO_TEST_CASE(andi_word)
{
	asmparser parser;
	auto opcode = parser.parseText("  andi.w #$1234,d2\n");
	validate_hasValue<uint16_t>(0b0000'0010'01'000'010, opcode);
	const std::vector<uint16_t>& code = parser.getCode();
	BOOST_CHECK_EQUAL(2, code.size());
	BOOST_CHECK_EQUAL(0x1234, code[1]);
}

BOOST_AUTO_TEST_CASE(andi_word_default)
{
	asmparser parser;
	auto opcode = parser.parseText("  andi #$1234,d2\n");
	validate_hasValue<uint16_t>(0b0000'0010'01'000'010, opcode);
	const std::vector<uint16_t>& code = parser.getCode();
	BOOST_CHECK_EQUAL(2, code.size());
	BOOST_CHECK_EQUAL(0x1234, code[1]);
}

BOOST_AUTO_TEST_CASE(andi_extended)
{
	asmparser parser;
	auto opcode = parser.parseText("   andi.w #$3456, $78(A4)\n");
	const std::vector<uint16_t>& code = parser.getCode();
	BOOST_CHECK_EQUAL(3, code.size());
	BOOST_CHECK_EQUAL(0x026c, code[0]);
	BOOST_CHECK_EQUAL(0x3456, code[1]);
	BOOST_CHECK_EQUAL(0x0078, code[2]);
}


BOOST_AUTO_TEST_CASE(andi_long)
{
	asmparser parser;
	auto opcode = parser.parseText("  andi.l #$12345678,d2\n");
	validate_hasValue<uint16_t>(0b0000'0010'10'000'010, opcode);
	const std::vector<uint16_t>& code = parser.getCode();
	BOOST_CHECK_EQUAL(3, code.size());
	BOOST_CHECK_EQUAL(0x1234, code[1]);
	BOOST_CHECK_EQUAL(0x5678, code[2]);
}

BOOST_AUTO_TEST_CASE(andi_invalid)
{
	asmparser parser;
	auto opcode = parser.parseText("  andi #1,4(PC)\n  andi #2,4(PC,D0)\n  andi #3,#4\n");
	BOOST_CHECK_EQUAL(3, parser.getErrors().get().size());
}

// ====================================================================================================
// ANDI to CCR
// ====================================================================================================
BOOST_AUTO_TEST_CASE(andi2ccr_valid)
{
	asmparser parser;
	auto opcode = parser.parseText("  andi #$34, CCR\n");
	validate_hasValue<uint16_t>(0b0000'0010'0011'1100, opcode);
	const std::vector<uint16_t>& code = parser.getCode();
	BOOST_CHECK_EQUAL(2, code.size());
	BOOST_CHECK_EQUAL(0x34, code[1]);
}

BOOST_AUTO_TEST_CASE(andi2ccr_error)
{
	asmparser parser;
	auto opcode = parser.parseText("  andi #$345, CCR\n");
	BOOST_CHECK_EQUAL(1, parser.getErrors().get().size());
}
// ====================================================================================================
// ANDI to SR
// ====================================================================================================
BOOST_AUTO_TEST_CASE(andi2sr_valid)
{
	asmparser parser;
	auto opcode = parser.parseText("  andi #$432, SR\n");
	validate_hasValue<uint16_t>(0b0000'0010'0111'1100, opcode);
	const std::vector<uint16_t>& code = parser.getCode();
	BOOST_CHECK_EQUAL(2, code.size());
	BOOST_CHECK_EQUAL(0x432, code[1]);
}

BOOST_AUTO_TEST_CASE(andi2sr_error)
{
	asmparser parser;
	auto opcode = parser.parseText("  andi #$34567, SR\n");
	BOOST_CHECK_EQUAL(1, parser.getErrors().get().size());
}

// ====================================================================================================
// ASL, ASR
// ====================================================================================================
BOOST_AUTO_TEST_CASE(asl_eam)
{
	asmparser parser;
	auto opcode = parser.parseText("  asl (a3)\n");
	validate_hasValue<uint16_t>(0b1110'000'1'11'010'011, opcode);
}
BOOST_AUTO_TEST_CASE(asr_eam)
{
	asmparser parser;
	auto opcode = parser.parseText("  asr (a4)\n");
	validate_hasValue<uint16_t>(0b1110'000'0'11'010'100, opcode);
}
BOOST_AUTO_TEST_CASE(asl_eam_size)
{
	asmparser parser;
	auto opcode = parser.parseText("  asl.w (a3)\n");
	validate_hasValue<uint16_t>(0b1110'000'1'11'010'011, opcode);
}
BOOST_AUTO_TEST_CASE(asl_eam_wrongsize)
{
	asmparser parser;
	auto opcode = parser.parseText("  asl.b (a3)\n");
	BOOST_CHECK_EQUAL(1, parser.getErrors().get().size());
}
BOOST_AUTO_TEST_CASE(asl_eam_error)
{
	asmparser parser;
	auto opcode = parser.parseText("  asl a3\n");
	BOOST_CHECK_EQUAL(1, parser.getErrors().get().size());
}
BOOST_AUTO_TEST_CASE(asl_eam_2errors)
{
	asmparser parser;
	auto opcode = parser.parseText("  asl.l a3\n");
	BOOST_CHECK_EQUAL(2, parser.getErrors().get().size());
}

BOOST_AUTO_TEST_CASE(asl_immediate_b)
{
	asmparser parser;
	auto opcode = parser.parseText("  asl.b #1,d0\n");
	validate_hasValue<uint16_t>(0b1110'001'1'00'0'00'000, opcode);
}
BOOST_AUTO_TEST_CASE(asr_immediate_b)
{
	asmparser parser;
	auto opcode = parser.parseText("  asr.b #1,d1\n");
	validate_hasValue<uint16_t>(0b1110'001'0'00'0'00'001, opcode);
}

BOOST_AUTO_TEST_CASE(asl_immediate_w)
{
	asmparser parser;
	auto opcode = parser.parseText("  asl.w #8,d0\n");
	validate_hasValue<uint16_t>(0xe140, opcode);
}

BOOST_AUTO_TEST_CASE(asl_immediate_error)
{
	asmparser parser;
	auto opcode = parser.parseText("  asl.w #12,d0\n");
	BOOST_CHECK_EQUAL(1, parser.getErrors().get().size());
}

BOOST_AUTO_TEST_CASE(asl_registers)
{
	asmparser parser;
	auto opcode = parser.parseText("  asl.l d1,d0\n");
	validate_hasValue<uint16_t>(0b1110'001'1'10'1'00'000, opcode);
}
BOOST_AUTO_TEST_CASE(asr_registers)
{
	asmparser parser;
	auto opcode = parser.parseText("  asr.l d1,d0\n");
	validate_hasValue<uint16_t>(0b1110'001'0'10'1'00'000, opcode);
}


// ====================================================================================================
// Immediate instructions
// ====================================================================================================
BOOST_AUTO_TEST_CASE(immediate)
{
	asmparser parser;
	auto opcode = parser.parseText("  addi.w #$3456, $78(A4)\n");
	validate_hasValue<uint16_t>(0x066c, opcode);

	opcode = parser.parseText("  subi.w #$3456, $78(A4)\n");
	validate_hasValue<uint16_t>(0x046c, opcode);

	opcode = parser.parseText("  andi.w #$3456, $78(A4)\n");
	validate_hasValue<uint16_t>(0x026c, opcode);

	opcode = parser.parseText("  eori.w #$3456, $78(A4)\n");
	validate_hasValue<uint16_t>(0x0a6c, opcode);

	opcode = parser.parseText("  ori.w #$3456, $78(A4)\n");
	validate_hasValue<uint16_t>(0x006c, opcode);

	opcode = parser.parseText("  cmpi.w #$3456, $78(A4)\n");
	validate_hasValue<uint16_t>(0x0c6c, opcode);
}

// ====================================================================================================
// BCC instructions
// ====================================================================================================
BOOST_AUTO_TEST_CASE(bcc_address)
{
	asmparser parser;
	auto opcode = parser.parseText("  bcc $3458\n");
	validate_hasValue<uint16_t>(0b0110'0100'0000'0000, opcode);
	const std::vector<uint16_t>& code = parser.getCode();
	BOOST_CHECK_EQUAL(2, code.size());
	BOOST_CHECK_EQUAL(0x3456, code[1]);
}

BOOST_AUTO_TEST_CASE(bcc_addressShort)
{
	asmparser parser;
	auto opcode = parser.parseText("  bcc $34\n");
	validate_hasValue<uint16_t>(0b0110'0100'00110010, opcode);
	const std::vector<uint16_t>& code = parser.getCode();
	BOOST_CHECK_EQUAL(1, code.size());
}

BOOST_AUTO_TEST_CASE(bcc_addressNegative)
{
	asmparser parser;
	auto opcode = parser.parseText("  nop\n  bcc $0\n");
	const std::vector<uint16_t>& code = parser.getCode();
	BOOST_CHECK_EQUAL(2, code.size());
	BOOST_CHECK_EQUAL(0b0110'0100'11111100, code[1]);
}

BOOST_AUTO_TEST_CASE(bcc_all)
{
	asmparser parser;
	auto opcode = parser.parseText("  nop\n  bcc $20\n  bcs $20\n  beq $20\n  bne $20\n  bge $20\n  bgt $20\n  bhi $20\n  ble $0\n  bls $0\n  blt $0\n  bmi $0\n  bpl $0\n  bvc $0\n  bvs $0\n  bra $0\n");
	const std::vector<uint16_t>& code = parser.getCode();
	BOOST_CHECK_EQUAL(16, code.size());
	BOOST_CHECK_EQUAL(0x4e71, code[0]);
	BOOST_CHECK_EQUAL(0x641c, code[1]);
	BOOST_CHECK_EQUAL(0x651a, code[2]);
	BOOST_CHECK_EQUAL(0x6718, code[3]);
	BOOST_CHECK_EQUAL(0x6616, code[4]);
	BOOST_CHECK_EQUAL(0x6c14, code[5]);
	BOOST_CHECK_EQUAL(0x6e12, code[6]);
	BOOST_CHECK_EQUAL(0x6210, code[7]);
	BOOST_CHECK_EQUAL(0x6fee, code[8]);
	BOOST_CHECK_EQUAL(0x63ec, code[9]);
	BOOST_CHECK_EQUAL(0x6dea, code[10]);
	BOOST_CHECK_EQUAL(0x6be8, code[11]);
	BOOST_CHECK_EQUAL(0x6ae6, code[12]);
	BOOST_CHECK_EQUAL(0x68e4, code[13]);
	BOOST_CHECK_EQUAL(0x69e2, code[14]);
	BOOST_CHECK_EQUAL(0x60e0, code[15]);
}

BOOST_AUTO_TEST_CASE(bcc_label)
{
	asmparser parser;
	auto opcode = parser.parseText(" bcc data\n nop\n nop\n nop\ndata:\n");
	validate_hasValue<uint16_t>(0b0110'0100'0000'0000, opcode);
	const std::vector<uint16_t>& code = parser.getCode();
	BOOST_CHECK_EQUAL(5, code.size());
	BOOST_CHECK_EQUAL(0x8, code[1]);
}

BOOST_AUTO_TEST_CASE(bcc_labelUnknown)
{
	asmparser parser;
	auto opcode = parser.parseText(" bcc data\n");
	BOOST_CHECK_EQUAL(1, parser.getErrors().get().size());
}

BOOST_AUTO_TEST_CASE(bcc_labelTooFar)
{
	asmparser parser;
	auto opcode = parser.parseText(" bcc $12346\n");
	BOOST_CHECK_EQUAL(1, parser.getErrors().get().size());
}
// -------------------------------
// Label and variable tests
// -------------------------------
BOOST_AUTO_TEST_CASE(address1)
{
	asmparser parser;
	auto opcode = parser.parseText("label1:\n add #1, d0\nlabel2:\n");

	auto labels = parser.getLabels();
	BOOST_CHECK_EQUAL(2, labels.size());
	BOOST_CHECK(labels.find("label1") != labels.end());
	BOOST_CHECK(labels.find("label2") != labels.end());

}






BOOST_AUTO_TEST_SUITE_END()
