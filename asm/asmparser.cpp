// from https://github.com/antlr/antlr4/blob/master/runtime/Cpp/demo/Windows/antlr4-cpp-demo/main.cpp
#include <iostream>

#include "antlr4-runtime.h"
#include "lexer68000.h"
#include "parser68000.h"
#include "parser68000BaseListener.h"
#include "asmparser.h"
#include "visitor.h"
#include "errorListener.h"

#pragma execution_character_set("utf-8")

using namespace antlr4;

bool asmparser::parseFile(const char *filename, bool showTree) 
{
	std::ifstream sourceFile(filename);

	ANTLRInputStream input(sourceFile);

	lexer68000 lexer(&input);
    ErrorListener lexerErrors;
    lexer.addErrorListener(&lexerErrors);

	CommonTokenStream tokens(&lexer);

	parser68000 parser(&tokens);
    ErrorListener parserErrors;
    parser.addErrorListener(&parserErrors);

	tree::ParseTree *tree = parser.prog();

	tree::ParseTreeWalker walker;
	walker.walk(new parser68000BaseListener(), tree);

	if (showTree)
	{
		auto s = tree->toStringTree(&parser);
		std::cout << "Parse Tree: " << s << std::endl;
	}
	visitor v(result, listingFile);
    auto res = v.generateCode(tree);

    bool finalResult = true;
    auto lexerErrorCount = lexerErrors.count();
    if (lexerErrorCount != 0)
    {
        std::cerr << lexerErrorCount << " errors found during lexing:" << std::endl;
        finalResult = false;
    }
    auto parserErrorCount = parserErrors.count();
    if (parserErrorCount != 0)
    {
        std::cerr << parserErrorCount << " errors found during parsing:" << std::endl;
        finalResult = false;
    }
    auto errs = result.errors.get();
    if (!errs.empty())
	{
		std::cerr << errs.size() << " errors found during parsing:" << std::endl;
		for (const auto& error : errs)
		{
			std::cerr << error.toString() << std::endl;
		}
        finalResult = false;
	}
	return finalResult;
}

std::any asmparser::parseText(const char* text)
{
#if defined(_WIN32)
	// Disable automatic leak detection
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF);
#endif
	ANTLRInputStream input(text);

	lexer68000 lexer(&input);
    ErrorListener lexerErrors;
    lexer.addErrorListener(&lexerErrors);

    CommonTokenStream tokens(&lexer);

	parser68000 parser(&tokens);
    ErrorListener parserErrors;
    parser.addErrorListener(&parserErrors);

    tree::ParseTree* tree = parser.prog();

    auto lexerErrorCount = lexerErrors.count();
    auto parserErrorCount = parserErrors.count();
    auto syntaxErrors = parser.getNumberOfSyntaxErrors();
    auto errors = lexerErrorCount + parserErrorCount;
	if (errors == 0)
	{
		visitor v(result, nullptr);
		auto result = v.generateCode(tree);
		return result;
	}

	return false;
}

size_t asmparser::checkSyntax(const char* text)
{
#if defined(_WIN32)
	// Disable automatic leak detection
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF);
#endif
    ANTLRInputStream input(text);

	lexer68000 lexer(&input);
    ErrorListener lexerErrors;
    lexer.addErrorListener(&lexerErrors);
	CommonTokenStream tokens(&lexer);

	parser68000 parser(&tokens);
    ErrorListener parserErrors;
    parser.addErrorListener(&parserErrors);

	tree::ParseTree* tree = parser.prog();

    auto lexerErrorCount = lexerErrors.count();
    auto parserErrorCount = parserErrors.count();
	auto syntaxErrors = parser.getNumberOfSyntaxErrors();
	return lexerErrorCount + parserErrorCount;
}

bool asmparser::saveBinary(const char* filename)
{
	return result.saveBinary(filename);
}

bool asmparser::saveSymbols(const char* filename)
{
    return result.saveSymbols(filename);
}

void asmparser::listingFileName(const char* filename)
{
    listingFile = filename;
}
