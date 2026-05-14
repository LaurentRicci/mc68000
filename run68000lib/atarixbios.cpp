#include "trapargs.h"
#include "ataribios.h"

using namespace mc68000;

void AtariBios::xbios(Cpu& cpu)
{
    // see also https://freemint.github.io/tos.hyp/en/xbios_about.html
    enum XBios {
        INITMOUS = 0,
        PHYSBASE = 2,
        LOGBASE = 3,
        GETREZ = 4,
        SETSCREEN = 5,
        SETPALETTE = 6,
        SETCOLOR = 7,
        FLOPRD = 8,
        FLOPWR = 9,
        FLOPFMT = 10,
        MIDIWS = 12,
        MFPINT = 13,
        IOREC = 14,
        RSCONF = 15,
        KEYTBL = 16,
        RANDOM = 17,
        PROTOTB = 18,
        FLOPVER = 19,
        SCRDMP = 20,
        CURSCONF = 21,
        SETTIME = 22,
        GETTIME = 23,
        BIOSKEYS = 24,
        IKBDWS = 25,
        JDISINT = 26,
        JENABINT = 27,
        GIACCESS = 28,
        OFFGIBIT = 29,
        ONGIBIT = 30,
        XBTIMER = 31,
        DOSOUND = 32,
        SETPRT = 33,
        KBDVBASE = 34,
        KBRATE = 35,
        PRTBLK = 36,
        VSYNC = 37,
        SUPEXEC = 38,
        BLITMODE = 39,
    };

    bool isSupervisor = callerIsSupervisor(cpu);
    uint16_t func = argWord(cpu, isSupervisor, 0);

    switch (func)
    {
    case INITMOUS: // initmous(uint16_t mode, uint32_t param, uint32_t vector)
    {
        uint16_t mode = argWord(cpu, isSupervisor,1);
        uint32_t param = argLong(cpu, isSupervisor, 2);
        uint32_t vector = argLong(cpu, isSupervisor, 4);
        initmous(mode, param, vector);
        break;
    }

    case PHYSBASE: // physbase() -> uint32_t
    {
        uint32_t ret = physbase();
        cpu.setDRegister(0, ret);
        break;
    }

    case LOGBASE: // logbase() -> uint32_t
    {
        uint32_t ret = logbase();
        cpu.setDRegister(0, ret);
        break;
    }

    case GETREZ: // getrez() -> uint16_t
    {
        uint16_t ret = getrez();
        cpu.setDRegister(0, static_cast<uint32_t>(ret));
        break;
    }

    case SETSCREEN: // setScreen(uint32_t logaddr, uint32_t physaddr, uint16_t rez) -> uint32_t
    {
        uint32_t logaddr = argLong(cpu, isSupervisor, 1);
        uint32_t physaddr = argLong(cpu, isSupervisor, 3);
        uint16_t rez = argWord(cpu, isSupervisor,5);
        uint32_t ret = setScreen(logaddr, physaddr, rez);
        cpu.setDRegister(0, ret);
        break;
    }

    case SETPALETTE: // setpalette(uint32_t palette)
    {
        uint32_t palette = argLong(cpu, isSupervisor, 1);
        setpalette(palette);
        break;
    }

    case SETCOLOR: // setcolor(uint16_t reg, uint16_t newcolor) -> uint16_t
    {
        uint16_t reg = argWord(cpu, isSupervisor,1);
        uint16_t newcolor = argWord(cpu, isSupervisor,2);
        uint16_t ret = setcolor(reg, newcolor);
        cpu.setDRegister(0, static_cast<uint32_t>(ret));
        break;
    }

    case FLOPRD: // floprd(uint32_t buf, uint32_t resvd, uint16_t devnum, uint16_t secnum, uint16_t tracknum, uint16_t sidenum, uint16_t numsecs) -> uint16_t
    {
        uint32_t buf = argLong(cpu, isSupervisor, 1);
        uint32_t resvd = argLong(cpu, isSupervisor, 3);
        uint16_t devnum = argWord(cpu, isSupervisor,5);
        uint16_t secnum = argWord(cpu, isSupervisor,6);
        uint16_t tracknum = argWord(cpu, isSupervisor,7);
        uint16_t sidenum = argWord(cpu, isSupervisor,8);
        uint16_t numsecs = argWord(cpu, isSupervisor,9);
        uint16_t ret = floprd(buf, resvd, devnum, secnum, tracknum, sidenum, numsecs);
        cpu.setDRegister(0, static_cast<uint32_t>(ret));
        break;
    }

    case FLOPWR: // flopwr(...) -> uint16_t
    {
        uint32_t buf = argLong(cpu, isSupervisor, 1);
        uint32_t resvd = argLong(cpu, isSupervisor, 3);
        uint16_t devnum = argWord(cpu, isSupervisor,5);
        uint16_t secnum = argWord(cpu, isSupervisor,6);
        uint16_t tracknum = argWord(cpu, isSupervisor,7);
        uint16_t sidenum = argWord(cpu, isSupervisor,8);
        uint16_t numsecs = argWord(cpu, isSupervisor,9);
        uint16_t ret = flopwr(buf, resvd, devnum, secnum, tracknum, sidenum, numsecs);
        cpu.setDRegister(0, static_cast<uint32_t>(ret));
        break;
    }

    case FLOPFMT: // flopfmt(uint32_t buffer, uint32_t skewtabl, uint16_t devnum, uint16_t spt, uint16_t tracknum, uint16_t sidenum, uint16_t intrlev, uint32_t magic, uint16_t initial) -> uint16_t
    {
        uint32_t buffer = argLong(cpu, isSupervisor, 1);
        uint32_t skewtabl = argLong(cpu, isSupervisor, 3);
        uint16_t devnum = argWord(cpu, isSupervisor,5);
        uint16_t spt = argWord(cpu, isSupervisor,6);
        uint16_t tracknum = argWord(cpu, isSupervisor,7);
        uint16_t sidenum = argWord(cpu, isSupervisor,8);
        uint16_t intrlev = argWord(cpu, isSupervisor,9);
        uint32_t magic = argLong(cpu, isSupervisor, 10);
        uint16_t initial = argWord(cpu, isSupervisor,12);
        uint16_t ret = flopfmt(buffer, skewtabl, devnum, spt, tracknum, sidenum, intrlev, magic, initial);
        cpu.setDRegister(0, static_cast<uint32_t>(ret));
        break;
    }

    case MIDIWS: // midiws(uint16_t bytes, uint32_t buffer)
    {
        uint16_t bytes = argWord(cpu, isSupervisor,1);
        uint32_t buffer = argLong(cpu, isSupervisor, 2);
        midiws(bytes, buffer);
        break;
    }

    case MFPINT: // mfpint(uint16_t number, uint32_t vector)
    {
        uint16_t number = argWord(cpu, isSupervisor,1);
        uint32_t vector = argLong(cpu, isSupervisor, 2);
        mfpint(number, vector);
        break;
    }

    case IOREC: // iorec(uint16_t dev) -> uint32_t
    {
        uint16_t dev = argWord(cpu, isSupervisor,1);
        uint32_t ret = iorec(dev);
        cpu.setDRegister(0, ret);
        break;
    }

    case RSCONF: // rsconf(uint16_t speed, uint16_t ucr, uint16_t rsr, uint16_t tsr, uint16_t scr)
    {
        uint16_t speed = argWord(cpu, isSupervisor,1);
        uint16_t ucr = argWord(cpu, isSupervisor,2);
        uint16_t rsr = argWord(cpu, isSupervisor,3);
        uint16_t tsr = argWord(cpu, isSupervisor,4);
        uint16_t scr = argWord(cpu, isSupervisor,5);
        rsconf(speed, ucr, rsr, tsr, scr);
        break;
    }

    case KEYTBL: // keytbl(uint32_t unshift, uint32_t shift, uint32_t capslock) -> uint32_t
    {
        uint32_t unshift = argLong(cpu, isSupervisor, 1);
        uint32_t shift = argLong(cpu, isSupervisor, 3);
        uint32_t capslock = argLong(cpu, isSupervisor, 5);
        uint32_t ret = keytbl(unshift, shift, capslock);
        cpu.setDRegister(0, ret);
        break;
    }

    case RANDOM: // random() -> uint32_t
    {
        uint32_t ret = random();
        cpu.setDRegister(0, ret);
        break;
    }

    case PROTOTB: // protobt(uint32_t buffer, uint32_t serialnum, uint16_t disktype, uint16_t execflag)
    {
        uint32_t buffer = argLong(cpu, isSupervisor, 1);
        uint32_t serialnum = argLong(cpu, isSupervisor, 3);
        uint16_t disktype = argWord(cpu, isSupervisor,5);
        uint16_t execflag = argWord(cpu, isSupervisor,6);
        protobt(buffer, serialnum, disktype, execflag);
        break;
    }

    case FLOPVER: // flopver(...) -> uint16_t
    {
        uint32_t buf = argLong(cpu, isSupervisor, 1);
        uint32_t resvd = argLong(cpu, isSupervisor, 3);
        uint16_t devnum = argWord(cpu, isSupervisor,5);
        uint16_t secnum = argWord(cpu, isSupervisor,6);
        uint16_t tracknum = argWord(cpu, isSupervisor,7);
        uint16_t sidenum = argWord(cpu, isSupervisor,8);
        uint16_t numsec = argWord(cpu, isSupervisor,9);
        uint16_t ret = flopver(buf, resvd, devnum, secnum, tracknum, sidenum, numsec);
        cpu.setDRegister(0, static_cast<uint32_t>(ret));
        break;
    }

    case SCRDMP: // scrdmp()
    {
        scrdmp();
        break;
    }

    case CURSCONF: // cursconf(uint16_t mode, uint16_t newrate) -> uint16_t
    {
        uint16_t mode = argWord(cpu, isSupervisor,1);
        uint16_t newrate = argWord(cpu, isSupervisor,2);
        uint16_t ret = cursconf(mode, newrate);
        cpu.setDRegister(0, static_cast<uint32_t>(ret));
        break;
    }

    case SETTIME: // settime(uint32_t datetime)
    {
        uint32_t datetime = argLong(cpu, isSupervisor, 1);
        settime(datetime);
        break;
    }

    case GETTIME: // gettime() -> uint32_t
    {
        uint32_t ret = gettime();
        cpu.setDRegister(0, ret);
        break;
    }

    case BIOSKEYS: // bioskeys()
    {
        bioskeys();
        break;
    }

    case IKBDWS: // ikbdws(uint16_t bytes, uint32_t buffer)
    {
        uint16_t bytes = argWord(cpu, isSupervisor,1);
        uint32_t buffer = argLong(cpu, isSupervisor, 2);
        ikbdws(bytes, buffer);
        break;
    }

    case JDISINT: // jdisint(uint16_t intnum)
    {
        uint16_t intnum = argWord(cpu, isSupervisor,1);
        jdisint(intnum);
        break;
    }

    case JENABINT: // jenabint(uint16_t intnum) -> uint16_t
    {
        uint16_t intnum = argWord(cpu, isSupervisor,1);
        uint16_t ret = jenabint(intnum);
        cpu.setDRegister(0, static_cast<uint32_t>(ret));
        break;
    }

    case GIACCESS: // giaccess(uint8_t value, uint16_t regnum) -> uint8_t
    {
        uint8_t value = static_cast<uint8_t>(argWord(cpu, isSupervisor,1) & 0xFF);
        uint16_t regnum = argWord(cpu, isSupervisor,2);
        uint8_t ret = giaccess(value, regnum);
        cpu.setDRegister(0, static_cast<uint32_t>(ret));
        break;
    }

    case OFFGIBIT: // offgibit(uint16_t bitnum)
    {
        uint16_t bitnum = argWord(cpu, isSupervisor,1);
        offgibit(bitnum);
        break;
    }

    case ONGIBIT: // ongibit(uint16_t bitnum)
    {
        uint16_t bitnum = argWord(cpu, isSupervisor,1);
        ongibit(bitnum);
        break;
    }

    case XBTIMER: // xbtimer(uint16_t timernum, uint16_t control, uint16_t data, uint32_t vector)
    {
        uint16_t timernum = argWord(cpu, isSupervisor,1);
        uint16_t control = argWord(cpu, isSupervisor,2);
        uint16_t data = argWord(cpu, isSupervisor,3);
        uint32_t vector = argLong(cpu, isSupervisor, 4);
        xbtimer(timernum, control, data, vector);
        break;
    }

    case DOSOUND: // dosound(uint32_t commandlist)
    {
        uint32_t commandlist = argLong(cpu, isSupervisor, 1);
        dosound(commandlist);
        break;
    }

    case SETPRT: // setprt(uint16_t newcode) -> uint16_t
    {
        uint16_t newcode = argWord(cpu, isSupervisor,1);
        uint16_t ret = setprt(newcode);
        cpu.setDRegister(0, static_cast<uint32_t>(ret));
        break;
    }

    case KBDVBASE: // kbdvbase()
    {
        kbdvbase();
        break;
    }

    case KBRATE: // kbrate(uint16_t delay, uint16_t rate) -> uint16_t
    {
        uint16_t delay = argWord(cpu, isSupervisor,1);
        uint16_t rate = argWord(cpu, isSupervisor,2);
        uint16_t ret = kbrate(delay, rate);
        cpu.setDRegister(0, static_cast<uint32_t>(ret));
        break;
    }

    case PRTBLK: // prtblk(uint32_t prtable)
    {
        uint32_t prtable = argLong(cpu, isSupervisor, 1);
        prtblk(prtable);
        break;
    }

    case VSYNC: // vsync()
    {
        vsync();
        break;
    }

    case SUPEXEC: // supexec(uint32_t sub)
    {
        uint32_t sub = argLong(cpu, isSupervisor, 1);
        supexec(sub);
        break;
    }

    case BLITMODE: // blitmode(uint16_t value) -> uint16_t
    {
        uint16_t value = argWord(cpu, isSupervisor,1);
        uint16_t ret = blitmode(value);
        cpu.setDRegister(0, static_cast<uint32_t>(ret));
        break;
    }

    default:
        cpu.setDRegister(0, 0);
        break;
    }
}
// XBIOS methods
void AtariBios::initmous(uint16_t mode, uint32_t param, uint32_t vector)
{
    // placeholder
}

