// from https://github.com/antlr/antlr4/blob/master/runtime/Cpp/demo/Windows/antlr4-cpp-demo/main.cpp
#include <iostream>
#include <cstring>
#include "asmparser.h"

#pragma execution_character_set("utf-8")

int main(int argc, const char * argv[]) 
{
	bool showTree = false;
	bool saveBinary = false;
	const char* filename = "test.68k";
	const char* binaryFilename = nullptr;

	for (int i=1; i<argc; i++)
	{
		if (argv[i][0] == '-')
		{
			std::cout << "Option " << i << ": " << argv[i] << std::endl;
			if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0)
			{
				std::cout << "Usage: asm68000 [options] <sourcefile>" << std::endl;
				std::cout << "Options:" << std::endl;
				std::cout << "  -h, --help    Show this help message" << std::endl;
				std::cout << "  -t, --tree    Show parse tree" << std::endl;
				std::cout << "  -o, --output  <binary file>  Save binary output" << std::endl;
				return 0;
			}
			else if (strcmp(argv[i], "-t") == 0 || strcmp(argv[i], "--tree") == 0)
			{
				showTree = true;
				continue;
			}
			else if (strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "--output") == 0)
			{
				if (i + 1 < argc)
				{
					saveBinary = true;
					binaryFilename = argv[i + 1];
					i++;
				}
				else
				{
					std::cerr << "Missing binary filename following " << argv[i] << std::endl;
				}
				continue;
			}
			else
			{
				std::cerr << "Unknown option: " << argv[i] << std::endl;
				return 1;
			}
		}
		else
		{
			filename = argv[i];
			break;
		}
	}

	asmparser parser;
	bool result = parser.parseFile(filename, showTree);
	if (!result)
	{
		std::cout << "Failed to parse file: " << filename << std::endl;
		return 1;
	}
	if (saveBinary && !parser.saveBinary(binaryFilename))
	{
		std::cerr << "error saving binary file: " << binaryFilename << std::endl;
		return 1;
	}
	return 0;
}