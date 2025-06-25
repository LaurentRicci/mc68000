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
