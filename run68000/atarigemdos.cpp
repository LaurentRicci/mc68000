#include "ataribios.h"

using namespace mc68000;

void AtariBios::gemdos(Cpu* cpu)
{
    //cf https://freemint.github.io/tos.hyp/en/gemdos_functions.html
    enum GemDos {
        GEMDOS_PTERM0 = 0,
        GEMDOS_CCONIN = 1,
        GEMDOS_CCONOUT = 2,
        GEMDOS_CAUXIN = 3,
        GEMDOS_CAUXOUT = 4,
        GEMDOS_CPRNOUT = 5,
        GEMDOS_CRAWIO = 6,
        GEMDOS_CRAWCIN = 7,
        GEMDOS_CNECIN = 8,
        GEMDOS_CCONWS = 9,
        GEMDOS_CCONRS = 10,
        GEMDOS_CCONIS = 11,
        GEMDOS_DSETDRV = 14,
        CCONOS = 16,
        CPRNOS = 17,
        CAUXIS = 18,
        CAUXOS = 19,
        MADDALT = 20,
        GEMDOS_DGETDRV = 25,
        GEMDOS_FSETDTA = 26,
        SUPER = 32,
        TGETDATE = 42,
        TSETDATE = 43,
        TGETTIME = 44,
        TSETTIME = 45,
        FGETDTA = 47,
        GEMDOS_SVERSION = 48,
        GEMDOS_PTERMRES = 49,
        GEMDOS_DFREE = 54,
        GEMDOS_DCREATE = 57,
        GEMDOS_DDELETE = 58,
        GEMDOS_DSETPATH = 59,
        GEMDOS_FCREATE = 60,
        GEMDOS_FOPEN = 61,
        GEMDOS_FCLOSE = 62,
        GEMDOS_FREAD = 63,
        GEMDOS_FWRITE = 64,
        GEMDOS_FDELETE = 65,
        GEMDOS_FSEEK = 66,
        GEMDOS_FATTRIB = 67,
        MXALLOC = 68,
        FDUP = 69,
        FFORCE = 71,
        GEMDOS_DGET_PATH = 71,
        GEMDOS_MALLOC = 72,
        GEMDOS_MFREE = 73,
        GEMDOS_MSHRINK = 74,
        GEMDOS_FSFIRST = 78,
        GEMDOS_FSNEXT = 79,
        GEMDOS_FRENAME = 86,
        GEMDOS_FDATIME = 87
    };

    uint32_t sp = cpu->a7; // A7 = SSP on entry to TRAP
    uint16_t savedSR = cpu->getFromStack<uint16_t>(true, 0); // Read saved SR from top of supervisor stack
    bool callerIsSupervisor = (savedSR & 0x2000) != 0; // Check S bit (bit 13)

    // Determine base of argument list: either sp+8 (supervisor) or usp+2 (user)
    uint32_t argBase;
    if (callerIsSupervisor)
    {
        argBase = 6;
    }
    else
    {
        argBase = 0;
    }

    // helpers to retrieve arguments from the stack.
    // The stack can have a mix of word and long arguments so the indexing is done in words.
    // argWord(0) is function number, argWord(1) is first argument word, etc.
    // argLong(1) reads a long starting at word index 1 (i.e., words 1 and 2)
    // argLong(n) reads two words starting at word index n to make a long
    //
    auto argWord = [&](unsigned index) -> uint16_t {
        return cpu->getFromStack<uint16_t>(callerIsSupervisor, argBase + index * 2);
    };
    auto argLong = [&](unsigned wordIndex) -> uint32_t {
        return cpu->getFromStack<uint32_t>(callerIsSupervisor, argBase + wordIndex * 2);
    };

    uint16_t func = argWord(0);

    // Dispatch GEMDOS functions
    switch (func)
    {
    case GEMDOS_PTERM0: // pterm0()
    {
        pterm0();
        break;
    }

    case GEMDOS_CCONIN: // cconin() -> uint32_t
    {
        uint32_t ret = cconin();
        cpu->setDRegister(0, ret);
        break;
    }

    case GEMDOS_CCONOUT: // cconout(uint16_t c)
    {
        uint16_t c = argWord(1);
        cconout(c);
        break;
    }

    case GEMDOS_CCONWS: // cconws(const char* str)
    {
        uint32_t address = argLong(1);
        char* str = static_cast<char*>(cpu->mem.get<void*>(address));
        cconws(str);
        break;
    }

    case GEMDOS_CCONIS: // cconis() -> uint16_t
    {
        uint16_t ret = cconis();
        cpu->setDRegister(0, static_cast<uint32_t>(ret));
        break;
    }

    case GEMDOS_DSETDRV: // dsetdrv(uint16_t drv) -> uint16_t
    {
        uint16_t drv = argWord(1);
        uint16_t ret = dsetdrv(drv);
        cpu->setDRegister(0, static_cast<uint32_t>(ret));
        break;
    }

    case GEMDOS_DGETDRV: // dgetdrv() -> uint16_t
    {
        uint16_t ret = dgetdrv();
        cpu->setDRegister(0, static_cast<uint32_t>(ret));
        break;
    }

    case GEMDOS_FSETDTA: // fsetdta(DTA* dta)
    {
        DTA* dta = reinterpret_cast<DTA*>(static_cast<uintptr_t>(argLong(1)));
        fsetdta(dta);
        break;
    }

    case GEMDOS_SVERSION: // sversion() -> uint16_t
    {
        uint16_t ret = sversion();
        cpu->setDRegister(0, static_cast<uint32_t>(ret));
        break;
    }

    case GEMDOS_PTERMRES: // ptermres(uint32_t keep, uint16_t rc)
    {
        uint32_t keep = argLong(1);
        uint16_t rc = argWord(3);
        ptermres(keep, rc);
        break;
    }

    case GEMDOS_DCREATE: // dcreate(const char* path) -> uint16_t
    {
        const char* path = reinterpret_cast<const char*>(static_cast<uintptr_t>(argLong(1)));
        uint16_t ret = dcreate(path);
        cpu->setDRegister(0, static_cast<uint32_t>(ret));
        break;
    }

    case GEMDOS_DDELETE: // ddelete(const char* path) -> uint16_t
    {
        const char* path = reinterpret_cast<const char*>(static_cast<uintptr_t>(argLong(1)));
        uint16_t ret = ddelete(path);
        cpu->setDRegister(0, static_cast<uint32_t>(ret));
        break;
    }

    case GEMDOS_DSETPATH: // dsetpath(const char* path) -> uint16_t
    {
        const char* path = reinterpret_cast<const char*>(static_cast<uintptr_t>(argLong(1)));
        uint16_t ret = dsetpath(path);
        cpu->setDRegister(0, static_cast<uint32_t>(ret));
        break;
    }

    case GEMDOS_FCREATE: // fcreate(const char* name, uint16_t attr) -> uint16_t
    {
        const char* name = reinterpret_cast<const char*>(static_cast<uintptr_t>(argLong(1)));
        uint16_t attr = argWord(3);
        uint16_t ret = fcreate(name, attr);
        cpu->setDRegister(0, static_cast<uint32_t>(ret));
        break;
    }

    case GEMDOS_FOPEN: // fopen(const char* name, uint16_t mode) -> uint16_t
    {
        const char* name = reinterpret_cast<const char*>(static_cast<uintptr_t>(argLong(1)));
        uint16_t mode = argWord(3);
        uint16_t ret = fopen(name, mode);
        cpu->setDRegister(0, static_cast<uint32_t>(ret));
        break;
    }

    case GEMDOS_FCLOSE: // fclose(uint16_t handle) -> uint16_t
    {
        uint16_t handle = argWord(1);
        uint16_t ret = fclose(handle);
        cpu->setDRegister(0, static_cast<uint32_t>(ret));
        break;
    }

    case GEMDOS_FREAD: // fread(uint16_t handle, uint32_t count, void* buf) -> uint32_t
    {
        uint16_t handle = argWord(1);
        uint32_t count = argLong(2);
        void* buf = reinterpret_cast<void*>(static_cast<uintptr_t>(argLong(4)));
        uint32_t ret = fread(handle, count, buf);
        cpu->setDRegister(0, ret);
        break;
    }

    case GEMDOS_FWRITE: // fwrite(uint16_t handle, uint32_t count, const void* buf) -> uint32_t
    {
        uint16_t handle = argWord(1);
        uint32_t count = argLong(2);
        const void* buf = reinterpret_cast<const void*>(static_cast<uintptr_t>(argLong(4)));
        uint32_t ret = fwrite(handle, count, buf);
        cpu->setDRegister(0, ret);
        break;
    }

    case GEMDOS_FDELETE: // fdelete(const char* name) -> uint16_t
    {
        const char* name = reinterpret_cast<const char*>(static_cast<uintptr_t>(argLong(1)));
        uint16_t ret = fdelete(name);
        cpu->setDRegister(0, static_cast<uint32_t>(ret));
        break;
    }

    case GEMDOS_FSEEK: // fseek(uint32_t offset, uint16_t handle, uint16_t mode) -> uint32_t
    {
        uint32_t offset = argLong(1);
        uint16_t handle = argWord(3);
        uint16_t mode = argWord(4);
        uint32_t ret = fseek(offset, handle, mode);
        cpu->setDRegister(0, ret);
        break;
    }

    case GEMDOS_FATTRIB: // fattrib(const char* name, uint16_t wflag, uint16_t attrib) -> uint16_t
    {
        const char* name = reinterpret_cast<const char*>(static_cast<uintptr_t>(argLong(1)));
        uint16_t wflag = argWord(3);
        uint16_t attrib = argWord(4);
        uint16_t ret = fattrib(name, wflag, attrib);
        cpu->setDRegister(0, static_cast<uint32_t>(ret));
        break;
    }

    case GEMDOS_DGET_PATH: // dgetpath(char* buf, uint16_t drv) -> uint16_t
    {
        char* buf = reinterpret_cast<char*>(static_cast<uintptr_t>(argLong(1)));
        uint16_t drv = argWord(3);
        uint16_t ret = dgetpath(buf, drv);
        cpu->setDRegister(0, static_cast<uint32_t>(ret));
        break;
    }

    case GEMDOS_MALLOC: // malloc(uint32_t size) -> void*
    {
        uint32_t size = argLong(1);
        void* ptr = malloc(size);
        cpu->setDRegister(0, static_cast<uint32_t>(reinterpret_cast<uintptr_t>(ptr)));
        break;
    }

    case GEMDOS_MFREE: // mfree(void* ptr) -> uint16_t
    {
        void* ptr = reinterpret_cast<void*>(static_cast<uintptr_t>(argLong(1)));
        uint16_t ret = mfree(ptr);
        cpu->setDRegister(0, static_cast<uint32_t>(ret));
        break;
    }

    case GEMDOS_MSHRINK: // mshrink(void* base, uint32_t size) -> uint16_t
    {
        void* base = reinterpret_cast<void*>(static_cast<uintptr_t>(argLong(1)));
        uint32_t size = argLong(3);
        uint16_t ret = mshrink(base, size);
        cpu->setDRegister(0, static_cast<uint32_t>(ret));
        break;
    }

    case GEMDOS_FSFIRST: // fsfirst(const char* pattern, uint16_t attr) -> uint16_t
    {
        const char* pattern = reinterpret_cast<const char*>(static_cast<uintptr_t>(argLong(1)));
        uint16_t attr = argWord(3);
        uint16_t ret = fsfirst(pattern, attr);
        cpu->setDRegister(0, static_cast<uint32_t>(ret));
        break;
    }

    case GEMDOS_FSNEXT: // fsnext() -> uint16_t
    {
        uint16_t ret = fsnext();
        cpu->setDRegister(0, static_cast<uint32_t>(ret));
        break;
    }

    case GEMDOS_FRENAME: // frename(uint16_t zero, const char* old, const char* neu) -> uint16_t
    {
        uint16_t zero = argWord(1);
        const char* oldp = reinterpret_cast<const char*>(static_cast<uintptr_t>(argLong(2)));
        const char* neu = reinterpret_cast<const char*>(static_cast<uintptr_t>(argLong(4)));
        uint16_t ret = frename(zero, oldp, neu);
        cpu->setDRegister(0, static_cast<uint32_t>(ret));
        break;
    }

    case GEMDOS_FDATIME: // fdatime(uint16_t* time, uint16_t* date, uint16_t handle, uint16_t rwflag) -> uint16_t
    {
        uint16_t* timep = reinterpret_cast<uint16_t*>(static_cast<uintptr_t>(argLong(1)));
        uint16_t* datep = reinterpret_cast<uint16_t*>(static_cast<uintptr_t>(argLong(3)));
        uint16_t handle = argWord(5);
        uint16_t rwflag = argWord(6);
        uint16_t ret = fdatime(timep, datep, handle, rwflag);
        cpu->setDRegister(0, static_cast<uint32_t>(ret));
        break;
    }

    default:
        cpu->setDRegister(0, 0);
        break;
    }
}

