#include <boost/test/unit_test.hpp>
#include "asmparser.h"
#include "util.h"

namespace addressing_mode
{
	BOOST_AUTO_TEST_SUITE(addressing_mode)

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

	BOOST_AUTO_TEST_CASE(address_SP)
	{
		asmparser parser;
		auto opcode = parser.parseText(" add sp, d0\n");
		validate_hasValue<uint16_t>(0b1101'000'001'001'111, opcode);
	}

	BOOST_AUTO_TEST_CASE(address_aReg_indirect)
	{
		asmparser parser;
		auto opcode = parser.parseText(" add (a1), d0\n");
		validate_hasValue<uint16_t>(0b1101'000'001'010'001, opcode);
	}

	BOOST_AUTO_TEST_CASE(address_SP_indirect)
	{
		asmparser parser;
		auto opcode = parser.parseText(" add (SP), d0\n");
		validate_hasValue<uint16_t>(0b1101'000'001'010'111, opcode);
	}

	BOOST_AUTO_TEST_CASE(address_aReg_post)
	{
		asmparser parser;
		auto opcode = parser.parseText(" add (a1)+, d0\n");
		validate_hasValue<uint16_t>(0b1101'000'001'011'001, opcode);
	}

	BOOST_AUTO_TEST_CASE(address_SP_post)
	{
		asmparser parser;
		auto opcode = parser.parseText(" add (SP)+, d0\n");
		validate_hasValue<uint16_t>(0b1101'000'001'011'111, opcode);
	}

	BOOST_AUTO_TEST_CASE(address_aReg_pre)
	{
		asmparser parser;
		auto opcode = parser.parseText(" add -(a1), d0\n");
		validate_hasValue<uint16_t>(0b1101'000'001'100'001, opcode);
	}

