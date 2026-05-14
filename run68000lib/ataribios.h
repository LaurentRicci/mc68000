#pragma once
#include <unordered_map>
#include "ibios.h"

namespace mc68000
{
    class AtariBios : public IBios, public TrapHandler
    {
    public:
        void setup() override;
        void registerTrapHandlers(Cpu* cpu) override;
        void handle(Cpu& cpu, uint16_t vector) override
        {
            switch (vector)
            {
            case 1:
                bios(cpu);
                break;
            case 13:
                xbios(cpu);
                break;
            case 14:
                gemdos(cpu);
                break;
            default:
                throw "AtariBios: unhandled trap vector";
            }
        }
    private:
        std::unordered_map<std::string, std::string> settings;
    private:
        static void bios(Cpu& cpu);
        static void xbios(Cpu& cpu);
        static void gemdos(Cpu& cpu);

        // BIOS methods
        static void getmpb(uint32_t buffer);
        static int32_t bconstat(uint16_t devnum);
        static uint32_t bconin(uint16_t devnum);
        static void bconout(uint16_t devnum, uint16_t chr);
        static uint32_t rwabs(uint16_t mode, uint32_t buffer, uint16_t sectors, uint16_t start, uint16_t drivenum);
        static uint32_t setexec(uint16_t vecnum, uint32_t vecaddr);
        static uint32_t Tickcal();
        static uint32_t getbpb(uint16_t drivenum);
        static uint32_t bcostat(uint16_t devnum);
        static uint32_t mediach(uint16_t drivenum);
        static uint32_t drvmap();
        static uint32_t kbshift(uint16_t mode);

        // XBIOS methods
        static void initmous(uint16_t mode, uint32_t param, uint32_t vector);
        static uint32_t physbase();
        static uint32_t logbase();
        static uint16_t getrez();
        static uint32_t setScreen(uint32_t logaddr, uint32_t physaddr, uint16_t rez);
        static void setpalette(uint32_t palette);
        static uint16_t setcolor(uint16_t reg, uint16_t newcolor);
        static uint16_t floprd(uint32_t buf, uint32_t resvd, uint16_t devnum, uint16_t secnum, uint16_t tracknum, uint16_t sidenum, uint16_t numsecs);
        static uint16_t flopwr(uint32_t buf, uint32_t resvd, uint16_t devnum, uint16_t secnum, uint16_t tracknum, uint16_t sidenum, uint16_t numsecs);
        static uint16_t flopfmt(uint32_t buffer, uint32_t skewtabl, uint16_t devnum, uint16_t spt, uint16_t tracknum, uint16_t sidenum, uint16_t intrlev, uint32_t magic, uint16_t initial);
        static void midiws(uint16_t bytes, uint32_t buffer);
        static void mfpint(uint16_t number, uint32_t vector);
        static uint32_t iorec(uint16_t dev);
        static void rsconf(uint16_t speed, uint16_t ucr, uint16_t rsr, uint16_t tsr, uint16_t scr);
        static uint32_t keytbl(uint32_t unshift, uint32_t shift, uint32_t capslock);
        static uint32_t random();
        static void protobt(uint32_t buffer, uint32_t serialnum, uint16_t disktype, uint16_t execflag);
        static uint16_t flopver(uint32_t buf, uint32_t resvd, uint16_t devnum, uint16_t secnum, uint16_t tracknum, uint16_t sidenum, uint16_t numsec);
        static void scrdmp();
        static uint16_t cursconf(uint16_t mode, uint16_t newrate);
        static void settime(uint32_t datetime);
        static uint32_t gettime();
        static void bioskeys();
        static void ikbdws(uint16_t bytes, uint32_t buffer);
        static void jdisint(uint16_t intnum);
        static uint16_t jenabint(uint16_t intnum);
        static uint8_t giaccess(uint8_t value, uint16_t regnum);
        static void offgibit(uint16_t bitnum);
        static void ongibit(uint16_t bitnum);
        static void xbtimer(uint16_t timernum, uint16_t control, uint16_t data, uint32_t vector);
        static void dosound(uint32_t commandlist);
        static uint16_t setprt(uint16_t newcode);
        static void kbdvbase();
        static uint16_t kbrate(uint16_t delay, uint16_t rate);
        static void prtblk(uint32_t prtable);
        static void vsync();
        static void supexec(uint32_t sub);
        static uint16_t blitmode(uint16_t value);

        // GEMDOS methods
        static void pterm0();
        static uint32_t cconin();
        static void cconout(uint16_t c);
        static void cconws(const char* s);
        static uint16_t cconis();
        static uint16_t dsetdrv(uint16_t drv);
        static uint16_t dgetdrv();

