#pragma once
#include <cstdint>
#include <algorithm>

namespace mc68000
{
    #pragma pack(push, 1) // Ensure no padding is added by the compiler
    struct BiosParameterBlock
    {
        uint8_t  jump[3];              // 0x00: Jump instruction
        char     oem[8];               // 0x03: OEM name
        uint16_t bytesPerSector;       // 0x0B: Usually 512
        uint8_t  sectorsPerCluster;    // 0x0D
        uint16_t reservedSectors;      // 0x0E
        uint8_t  numFATs;              // 0x10
        uint16_t rootEntryCount;       // 0x11
        uint16_t totalSectors16;       // 0x13
        uint8_t  mediaDescriptor;      // 0x15
        uint16_t sectorsPerFAT;        // 0x16
        uint16_t sectorsPerTrack;      // 0x18 (logical, may lie)
        uint16_t numHeads;             // 0x1A (logical, may lie)
        uint32_t hiddenSectors;        // 0x1C (usually 0 on floppies)
        uint32_t totalSectors32;       // 0x20 (0 on floppies)
        // FAT12/FAT16 extended BPB (not always present on Atari ST)
        uint8_t  driveNumber;          // 0x24
        uint8_t  reserved1;            // 0x25
        uint8_t  bootSignature;        // 0x26 (0x29 if next fields valid)
        uint32_t volumeID;             // 0x27
        char     volumeLabel[11];      // 0x2B
        char     fsType[8];            // 0x36 ("FAT12   " or "FAT16   ")

        BiosParameterBlock();
        BiosParameterBlock(bool isHighDensity);
        BiosParameterBlock(const BiosParameterBlock& other) = default;

        uint16_t swap(uint16_t value) const
        {
            return (value >> 8) | (value << 8);
        }

        uint32_t swap(uint32_t value) const
        {
            return ((value >> 24) & 0x000000FF) |
                   ((value >> 8) & 0x0000FF00) |
                   ((value << 8) & 0x00FF0000) |
                   ((value << 24) & 0xFF000000);
        }

        void convert()
        {
            bytesPerSector = swap(bytesPerSector);
            reservedSectors = swap(reservedSectors);
            rootEntryCount = swap(rootEntryCount);
            totalSectors16 = swap(totalSectors16);
            sectorsPerFAT = swap(sectorsPerFAT);
            sectorsPerTrack = swap(sectorsPerTrack);
            numHeads = swap(numHeads);
            hiddenSectors = swap(hiddenSectors);
            totalSectors32 = swap(totalSectors32);
            volumeID = swap(volumeID);
        }
    };
    #pragma pack(pop)
}
