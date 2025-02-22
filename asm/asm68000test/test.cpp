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
	auto error = checkSyntax("   move.l #1,d0\n");
	BOOST_CHECK_EQUAL(0, error);
}

BOOST_AUTO_TEST_CASE(error)
{
	auto error = checkSyntax("bad.l #1, d0");
	BOOST_CHECK_EQUAL(1, error);
}

BOOST_AUTO_TEST_CASE(label1)
{
	auto error = checkSyntax("label move #1,d0\n");
	BOOST_CHECK_EQUAL(0, error);
}

BOOST_AUTO_TEST_CASE(label2)
{
	auto error = checkSyntax("label: move #1,d0\n");
	BOOST_CHECK_EQUAL(0, error);
}

BOOST_AUTO_TEST_CASE(label3)
{
	auto error = checkSyntax(" label: move #1,d0\n");
	BOOST_CHECK_EQUAL(0, error);
}

BOOST_AUTO_TEST_CASE(label4)
{
	auto error = checkSyntax(" label:\n move #1,d0\n");
	BOOST_CHECK_EQUAL(0, error);
}

BOOST_AUTO_TEST_CASE(comment)
{
	auto error = checkSyntax("* comment \n move #1,d0\n");
	BOOST_CHECK_EQUAL(0, error);
}

BOOST_AUTO_TEST_CASE(abcd_register)
{
	auto opcode = parseText("  abcd d2,d4\n");
	validate_hasValue<uint16_t>(0b1100'100'10000'0'010, opcode);
}

BOOST_AUTO_TEST_CASE(abcd_decrement)
{
	auto opcode = parseText("  ABCD -(a3), -(a4)\n");
	validate_hasValue<uint16_t>(0b1100'100'10000'1'011, opcode);
}

BOOST_AUTO_TEST_CASE(abcd_fail)
{
	auto error = checkSyntax("  abcd d2,(a4)\n");
	BOOST_CHECK_EQUAL(1, error);
}

BOOST_AUTO_TEST_SUITE_END()