#pragma once
#include "../core/cpu.h"

class Emulator
{
private:
    mc68000::memory memory;
    mc68000::Cpu cpu;

private:
    static Emulator* current;
    static void trap15(uint32_t d0, uint32_t d1, uint32_t a0, uint32_t a1);
	static int32_t getInteger();
	static int32_t getTime();
	static void displayString(uint32_t address);

public:
    Emulator();
	Emulator(uint32_t memorySize, uint32_t base, const uint8_t* code, size_t codeSize);

    void run();
};