        struct DTA {
            uint8_t d_reserved[21];
            uint8_t d_attrib;
            uint16_t d_time;
            uint16_t d_date;
            uint32_t d_length;
            uint8_t  d_fname[14];
        };
        static void fsetdta(DTA* dta);
        static uint16_t sversion();
        static void ptermres(uint32_t keep, uint16_t rc);
        static uint16_t dcreate(const char* path);
        static uint16_t ddelete(const char* path);
        static uint16_t dsetpath(const char* path);
        static uint16_t fcreate(const char* name, uint16_t attr);
        static uint16_t fopen(const char* name, uint16_t mode);
        static uint16_t fclose(uint16_t handle);
        static uint32_t fread(uint16_t handle, uint32_t count, void* buf);
        static uint32_t fwrite(uint16_t handle, uint32_t count, const void* buf);
        static uint16_t fdelete(const char* name);
        static uint32_t fseek(uint32_t offset, uint16_t handle, uint16_t mode);
        static uint16_t fattrib(const char* name, uint16_t wflag, uint16_t attrib);
        static uint16_t dgetpath(char* buf, uint16_t drv);
        static void* malloc(uint32_t size);
        static uint16_t mfree(void* ptr);
        static uint16_t mshrink(void* base, uint32_t size);
        static uint16_t fsfirst(const char* pattern, uint16_t attr);
        static uint16_t fsnext();
        static uint16_t frename(uint16_t zero, const char* old, const char* neu);
        static uint16_t fdatime(uint16_t* time, uint16_t* date, uint16_t handle, uint16_t rwflag);
    };
}
/*
see also https://www.atari-wiki.com/index.php?title=GemDos_BIOS_functions
#ifndef ATARI_TOS_API_HPP
#define ATARI_TOS_API_HPP

extern "C" {

// ---------------------------------------------------------------------
// Low-level trap entry points (usually provided by the C runtime)
// ---------------------------------------------------------------------

long gemdos(short fn, ...);   // TRAP #1
long bios(short fn, ...);     // TRAP #13
long xbios(short fn, ...);    // TRAP #14

// ---------------------------------------------------------------------
// BIOS (TRAP #13)
// ---------------------------------------------------------------------

// Function numbers
enum Bios {
    GETMPB   = 0,
    BCONSTAT = 1,
    BCONIN   = 2,
    BCONOUT  = 3,
    RWABS    = 4,
    SETEXC   = 5,
    TICKCAL  = 6,
    GETBPB   = 7,
    BCOSTAT  = 8,
    MEDIACH  = 9,
    DRVMAP   = 10,
    KBSHIFT  = 11,
};

// Basic types
typedef unsigned char  UBYTE;
typedef unsigned short UWORD;
typedef unsigned long  ULONG;
typedef short          WORD;
typedef long           LONG;

// Example structures (simplified)
struct MPB {
    void *mp_mfl;
    void *mp_mal;
    void *mp_rover;
};

struct IOREC {
    UBYTE *ibuf;
    WORD   ibufsiz;
    WORD   ibufhd;
    WORD   ibuftl;
    WORD   ibuflow;
    WORD   ibufhi;
};

// BIOS wrappers

inline void Getmpb(MPB *p) {
    bios(BIOS_GETMPB, p);
}

inline WORD Bconstat(WORD dev) {
    return (WORD)bios(BIOS_BCONST, dev);
}

inline LONG Bconin(WORD dev) {
    return bios(BIOS_BCONIN, dev);
}

inline void Bconout(WORD dev, WORD c) {
    bios(BIOS_BCONOUT, dev, c);
}

inline WORD Rwabs(WORD rwflag, void *buf, WORD qty, WORD recnr, WORD dev) {
    return (WORD)bios(BIOS_RWABS, rwflag, buf, qty, recnr, dev);
}

inline void *Setexc(WORD vecno, void *newvec) {
    return (void*)bios(BIOS_SETEXC, vecno, newvec);
}

inline LONG Tickcal() {
    return bios(BIOS_TICKCAL);
}

inline WORD Bcostat(WORD dev) {
    return (WORD)bios(BIOS_BCOSTAT, dev);
}

inline WORD Mediach(WORD dev) {
    return (WORD)bios(BIOS_MEDIACH, dev);
}

inline LONG Drvmap() {
    return bios(BIOS_DRVMAP);
}

inline WORD Kbshift(WORD mode) {
    return (WORD)bios(BIOS_KBSHIFT, mode);
}

inline void *Getbpb(WORD dev) {
    return (void*)bios(BIOS_GETBPB, dev);
}

inline WORD Bconmap(WORD dev) {
    return (WORD)bios(BIOS_BCONMAP, dev);
}

inline WORD Kbrate(WORD delay, WORD rate) {
    return (WORD)bios(BIOS_KBRATE, delay, rate);
}

inline void *Prtblk() {
    return (void*)bios(BIOS_PRTBLK);
}

inline IOREC *Iorec(WORD dev) {
    return (IOREC*)bios(BIOS_IOREC, dev);
}

inline LONG Rsconf(WORD baud, WORD ctrl, WORD ucr, WORD rsr, WORD tsr, WORD scr) {
    return bios(BIOS_RSCONF, baud, ctrl, ucr, rsr, tsr, scr);
}

inline void **Keytbl(void *unshift, void *shift, void *caps) {
    return (void**)bios(BIOS_KEYTBL, unshift, shift, caps);
}

inline LONG Random() {
    return bios(BIOS_RANDOM);
}

inline WORD Protobt(void *buf, LONG serial, WORD exec, WORD ctrl) {
    return (WORD)bios(BIOS_PROTObT, buf, serial, exec, ctrl);
}

inline WORD Floprate(WORD dev, WORD rate) {
    return (WORD)bios(BIOS_FLOPRATE, dev, rate);
}

// ---------------------------------------------------------------------
// XBIOS (TRAP #14) – subset of common calls
// ---------------------------------------------------------------------

enum : short {
    XBIOS_INITMOUS   = 0,
    XBIOS_PHYSBASE   = 2,
    XBIOS_LOGBASE    = 3,
    XBIOS_GETREZ     = 4,
    XBIOS_SETSCREEN  = 5,
    XBIOS_SETPALETTE = 6,
    XBIOS_SETCOLOR   = 7,
    XBIOS_FLOPRD     = 8,
    XBIOS_FLOPWR     = 9,
    XBIOS_FLOPFMT    = 10,
    XBIOS_MIDIWS     = 12,
    XBIOS_KBDVBASE   = 13,
    XBIOS_IKBDWS     = 14,
    XBIOS_BLITMODE   = 64 // STE+
};

inline void Initmous(WORD type, void (*vec)(void), void *param) {
    xbios(XBIOS_INITMOUS, type, vec, param);
}

inline void *Physbase() {
    return (void*)xbios(XBIOS_PHYSBASE);
}

inline void *Logbase() {
    return (void*)xbios(XBIOS_LOGBASE);
}

inline WORD Getrez() {
    return (WORD)xbios(XBIOS_GETREZ);
}

inline void Setscreen(void *log, void *phys, WORD rez) {
    xbios(XBIOS_SETSCREEN, log, phys, rez);
}

inline void Setpalette(const UWORD *pal) {
    xbios(XBIOS_SETPALETTE, pal);
}

inline WORD Setcolor(WORD index, WORD color) {
    return (WORD)xbios(XBIOS_SETCOLOR, index, color);
}

// (Floprd/Flopwr/Flopfmt prototypes omitted here for brevity;
// they mirror BIOS Rwabs semantics with extra parameters.)

inline void Midiws(WORD cnt, const UBYTE *buf) {
    xbios(XBIOS_MIDIWS, cnt, buf);
}

inline void **Kbdvbase() {
    return (void**)xbios(XBIOS_KBDVBASE);
}

inline void Ikbdws(WORD cnt, const UBYTE *buf) {
    xbios(XBIOS_IKBDWS, cnt, buf);
}

inline WORD Blitmode(WORD mode) {
    return (WORD)xbios(XBIOS_BLITMODE, mode);
}

// ---------------------------------------------------------------------
// GEMDOS (TRAP #1) – core calls
// ---------------------------------------------------------------------

enum : short {
    GEMDOS_PTERM0   = 0,
    GEMDOS_CCONIN   = 1,
    GEMDOS_CCONOUT  = 2,
    GEMDOS_CAUXIN   = 3,
    GEMDOS_CAUXOUT  = 4,
    GEMDOS_CPRNOUT  = 5,
    GEMDOS_CRAWIO   = 6,
    GEMDOS_CRAWCIN  = 7,
    GEMDOS_CNECIN   = 8,
    GEMDOS_CCONWS   = 9,
    GEMDOS_CCONRS   = 10,
    GEMDOS_CCONIS   = 11,
    GEMDOS_DSETDRV  = 14,
    GEMDOS_DGETDRV  = 25,
    GEMDOS_FSETDTA  = 26,
    GEMDOS_SVERSION = 48,
    GEMDOS_PTERMRES = 49,
    GEMDOS_DFREE    = 54,
    GEMDOS_DCREATE  = 57,
    GEMDOS_DDELETE  = 58,
    GEMDOS_DSETPATH = 59,
    GEMDOS_FCREATE  = 60,
    GEMDOS_FOPEN    = 61,
    GEMDOS_FCLOSE   = 62,
    GEMDOS_FREAD    = 63,
    GEMDOS_FWRITE   = 64,
    GEMDOS_FDELETE  = 65,
    GEMDOS_FSEEK    = 66,
    GEMDOS_FATTRIB  = 67,
    GEMDOS_DGETPATH = 71,
    GEMDOS_MALLOC   = 72,
    GEMDOS_MFREE    = 73,
    GEMDOS_MSHRINK  = 74,
    GEMDOS_FSFIRST  = 78,
    GEMDOS_FSNEXT   = 79,
    GEMDOS_FRENAME  = 86,
    GEMDOS_FDATIME  = 87
};

struct DTA {
    UBYTE d_reserved[21];
    UBYTE d_attrib;
    UWORD d_time;
    UWORD d_date;
    LONG  d_length;
    char  d_fname[14];
};

// GEMDOS wrappers

inline void Pterm0() {
    gemdos(GEMDOS_PTERM0);
}

inline LONG Cconin() {
    return gemdos(GEMDOS_CCONIN);
}

inline void Cconout(WORD c) {
    gemdos(GEMDOS_CCONOUT, c);
}

inline void Cconws(const char *s) {
    gemdos(GEMDOS_CCONWS, s);
}

inline WORD Cconis() {
    return (WORD)gemdos(GEMDOS_CCONIS);
}

inline WORD Dsetdrv(WORD drv) {
    return (WORD)gemdos(GEMDOS_DSETDRV, drv);
}

inline WORD Dgetdrv() {
    return (WORD)gemdos(GEMDOS_DGETDRV);
}

inline void Fsetdta(DTA *dta) {
    gemdos(GEMDOS_FSETDTA, dta);
}

inline WORD Sversion() {
    return (WORD)gemdos(GEMDOS_SVERSION);
}

inline void Ptermres(LONG keep, WORD rc) {
    gemdos(GEMDOS_PTERMRES, keep, rc);
}

inline WORD Dcreate(const char *path) {
    return (WORD)gemdos(GEMDOS_DCREATE, path);
}

inline WORD Ddelete(const char *path) {
    return (WORD)gemdos(GEMDOS_DDELETE, path);
}

inline WORD Dsetpath(const char *path) {
    return (WORD)gemdos(GEMDOS_DSETPATH, path);
}

inline WORD Fcreate(const char *name, WORD attr) {
    return (WORD)gemdos(GEMDOS_FCREATE, name, attr);
}

inline WORD Fopen(const char *name, WORD mode) {
    return (WORD)gemdos(GEMDOS_FOPEN, name, mode);
}

inline WORD Fclose(WORD handle) {
    return (WORD)gemdos(GEMDOS_FCLOSE, handle);
}

inline LONG Fread(WORD handle, LONG count, void *buf) {
    return gemdos(GEMDOS_FREAD, handle, count, buf);
}

inline LONG Fwrite(WORD handle, LONG count, const void *buf) {
    return gemdos(GEMDOS_FWRITE, handle, count, buf);
}

inline WORD Fdelete(const char *name) {
    return (WORD)gemdos(GEMDOS_FDELETE, name);
}

inline LONG Fseek(LONG offset, WORD handle, WORD mode) {
    return gemdos(GEMDOS_FSEEK, offset, handle, mode);
}

inline WORD Fattrib(const char *name, WORD wflag, WORD attrib) {
    return (WORD)gemdos(GEMDOS_FATTRIB, name, wflag, attrib);
}

inline WORD Dgetpath(char *buf, WORD drv) {
    return (WORD)gemdos(GEMDOS_DGETPATH, buf, drv);
}

inline void *Malloc(LONG size) {
    return (void*)gemdos(GEMDOS_MALLOC, size);
}

inline WORD Mfree(void *ptr) {
    return (WORD)gemdos(GEMDOS_MFREE, ptr);
}

inline WORD Mshrink(void *base, LONG size) {
    return (WORD)gemdos(GEMDOS_MSHRINK, 0, base, size);
}

inline WORD Fsfirst(const char *pattern, WORD attr) {
    return (WORD)gemdos(GEMDOS_FSFIRST, pattern, attr);
}

inline WORD Fsnext() {
    return (WORD)gemdos(GEMDOS_FSNEXT);
}

inline WORD Frename(WORD zero, const char *old, const char *neu) {
    return (WORD)gemdos(GEMDOS_FRENAME, zero, old, neu);
}

inline WORD Fdatime(UWORD *time, UWORD *date, WORD handle, WORD rwflag) {
    return (WORD)gemdos(GEMDOS_FDATIME, time, date, handle, rwflag);
}

} // extern "C"

#endif // ATARI_TOS_API_HPP



*/
