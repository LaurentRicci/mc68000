#include <boost/test/unit_test.hpp>
#include "asmparser.h"
#include "util.h"

namespace directiveTest
{
	BOOST_AUTO_TEST_SUITE(directiveTest)

	BOOST_AUTO_TEST_CASE(dcb_byte)
	{
		asmparser parser;
		auto opcode = parser.parseText(" dc.b 42\n");
		validate_hasValue<uint16_t>(0x2A00, opcode);
	}

	BOOST_AUTO_TEST_CASE(dcb_negative)
	{
		asmparser parser;
		auto opcode = parser.parseText(" dc.b -25\n");
		validate_hasValue<uint16_t>(0xe700, opcode);
	}

	BOOST_AUTO_TEST_CASE(dcb_2bytes)
	{
		asmparser parser;
		auto opcode = parser.parseText(" dc.b $12, $34\n");
		validate_hasValue<uint16_t>(0x1234, opcode);
	}

	BOOST_AUTO_TEST_CASE(dcb_string)
	{
		asmparser parser;
		auto opcode = parser.parseText(" dc.b 'AB'\n");
		validate_hasValue<uint16_t>(0x4142, opcode);
	}

	BOOST_AUTO_TEST_CASE(dcb_2string)
	{
		asmparser parser;
		auto opcode = parser.parseText(" DC.B   'LIS', 'T'\n");
		validate_hasValue<uint16_t>(0x5354, opcode);

		const std::vector<uint16_t>& code = parser.getCode();
		BOOST_CHECK_EQUAL(2, code.size());
		BOOST_CHECK_EQUAL(0x4C49, code[0]); // 'LI'
		BOOST_CHECK_EQUAL(0x5354, code[1]); // 'ST'
	}

	BOOST_AUTO_TEST_CASE(dcb_stringByte)
	{
		asmparser parser;
		auto opcode = parser.parseText(" DC.B   'ABC', $ff\n");
		validate_hasValue<uint16_t>(0x43ff, opcode);

		const std::vector<uint16_t>& code = parser.getCode();
		BOOST_CHECK_EQUAL(2, code.size());
		BOOST_CHECK_EQUAL(0x4142, code[0]); // 'AB'
		BOOST_CHECK_EQUAL(0x43ff, code[1]); // 'C' + $ff
	}

	BOOST_AUTO_TEST_CASE(dcb_multi)
	{
		asmparser parser;
		auto opcode = parser.parseText(" dc.b 'lau', 10, 129\n");

		const std::vector<uint16_t>& code = parser.getCode();
		BOOST_CHECK_EQUAL(3, code.size());
		BOOST_CHECK_EQUAL(0x6C61, code[0]); // 'la'
		BOOST_CHECK_EQUAL(0x750A, code[1]); // 'u' 10
		BOOST_CHECK_EQUAL(0x8100, code[2]); // 129
	}

	BOOST_AUTO_TEST_CASE(dcb_mixed)
	{
		asmparser parser;
		auto opcode = parser.parseText(" dc.b 10\n nop\n");

		const std::vector<uint16_t>& code = parser.getCode();
		BOOST_CHECK_EQUAL(2, code.size());
		BOOST_CHECK_EQUAL(0x0a00, code[0]); // 10
		BOOST_CHECK_EQUAL(0x4e71, code[1]); // nop
	}

	BOOST_AUTO_TEST_CASE(dcb_dcb)
	{
		asmparser parser;
		auto opcode = parser.parseText(" dc.b 10\n dc.b 129\n nop\n");

		const std::vector<uint16_t>& code = parser.getCode();
		BOOST_CHECK_EQUAL(2, code.size());
		BOOST_CHECK_EQUAL(0x0a81, code[0]); // 10 129
		BOOST_CHECK_EQUAL(0x4e71, code[1]); // nop
	}

	BOOST_AUTO_TEST_CASE(dcb_nop_dcb)
	{
		asmparser parser;
		auto opcode = parser.parseText(" dc.b 10\n nop\n dc.b 129\n nop\n");

		const std::vector<uint16_t>& code = parser.getCode();
		BOOST_CHECK_EQUAL(4, code.size());
		BOOST_CHECK_EQUAL(0x0a00, code[0]);
		BOOST_CHECK_EQUAL(0x4e71, code[1]);
		BOOST_CHECK_EQUAL(0x8100, code[2]);
		BOOST_CHECK_EQUAL(0x4e71, code[3]);
	}

	BOOST_AUTO_TEST_CASE(dcb_toolong)
	{
		asmparser parser;
		auto opcode = parser.parseText(" dc.b 1000\n");
		BOOST_CHECK_EQUAL(1, parser.getErrors().get().size());
	}

	// ====================================================================================================
	// DC.W
	// ====================================================================================================
	BOOST_AUTO_TEST_CASE(dcw_byte)
	{
		asmparser parser;
		auto opcode = parser.parseText(" dc.w 42\n");
		validate_hasValue<uint16_t>(0x002A, opcode);
	}

	BOOST_AUTO_TEST_CASE(dcw_negative)
	{
		asmparser parser;
		auto opcode = parser.parseText(" dc.w -2500\n");
		validate_hasValue<uint16_t>(0xf63c, opcode);
	}

	BOOST_AUTO_TEST_CASE(dcw_2bytes)
	{
		asmparser parser;
		auto opcode = parser.parseText(" dc.w $12, $34\n");

		const std::vector<uint16_t>& code = parser.getCode();
		BOOST_CHECK_EQUAL(2, code.size());
		BOOST_CHECK_EQUAL(0x0012, code[0]);
		BOOST_CHECK_EQUAL(0x0034, code[1]);
	}

