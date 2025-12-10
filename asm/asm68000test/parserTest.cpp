#include <boost/test/unit_test.hpp>
#include "asmparser.h"
#include "util.h"
namespace parserTest
{
	BOOST_AUTO_TEST_SUITE(parserTest)

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

	BOOST_AUTO_TEST_CASE(label_empty)
	{
		asmparser parser;
		auto opcode = parser.parseText(" move #1,d0\n");

		auto labels = parser.getLabels();
		BOOST_CHECK_EQUAL(0, labels.size());
	}

	BOOST_AUTO_TEST_CASE(label1_syntax)
	{
		asmparser parser;
		auto error = parser.checkSyntax("label move #1,d0\n");
		BOOST_CHECK_EQUAL(0, error);
	}

	BOOST_AUTO_TEST_CASE(label1_exists)
	{
		asmparser parser;
		auto opcode = parser.parseText("label move #1,d0\n");

		auto labels = parser.getLabels();
		BOOST_CHECK_EQUAL(1, labels.size());
		BOOST_CHECK(labels.find("label") != labels.end());
	}

	BOOST_AUTO_TEST_CASE(label2_syntax)
	{
		asmparser parser;
		auto error = parser.checkSyntax("label: move #1,d0\n");
		BOOST_CHECK_EQUAL(0, error);
	}

	BOOST_AUTO_TEST_CASE(label2_exists)
	{
		asmparser parser;
		auto opcode = parser.parseText("label: move #1,d0\n");

		auto labels = parser.getLabels();
		BOOST_CHECK_EQUAL(1, labels.size());
		BOOST_CHECK(labels.find("label") != labels.end());
	}

	BOOST_AUTO_TEST_CASE(label3_syntax)
	{
		asmparser parser;
		auto error = parser.checkSyntax(" label: move #1,d0\n");
		BOOST_CHECK_EQUAL(0, error);
	}

	BOOST_AUTO_TEST_CASE(label3_exists)
	{
		asmparser parser;
		auto opcode = parser.parseText(" label: move #1,d0\n");

		auto labels = parser.getLabels();
		BOOST_CHECK_EQUAL(1, labels.size());
		BOOST_CHECK(labels.find("label") != labels.end());
	}

	BOOST_AUTO_TEST_CASE(label4_syntax)
	{
		asmparser parser;
		auto error = parser.checkSyntax(" label:\n move #1,d0\n");
		BOOST_CHECK_EQUAL(0, error);
	}

	BOOST_AUTO_TEST_CASE(label4_exists)
	{
		asmparser parser;
		auto opcode = parser.parseText("label:\n move #1,d0\n");

		auto labels = parser.getLabels();
		BOOST_CHECK_EQUAL(1, labels.size());
		BOOST_CHECK(labels.find("label") != labels.end());
	}

	BOOST_AUTO_TEST_CASE(label_code_comment)
	{
		asmparser parser;
		auto opcode = parser.parseText("label:\n move #1,d0  ; This is a move instruction\n");

		auto labels = parser.getLabels();
		BOOST_CHECK_EQUAL(1, labels.size());
		BOOST_CHECK(labels.find("label") != labels.end());
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
	BOOST_AUTO_TEST_CASE(label_comment)
	{
		asmparser parser;
		auto opcode = parser.parseText("here:   ; comment\n");
		validate_noErrors(parser);
	}

	BOOST_AUTO_TEST_CASE(code_save)
	{
		asmparser parser;
		parser.parseText(" org $1000\nstart: nop\n lea $4242,a0\n end start\n");
		validate_noErrors(parser);
		const asmResult& result = parser.getCode68000();
		BOOST_CHECK_EQUAL(0x1000, result.code.front().origin);
		BOOST_CHECK_EQUAL(0x1000, result.start);
		BOOST_CHECK_EQUAL(3, result.code.front().code.size());

		bool saved = result.saveBinary("test.bin");
		BOOST_CHECK(saved);

		asmResult loadedCode;
		bool loaded = loadedCode.loadBinary("test.bin");
		BOOST_CHECK(loaded);
		BOOST_CHECK_EQUAL(result.code.front().origin, loadedCode.code.front().origin);
		BOOST_CHECK_EQUAL(result.start, loadedCode.start);
		BOOST_CHECK_EQUAL(result.code.size(), loadedCode.code.size());
		for (size_t i = 0; i < result.code.front().code.size(); ++i)
		{
			BOOST_CHECK_EQUAL(result.code.front().code[i], loadedCode.code.front().code[i]);
		}
	}
    BOOST_AUTO_TEST_CASE(code_memory)
    {
        asmparser parser;
        parser.parseText(" memory $100,$300\n org $120\nstart: nop\n lea $4242,a0\n end start\n");
        validate_noErrors(parser);
        const asmResult& result = parser.getCode68000();
        BOOST_CHECK_EQUAL(0x120, result.start);
        BOOST_CHECK_EQUAL(0x100, result.memoryStart);
        BOOST_CHECK_EQUAL(0x300, result.memoryEnd);
        BOOST_CHECK_EQUAL(0x120, result.code.front().origin);
        BOOST_CHECK_EQUAL(3, result.code.front().code.size());

        bool saved = result.saveBinary("test.bin");
        BOOST_CHECK(saved);

        asmResult loadedCode;
        bool loaded = loadedCode.loadBinary("test.bin");
        BOOST_CHECK(loaded);
        BOOST_CHECK_EQUAL(result.start, loadedCode.start);
        BOOST_CHECK_EQUAL(result.memoryStart, loadedCode.memoryStart);
        BOOST_CHECK_EQUAL(result.memoryEnd, loadedCode.memoryEnd);
        BOOST_CHECK_EQUAL(result.code.front().origin, loadedCode.code.front().origin);
        BOOST_CHECK_EQUAL(result.code.size(), loadedCode.code.size());
        for (size_t i = 0; i < result.code.front().code.size(); ++i)
        {
            BOOST_CHECK_EQUAL(result.code.front().code[i], loadedCode.code.front().code[i]);
		}
	}
	
    BOOST_AUTO_TEST_CASE(symbol_save)
    {
        asmparser parser;
        parser.parseText(" org $1000\nstart: nop\n bsr finish\nfinish: nop\n end start\n");
        validate_noErrors(parser);
        const asmResult& result = parser.getCode68000();
        BOOST_CHECK_EQUAL(2, result.labels.size());

        bool saved = result.saveSymbols("symbols.txt");
        BOOST_CHECK(saved);

        asmResult loadedCode;
        bool loaded = loadedCode.loadSymbols("symbols.txt");
        BOOST_CHECK(loaded);
        BOOST_CHECK_EQUAL(result.labels.size(), loadedCode.labels.size());
        for (auto it = result.labels.begin(); it != result.labels.end(); it++)
        {
            const std::string& label = it->first;
            uint32_t address = it->second;

            BOOST_CHECK(loadedCode.labels.find(label) != loadedCode.labels.end());
            BOOST_CHECK_EQUAL(address, loadedCode.labels[label]);
        }
    }

	BOOST_AUTO_TEST_SUITE_END()
}
