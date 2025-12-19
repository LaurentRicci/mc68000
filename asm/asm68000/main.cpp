// from https://github.com/antlr/antlr4/blob/master/runtime/Cpp/demo/Windows/antlr4-cpp-demo/main.cpp
#include <iostream>
#include <cstring>
#include "asmparser.h"

#pragma execution_character_set("utf-8")

int main(int argc, const char * argv[]) 
{
	bool showTree = false;
	bool saveBinary = false;
    bool saveSymbols = false;
    bool saveListing = false;
	const char* filename = nullptr;
	const char* binaryFilename = nullptr;
    const char* symbolsFilename = nullptr;
    const char* listingFilename = nullptr;

	for (int i=1; i<argc; i++)
	{
		if (argv[i][0] == '-')
		{
			if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0)
			{
				std::cout << "Usage: asm68000 [options] <sourcefile>" << std::endl;
				std::cout << "Options:" << std::endl;
				std::cout << "  -h, --help    Show this help message" << std::endl;
				std::cout << "  -t, --tree    Show parse tree" << std::endl;
				std::cout << "  -o, --output  <binary file>  Save binary output" << std::endl;
                std::cout << "  -s, --symbols <symbols file> Save the symbol table" << std::endl;
                std::cout << "  -l, --listing <listing file> Save the assembly listing" << std::endl;
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
            else if (strcmp(argv[i], "-s") == 0 || strcmp(argv[i], "--symbols") == 0)
            {
                if (i + 1 < argc)
                {
                    saveSymbols = true;
                    symbolsFilename = argv[i + 1];
                    i++;
                }
                else
                {
                    std::cerr << "Missing symbols filename following " << argv[i] << std::endl;
                }
                continue;
            }
            else if (strcmp(argv[i], "-l") == 0 || strcmp(argv[i], "--listing") == 0)
            {
                if (i + 1 < argc)
                {
                    saveListing = true;
                    listingFilename = argv[i + 1];
                    i++;
                }
                else
                {
                    std::cerr << "Missing listing filename following " << argv[i] << std::endl;
                }
                continue;
            }
            else
			{
				std::cerr << "Unknown option: " << argv[i] << std::endl;
				return 1;
			}
		}
        else if (filename == nullptr)
		{
			filename = argv[i];
		}
        else
        {
            std::cerr << "Unexpected argument: " << argv[i] << std::endl;
            return 1;
        }
	}

	asmparser parser;
    if (saveListing)
    {
        parser.listingFileName(listingFilename);
    }
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
    if (saveSymbols && !parser.saveSymbols(symbolsFilename))
    {
        std::cerr << "error saving symbols file: " << symbolsFilename << std::endl;
        return 1;
    }
	return 0;
}
