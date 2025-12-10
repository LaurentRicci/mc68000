#pragma once
#include "../core/cpu.h"
#include "ibios.h"

namespace mc68000
{
    class Emulator
    {
    private:
        Memory memory;
        Cpu cpu;
		IBios* bios;

        bool debugMode = false;
        const char* symbolsFile = nullptr;

    public:
        Emulator();
	    Emulator(const char* binaryFile, const char* symbolsFilename);
	    Emulator(uint32_t memorySize, uint32_t base, const uint8_t* code, size_t codeSize);

	    bool debug(bool enable);
        void run();
        void run(uint32_t startPc, uint32_t startSP = 0, uint32_t startUSP = 0);
    };
}