uint32_t AtariBios::physbase()
{
    // placeholder
    return 0;
}

uint32_t AtariBios::logbase()
{
    // placeholder
    return 0;
}

uint16_t AtariBios::getrez()
{
    // placeholder
    return 0;
}

uint32_t AtariBios::setScreen(uint32_t logaddr, uint32_t physaddr, uint16_t rez)
{
    // placeholder
    return 0;
}

void AtariBios::setpalette(uint32_t palette)
{
    // placeholder
}

uint16_t AtariBios::setcolor(uint16_t reg, uint16_t newcolor)
{
    // placeholder
    return 0;
}

uint16_t AtariBios::floprd(uint32_t buf, uint32_t resvd, uint16_t devnum, uint16_t secnum, uint16_t tracknum, uint16_t sidenum, uint16_t numsecs)
{
    // placeholder
    return 0;
}

uint16_t AtariBios::flopwr(uint32_t buf, uint32_t resvd, uint16_t devnum, uint16_t secnum, uint16_t tracknum, uint16_t sidenum, uint16_t numsecs)
{
    // placeholder
    return 0;
}

uint16_t AtariBios::flopfmt(uint32_t buffer, uint32_t skewtabl, uint16_t devnum, uint16_t spt, uint16_t tracknum, uint16_t sidenum, uint16_t intrlev, uint32_t magic, uint16_t initial)
{
    // placeholder
    return 0;
}

