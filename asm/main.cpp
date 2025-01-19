// from https://github.com/antlr/antlr4/blob/master/runtime/Cpp/demo/Windows/antlr4-cpp-demo/main.cpp
#include <iostream>

#include "antlr4-runtime.h"
#include "asm68000Lexer.h"
#include "asm68000Parser.h"

#pragma execution_character_set("utf-8")

using namespace antlr4;

int main(int argc, const char * argv[]) {

  ANTLRInputStream input("10 20 30");
  asm68000Lexer lexer(&input);
  CommonTokenStream tokens(&lexer);

  asm68000Parser parser(&tokens);
  tree::ParseTree *tree = parser.prog();

  auto s = tree->toStringTree(&parser);
  std::cout << "Parse Tree: " << s << std::endl;

  return 0;
}