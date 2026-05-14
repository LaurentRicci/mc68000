#include <iostream>
#include <chrono>

#ifdef _WIN32
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#endif

#include "simplebios.h"
#include "trapargs.h"

using namespace mc68000;
FILE* SimpleBios::diskFile = nullptr;

void SimpleBios::setup()
{
    // no setup needed for now
}

void SimpleBios::registerTrapHandlers(Cpu* cpu)
{
    cpu->registerTrapHandler(0, this);
    cpu->registerTrapHandler(15, this);
}

void SimpleBios::trap0(Cpu& cpu)
{
}

void SimpleBios::trap15(Cpu& cpu)
{
    bool isSupervisor = callerIsSupervisor(cpu);
    uint16_t func = argWord(cpu, isSupervisor, 0);

    switch (func)
    {
        case 1:
        {
            int32_t d0 = getCharacter() & 0xff;
            cpu.setDRegister(0, d0);
            break;
        }
        case 2:
        {
            int32_t d0 = keyPressed() & 0xff;
            cpu.setDRegister(0, d0);
            cpu.setCCR(d0 ? 0 : 4); // set Z flag
            break;
        }
        case 4:
        {
            int32_t d0 = getInteger();
            cpu.setDRegister(0, d0);
            break;
        }
        case 8:
        {
            int32_t d0 = getTime();
            cpu.setDRegister(0, d0);
            break;
        }
        case 10:
        {
            uint16_t chr = argWord(cpu, isSupervisor, 1);
            putCharacter(chr & 0xff);
            break;
        }
        case 14:
        {
            uint32_t address = argLong(cpu, isSupervisor, 1);
            displayString(cpu, address);
            break;
        }
        case 20:
        {
            uint16_t chr = argWord(cpu, isSupervisor, 1);
            writeCharacterToDisk(chr & 0xff);
            break;
        }
        case 21:
        {
            int32_t d0 = readCharacterFromDisk();
            cpu.setDRegister(0, d0);
            break;
        }
        case 101: // For testing trap with 1 word argument
        {
            uint16_t arg1 = argWord(cpu, isSupervisor, 1);
            cpu.setDRegister(0, arg1);
            break;
        }
        case 102: // For testing trap with 1 long argument
        {
            uint32_t arg1 = argLong(cpu, isSupervisor, 1);
            cpu.setDRegister(0, arg1);
            break;
        }
        case 103: // For testing trap with 1 word 1 long 1 word arguments
        {
            uint16_t arg1 = argWord(cpu, isSupervisor, 1);
            uint32_t arg2 = argLong(cpu, isSupervisor, 2);
            uint16_t arg3 = argWord(cpu, isSupervisor, 4);
            uint32_t res = arg1 + arg2 + arg3;
            cpu.setDRegister(0, res);
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
#endif
    return ch;
}

int32_t SimpleBios::keyPressed()
{
#ifdef _WIN32
    if(_kbhit())
    {
        int ch = _getch();
        return ch;
    }
    return 0;
#else
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);          // Get current terminal settings
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);        // Disable canonical mode and echo
    tcsetattr(STDIN_FILENO, TCSANOW, &newt); // Apply new settings

    int bytesWaiting;
    ioctl(STDIN_FILENO, FIONREAD, &bytesWaiting);
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt); // Restore old settings
    if (bytesWaiting > 0)
    {
        char ch;
        read(STDIN_FILENO, &ch, 1);          // Read one char
        return ch;
    }
    return 0;
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
void SimpleBios::displayString(Cpu& cpu, uint32_t address)
{
    char* str = static_cast<char*>(cpu.mem.get<void*>(address));
    std::cout << str << std::flush;
}
const int32_t CTRL_Z = 0x1a;

void SimpleBios::writeCharacterToDisk(uint8_t ch)
{
    if (diskFile == nullptr)
    {
        diskFile = fopen("basic.bas", "wb");
        if (diskFile == nullptr)
        {
            std::cerr << "Cannot open diskfile.bin for writing" << std::endl;
            return;
        }
    }
    if (ch != CTRL_Z)
    {
        fwrite(&ch, sizeof(uint8_t), 1, diskFile);
    }
    else
    {
        // CTRL-Z indicates end of file
        fflush(diskFile);
        fclose(diskFile);
        diskFile = nullptr;
    }
}
int32_t SimpleBios::readCharacterFromDisk()
{
    if (diskFile == nullptr)
    {
        diskFile = fopen("basic.bas", "rb");
        if (diskFile == nullptr)
        {
            std::cerr << "Cannot open diskfile.bin for reading" << std::endl;
            return 0;
        }
    }
    int ch = fgetc(diskFile);
    if (ch == EOF)
    {
        fclose(diskFile);
        diskFile = nullptr;
        return CTRL_Z;
    }
    return static_cast<uint8_t>(ch);
}