// GEMDOS helper implementations (placeholders)
void AtariBios::pterm0()
{
}

uint32_t AtariBios::cconin()
{
    return 0;
}

void AtariBios::cconout(uint16_t c)
{
}

/// <summary>
/// Write a string to the standard output device
/// </summary>
/// <param name="s">The string to output</param>
void AtariBios::cconws(const char* s)
{
    std::cout << s << std::flush;
}

uint16_t AtariBios::cconis()
{
    return 0;
}

uint16_t AtariBios::dsetdrv(uint16_t drv)
{
    return 0;
}

uint16_t AtariBios::dgetdrv()
{
    return 0;
}

void AtariBios::fsetdta(DTA* dta)
{
}

uint16_t AtariBios::sversion()
{
    return 0;
}

void AtariBios::ptermres(uint32_t keep, uint16_t rc)
{
}

uint16_t AtariBios::dcreate(const char* path)
{
    return 0;
}

uint16_t AtariBios::ddelete(const char* path)
{
    return 0;
}

uint16_t AtariBios::dsetpath(const char* path)
{
    return 0;
}

uint16_t AtariBios::fcreate(const char* name, uint16_t attr)
{
    return 0;
}

uint16_t AtariBios::fopen(const char* name, uint16_t mode)
{
    return 0;
}

