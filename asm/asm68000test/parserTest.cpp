#include <fstream>
#include <sstream>

#include <boost/test/unit_test.hpp>
#include "asmparser.h"
#include "util.h"
#include "fileCleanup.h"

namespace parserTest
{
	static const char* binaryFileName = "test.bin";
	static const char* symbolsFileName = "symbols.txt";

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
		BOOST_CHECK_NE(0, error);
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
		FileCleanup binaryFileCleanup(binaryFileName);

		asmparser parser;
		parser.parseText(" org $1000\nstart: nop\n lea $4242,a0\n end start\n");
		validate_noErrors(parser);
		const asmResult& result = parser.getCode68000();
		BOOST_CHECK_EQUAL(0x1000, result.code.front().origin);
		BOOST_CHECK_EQUAL(0x1000, result.start);
		BOOST_CHECK_EQUAL(3, result.code.front().code.size());

		bool saved = result.saveBinary(binaryFileName);
		BOOST_CHECK(saved);

		asmResult loadedCode;
		bool loaded = loadedCode.loadBinary(binaryFileName);
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
		FileCleanup binaryFileCleanup(binaryFileName);

        asmparser parser;
        parser.parseText(" memory $100,$300\n org $120\nstart: nop\n lea $4242,a0\n end start\n");
        validate_noErrors(parser);
        const asmResult& result = parser.getCode68000();
        BOOST_CHECK_EQUAL(0x120, result.start);
        BOOST_CHECK_EQUAL(0x100, result.memoryStart);
        BOOST_CHECK_EQUAL(0x300, result.memoryEnd);
        BOOST_CHECK_EQUAL(0x120, result.code.front().origin);
        BOOST_CHECK_EQUAL(3, result.code.front().code.size());

        bool saved = result.saveBinary(binaryFileName);
        BOOST_CHECK(saved);

        asmResult loadedCode;
        bool loaded = loadedCode.loadBinary(binaryFileName);
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
		FileCleanup binaryFileCleanup(symbolsFileName);

        asmparser parser;
        parser.parseText(" org $1000\nstart: nop\n bsr finish\nfinish: nop\n end start\n");
        validate_noErrors(parser);
        const asmResult& result = parser.getCode68000();
        BOOST_CHECK_EQUAL(2, result.labels.size());

        bool saved = result.saveSymbols(symbolsFileName);
        BOOST_CHECK(saved);

        asmResult loadedCode;
        bool loaded = loadedCode.loadSymbols(symbolsFileName);
        BOOST_CHECK(loaded);
        BOOST_CHECK_EQUAL(result.labels.size(), loadedCode.labels.size());
        for (auto it = result.labels.begin(); it != result.labels.end(); it++)
        {
            const std::string& label = it->first;
            uint32_t address = it->second.offset;

            BOOST_CHECK(loadedCode.labels.find(label) != loadedCode.labels.end());
            BOOST_CHECK_EQUAL(address, loadedCode.labels[label].offset);
        }
    }

    void checkExportedSymbol(std::ifstream& binaryFile, const std::string& expectedLabel, uint32_t expectedAddress)
    {
	    uint32_t labelSize;
	    binaryFile.read(reinterpret_cast<char*>(&labelSize), sizeof(uint32_t));
	    BOOST_CHECK_EQUAL(expectedLabel.size(), labelSize);

	    char label[256];
	    binaryFile.read(reinterpret_cast<char*>(label), labelSize);
	    label[labelSize] = '\0';
	    BOOST_CHECK_EQUAL(expectedLabel, std::string(label));

	    uint32_t labelAddress;
	    binaryFile.read(reinterpret_cast<char*>(&labelAddress), sizeof(uint32_t));
	    BOOST_CHECK_EQUAL(expectedAddress, labelAddress);
    }

	BOOST_AUTO_TEST_CASE(exportbinary_1label)
    {
		FileCleanup binaryFileCleanup(binaryFileName);

        asmparser parser;
        parser.parseText(" org $1000\nstart: nop\n bsr finish\nfinish: nop\n end start\n .export start\n");
        validate_noErrors(parser);
        const asmResult& result = parser.getCode68000();
        BOOST_CHECK_EQUAL(2, result.labels.size());

        bool saved = result.saveBinary(binaryFileName);
        BOOST_CHECK(saved);

		// open the binary file and seek to the exported symbols section
		std::ifstream binaryFile(binaryFileName, std::ios::binary);
	    uint32_t codeSize = result.code[0].code.size() * sizeof(uint16_t);    // 8
	    uint32_t offset = 28 + codeSize;
	    binaryFile.seekg(offset, std::ios::beg);

		// now read the exported symbols
	    uint32_t exportedCount;
	    binaryFile.read(reinterpret_cast<char*>(&exportedCount), sizeof(uint32_t));
	    BOOST_CHECK_EQUAL(1, exportedCount);

		checkExportedSymbol(binaryFile, "start", 0x1000);
    }

	BOOST_AUTO_TEST_CASE(exportbinary_2labels)
    {
		FileCleanup binaryFileCleanup(binaryFileName);

        asmparser parser;
        parser.parseText("start: nop\n bsr finish\nfinish: nop\n end start\n .export start,finish\n");
        validate_noErrors(parser);
        const asmResult& result = parser.getCode68000();
        BOOST_CHECK_EQUAL(2, result.labels.size());

        bool saved = result.saveBinary(binaryFileName);
        BOOST_CHECK(saved);

		// open the binary file and seek to the exported symbols section
		std::ifstream binaryFile(binaryFileName, std::ios::binary);
	    uint32_t codeSize = result.code[0].code.size() * sizeof(uint16_t);    // 8
	    uint32_t offset = 28 + codeSize;
	    binaryFile.seekg(offset, std::ios::beg);

		// now read the exported symbols
	    uint32_t exportedCount;
	    binaryFile.read(reinterpret_cast<char*>(&exportedCount), sizeof(uint32_t));
	    BOOST_CHECK_EQUAL(2, exportedCount);

	    checkExportedSymbol(binaryFile, "finish", 0x6);
		checkExportedSymbol(binaryFile, "start", 0x0);
    }

	BOOST_AUTO_TEST_SUITE_END()
}
