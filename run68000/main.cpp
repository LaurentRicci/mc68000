#include <iostream>
#include <cstring>
#include <filesystem>
#include "emulator.h"
extern void game(bool);
extern void game(const char*, bool);

void usage()
{
    std::cout << "Usage: simple [options] [binaryFile]" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  -h, --help                   Show this help message" << std::endl;
    std::cout << "  -d, --debug                  Debug mode" << std::endl;
    std::cout << "  -s, --symbols <symbols file> Load the symbols from the file" << std::endl;
}

int main(int argc, const char* argv[])
{
	bool debugMode = false;
    std::string symbolsFilename;

    if (argc < 2)
    {
        std::cerr << "No binary file specified. Use -h or --help for usage information." << std::endl;
        std::cerr << "Running demo code." << std::endl << std::endl;

        mc68000::Emulator emulator;
        emulator.run();
        return 1;
    }

    const char* lastArgument = argv[argc - 1];
    if (strcmp(lastArgument, "-h") == 0 || strcmp(lastArgument, "--help") == 0)
    {
        usage();
        return 0;
    }
    if (lastArgument[0] == '-')
    {
        std::cerr << "No source file specified. Use -h or --help for usage information." << std::endl;
        return 1;
    }
    std::string binaryFilename = lastArgument;
    std::filesystem::path baseFilename(binaryFilename);
    std::string basename = baseFilename.replace_extension().string();

    for (int i=1; i<argc-1; i++)
    {
        if (argv[i][0] == '-')
        {
            if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0)
            {
                usage();
                return 0;
            }
            else if (strcmp(argv[i], "-d") == 0 || strcmp(argv[i], "--debug") == 0)
            {
                debugMode = true;
            }
            else if (strcmp(argv[i], "-s") == 0 || strcmp(argv[i], "--symbols") == 0)
            {
                if ((i + 1 < argc - 1) && (argv[i + 1][0] != '-'))
                {
                    symbolsFilename = argv[i + 1];
                    i++;
                }
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
            std::cerr << "Unknown argument: " << argv[i] << std::endl;
            std::cerr << "Use -h or --help to see available options" << std::endl;
            return 1;
        }
    }
    if (debugMode and symbolsFilename.empty())
    {
        symbolsFilename = (basename + ".sym");
        std::filesystem::path p(symbolsFilename);

        if (std::filesystem::exists(p))
        {
            std::cerr << "using default symbols filename: " << symbolsFilename << std::endl;
        }
        else
        {
            std::cerr << "no symbols file found, symbols won't be used for debugging" << std::endl;
            symbolsFilename.clear();
        }

    }

    mc68000::Emulator emulator(binaryFilename.c_str(), symbolsFilename.c_str());
    emulator.debug(debugMode);
    emulator.run(0, 1024, 1024);
    return 0;
}
