#include <boost/test/unit_test.hpp>
#include "asmparser.h"

BOOST_AUTO_TEST_SUITE(asmSuite)



bool hasFailed(std::any result)
{
	try
	{
		bool ok = std::any_cast<bool>(result);
		return !ok;
	}
	catch (const std::bad_any_cast& )
	{
		// The parsrer returned something so the parsing was correct and hasn't failed
		return false;
	}
}

template<typename T>
void validate_hasValue(T expected, std::any result)
{
	auto results = std::any_cast<std::vector<std::any>>(result);
	std::any element = results[0];
	T actual = std::any_cast<T>(element);

	BOOST_CHECK_EQUAL(expected, actual);
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

// -------------------------------
// Addressing modes tests
// -------------------------------
BOOST_AUTO_TEST_CASE(address_dReg)
{
	asmparser parser;
	auto opcode = parser.parseText(" add d1, d0\n");
	validate_hasValue<uint16_t>(0b1101'000'001'000'001, opcode);
}

BOOST_AUTO_TEST_CASE(address_aReg)
{
	asmparser parser;
	auto opcode = parser.parseText(" add a1, d0\n");
	validate_hasValue<uint16_t>(0b1101'000'001'001'001, opcode);
}

BOOST_AUTO_TEST_CASE(address_aReg_indirect)
{
	asmparser parser;
	auto opcode = parser.parseText(" add (a1), d0\n");
	validate_hasValue<uint16_t>(0b1101'000'001'010'001, opcode);
}

BOOST_AUTO_TEST_CASE(address_aReg_post)
{
	asmparser parser;
	auto opcode = parser.parseText(" add (a1)+, d0\n");
	validate_hasValue<uint16_t>(0b1101'000'001'011'001, opcode);
}

BOOST_AUTO_TEST_CASE(address_aReg_pre)
{
	asmparser parser;
	auto opcode = parser.parseText(" add -(a1), d0\n");
	validate_hasValue<uint16_t>(0b1101'000'001'100'001, opcode);
}

BOOST_AUTO_TEST_CASE(address_aReg_displacement)
{
	asmparser parser;
	auto opcode = parser.parseText(" add 4(a1), d0\n");
	validate_hasValue<uint16_t>(0b1101'000'001'101'001, opcode);

	const std::vector<uint16_t>& code = parser.getCode();
	BOOST_CHECK_EQUAL(2, code.size());
	BOOST_CHECK_EQUAL(4, code[1]);
}

BOOST_AUTO_TEST_CASE(address_aReg_index)
{
	asmparser parser;
	auto opcode = parser.parseText(" add 4(a1, d2), d0\n");
	validate_hasValue<uint16_t>(0b1101'000'001'110'001, opcode);

	const std::vector<uint16_t>& code = parser.getCode();
	BOOST_CHECK_EQUAL(2, code.size());
	BOOST_CHECK_EQUAL(0b0'010'0'000'00000100, code[1]);
}

BOOST_AUTO_TEST_CASE(address_aReg_index_negative)
{
	asmparser parser;
	auto opcode = parser.parseText(" add -4(a1, d2), d0\n");
	validate_hasValue<uint16_t>(0b1101'000'001'110'001, opcode);

	const std::vector<uint16_t>& code = parser.getCode();
	BOOST_CHECK_EQUAL(2, code.size());
	BOOST_CHECK_EQUAL(0b0'010'0'000'11111100, code[1]);
}

BOOST_AUTO_TEST_CASE(address_aReg_index_long)
{
	asmparser parser;
	auto opcode = parser.parseText(" add 6(a1, a3.l), d0\n");
	validate_hasValue<uint16_t>(0b1101'000'001'110'001, opcode);

	const std::vector<uint16_t>& code = parser.getCode();
	BOOST_CHECK_EQUAL(2, code.size());
	BOOST_CHECK_EQUAL(0b1'011'1'000'00000110, code[1]);
}

BOOST_AUTO_TEST_CASE(address_absoluteS)
{
	asmparser parser;
	auto opcode = parser.parseText(" add $1234, d0\n");
	validate_hasValue<uint16_t>(0b1101'000'001'111'000, opcode);

	const std::vector<uint16_t>& code = parser.getCode();
	BOOST_CHECK_EQUAL(2, code.size());
	BOOST_CHECK_EQUAL(0x1234, code[1]);
}

BOOST_AUTO_TEST_CASE(address_absoluteL)
{
	asmparser parser;
	auto opcode = parser.parseText(" add $12345678.L, d0\n");
	validate_hasValue<uint16_t>(0b1101'000'001'111'001, opcode);

	const std::vector<uint16_t>& code = parser.getCode();
	BOOST_CHECK_EQUAL(3, code.size());
	BOOST_CHECK_EQUAL(0x1234, code[1]);
	BOOST_CHECK_EQUAL(0x5678, code[2]);
}

BOOST_AUTO_TEST_CASE(address_pc_displacement)
{
	asmparser parser;
	auto opcode = parser.parseText(" add 4(pc), d0\n");
	validate_hasValue<uint16_t>(0b1101'000'001'111'010, opcode);
}

BOOST_AUTO_TEST_CASE(address_pc_index)
{
	asmparser parser;
	auto opcode = parser.parseText(" add 4(pc, d2), d0\n");
	validate_hasValue<uint16_t>(0b1101'000'001'111'011, opcode);
}

BOOST_AUTO_TEST_CASE(address_immediate)
{
	asmparser parser;
	auto opcode = parser.parseText(" add #4, d0\n");
	validate_hasValue<uint16_t>(0b1101'000'001'111'100, opcode);

	const std::vector<uint16_t>& code = parser.getCode();
	BOOST_CHECK_EQUAL(2, code.size());
	BOOST_CHECK_EQUAL(0x4, code[1]);
}

BOOST_AUTO_TEST_CASE(address_immediate_long)
{
	asmparser parser;
	auto opcode = parser.parseText(" add.l #$12345678, d0\n");
	validate_hasValue<uint16_t>(0b1101'000'010'111'100, opcode);

	const std::vector<uint16_t>& code = parser.getCode();
	BOOST_CHECK_EQUAL(3, code.size());
	BOOST_CHECK_EQUAL(0x1234, code[1]);
	BOOST_CHECK_EQUAL(0x5678, code[2]);
}

BOOST_AUTO_TEST_CASE(address_immediate_long_short)
{
	asmparser parser;
	auto opcode = parser.parseText(" add.l #$1234, d0\n");
	validate_hasValue<uint16_t>(0b1101'000'010'111'100, opcode);

	const std::vector<uint16_t>& code = parser.getCode();
	BOOST_CHECK_EQUAL(3, code.size());
	BOOST_CHECK_EQUAL(0x0, code[1]);
	BOOST_CHECK_EQUAL(0x1234, code[2]);
}
BOOST_AUTO_TEST_SUITE_END()