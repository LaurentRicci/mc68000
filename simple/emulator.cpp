#include <iostream>
#include <chrono>
#include "emulator.h"


const uint8_t code[] = {
    0x30, 0x3c, 0x00, 0x0e,             //        move    #14,D0  D0 = task number 14.
    0x43, 0xf9, 0x00, 0x00, 0x10, 0x12, // lea     text,A0 A0 = address of string to display.
    0x4e, 0x4f,                         // trap    #15     Activate input / output task.
    0x4e, 0x40,                         // trap    #0
    0xff, 0xff, 0xff, 0xff,
    // text    dc.b    'MC68000 hello',0   Null terminated string.
    'M', 'C', '6','8', '0', '0', '0', ' ', 'H', 'e', 'l', 'l', 'o', '\n',0x00
};
const uint32_t base = 0x001000;
Emulator* Emulator::current;

void Emulator::trap15(uint32_t d0, uint32_t d1, uint32_t a0, uint32_t a1)
{
    if (current != nullptr)
    {
        switch (d0)
        {
            case 4:
            {
                uint32_t d1 = getInteger();
                current->cpu.setDRegister(1, d1);
                break;
            }
			case 8:
			{
				uint32_t d1 = getTime();
				current->cpu.setDRegister(1, d1);
				break;
			}
            case 14:
                displayString(a1);
                break;
            default:
                break;
        }
    }
}

int32_t Emulator::getInteger()
{
	int32_t value;
	std::cin >> value;
	return value;
}

int32_t Emulator::getTime()
{
	std::time_t now = std::time(0);
	std::tm local_tm = *std::localtime(&now);
    local_tm.tm_hour = 0;
	local_tm.tm_min = 0;
	local_tm.tm_sec = 0;
	std::time_t midnight = std::mktime(&local_tm);


    auto now_tp = std::chrono::system_clock::now();
	auto midnight_tp = std::chrono::system_clock::from_time_t(midnight);

	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now_tp - midnight_tp);
	return static_cast<uint32_t>(duration.count());
}

void Emulator::displayString(uint32_t address)
{
	char* str = static_cast<char*>(current->memory.get<void*>(address));
	std::cout << str;
}

Emulator::Emulator() :
    memory(1024, base, code, sizeof(code)),
    cpu(memory)
{
    cpu.registerTrapHandler(15, trap15);
    current = this;
}

Emulator::Emulator(uint32_t memorySize, uint32_t base, const uint8_t* code, size_t codeSize) :
	memory(memorySize, base, code, codeSize),
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

