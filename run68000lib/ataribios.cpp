#ifdef _WIN32
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#endif
#include <string>

#include "ataribios.h"
#include "trapargs.h"

using namespace mc68000;


void AtariBios::setup()
{
    getConfig("ataribios.conf", settings);
}


void AtariBios::registerTrapHandlers(Cpu* cpu)
{
    cpu->registerTrapHandler(1, this);
    cpu->registerTrapHandler(13, this);
    cpu->registerTrapHandler(14, this);
}

void AtariBios::bios(Cpu& cpu)
{
    // see also https://freemint.github.io/tos.hyp/en/About_the_BIOS.html
    enum Bios {
        GETMPB = 0,
        BCONSTAT = 1,
        BCONIN = 2,
        BCONOUT = 3,
        RWABS = 4,
        SETEXC = 5,
        TICKCAL = 6,
        GETBPB = 7,
        BCOSTAT = 8,
        MEDIACH = 9,
        DRVMAP = 10,
        KBSHIFT = 11,
    };
    bool isSupervisor = callerIsSupervisor(cpu);
    uint16_t func = argWord(cpu, isSupervisor, 0);

    switch (func)
    {
        case GETMPB: // getmpb(uint32_t buffer)
        {
            uint32_t buffer = argLong(cpu, isSupervisor, 1);
            getmpb(buffer);
            // no return value
            break;
        }

        case BCONSTAT: // bconstat(uint16_t devnum) -> int32_t
        {
            uint16_t devnum = argWord(cpu, isSupervisor, 1);
            int32_t ret = bconstat(devnum);
            cpu.setDRegister(0, static_cast<uint32_t>(ret));
            break;
        }

        case BCONIN: // bconin(uint16_t devnum) -> uint32_t (character/errno)
        {
            uint16_t devnum = argWord(cpu, isSupervisor, 1);
            uint32_t ch = bconin(devnum);
            cpu.setDRegister(0, ch);
            break;
        }

        case BCONOUT: // bconout(uint16_t devnum, uint16_t chr)
        {
            uint16_t devnum = argWord(cpu, isSupervisor, 1);
            uint16_t chr = argWord(cpu, isSupervisor, 2);
            bconout(devnum, chr);
            break;
        }

        case RWABS: // rwabs(uint16_t mode, uint32_t buffer, uint16_t sectors, uint16_t start, uint16_t drivenum) -> uint32_t
        {
            uint16_t mode = argWord(cpu, isSupervisor, 1);
            uint32_t buffer = argLong(cpu, isSupervisor, 2); // occupies word indices 2 and 3
            uint16_t sectors = argWord(cpu, isSupervisor, 4);
            uint16_t start = argWord(cpu, isSupervisor, 5);
            uint16_t drivenum = argWord(cpu, isSupervisor, 6);
            uint32_t ret = rwabs(mode, buffer, sectors, start, drivenum);
            cpu.setDRegister(0, ret);
            break;
        }

        case SETEXC: // setexec(uint16_t vecnum, uint32_t vecaddr) -> uint32_t
        {
            uint16_t vecnum = argWord(cpu, isSupervisor, 1);
            uint32_t vecaddr = argLong(cpu, isSupervisor, 2);
            uint32_t ret = setexec(vecnum, vecaddr);
            cpu.setDRegister(0, ret);
            break;
        }

        case TICKCAL: // Tickcal() -> uint32_t
        {
            uint32_t ret = Tickcal();
            cpu.setDRegister(0, ret);
            break;
        }

        case GETBPB: // getbpb(uint16_t drivenum) -> uint32_t
        {
            uint16_t drivenum = argWord(cpu, isSupervisor, 1);
            uint32_t ret = getbpb(drivenum);
            cpu.setDRegister(0, ret);
            break;
        }

        case BCOSTAT: // bcostat(uint16_t devnum) -> uint32_t
        {
            uint16_t devnum = argWord(cpu, isSupervisor, 1);
            uint32_t ret = bcostat(devnum);
            cpu.setDRegister(0, ret);
            break;
        }

        case MEDIACH: // mediach(uint16_t drivenum) -> uint32_t
        {
            uint16_t drivenum = argWord(cpu, isSupervisor, 1);
            uint32_t ret = mediach(drivenum);
            cpu.setDRegister(0, ret);
            break;
        }

        case DRVMAP: // drvmap() -> uint32_t
        {
            uint32_t ret = drvmap();
            cpu.setDRegister(0, ret);
            break;
        }

        case KBSHIFT: // kbshift(uint16_t mode) -> uint32_t
        {
            uint16_t mode = argWord(cpu, isSupervisor, 1);
            uint32_t ret = kbshift(mode);
            cpu.setDRegister(0, ret);
            break;
        }

        default:
            // For testing, return the given argument
            uint16_t arg1 = argWord(cpu, isSupervisor, 1);
            if (func & 1)
            {
                uint16_t result = argWord(cpu, isSupervisor, arg1);
                cpu.setDRegister(0, static_cast<uint32_t>(result));
            }
            else
            {
                uint32_t result = argLong(cpu, isSupervisor, arg1);
                cpu.setDRegister(0, result);
            }
            break;
    }
}



// BIOS methods
void AtariBios::getmpb(uint32_t buffer)
{
    // placeholder
}

/// <summary>
/// Return character-device input status,  D0.L  will
/// be  $0000 if no characters available, or $ffff if
/// (at least one) character is available.
/// </summary>
/// <param name="devnum">The device number</param>
/// <returns></returns>
int32_t AtariBios::bconstat(uint16_t devnum)
{
    if (devnum == 2) // keyboard
    {
#ifdef _WIN32
        if (_kbhit())
        {
            return 0xffff;
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
            return 0xffff;
        }
        return 0;
#endif
    }
    return 0;
}

/// <summary>
/// Wait until a character has been  input on the specified character device, and return the character
/// value in D0.L, with the high word zero.
/// </summary>
/// <param name="devnum">The device number</param>
/// <returns></returns>
uint32_t AtariBios::bconin(uint16_t devnum)
{
    if (devnum == 2) // keyboard
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
    return 0;
}

/// <summary>
/// Output character 'c' to  the  device. Does not return until the character has been written.
/// </summary>
/// <param name="devnum">The device number</param>
/// <param name="chr"><The character to ouput/param>
void AtariBios::bconout(uint16_t devnum, uint16_t chr)
{
    if (devnum == 2) // console
    {
        std::cout << static_cast<char>(chr & 0xff) << std::flush;
    }
}

uint32_t AtariBios::rwabs(uint16_t mode, uint32_t buffer, uint16_t sectors, uint16_t start, uint16_t drivenum)
{
    // placeholder
    return 0;
}

uint32_t AtariBios::setexec(uint16_t vecnum, uint32_t vecaddr)
{
    // placeholder
    return 0;
}

uint32_t AtariBios::Tickcal()
{
    // placeholder
    return 0;
}

uint32_t AtariBios::getbpb(uint16_t drivenum)
{
    // placeholder
    return 0;
}

uint32_t AtariBios::bcostat(uint16_t devnum)
{
    // placeholder
    return 0;
}

uint32_t AtariBios::mediach(uint16_t drivenum)
{
    // placeholder
    return 0;
}

uint32_t AtariBios::drvmap()
{
    // placeholder
    return 0;
}

uint32_t AtariBios::kbshift(uint16_t mode)
{
    // placeholder
    return 0;
}

