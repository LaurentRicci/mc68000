#pragma once
#include <unordered_map>
#include "ibios.h"
#include "biosparameterblock.h"

namespace mc68000
{

    class OSBios : public IBios, public TrapHandler
    {
    public:
        void setup() override;
        void registerTrapHandlers(Cpu* cpu) override;
        void handle(Cpu& cpu, uint16_t vector) override
        {
            if (vector == 2)
            {
                trap(cpu);
            }
            else
            {
                throw "OSBios: unhandled trap vector";
            }
        }
    private:
        void trap(Cpu& cpu);
        uint32_t diskRead(void* buffer, uint16_t deviceNumber, uint16_t sectorNumber, uint16_t sectorCount);

        std::string getDiskFileName(uint16_t deviceNumber);
        BiosParameterBlock getBiosParameterBlock(uint16_t deviceNumber);

    private:
        std::unordered_map<std::string, std::string> settings;
    };

};
