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
