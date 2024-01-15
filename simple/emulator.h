#pragma once
#include "../core/cpu.h"

class Emulator
{
private:
    mc68000::memory memory;
    mc68000::Cpu cpu;

private:
    static Emulator* current;
    static void trap15(uint32_t d0, uint32_t a0);

public:
    Emulator();

    void run();
};