#include <boost/test/unit_test.hpp>
#include "asmparser.h"
#include "util.h"

namespace directiveTest
{
	BOOST_AUTO_TEST_SUITE(directiveTest)

	// ====================================================================================================
	// DIRECTIVE TESTS
	// ====================================================================================================
	BOOST_AUTO_TEST_CASE(dc_hasLabel)
	{
		asmparser parser;
		auto opcode = parser.parseText("lbl dc.b 10\n");

		validate_noErrors(parser);
		validate_labelsCount(parser, 1);
	}
		
	BOOST_AUTO_TEST_CASE(dc_labelUsable)
	{
		asmparser parser;
		parser.parseText(" nop\nLBL dc.b 42,42\n lea LBL,a0\n");

		validate_noErrors(parser);
		validate_labelsCount(parser, 1);
		const auto& code = validate_codeSize(parser, 4);
		BOOST_CHECK_EQUAL(0x4e71, code[0]); // nop
		BOOST_CHECK_EQUAL(0x2a2a, code[1]); // 42,42
		BOOST_CHECK_EQUAL(0x41f8, code[2]); // lea instruction
		BOOST_CHECK_EQUAL(0x0002, code[3]); // LBL address
	}
	// ====================================================================================================
	// DC.B
	// ====================================================================================================
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

