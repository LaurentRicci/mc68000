#include <iostream>
#include <cstring>
#include "emulator.h"
extern void game(bool);
extern void game(const char*, bool);

int main(int argc, const char* argv[])
{
    Emulator emulator;

    emulator.run();
	bool debugMode = false;
	const char* binaryFile = nullptr;

    for (int i = 1; i < argc; i++)
    {
        if (argv[i][0] == '-')
        {
            std::cout << "Option " << i << ": " << argv[i] << std::endl;
            if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0)
            {
                std::cout << "Usage: simple [options] [binaryFile]" << std::endl;
                std::cout << "Options:" << std::endl;
                std::cout << "  -h, --help    Show this help message" << std::endl;
                std::cout << "  -d, --debug   Debug mode" << std::endl;
                return 0;
            }
            else if (strcmp(argv[i], "-d") == 0 || strcmp(argv[i], "--debug") == 0)
            {
                debugMode = true;
            }
        }
        else
        {
            binaryFile = argv[i];
            break;
        }
    }
	if (binaryFile != nullptr)
    {
        game(binaryFile, debugMode);
    }
    else
    {
        game(debugMode);
    }
}
