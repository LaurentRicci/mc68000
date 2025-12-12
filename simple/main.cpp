#include <iostream>
#include <cstring>
#include "emulator.h"
extern void game(bool);
extern void game(const char*, bool);

int main(int argc, const char* argv[])
{
	bool debugMode = false;
	const char* binaryFile = nullptr;
    const char* symbolsFile = nullptr;

    for (int i = 1; i < argc; i++)
    {
        if (argv[i][0] == '-')
        {
            if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0)
            {
                std::cout << "Usage: simple [options] [binaryFile]" << std::endl;
                std::cout << "Options:" << std::endl;
                std::cout << "  -h, --help                   Show this help message" << std::endl;
                std::cout << "  -d, --debug                  Debug mode" << std::endl;
                std::cout << "  -s, --symbols <symbols file> Load the symbols from the file" << std::endl;
                return 0;
            }
            else if (strcmp(argv[i], "-d") == 0 || strcmp(argv[i], "--debug") == 0)
            {
                debugMode = true;
            }
            else if (strcmp(argv[i], "-s") == 0 || strcmp(argv[i], "--symbols") == 0)
            {
                if (i + 1 < argc)
                {
                    symbolsFile = argv[i + 1];
                    i++;
                }
            }
            else
            {
                std::cerr << "Unknown option: " << argv[i] << std::endl;
                std::cerr << "Use --help to see available options" << std::endl;
                return 1;
            }
        }
        else if (binaryFile == nullptr)
        {
            binaryFile = argv[i];
        }
        else
        {
            std::cerr << "Unknown argument: " << argv[i] << std::endl;
            return 1;
        }
    }
	if (binaryFile != nullptr)
    {
        mc68000::Emulator emulator(binaryFile, symbolsFile);
        emulator.debug(debugMode);
        emulator.run(0, 1024, 1024);
    }
    else
    {
        std::cerr << "No binary provided. Running default code." << std::endl << std::endl;

        mc68000::Emulator emulator;
        emulator.run();
    }
}
