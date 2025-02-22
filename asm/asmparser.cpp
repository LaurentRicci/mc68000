// from https://github.com/antlr/antlr4/blob/master/runtime/Cpp/demo/Windows/antlr4-cpp-demo/main.cpp
#include <iostream>

#include "antlr4-runtime.h"
#include "lexer68000.h"
#include "parser68000.h"
#include "parser68000BaseListener.h"
#include "listener.h"
#include "asmparser.h"
#include "visitor.h"

#pragma execution_character_set("utf-8")

using namespace antlr4;

bool parseFile(const char *filename) 
{
	std::ifstream sourceFile(filename);

	ANTLRInputStream input(sourceFile); 
	lexer68000 lexer(&input);
	CommonTokenStream tokens(&lexer);

	parser68000 parser(&tokens);
	tree::ParseTree *tree = parser.prog();

	tree::ParseTreeWalker walker;
	walker.walk(new listener(), tree);

	auto s = tree->toStringTree(&parser);
	std::cout << "Parse Tree: " << s << std::endl;

	return true;
}

std::any parseText(const char* text)
{
#if defined(_WIN32)
	// Disable automatic leak detection
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF);
#endif
	ANTLRInputStream input(text);
	lexer68000 lexer(&input);
	CommonTokenStream tokens(&lexer);

	parser68000 parser(&tokens);
	tree::ParseTree* tree = parser.prog();
	tree::ParseTreeWalker walker;
	walker.walk(new listener(), tree);

	auto errors = parser.getNumberOfSyntaxErrors();
	if (errors == 0)
	{

		visitor v;
		auto result = v.visit(tree);
		return result;
	}

	return false;
}

size_t checkSyntax(const char* text)
{
#if defined(_WIN32)
	// Disable automatic leak detection
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF);
#endif
	ANTLRInputStream input(text);
	lexer68000 lexer(&input);
	CommonTokenStream tokens(&lexer);

	parser68000 parser(&tokens);
	tree::ParseTree* tree = parser.prog();

	return parser.getNumberOfSyntaxErrors();
}