#pragma once
#include "ibios.h"

namespace mc68000
{
    class SimpleBios : public IBios
    {
    public:
        void registerTrapHandlers(Cpu* cpu) override;
    private:
        static void trap15(Cpu*);
        static void trap0(Cpu*);

        static int32_t getCharacter();
        static int32_t keyPressed();
        static int32_t getInteger();
        static int32_t getTime();
        static void putCharacter(uint32_t c);
        static void displayString(Cpu* cpu, uint32_t address);
        static void writeCharacterToDisk(uint8_t value);
        static int32_t readCharacterFromDisk();

        static FILE* diskFile;
    };
};

