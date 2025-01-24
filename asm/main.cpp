// from https://github.com/antlr/antlr4/blob/master/runtime/Cpp/demo/Windows/antlr4-cpp-demo/main.cpp
#include <iostream>

#include "antlr4-runtime.h"
#include "lexer68000.h"
#include "parser68000.h"
#include "parser68000BaseListener.h"
#include "listener.h"

#pragma execution_character_set("utf-8")

using namespace antlr4;

int main(int argc, const char * argv[]) 
{
	std::ifstream sourceFile("test.68k");

	ANTLRInputStream input(sourceFile); 
  lexer68000 lexer(&input);
  CommonTokenStream tokens(&lexer);

  parser68000 parser(&tokens);
  tree::ParseTree *tree = parser.prog();

  tree::ParseTreeWalker walker;
  walker.walk(new listener(), tree);

  auto s = tree->toStringTree(&parser);
  std::cout << "Parse Tree: " << s << std::endl;

  return 0;
}