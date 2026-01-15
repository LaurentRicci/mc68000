#include <iostream>
#include <chrono>
#include "emulator.h"
#include "simplebios.h"

using namespace mc68000;

const uint8_t code[] = {
    0x30, 0x3c, 0x00, 0x0e,             // move    #14,D0  D0 = task number 14.
    0x43, 0xf9, 0x00, 0x00, 0x10, 0x12, // lea     text,A0 A0 = address of string to display.
    0x4e, 0x4f,                         // trap    #15     Activate input / output task.
    0x4e, 0x40,                         // trap    #0
    0xff, 0xff, 0xff, 0xff,
    // text    dc.b    'MC68000 hello',0   Null terminated string.
    'M', 'C', '6','8', '0', '0', '0', ' ', 'H', 'e', 'l', 'l', 'o', '\n',0x00
};
const uint32_t base = 0x001000;
Emulator::Emulator() :
    memory(1024, base, code, sizeof(code)),
    cpu(memory),
    bios(new SimpleBios())
{
    bios->registerTrapHandlers(&cpu);
}

Emulator::Emulator(const char* binaryFile, const char* symbolsFilename) :
    memory(binaryFile),
    cpu(memory),
    bios(new SimpleBios()),
    symbolsFile(symbolsFilename)
{
    bios->registerTrapHandlers(&cpu);
}

Emulator::Emulator(uint32_t memorySize, uint32_t base, const uint8_t* code, size_t codeSize) :
	memory(memorySize, base, code, codeSize),
	cpu(memory),
    bios(new SimpleBios())
{
	bios->registerTrapHandlers(&cpu);
}

bool Emulator::debug(bool enable)
{
    debugMode = enable;
    return debugMode;
}

void Emulator::run()
{
    cpu.reset();
    auto memoryInfo = memory.getMemoryRange();
    uint32_t base = memoryInfo.first;
    uint16_t size = memoryInfo.second;

    if (debugMode)
    {
        cpu.debug(base, base + size, base + size, symbolsFile);
	}
    else
    {
        cpu.start(base, base + size, base + size);
    }
}

void Emulator::run(uint32_t startPc, uint32_t startSP, uint32_t startUSP)
{
    cpu.reset();
    auto memoryInfo = memory.getMemoryRange();
    uint32_t base = memoryInfo.first;
    uint16_t size = memoryInfo.second;
    if (debugMode)
    {
        cpu.debug(base, base + size, base + size, symbolsFile);
    }
    else
    {
        cpu.start(base, base + size, base + size);
    }
}

