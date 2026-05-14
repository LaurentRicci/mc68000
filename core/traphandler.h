#pragma once
#include "cpu.h"
namespace mc68000
{
    class Cpu;

    class TrapHandler {
    public:
        virtual ~TrapHandler() = default;
        virtual void handle(Cpu& cpu, uint16_t vector) = 0;
    protected:
    };
}