void AtariBios::midiws(uint16_t bytes, uint32_t buffer)
{
    // placeholder
}

void AtariBios::mfpint(uint16_t number, uint32_t vector)
{
    // placeholder
}

uint32_t AtariBios::iorec(uint16_t dev)
{
    // placeholder
    return 0;
}

void AtariBios::rsconf(uint16_t speed, uint16_t ucr, uint16_t rsr, uint16_t tsr, uint16_t scr)
{
    // placeholder
}

uint32_t AtariBios::keytbl(uint32_t unshift, uint32_t shift, uint32_t capslock)
{
    // placeholder
    return 0;
}

uint32_t AtariBios::random()
{
    // placeholder
    return 0;
}

void AtariBios::protobt(uint32_t buffer, uint32_t serialnum, uint16_t disktype, uint16_t execflag)
{
    // placeholder
}

uint16_t AtariBios::flopver(uint32_t buf, uint32_t resvd, uint16_t devnum, uint16_t secnum, uint16_t tracknum, uint16_t sidenum, uint16_t numsec)
{
    // placeholder
    return 0;
}

void AtariBios::scrdmp()
{
    // placeholder
}

uint16_t AtariBios::cursconf(uint16_t mode, uint16_t newrate)
{
    // placeholder
    return 0;
}

