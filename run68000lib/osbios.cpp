#include "osbios.h"
#include "trapargs.h"
using namespace mc68000;

void OSBios::setup()
{
    getConfig("osbios.conf", settings);

}

void OSBios::registerTrapHandlers(Cpu* cpu)
{
    cpu->registerTrapHandler(2, this);
}

void OSBios::trap(Cpu& cpu)
{
    // placeholder: just return a fixed value for testing
    cpu.setDRegister(0, 0x12ab34cd);
    cpu.setDRegister(1, 2);

    bool isSupervisor = callerIsSupervisor(cpu);
    uint16_t func = argWord(cpu, isSupervisor, 0);

    switch (func)
    {
        case 0:
        {
            // For testing, return the given argument
            uint16_t arg1 = argWord(cpu, isSupervisor, 1);
            uint16_t result = argWord(cpu, isSupervisor, arg1);
            cpu.setDRegister(0, static_cast<uint32_t>(result));
            break;
        }
        case 1:
        {
            uint32_t address = argLong(cpu, isSupervisor, 1);
            void* buffer = cpu.mem.get<void*>(address);
            uint16_t deviceNumber = argWord(cpu, isSupervisor, 3);
            uint16_t sectorNumber = argWord(cpu, isSupervisor, 4);
            uint16_t sectorCount = argWord(cpu, isSupervisor, 5);
            uint32_t ret = diskRead(buffer, deviceNumber, sectorNumber, sectorCount);
            cpu.setDRegister(0, ret);
            break;
        }
    }
}

uint32_t OSBios::diskRead(void* buffer, uint16_t deviceNumber, uint16_t sectorNumber, uint16_t sectorCount)
{
    try
    {
        std::string deviceFile = getDiskFileName(deviceNumber);

        BiosParameterBlock bpb = getBiosParameterBlock(deviceNumber);
        uint32_t bytesPerSector = bpb.bytesPerSector;
        uint32_t offset = sectorNumber * bytesPerSector;
        uint32_t bytesToRead = bytesPerSector * sectorCount;

        std::ifstream f(deviceFile, std::ios::binary);
        if (!f)
        {
            return 0;
        }

        f.seekg(offset);
        f.read(reinterpret_cast<char*>(buffer), bytesToRead);
        return bytesToRead;
    }
    catch (const std::exception& ex)
    {
        std::cerr << "diskRead error: " << ex.what() << std::endl;
        return 0;
    }
    catch (const std::string& ex)
    {
        std::cerr << "diskRead error: " << ex << std::endl;
        return 0;
    }
}

std::string OSBios::getDiskFileName(uint16_t deviceNumber)
{
    std::string name = "disk" + std::to_string(deviceNumber);
    auto it = settings.find(name);
    if (it == settings.end())
    {
        std::cerr << "getDiskFileName error: " << name << " not found in configuration" << std::endl;
        throw "disk file not configured for device " + std::to_string(deviceNumber);
    }
    return it->second;
}

BiosParameterBlock OSBios::getBiosParameterBlock(uint16_t deviceNumber)
{
    std::string deviceFile = getDiskFileName(deviceNumber);
    std::ifstream f(deviceFile, std::ios::binary);
    if (!f)
    {
        std::cerr << "getBiosParameterBlock error: failed to open disk file " << deviceFile << std::endl;
        throw std::string("failed to open disk file");
    }

    BiosParameterBlock bpb;
    f.read(reinterpret_cast<char*>(&bpb), sizeof(BiosParameterBlock));
    if (!f)
    {
        std::cerr << "getBiosParameterBlock error: failed to read BPB from disk file " << deviceFile << std::endl;
        throw std::string("failed to read bpb");
    }

    return bpb;
}