uint16_t AtariBios::fclose(uint16_t handle)
{
    return 0;
}

uint32_t AtariBios::fread(uint16_t handle, uint32_t count, void* buf)
{
    return 0;
}

uint32_t AtariBios::fwrite(uint16_t handle, uint32_t count, const void* buf)
{
    return 0;
}

uint16_t AtariBios::fdelete(const char* name)
{
    return 0;
}

uint32_t AtariBios::fseek(uint32_t offset, uint16_t handle, uint16_t mode)
{
    return 0;
}

uint16_t AtariBios::fattrib(const char* name, uint16_t wflag, uint16_t attrib)
{
    return 0;
}

uint16_t AtariBios::dgetpath(char* buf, uint16_t drv)
{
    return 0;
}

void* AtariBios::malloc(uint32_t size)
{
    return nullptr;
}

uint16_t AtariBios::mfree(void* ptr)
{
    return 0;
}

uint16_t AtariBios::mshrink(void* base, uint32_t size)
{
    return 0;
}

uint16_t AtariBios::fsfirst(const char* pattern, uint16_t attr)
{
    return 0;
}

uint16_t AtariBios::fsnext()
{
    return 0;
}

uint16_t AtariBios::frename(uint16_t zero, const char* old, const char* neu)
{
    return 0;
}

uint16_t AtariBios::fdatime(uint16_t* time, uint16_t* date, uint16_t handle, uint16_t rwflag)
{
    return 0;
}
