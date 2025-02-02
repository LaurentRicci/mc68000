#include <boost/test/unit_test.hpp>
#include "asmparser.h"

BOOST_AUTO_TEST_SUITE(asmSuite)

BOOST_AUTO_TEST_CASE(start)
{
	BOOST_CHECK(1 == 1);
}

BOOST_AUTO_TEST_CASE(move)
{
	auto ok = parseText("   move.l #1,d0\n");
	BOOST_CHECK(ok);
}

BOOST_AUTO_TEST_CASE(error)
{
	auto ok = parseText("bad.l #1, d0");
	BOOST_CHECK(!ok);
}

BOOST_AUTO_TEST_SUITE_END()