	BOOST_AUTO_TEST_CASE(dcw_string)
	{
		asmparser parser;
		auto opcode = parser.parseText(" dc.w 'AB'\n");
		validate_hasValue<uint16_t>(0x4142, opcode);
	}

	BOOST_AUTO_TEST_CASE(dcw_multi)
	{
		asmparser parser;
		auto opcode = parser.parseText(" dc.w 'lau', 10, 129\n");

		const std::vector<uint16_t>& code = parser.getCode();
		BOOST_CHECK_EQUAL(4, code.size());
		BOOST_CHECK_EQUAL(0x6C61, code[0]); // 'la'
		BOOST_CHECK_EQUAL(0x7500, code[1]); // 'u'
		BOOST_CHECK_EQUAL(0x000A, code[2]); // 10
		BOOST_CHECK_EQUAL(0x0081, code[3]); // 129
	}
	BOOST_AUTO_TEST_CASE(dcw_toolong)
	{
		asmparser parser;
		auto opcode = parser.parseText(" dc.w $12345\n");
		BOOST_CHECK_EQUAL(1, parser.getErrors().get().size());
	}

	// ====================================================================================================
	// DC.L
	// ====================================================================================================
	BOOST_AUTO_TEST_CASE(dcl_byte)
	{
		asmparser parser;
		auto opcode = parser.parseText(" dc.l 42\n");
		const std::vector<uint16_t>& code = parser.getCode();

		BOOST_CHECK_EQUAL(2, code.size());
		BOOST_CHECK_EQUAL(0, code[0]);
		BOOST_CHECK_EQUAL(42, code[1]);
	}

	BOOST_AUTO_TEST_CASE(dcl_negative)
	{
		asmparser parser;
		auto opcode = parser.parseText(" dc.l -50000\n");
		const std::vector<uint16_t>& code = parser.getCode();

		BOOST_CHECK_EQUAL(2, code.size());
		BOOST_CHECK_EQUAL(0xffff, code[0]);
		BOOST_CHECK_EQUAL(0x3cb0, code[1]);
	}

	BOOST_AUTO_TEST_CASE(dcl_2bytes)
	{
		asmparser parser;
		auto opcode = parser.parseText(" dc.l $12, $34\n");

		const std::vector<uint16_t>& code = parser.getCode();
		BOOST_CHECK_EQUAL(4, code.size());
		BOOST_CHECK_EQUAL(0x0000, code[0]);
		BOOST_CHECK_EQUAL(0x0012, code[1]);
		BOOST_CHECK_EQUAL(0x0000, code[2]);
		BOOST_CHECK_EQUAL(0x0034, code[3]);
	}

	BOOST_AUTO_TEST_CASE(dcl_string)
	{
		asmparser parser;
		auto opcode = parser.parseText(" dc.l 'AB'\n");

		const std::vector<uint16_t>& code = parser.getCode();
		BOOST_CHECK_EQUAL(2, code.size());
		BOOST_CHECK_EQUAL(0x4142, code[0]); // AB
		BOOST_CHECK_EQUAL(0x0000, code[1]);
	}
	BOOST_AUTO_TEST_CASE(dcl_multi)
	{
		asmparser parser;
		auto opcode = parser.parseText(" dc.l 'lau', 10, 129\n");

		const std::vector<uint16_t>& code = parser.getCode();
		BOOST_CHECK_EQUAL(6, code.size());
		BOOST_CHECK_EQUAL(0x6C61, code[0]); // 'la'
		BOOST_CHECK_EQUAL(0x7500, code[1]); // 'u'

		BOOST_CHECK_EQUAL(0x0000, code[2]); // 10
		BOOST_CHECK_EQUAL(0x000A, code[3]); // 10

		BOOST_CHECK_EQUAL(0x0000, code[4]); // 10
		BOOST_CHECK_EQUAL(0x0081, code[5]); // 129
	}
	// ====================================================================================================
	// Expression
	// ====================================================================================================
	BOOST_AUTO_TEST_CASE(dcb_expression)
	{
		asmparser parser;
		auto opcode = parser.parseText(" dc.b (40 + 2)\n");
		validate_hasValue<uint16_t>(0x2A00, opcode);
	}
	// ====================================================================================================
	// Identifier
	// ====================================================================================================
	BOOST_AUTO_TEST_CASE(dcb_identifier)
	{
		asmparser parser;
		auto opcode = parser.parseText(" nop\nlbl: nop\n dc.b lbl\n");

		const std::vector<uint16_t>& code = parser.getCode();
		BOOST_CHECK_EQUAL(3, code.size());
		BOOST_CHECK_EQUAL(0x4e71, code[0]); // nop
		BOOST_CHECK_EQUAL(0x4e71, code[1]); // nop
		BOOST_CHECK_EQUAL(0x0200, code[2]); // lbl
	}
	// ====================================================================================================
	// Directive name
	// ====================================================================================================
	BOOST_AUTO_TEST_CASE(dcb_directivename)
	{
		asmparser parser;
		auto opcode = parser.parseText(" nop\nLIST: nop\n dc.b LIST\n");

		const std::vector<uint16_t>& code = parser.getCode();
		BOOST_CHECK_EQUAL(3, code.size());
		BOOST_CHECK_EQUAL(0x4e71, code[0]); // nop
		BOOST_CHECK_EQUAL(0x4e71, code[1]); // nop
		BOOST_CHECK_EQUAL(0x0200, code[2]); // lbl
	}

	BOOST_AUTO_TEST_SUITE_END()
}