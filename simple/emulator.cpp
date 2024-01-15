#include <iostream>
#include "emulator.h"


const uint8_t code[] = {
    0x30, 0x3c, 0x00, 0x0e,             //        move    #14,D0  D0 = task number 14.
    0x41, 0xf9, 0x00, 0x00, 0x10, 0x12, // lea     text,A0 A0 = address of string to display.
    0x4e, 0x4f,                         // trap    #15     Activate input / output task.
    0x4e, 0x40,                         // trap    #0
    0xff, 0xff, 0xff, 0xff,
    // text    dc.b    'MC68000 hello',0   Null terminated string.
    'M', 'C', '6','8', '0', '0', '0', ' ', 'H', 'e', 'l', 'l', 'o', '\n',0x00
};
const uint32_t base = 0x001000;
Emulator* Emulator::current;

void Emulator::trap15(uint32_t d0, uint32_t a0)
{
    std::cout << "trap #15\n";
    std::cout << "D0 = " << d0 << "\n";
    std::cout << "A0 = " << a0 << "\n";
    if (current != nullptr)
    {
        char* str = static_cast<char *>(current->memory.get<void*>(a0));
        std::cout << str;
    }
}

Emulator::Emulator() :
    memory(1024, base, code, sizeof(code)),
    cpu(memory)
{
    cpu.registerTrapHandler(15, trap15);
    current = this;
}

void Emulator::run()
{
    cpu.reset();
    cpu.start(base, base + 1024);
}

