// from https://github.com/antlr/antlr4/blob/master/runtime/Cpp/demo/Windows/antlr4-cpp-demo/main.cpp
#include <iostream>
#include <cstring>
#include <filesystem>
#include "asmparser.h"

#pragma execution_character_set("utf-8")

int usage()
{
    std::cout << "Usage: asm68000 [options] <sourcefile>+" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  -h, --help    Show this help message" << std::endl;
    std::cout << "  -t, --tree    Show parse tree" << std::endl;
    std::cout << "  -o, --output  <binary file>  Save binary output" << std::endl;
    std::cout << "  -s, --symbols <symbols file> Save the symbol table" << std::endl;
    std::cout << "  -l, --listing <listing file> Save the assembly listing" << std::endl;
    return 0;
}

int main(int argc, const char * argv[]) 
{
	bool showTree = false;
	bool saveBinary = false;
    bool saveSymbols = false;
    bool saveListing = false;
    std::string filename;
    std::string binaryFilename;
    std::string symbolsFilename;
    std::string listingFilename;

    if (argc < 2)
    {
        std::cerr << "No source file specified. Use -h or --help for usage information." << std::endl;
        return 1;
    }
    if (strcmp(argv[argc-1], "-h") == 0 || strcmp(argv[argc-1], "--help") == 0)
    {
        return usage();
    }
    if (argv[argc - 1][0] == '-')
    {
        std::cerr << "Invalid arguments. Use -h or --help for usage information." << std::endl;
        return 1;
    }

    int arg = 1;
    bool endOfOptions = false;
    while (!endOfOptions && arg < argc-1)
    {
        if (argv[arg][0] == '-')
        {
            if (strcmp(argv[arg], "-h") == 0 || strcmp(argv[arg], "--help") == 0)
            {
                return usage();
            }
            // if the argument is an option that require a parameter we skip the parameter if provided
            if ((strcmp(argv[arg], "-o") == 0 || strcmp(argv[arg], "--output") == 0) ||
                (strcmp(argv[arg], "-s") == 0 || strcmp(argv[arg], "--symbols") == 0) ||
                (strcmp(argv[arg], "-l") == 0 || strcmp(argv[arg], "--listing") == 0))
            {
                if ((arg + 1 < argc-1) && (argv[arg + 1][0] != '-'))
                {
                    arg++;
                }
            }
            arg++;
        }
        else
        {
            endOfOptions = true;
        }
    }

    if (arg == argc)
    {
        std::cerr << "No source file specified. Use -h or --help for usage information." << std::endl;
        return 1;
    }
    const char* lastArgument = argv[arg];
    filename = lastArgument;
    std::filesystem::path sourceFilename(filename);
    std::string basename = sourceFilename.replace_extension().string();

	for (int i=1; i<arg; i++)
	{
		if (argv[i][0] == '-')
		{
			if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0)
			{
                return usage();
			}
			else if (strcmp(argv[i], "-t") == 0 || strcmp(argv[i], "--tree") == 0)
			{
				showTree = true;
				continue;
			}
			else if (strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "--output") == 0)
			{
                saveBinary = true;
				if ((i + 1 < argc - 1) && (argv[i + 1][0] != '-'))
				{
                    binaryFilename = argv[i + 1];
                    i++;
				}
				else
				{
                    binaryFilename = (basename + ".bin");
					std::cerr << "using default binary filename: " << binaryFilename << std::endl;
				}
				continue;
			}
            else if (strcmp(argv[i], "-s") == 0 || strcmp(argv[i], "--symbols") == 0)
            {
                saveSymbols = true;
                if ((i + 1 < argc - 1) && (argv[i + 1][0] != '-'))
                {
                    symbolsFilename = argv[i + 1];
                    i++;
                }
                else
                {
                    symbolsFilename = (basename + ".sym");
                    std::cerr << "using default symbols filename: " << symbolsFilename << std::endl;
                }
                continue;
            }
            else if (strcmp(argv[i], "-l") == 0 || strcmp(argv[i], "--listing") == 0)
            {
                saveListing = true;
                if ((i + 1 < argc - 1) && (argv[i + 1][0] != '-'))
                {
                    listingFilename = argv[i + 1];
                    i++;
                }
                else
                {
                    listingFilename = (basename + ".lst");
                    std::cerr << "using default listing filename: " << listingFilename << std::endl;
                }
                continue;
            }
            else
			{
                std::cerr << "Unknown option: " << argv[i] << std::endl;
                std::cerr << "Use -h or --help to see available options" << std::endl;
                return 1;
            }
		}
        else
        {
            std::cerr << "Unexpected argument: " << argv[i] << std::endl;
            std::cerr << "Use -h or --help to see available options" << std::endl;
            return 1;
        }
	}
    if (filename.empty())
    {
        std::cerr << "No source file specified." << std::endl;
        return 1;
    }


	asmparser parser;
    if (saveListing)
    {
        parser.listingFileName(listingFilename.c_str());
    }
	bool result = parser.parseFile(filename.c_str(), showTree);
	if (!result)
	{
		std::cout << "Failed to parse file: " << filename << std::endl;
		return 1;
	}
    else
    {
        std::cout << "Successfully parsed file: " << filename << std::endl;
    }
	if (saveBinary && !parser.saveBinary(binaryFilename.c_str()))
	{
		std::cerr << "error saving binary file: " << binaryFilename << std::endl;
		return 1;
	}
    if (saveSymbols && !parser.saveSymbols(symbolsFilename.c_str()))
    {
        std::cerr << "error saving symbols file: " << symbolsFilename << std::endl;
        return 1;
    }
	return 0;
}