	BOOST_AUTO_TEST_CASE(address_SP_pre)
	{
		asmparser parser;
		auto opcode = parser.parseText(" add -(SP), d0\n");
		validate_hasValue<uint16_t>(0b1101'000'001'100'111, opcode);
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

	BOOST_AUTO_TEST_CASE(address_SP_displacement)
	{
		asmparser parser;
		auto opcode = parser.parseText(" add 4(SP), d0\n");
		validate_hasValue<uint16_t>(0b1101'000'001'101'111, opcode);

		const std::vector<uint16_t>& code = parser.getCode();
		BOOST_CHECK_EQUAL(2, code.size());
		BOOST_CHECK_EQUAL(4, code[1]);
	}
	BOOST_AUTO_TEST_CASE(address_aReg_displacement_error)
	{
		asmparser parser;
		auto opcode = parser.parseText(" add $4ffff(a1), d0\n");
		validate_hasValue<uint16_t>(0b1101'000'001'101'001, opcode);

		const std::vector<uint16_t>& code = parser.getCode();
		BOOST_CHECK_EQUAL(2, code.size());
		BOOST_CHECK_EQUAL(1, parser.getErrors().get().size());
	}

	BOOST_AUTO_TEST_CASE(address_aReg_displacement_new)
	{
		asmparser parser;
		auto opcode = parser.parseText(" add (4,a1), d0\n");
		validate_hasValue<uint16_t>(0b1101'000'001'101'001, opcode);

		const std::vector<uint16_t>& code = parser.getCode();
		BOOST_CHECK_EQUAL(2, code.size());
		BOOST_CHECK_EQUAL(4, code[1]);
	}

	BOOST_AUTO_TEST_CASE(address_SP_displacement_new)
	{
		asmparser parser;
		auto opcode = parser.parseText(" add (4,SP), d0\n");
		validate_hasValue<uint16_t>(0b1101'000'001'101'111, opcode);

		const std::vector<uint16_t>& code = parser.getCode();
		BOOST_CHECK_EQUAL(2, code.size());
		BOOST_CHECK_EQUAL(4, code[1]);
	}

	BOOST_AUTO_TEST_CASE(address_aReg_indexD)
	{
		asmparser parser;
		auto opcode = parser.parseText(" add 4(a1, d2), d0\n");
		validate_hasValue<uint16_t>(0b1101'000'001'110'001, opcode);

		const std::vector<uint16_t>& code = parser.getCode();
		BOOST_CHECK_EQUAL(2, code.size());
		BOOST_CHECK_EQUAL(0b0'010'0'000'00000100, code[1]);
	}

	BOOST_AUTO_TEST_CASE(address_aReg_indexA)
	{
		asmparser parser;
		auto opcode = parser.parseText(" add 4(a1, a2), d0\n");
		validate_hasValue<uint16_t>(0b1101'000'001'110'001, opcode);

		const std::vector<uint16_t>& code = parser.getCode();
		BOOST_CHECK_EQUAL(2, code.size());
		BOOST_CHECK_EQUAL(0b1'010'0'000'00000100, code[1]);
	}

	BOOST_AUTO_TEST_CASE(address_aReg_indexSP)
	{
		asmparser parser;
		auto opcode = parser.parseText(" add 4(a1, SP), d0\n");
		validate_hasValue<uint16_t>(0b1101'000'001'110'001, opcode);

		const std::vector<uint16_t>& code = parser.getCode();
		BOOST_CHECK_EQUAL(2, code.size());
		BOOST_CHECK_EQUAL(0b1'111'0'000'00000100, code[1]);
	}

	BOOST_AUTO_TEST_CASE(address_aReg_index_new)
	{
		asmparser parser;
		auto opcode = parser.parseText(" add (4, a1, d2), d0\n");
		validate_hasValue<uint16_t>(0b1101'000'001'110'001, opcode);

		const std::vector<uint16_t>& code = parser.getCode();
		BOOST_CHECK_EQUAL(2, code.size());
		BOOST_CHECK_EQUAL(0b0'010'0'000'00000100, code[1]);
	}

	BOOST_AUTO_TEST_CASE(address_SP_index)
	{
		asmparser parser;
		auto opcode = parser.parseText(" add 4(SP, d2), d0\n");
		validate_hasValue<uint16_t>(0b1101'000'001'110'111, opcode);

		const std::vector<uint16_t>& code = parser.getCode();
		BOOST_CHECK_EQUAL(2, code.size());
		BOOST_CHECK_EQUAL(0b0'010'0'000'00000100, code[1]);
	}

	BOOST_AUTO_TEST_CASE(address_SP_index_new)
	{
		asmparser parser;
		auto opcode = parser.parseText(" add (4, SP, d2), d0\n");
		validate_hasValue<uint16_t>(0b1101'000'001'110'111, opcode);

		const std::vector<uint16_t>& code = parser.getCode();
		BOOST_CHECK_EQUAL(2, code.size());
		BOOST_CHECK_EQUAL(0b0'010'0'000'00000100, code[1]);
	}

	BOOST_AUTO_TEST_CASE(address_aReg_index_error)
	{
		asmparser parser;
		auto opcode = parser.parseText(" add (420, a1, d2), d0\n");
		validate_hasValue<uint16_t>(0b1101'000'001'110'001, opcode);

		const std::vector<uint16_t>& code = parser.getCode();
		BOOST_CHECK_EQUAL(2, code.size());
		BOOST_CHECK_EQUAL(0b0'010'0'000'10100100, code[1]);
		// TODO: Check for error message
		BOOST_CHECK_EQUAL(1, parser.getErrors().get().size());
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

	BOOST_AUTO_TEST_CASE(address_aReg_index_byte)
	{
		asmparser parser;
		auto opcode = parser.parseText(" add 6(a1, a3.b), d0\n");
		BOOST_CHECK_EQUAL(1, parser.getErrors().get().size());

	}

	BOOST_AUTO_TEST_CASE(address_absoluteW)
	{
		asmparser parser;
		auto opcode = parser.parseText(" add $1234.W, d0\n");
		validate_hasValue<uint16_t>(0b1101'000'001'111'000, opcode);

		const std::vector<uint16_t>& code = parser.getCode();
		BOOST_CHECK_EQUAL(2, code.size());
		BOOST_CHECK_EQUAL(0x1234, code[1]);
	}

	BOOST_AUTO_TEST_CASE(address_absoluteWDefault)
	{
		asmparser parser;
		auto opcode = parser.parseText(" add $1234, d0\n");
		validate_hasValue<uint16_t>(0b1101'000'001'111'000, opcode);

		const std::vector<uint16_t>& code = parser.getCode();
		BOOST_CHECK_EQUAL(2, code.size());
		BOOST_CHECK_EQUAL(0x1234, code[1]);
	}

	BOOST_AUTO_TEST_CASE(address_absoluteW_error)
	{
		asmparser parser;
		auto opcode = parser.parseText(" add $12345.W, d0\n");
		BOOST_CHECK_EQUAL(1, parser.getErrors().get().size());
	}

	BOOST_AUTO_TEST_CASE(address_labelW)
	{
		asmparser parser;
		auto opcode = parser.parseText(" add data, d0\n add #1, d0\n nop\n nop\ndata:\n");
		validate_hasValue<uint16_t>(0b1101'000'001'111'000, opcode);

		const std::vector<uint16_t>& code = parser.getCode();
		BOOST_CHECK_EQUAL(6, code.size());
		BOOST_CHECK_EQUAL(0xc,    code[1]);
		BOOST_CHECK_EQUAL(0xd07c, code[2]);
		BOOST_CHECK_EQUAL(0x1,    code[3]);
		BOOST_CHECK_EQUAL(0x4e71, code[4]);
		BOOST_CHECK_EQUAL(0x4e71, code[5]);
	}

	BOOST_AUTO_TEST_CASE(address_labelW_error)
	{
		asmparser parser;
		auto opcode = parser.parseText(" add data, d0\n");
		BOOST_CHECK_EQUAL(1, parser.getErrors().get().size());
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

	BOOST_AUTO_TEST_CASE(address_absoluteLDefault)
	{
		asmparser parser;
		auto opcode = parser.parseText(" add $12345678, d0\n");
		validate_hasValue<uint16_t>(0b1101'000'001'111'001, opcode);

		const std::vector<uint16_t>& code = parser.getCode();
		BOOST_CHECK_EQUAL(3, code.size());
		BOOST_CHECK_EQUAL(0x1234, code[1]);
		BOOST_CHECK_EQUAL(0x5678, code[2]);
	}

	BOOST_AUTO_TEST_CASE(address_pc_displacement)
	{
		asmparser parser;
		auto opcode = parser.parseText(" add 42(pc), d0\n");
		validate_hasValue<uint16_t>(0b1101'000'001'111'010, opcode);

		const std::vector<uint16_t>& code = parser.getCode();
		BOOST_CHECK_EQUAL(2, code.size());
		BOOST_CHECK_EQUAL(42, code[1]);
	}

	BOOST_AUTO_TEST_CASE(address_pc_displacement_error)
	{
		asmparser parser;
		auto opcode = parser.parseText(" add $8ffff(pc), d0\n");
		BOOST_CHECK_EQUAL(1, parser.getErrors().get().size());
	}

	BOOST_AUTO_TEST_CASE(address_pc_indexD)
	{
		asmparser parser;
		auto opcode = parser.parseText(" add 4(pc, d2), d0\n");
		validate_hasValue<uint16_t>(0b1101'000'001'111'011, opcode);

		const std::vector<uint16_t>& code = parser.getCode();
		BOOST_CHECK_EQUAL(2, code.size());
		BOOST_CHECK_EQUAL(0b0'010'0'000'00000100, code[1]);
	}

	BOOST_AUTO_TEST_CASE(address_pc_indexA)
	{
		asmparser parser;
		auto opcode = parser.parseText(" add 4(pc, a2), d0\n");
		validate_hasValue<uint16_t>(0b1101'000'001'111'011, opcode);

		const std::vector<uint16_t>& code = parser.getCode();
		BOOST_CHECK_EQUAL(2, code.size());
		BOOST_CHECK_EQUAL(0b1'010'0'000'00000100, code[1]);
	}
	BOOST_AUTO_TEST_CASE(address_pc_indexSP)
	{
		asmparser parser;
		auto opcode = parser.parseText(" add 4(pc, SP), d0\n");
		validate_hasValue<uint16_t>(0b1101'000'001'111'011, opcode);
		const std::vector<uint16_t>& code = parser.getCode();
		BOOST_CHECK_EQUAL(2, code.size());
		BOOST_CHECK_EQUAL(0b1'111'0'000'00000100, code[1]);
	}

	BOOST_AUTO_TEST_CASE(address_pc_index_error)
	{
		asmparser parser;
		auto opcode = parser.parseText(" add 421(pc, d2), d0\n");
		BOOST_CHECK_EQUAL(1, parser.getErrors().get().size());
	}

	BOOST_AUTO_TEST_CASE(address_immediate_short)
	{
		asmparser parser;
		auto opcode = parser.parseText(" add.b #4, d0\n");
		validate_hasValue<uint16_t>(0b1101'000'000'111'100, opcode);

		const std::vector<uint16_t>& code = parser.getCode();
		BOOST_CHECK_EQUAL(2, code.size());
		BOOST_CHECK_EQUAL(0x4, code[1]);
	}

	BOOST_AUTO_TEST_CASE(address_immediate_char)
	{
		asmparser parser;
		auto opcode = parser.parseText(" add.b #'A', d0\n");
		validate_hasValue<uint16_t>(0b1101'000'000'111'100, opcode);

		const std::vector<uint16_t>& code = parser.getCode();
		BOOST_CHECK_EQUAL(2, code.size());
		BOOST_CHECK_EQUAL(0x41, code[1]);
	}

	BOOST_AUTO_TEST_CASE(address_immediate_short_error)
	{
		asmparser parser;
		auto opcode = parser.parseText(" add.b #747, d0\n");
		BOOST_CHECK_EQUAL(1, parser.getErrors().get().size());
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

	BOOST_AUTO_TEST_CASE(address_immediate_error)
	{
		asmparser parser;
		auto opcode = parser.parseText(" add.w #$87654, d0\n");
		BOOST_CHECK_EQUAL(1, parser.getErrors().get().size());
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
}