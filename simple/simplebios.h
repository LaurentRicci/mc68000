#pragma once
#include "ibios.h"

namespace mc68000
{
    class SimpleBios : public IBios
    {
    public:
        void registerTrapHandlers(Cpu* cpu) override;
    private:
        static void trap15(uint32_t d0, uint32_t d1, uint32_t a0, uint32_t a1);
        static void trap0(uint32_t d0, uint32_t d1, uint32_t a0, uint32_t a1);

        static int32_t getCharacter();
        static int32_t getInteger();
        static int32_t getTime();
        static void putCharacter(uint32_t c);
        static void displayString(uint32_t address);
        static Cpu* cpu;
    };
};