void AtariBios::settime(uint32_t datetime)
{
    // placeholder
}

uint32_t AtariBios::gettime()
{
    // placeholder
    return 0;
}

void AtariBios::bioskeys()
{
    // placeholder
}

void AtariBios::ikbdws(uint16_t bytes, uint32_t buffer)
{
    // placeholder
}

void AtariBios::jdisint(uint16_t intnum)
{
    // placeholder
}

uint16_t AtariBios::jenabint(uint16_t intnum)
{
    // placeholder
    return 0;
}

uint8_t AtariBios::giaccess(uint8_t value, uint16_t regnum)
{
    // placeholder
    return 0;
}

void AtariBios::offgibit(uint16_t bitnum)
{
    // placeholder
}

void AtariBios::ongibit(uint16_t bitnum)
{
    // placeholder
}

void AtariBios::xbtimer(uint16_t timernum, uint16_t control, uint16_t data, uint32_t vector)
{
    // placeholder
}

void AtariBios::dosound(uint32_t commandlist)
{
    // placeholder
}

uint16_t AtariBios::setprt(uint16_t newcode)
{
    // placeholder
    return 0;
}

void AtariBios::kbdvbase()
{
    // placeholder
}

uint16_t AtariBios::kbrate(uint16_t delay, uint16_t rate)
{
    // placeholder
    return 0;
}

void AtariBios::prtblk(uint32_t prtable)
{
    // placeholder
}

void AtariBios::vsync()
{
    // placeholder
}

void AtariBios::supexec(uint32_t sub)
{
    // placeholder
}

uint16_t AtariBios::blitmode(uint16_t value)
{
    // placeholder
    return 0;
}