		const auto& code = validate_codeSize(parser, 2);
		BOOST_CHECK_EQUAL(0x4C49, code[0]); // 'LI'
		BOOST_CHECK_EQUAL(0x5354, code[1]); // 'ST'
	}

	BOOST_AUTO_TEST_CASE(dcb_stringByte)
	{
		asmparser parser;
		auto opcode = parser.parseText(" DC.B   'ABC', $ff\n");
		validate_hasValue<uint16_t>(0x43ff, opcode);

		const auto& code = validate_codeSize(parser, 2);
		BOOST_CHECK_EQUAL(0x4142, code[0]); // 'AB'
		BOOST_CHECK_EQUAL(0x43ff, code[1]); // 'C' + $ff
	}

	BOOST_AUTO_TEST_CASE(dcb_multi)
	{
		asmparser parser;
		auto opcode = parser.parseText(" dc.b 'lau', 10, 129\n");

		const auto& code = validate_codeSize(parser, 3);
		BOOST_CHECK_EQUAL(0x6C61, code[0]); // 'la'
		BOOST_CHECK_EQUAL(0x750A, code[1]); // 'u' 10
		BOOST_CHECK_EQUAL(0x8100, code[2]); // 129
	}

	BOOST_AUTO_TEST_CASE(dcb_mixed)
	{
		asmparser parser;
		auto opcode = parser.parseText(" dc.b 10\n nop\n");

		const auto& code = validate_codeSize(parser, 2);
		BOOST_CHECK_EQUAL(0x0a00, code[0]); // 10
		BOOST_CHECK_EQUAL(0x4e71, code[1]); // nop
	}

	BOOST_AUTO_TEST_CASE(dcb_dcb)
	{
		asmparser parser;
		auto opcode = parser.parseText(" dc.b 10\n dc.b 129\n nop\n");

		const auto& code = validate_codeSize(parser, 2);
		BOOST_CHECK_EQUAL(0x0a81, code[0]); // 10 129
		BOOST_CHECK_EQUAL(0x4e71, code[1]); // nop
	}

	BOOST_AUTO_TEST_CASE(dcb_nop_dcb)
	{
		asmparser parser;
		auto opcode = parser.parseText(" dc.b 10\n nop\n dc.b 129\n nop\n");

		const auto& code = validate_codeSize(parser, 4);
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

		const auto& code = validate_codeSize(parser, 2);
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

		const auto& code = validate_codeSize(parser, 4);
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

	BOOST_AUTO_TEST_CASE(dcw_address)
	{
		asmparser parser;
		auto opcode = parser.parseText(" bra label\n dc.w $ffff\nlabel: nop\n");

		const auto& code = validate_codeSize(parser, 4);
		BOOST_CHECK_EQUAL(0x6000, code[0]); // bra label
		BOOST_CHECK_EQUAL(0x0004, code[1]); // 
		BOOST_CHECK_EQUAL(0xffff, code[2]); // dc.w $ffff
		BOOST_CHECK_EQUAL(0x4e71, code[3]); // nop
	}

	// ====================================================================================================
	// DC.L
	// ====================================================================================================
	BOOST_AUTO_TEST_CASE(dcl_byte)
	{
		asmparser parser;
		auto opcode = parser.parseText(" dc.l 42\n");

		const auto& code = validate_codeSize(parser, 2);
		BOOST_CHECK_EQUAL(0, code[0]);
		BOOST_CHECK_EQUAL(42, code[1]);
	}

	BOOST_AUTO_TEST_CASE(dcl_negative)
	{
		asmparser parser;
		auto opcode = parser.parseText(" dc.l -50000\n");

		const auto& code = validate_codeSize(parser, 2);
		BOOST_CHECK_EQUAL(0xffff, code[0]);
		BOOST_CHECK_EQUAL(0x3cb0, code[1]);
	}

	BOOST_AUTO_TEST_CASE(dcl_2bytes)
	{
		asmparser parser;
		auto opcode = parser.parseText(" dc.l $12, $34\n");

		const auto& code = validate_codeSize(parser, 4);
		BOOST_CHECK_EQUAL(0x0000, code[0]);
		BOOST_CHECK_EQUAL(0x0012, code[1]);
		BOOST_CHECK_EQUAL(0x0000, code[2]);
		BOOST_CHECK_EQUAL(0x0034, code[3]);
	}

	BOOST_AUTO_TEST_CASE(dcl_string)
	{
		asmparser parser;
		auto opcode = parser.parseText(" dc.l 'AB'\n");

		const auto& code = validate_codeSize(parser, 2);
		BOOST_CHECK_EQUAL(0x4142, code[0]); // AB
		BOOST_CHECK_EQUAL(0x0000, code[1]);
	}
	BOOST_AUTO_TEST_CASE(dcl_multi)
	{
		asmparser parser;
		auto opcode = parser.parseText(" dc.l 'lau', 10, 129\n");

		const auto& code = validate_codeSize(parser, 6);
		BOOST_CHECK_EQUAL(0x6C61, code[0]); // 'la'
		BOOST_CHECK_EQUAL(0x7500, code[1]); // 'u'

		BOOST_CHECK_EQUAL(0x0000, code[2]); // 10
		BOOST_CHECK_EQUAL(0x000A, code[3]); // 10

		BOOST_CHECK_EQUAL(0x0000, code[4]); // 10
		BOOST_CHECK_EQUAL(0x0081, code[5]); // 129
	}
	BOOST_AUTO_TEST_CASE(dcl_address)
	{
		asmparser parser;
		auto opcode = parser.parseText(" bra label\n dc.l $ffffeeee\nlabel: nop\n");

		const auto& code = validate_codeSize(parser, 5);
		BOOST_CHECK_EQUAL(0x6000, code[0]); // bra label
		BOOST_CHECK_EQUAL(0x0006, code[1]); // 
		BOOST_CHECK_EQUAL(0xffff, code[2]); // dc.l $ffffeeee
		BOOST_CHECK_EQUAL(0xeeee, code[3]); // 
		BOOST_CHECK_EQUAL(0x4e71, code[4]); // nop
	}
	// ====================================================================================================
	// EQU
	// ====================================================================================================
	BOOST_AUTO_TEST_CASE(equ_instruction)
	{
		asmparser parser;
		parser.parseText("X EQU 42\n moveq #X,d0\n");

		BOOST_CHECK_EQUAL(0, parser.getErrors().get().size());
		const auto& code = validate_codeSize(parser, 1);
		BOOST_CHECK_EQUAL(0x702A, code[0]);
	}

	BOOST_AUTO_TEST_CASE(equ_directive)
	{
		asmparser parser;
		parser.parseText("X EQU 42\n dc.b X,X\n");

		BOOST_CHECK_EQUAL(0, parser.getErrors().get().size());
		const auto& code = validate_codeSize(parser, 1);
		BOOST_CHECK_EQUAL(0x2A2A, code[0]);
	}

	BOOST_AUTO_TEST_CASE(equ_mixed)
	{
		asmparser parser;
		parser.parseText("CR EQU $0d \nCODE CMPI.B #CR,(A0) \n DC.B CR\n");

		BOOST_CHECK_EQUAL(0, parser.getErrors().get().size());

		const auto& code = validate_codeSize(parser, 3);
		BOOST_CHECK_EQUAL(0x0c10, code[0]);
		BOOST_CHECK_EQUAL(0x000d, code[1]);
		BOOST_CHECK_EQUAL(0x0d00, code[2]);
	}
	BOOST_AUTO_TEST_CASE(equ_expression)
	{
		asmparser parser;
		parser.parseText("X EQU 'x' \nY EQU 'y' \n DC.B X,Y\n");

		BOOST_CHECK_EQUAL(0, parser.getErrors().get().size());

		const auto& code = validate_codeSize(parser, 1);
		BOOST_CHECK_EQUAL(0x7879, code[0]);
	}

	BOOST_AUTO_TEST_CASE(equ_numeric_expression)
	{
		asmparser parser;
		parser.parseText("X EQU 42 \nY EQU 43 \n DC.B X+Y\n");

		BOOST_CHECK_EQUAL(0, parser.getErrors().get().size());

		const auto& code = validate_codeSize(parser, 1);
		BOOST_CHECK_EQUAL(0x5500, code[0]);
	}

	BOOST_AUTO_TEST_CASE(equ_symbol_label)
	{
		asmparser parser;
		parser.parseText("X EQU 42 \nX nop\n");

		BOOST_CHECK_EQUAL(1, parser.getErrors().get().size());
	}
	BOOST_AUTO_TEST_CASE(equ_label_symbol)
	{
		asmparser parser;
		parser.parseText("X nop \nX EQU 42 \n");

		BOOST_CHECK_EQUAL(1, parser.getErrors().get().size());
	}
	// ====================================================================================================
	// DS
	// ====================================================================================================
	BOOST_AUTO_TEST_CASE(dsb)
	{
		asmparser parser;
		parser.parseText(" nop\nfrom ds.b 2\nto lea from,a0\n lea to,a1\n");

		validate_noErrors(parser);
		validate_labelsCount(parser, 2);
		const auto& code = validate_codeSize(parser, 6);
		BOOST_CHECK_EQUAL(0x4e71, code[0]);
		BOOST_CHECK_EQUAL(0x0000, code[1]);
		BOOST_CHECK_EQUAL(0x41f8, code[2]);
		BOOST_CHECK_EQUAL(0x0002, code[3]);
		BOOST_CHECK_EQUAL(0x43f8, code[4]);
		BOOST_CHECK_EQUAL(0x0004, code[5]);
	}

	BOOST_AUTO_TEST_CASE(dsb_symbol_count)
	{
		asmparser parser;
		parser.parseText("count EQU 2\n nop\nfrom ds.b count\nto lea from,a0\n lea to,a1\n");
		validate_noErrors(parser);
		validate_labelsCount(parser, 2);
		const auto& code = validate_codeSize(parser, 6);
		BOOST_CHECK_EQUAL(0x4e71, code[0]);
		BOOST_CHECK_EQUAL(0x0000, code[1]);
		BOOST_CHECK_EQUAL(0x41f8, code[2]);
		BOOST_CHECK_EQUAL(0x0002, code[3]);
		BOOST_CHECK_EQUAL(0x43f8, code[4]);
		BOOST_CHECK_EQUAL(0x0004, code[5]);
	}

	BOOST_AUTO_TEST_CASE(dsb_align)
	{
		asmparser parser;
		parser.parseText(" dc.b '?'\nfrom ds.b 2\nto lea from,a0\n lea to,a1\n");

		validate_noErrors(parser);
		validate_labelsCount(parser, 2);
		const auto& code = validate_codeSize(parser, 6);
		BOOST_CHECK_EQUAL(0x3f00, code[0]);
		BOOST_CHECK_EQUAL(0x0000, code[1]);
		BOOST_CHECK_EQUAL(0x41f8, code[2]);
		BOOST_CHECK_EQUAL(0x0001, code[3]);
		BOOST_CHECK_EQUAL(0x43f8, code[4]);
		BOOST_CHECK_EQUAL(0x0004, code[5]);
	}
	BOOST_AUTO_TEST_CASE(dsw)
	{
		asmparser parser;
		parser.parseText(" nop\nfrom ds.w 2\nto lea from,a0\n lea to,a1\n");

		validate_noErrors(parser);
		validate_labelsCount(parser, 2);
		const auto& code = validate_codeSize(parser, 7);
		BOOST_CHECK_EQUAL(0x4e71, code[0]);
		BOOST_CHECK_EQUAL(0x0000, code[1]);
		BOOST_CHECK_EQUAL(0x0000, code[2]);
		BOOST_CHECK_EQUAL(0x41f8, code[3]);
		BOOST_CHECK_EQUAL(0x0002, code[4]);
		BOOST_CHECK_EQUAL(0x43f8, code[5]);
		BOOST_CHECK_EQUAL(0x0006, code[6]);
	}
	BOOST_AUTO_TEST_CASE(dsw_align)
	{
		asmparser parser;
		parser.parseText(" dc.b '?'\nfrom ds.w 2\nto lea from,a0\n lea to,a1\n");

		validate_noErrors(parser);
		validate_labelsCount(parser, 2);
		const auto& code = validate_codeSize(parser, 7);
		BOOST_CHECK_EQUAL(0x3f00, code[0]);
		BOOST_CHECK_EQUAL(0x0000, code[1]);
		BOOST_CHECK_EQUAL(0x0000, code[2]);
		BOOST_CHECK_EQUAL(0x41f8, code[3]);
		BOOST_CHECK_EQUAL(0x0002, code[4]);
		BOOST_CHECK_EQUAL(0x43f8, code[5]);
		BOOST_CHECK_EQUAL(0x0006, code[6]);
	}

	BOOST_AUTO_TEST_CASE(dsl)
	{
		asmparser parser;
		parser.parseText(" nop\nfrom ds.l 2\nto lea from,a0\n lea to,a1\n");

		validate_noErrors(parser);
		validate_labelsCount(parser, 2);
		const auto& code = validate_codeSize(parser, 9);
		BOOST_CHECK_EQUAL(0x4e71, code[0]);
		BOOST_CHECK_EQUAL(0x0000, code[1]);
		BOOST_CHECK_EQUAL(0x0000, code[2]);
		BOOST_CHECK_EQUAL(0x0000, code[3]);
		BOOST_CHECK_EQUAL(0x0000, code[4]);
		BOOST_CHECK_EQUAL(0x41f8, code[5]);
		BOOST_CHECK_EQUAL(0x0002, code[6]);
		BOOST_CHECK_EQUAL(0x43f8, code[7]);
		BOOST_CHECK_EQUAL(0x000a, code[8]);
	}

	BOOST_AUTO_TEST_CASE(dsl_align)
	{
		asmparser parser;
		parser.parseText(" dc.b '?'\nfrom ds.l 2\nto lea from,a0\n lea to,a1\n");

		validate_noErrors(parser);
		validate_labelsCount(parser, 2);
		const auto& code = validate_codeSize(parser, 9);
		BOOST_CHECK_EQUAL(0x3f00, code[0]);
		BOOST_CHECK_EQUAL(0x0000, code[1]);
		BOOST_CHECK_EQUAL(0x0000, code[2]);
		BOOST_CHECK_EQUAL(0x0000, code[3]);
		BOOST_CHECK_EQUAL(0x0000, code[4]);
		BOOST_CHECK_EQUAL(0x41f8, code[5]);
		BOOST_CHECK_EQUAL(0x0002, code[6]);
		BOOST_CHECK_EQUAL(0x43f8, code[7]);
		BOOST_CHECK_EQUAL(0x000a, code[8]);
	}

	BOOST_AUTO_TEST_CASE(ds_negative_count)
	{
		asmparser parser;
		parser.parseText("  ds -20\n");
		validate_errorsCount(parser, 2);
	}
	// ====================================================================================================
	// ORG
	// ====================================================================================================
	BOOST_AUTO_TEST_CASE(org_number)
	{
		asmparser parser;
		parser.parseText(" org $2A00\n lea *,a0\n");

		validate_noErrors(parser);
		const auto& code = validate_codeSize(parser, 2);
		BOOST_CHECK_EQUAL(0x41F8, code[0]);
		BOOST_CHECK_EQUAL(0x2A00, code[1]);
	}
	BOOST_AUTO_TEST_CASE(org_number_negative)
	{
		asmparser parser;
		parser.parseText(" org -100\n");

		validate_errorsCount(parser, 1);
	}

	BOOST_AUTO_TEST_CASE(org_symbol)
	{
		asmparser parser;
		parser.parseText("START EQU $2A00\n org START\n lea *,a0\n");

		validate_noErrors(parser);
		const auto& code = validate_codeSize(parser, 2);
		BOOST_CHECK_EQUAL(0x41F8, code[0]);
		BOOST_CHECK_EQUAL(0x2A00, code[1]);
	}

	BOOST_AUTO_TEST_CASE(org_symbol_string)
	{
		asmparser parser;
		parser.parseText("START EQU 'start'\n org START\n");

		validate_errorsCount(parser, 1);
	}

	BOOST_AUTO_TEST_CASE(org_symbol_negative)
	{
		asmparser parser;
		parser.parseText("START EQU -100\n org START\n");

		validate_errorsCount(parser, 1);
	}

	BOOST_AUTO_TEST_CASE(org_multi)
	{
		asmparser parser;
		parser.parseText(" org $1000\n nop\n org $2000\n nop\n");

		validate_noErrors(parser);

		const auto& codeBlocks = parser.getCodeBlocks();
		BOOST_CHECK_EQUAL(2, codeBlocks.size());
		BOOST_CHECK_EQUAL(0x1000, codeBlocks[0].origin);
		BOOST_CHECK_EQUAL(0x2000, codeBlocks[1].origin);
	}

	BOOST_AUTO_TEST_CASE(org_unknown)
	{
		asmparser parser;
		parser.parseText(" org START\n");

		validate_errorsCount(parser, 1);
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

	BOOST_AUTO_TEST_CASE(dcb_expression2)
	{
		asmparser parser;
		parser.parseText(" DC.B   'LIS',   ('T'+$80) \n");

		const auto& code = validate_codeSize(parser, 2);
		BOOST_CHECK_EQUAL(0x4C49, code[0]); // LI
		BOOST_CHECK_EQUAL(0x53D4, code[1]); // S, 'T' + $80
	}

	BOOST_AUTO_TEST_CASE(dcb_expression3)
	{
		asmparser parser;
		parser.parseText(" DC.B   ':',PR2-*\nPR2 NOP\n");

		const auto& code = validate_codeSize(parser, 2);
		BOOST_CHECK_EQUAL(0x3A01, code[0]); // : PR2-*
		BOOST_CHECK_EQUAL(0x4E71, code[1]); // NOP
	}

	BOOST_AUTO_TEST_CASE(dcb_expression_complex)
	{
		asmparser parser;
		auto opcode = parser.parseText(" dc.b 7 + (40 + 2 - 7)\n");
		validate_hasValue<uint16_t>(0x2A00, opcode);
	}

	BOOST_AUTO_TEST_CASE(dcb_expression_string)
	{
		asmparser parser;
		parser.parseText(" dc.b 'a' + 'bb'\n");
		auto errors = parser.getErrors().get();
		BOOST_CHECK_EQUAL(1, errors.size());
	}

	BOOST_AUTO_TEST_CASE(dcb_star)
	{
		asmparser parser;
		auto opcode = parser.parseText(" dc.b *\n nop\n");
		validate_hasValue<uint16_t>(0x00, opcode);
	}
	BOOST_AUTO_TEST_CASE(dcb_expr_star)
	{
		asmparser parser;
		auto opcode = parser.parseText(" dc.b lbl - *\n nop\nlbl nop\n");

		const auto& code = validate_codeSize(parser, 3);
		BOOST_CHECK_EQUAL(0x0400, code[0]); // lbl-*
		BOOST_CHECK_EQUAL(0x4e71, code[1]); // nop
		BOOST_CHECK_EQUAL(0x4e71, code[2]); // nop
	}
	// ====================================================================================================
	// Identifier
	// ====================================================================================================
	BOOST_AUTO_TEST_CASE(dcb_identifier)
	{
		asmparser parser;
		auto opcode = parser.parseText(" nop\nlbl: nop\n dc.b lbl\n");

		const auto& code = validate_codeSize(parser, 3);
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

		const auto& code = validate_codeSize(parser, 3);
		BOOST_CHECK_EQUAL(0x4e71, code[0]); // nop
		BOOST_CHECK_EQUAL(0x4e71, code[1]); // nop
		BOOST_CHECK_EQUAL(0x0200, code[2]); // lbl
	}

	BOOST_AUTO_TEST_CASE(dcb_labelmissing)
	{
		asmparser parser;
		auto opcode = parser.parseText("here dc.b John\n");

		auto errors = parser.getErrors().get();
		BOOST_CHECK_EQUAL(1, errors.size());
		BOOST_CHECK_NE(0, std::get<0>(errors[0].getPosition()));
		BOOST_CHECK_NE(0, std::get<1>(errors[0].getPosition()));
	}

	BOOST_AUTO_TEST_SUITE_END()
}