#include <iostream>
#include <chrono>

#ifdef _WIN32
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#endif

#include "simplebios.h"

using namespace mc68000;
Cpu* SimpleBios::cpu = nullptr;

void SimpleBios::registerTrapHandlers(Cpu* cpu)
{
    this->cpu = cpu;
    cpu->registerTrapHandler(0, trap0);
    cpu->registerTrapHandler(15, trap15);
}

void SimpleBios::trap0(uint32_t d0, uint32_t d1, uint32_t a0, uint32_t a1)
{
}

void SimpleBios::trap15(uint32_t d0, uint32_t d1, uint32_t a0, uint32_t a1)
{
    switch (d0)
    {
        case 1:
        {
            int32_t d1 = getCharacter() & 0xff;
            cpu->setDRegister(1, d1);
            break;
        }
        case 4:
        {
            int32_t d1 = getInteger();
            cpu->setDRegister(1, d1);
            break;
        }
        case 8:
        {
            int32_t d1 = getTime();
            cpu->setDRegister(1, d1);
            break;
        }
        case 10:
        {
            putCharacter(d1 & 0xff);
            break;
        }
        case 14:
        {
            displayString(a1);
            break;
        }
        default:
            break;
    }
}
int32_t SimpleBios::getCharacter()
{
#ifdef _WIN32
    int ch = _getch();  // Windows: no Enter required
    if (ch == '\n')
    {
        ch = '\r';    // Convert newline to carriage return
    }
#else
    struct termios oldt, newt;
    char ch;
    tcgetattr(STDIN_FILENO, &oldt);          // Get current terminal settings
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);        // Disable canonical mode and echo
    tcsetattr(STDIN_FILENO, TCSANOW, &newt); // Apply new settings
    read(STDIN_FILENO, &ch, 1);              // Read one char    tcsetattr(STDIN_FILENO, TCSANOW, &oldt); // Restore old settings
    if (ch == '\n')
    {
        ch = '\r';                          // Convert newline to carriage return
    }
    else if (ch == 127)
    {
        ch = 8;                             // convert backspace
    }
    return ch;
#endif
}

int32_t SimpleBios::getInteger()
{
    int32_t value;
    std::cin >> value;
    return value;
}

int32_t SimpleBios::getTime()
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

void SimpleBios::putCharacter(uint32_t c)
{
    std::cout << static_cast<char>(c & 0xff) << std::flush;
}
void SimpleBios::displayString(uint32_t address)
{
    char* str = static_cast<char*>(cpu->mem.get<void*>(address));
    std::cout << str << std::flush;
}
