#ifdef _WIN32
#include <windows.h>
#include <io.h>
#include <string>
#include <vector>
#include <iostream>
#endif

#include "cpu.h"
#include "disasm.h"

namespace mc68000
{

    void Cpu::debug(uint32_t startPc, uint32_t startSP, uint32_t startSSP, const char* symbolsFile)
    {
#ifdef _WIN32
        // 1. Create a pipe for the child process's STDIN
        HANDLE hChildStdinRd = NULL, hChildStdinWr = NULL;
        SECURITY_ATTRIBUTES saAttr{};
        saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
        saAttr.bInheritHandle = TRUE;
        saAttr.lpSecurityDescriptor = NULL;

        if (!CreatePipe(&hChildStdinRd, &hChildStdinWr, &saAttr, 0))
        {
            std::cerr << "CreatePipe failed\n";
            return;
        }
        if (!SetHandleInformation(hChildStdinWr, HANDLE_FLAG_INHERIT, 0))
        {
            std::cerr << "SetHandleInformation failed\n";
            CloseHandle(hChildStdinRd);
            CloseHandle(hChildStdinWr);
            return;
        }
        STARTUPINFOA si{};
        PROCESS_INFORMATION pi{};
        si.cb = sizeof(STARTUPINFOA);
        si.dwFlags = STARTF_USESTDHANDLES;
        si.hStdInput = hChildStdinRd;
        si.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
        si.hStdError = GetStdHandle(STD_ERROR_HANDLE);

        // Path to your child process executable
        char buf[MAX_PATH] = { 0 };
        uint32_t len = GetModuleFileNameA(NULL, buf, MAX_PATH);
        char* p = strrchr(buf, '\\');
        *(p+1) = 0;
        strcat(buf, "..\\..\\dasmconsole\\debug\\dasmconsole.exe");

        BOOL success = CreateProcessA(
            buf, NULL, NULL, NULL, TRUE,
            CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);

        if (!success)
        {
            std::cerr << "CreateProcess failed: " << GetLastError() << std::endl;
            CloseHandle(hChildStdinRd);
            CloseHandle(hChildStdinWr);
            return;
        }

        // Parent doesn't need the read end
        CloseHandle(hChildStdinRd);

        // 3. Write disassembly output to the child's stdin
        FILE* pipeStream = _fdopen(_open_osfhandle((intptr_t)hChildStdinWr, 0), "w");
        if (!pipeStream)
        {
            std::cerr << "fdopen failed\n";
            CloseHandle(hChildStdinWr);
            return;
        }
#endif

        done = false;
        pc = startPc;
        aRegisters[7] = startSP;
        usp = startSP;
        ssp = startSSP;

        uint16_t* memory = static_cast<uint16_t*>(localMemory.get<void*>(startPc));
        auto memoryInfo = localMemory.getMemoryRange();
        DisAsm disAsm(memory, memoryInfo.first);
        if (symbolsFile)
        {
            disAsm.loadSymbols(symbolsFile);
        }

        while (!done)
        {
            uint16_t x = localMemory.getWord(pc);
            auto s = disAsm.disassembleInstruction(pc);
#ifdef _WIN32
            std::string address = disAsm.findSymbol(pc);
            if (!address.empty())
            {
                fprintf(pipeStream, "%8s - %04x - %s\n", address.c_str(), x, s.c_str());
            }
            else
            {
                fprintf(pipeStream, "%08x - %04x - %s\n", pc, x, s.c_str());
            }
            fflush(pipeStream);
#else
            std::cout << std::hex << pc << " - " << std::hex << x << " - " << s << std::endl;
#endif
            pc += 2;
            (this->*handlers[x])(x);
        }
    }
}
