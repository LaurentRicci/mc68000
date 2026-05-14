#pragma once
#include <algorithm>
#include <unordered_map>
#include <string>

#include "cpu.h"
namespace mc68000
{
    /// <summary>
    /// Retrieves a 16-bit argument from the CPU stack, handling supervisor vs. user stack layout.
    /// </summary>
    /// <param name="cpu">Pointer to the CPU instance.</param>
    /// <param name="callerIsSupervisor">True if the call was made in supervisor mode. </param>
    /// <param name="index">Zero-based index of the 16-bit argument to retrieve (each argument is 2 bytes).</param>
    /// <returns>The uint16_t value of the requested argument read from the calculated stack offset.</returns>
    static inline uint16_t argWord(Cpu& cpu, bool callerIsSupervisor, unsigned index)
    {
        if (callerIsSupervisor)
        {
            // in Supervisor mode, the argument list starts at ssp+6 (PC and SR are saved on the supervisor stack)
            return cpu.getFromStack<uint16_t>(true, 6 + index * 2);
        }
        else
        {
            // in User mode, the argument list starts at usp 
            return cpu.getFromStack<uint16_t>(false, index * 2);
        }
    }

    /// <summary>
    /// Fetches a 32-bit (long) argument from the CPU stack, handling supervisor vs. user stack layout.
    /// </summary>
    /// <param name="cpu">Pointer to the Cpu instance.</param>
    /// <param name="callerIsSupervisor">True if the call was made in supervisor mode. </param>
    /// <param name="wordIndex">Zero-based index of the 32-bit argument to fetch.</param>
    /// <returns>The 32-bit (uint32_t) value of the requested argument read from the appropriate stack.</returns>
    /// <remarks>The stack can have a mix of word and long arguments so the indexing is done in words.</remarks>
    static inline uint32_t argLong(Cpu& cpu, bool callerIsSupervisor, unsigned wordIndex)
    {
        if (callerIsSupervisor)
        {
            // in Supervisor mode, the argument list starts at ssp+6 (PC and SR are saved on the supervisor stack)
            return cpu.getFromStack<uint32_t>(true, 6 + wordIndex * 2);
        }
        else
        {
            // in User mode, the argument list starts at usp 
            return cpu.getFromStack<uint32_t>(false, wordIndex * 2);
        }
    }

    /// <summary>
    /// Determines whether the caller was running in supervisor mode.
    /// </summary>
    /// <param name="cpu">Pointer to the Cpu instance.</param>
    /// <returns>True if the caller was running in supervisor mode; otherwise false.</returns>
    static inline bool callerIsSupervisor(Cpu& cpu)
    {
        uint16_t savedSR = cpu.getFromStack<uint16_t>(true, 0); // Read saved SR from top of supervisor stack
        return (savedSR & 0x2000) != 0; // Check S bit (bit 13)
    }

    static inline void trim(std::string& s)
    {
        s.erase(
            s.begin(),
            std::find_if_not(
                s.begin(),
                s.end(),
                [](unsigned char c) { return std::isspace(c); }
            )
        );
        s.erase(
            std::find_if_not(
                s.rbegin(),
                s.rend(),
                [](unsigned char c) { return std::isspace(c); }
            ).base(),
            s.end()
        );
    }

    static inline void getConfig(const char* configFile, std::unordered_map<std::string, std::string>& configs)
    {
        std::ifstream sourceFile(configFile);
        if (sourceFile)
        {
            std::string line;
            while (std::getline(sourceFile, line))
            {
                // strip comments starting with # or ;
                auto posc = line.find_first_of("#;");
                if (posc != std::string::npos)
                {
                    line.resize(posc);
                }
                trim(line);
                //if (line.empty()) continue;
                auto pos = line.find('=');
                if (pos != std::string::npos)
                {
                    std::string key = line.substr(0, pos);
                    std::string val = line.substr(pos + 1);
                    trim(key);
                    trim(val);
                    if (!key.empty())
                    {
                        configs.emplace(std::move(key), std::move(val));
                    }
                }
            }
        }
    }

}
