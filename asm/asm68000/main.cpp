// from https://github.com/antlr/antlr4/blob/master/runtime/Cpp/demo/Windows/antlr4-cpp-demo/main.cpp
#include <iostream>

#include "asmparser.h"

#pragma execution_character_set("utf-8")


int main(int argc, const char * argv[]) 
{
	const char* filename;
	if (argc < 2)
	{
		filename = "test.68k";
	}
	else
	{
		filename = argv[1];
	}
	bool result = parseFile(filename);
	if (!result)
	{
		std::cout << "Failed to parse file: " << filename << std::endl;
		return 1;
	}
	return 0;
}