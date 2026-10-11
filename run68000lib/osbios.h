#pragma once
#include <unordered_map>
#include "ibios.h"
#include "biosparameterblock.h"
extern "C" uint16_t find_file_in_root(const char* filename);
extern "C" void read_sector(uint32_t lba, uint8_t* buffer);

namespace mc68000
{

    class OSBios : public IBios, public TrapHandler
    {
    public:
        void setup() override;
        void registerTrapHandlers(Cpu* cpu) override;
        bool handle(Cpu& cpu, uint16_t vector) override
        {
            if (vector == 2)
            {
                return trap(cpu);
            }
            else
            {
                throw "OSBios: unhandled trap vector";
            }
        }
    private:
        bool trap(Cpu& cpu);
        uint32_t diskRead(void* buffer, uint16_t deviceNumber, uint16_t sectorNumber, uint16_t sectorCount);
        uint32_t diskWrite(void* buffer, uint16_t deviceNumber, uint16_t sectorNumber, uint16_t sectorCount);

        std::string getDiskFileName(uint16_t deviceNumber);
        BiosParameterBlock getBiosParameterBlock(uint16_t deviceNumber);

    private:
        static std::unordered_map<std::string, std::string> settings;

        friend void ::read_sector(uint32_t lba, uint8_t* buffer);
    };
};
