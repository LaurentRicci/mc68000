#include "biosparameterblock.h"

namespace mc68000
{
    #pragma warning(disable:26495) // Disable uninitialized member warning for BiosParameterBlock
    BiosParameterBlock::BiosParameterBlock()
    {
        uint8_t* start = reinterpret_cast<uint8_t*>(this);
        uint8_t* end = reinterpret_cast<uint8_t*>(this + 1);
        std::fill(start, end, 0x00);
    }
    #pragma warning(default:26495)

    BiosParameterBlock::BiosParameterBlock(bool isHighDensity)
    {
        if (isHighDensity)
        {
             // Initialize with high-density floppy parameters
            std::fill(jump, jump + sizeof(jump), 0x00);
            std::copy_n("MSDOS5.0", 8, oem);
            bytesPerSector = 512;   // Universal for PC/ST floppie
            sectorsPerCluster = 1;  // FAT12 default for small media
            reservedSectors = 1;    // Boot sector only
            numFATs = 2;            // Standard DOS layout
            rootEntryCount = 224;   // 224 entries × 32 bytes = 7168 bytes = 14 sectors
            totalSectors16 = 2880;  // 1440 KB / 512 bytes
            mediaDescriptor = 0xF0; // Standard for 1.44MB FAT12
            sectorsPerFAT = 9;      // FAT12 size for 1.44MB
            sectorsPerTrack = 18;   // Standard for 1.44MB FAT12
            numHeads = 2;           // Double‑sided
            hiddenSectors = 0;      // Floppies are unpartitioned
            totalSectors32 = 0;     // Only used if >65535 sectors
            driveNumber = 0x00;     // Default drive number
            reserved1 = 0x00;
            bootSignature = 0x29;   // Indicates extended BPB is present
            volumeID = 12345678;    // Example volume ID
            std::copy_n("NO NAME    ", 11, volumeLabel);
            std::copy_n("FAT12   ", 8, fsType);
        }
        else
        {
            // Initialize with standard-density floppy parameters
            std::fill(jump, jump + sizeof(jump), 0x00);
            std::copy_n("MSDOS5.0", 8, oem);
            bytesPerSector = 512;   // Universal for PC/ST floppie
            sectorsPerCluster = 1;  // FAT12 default for small media
            reservedSectors = 1;    // Boot sector only
            numFATs = 2;            // Standard DOS layout
            rootEntryCount = 112;   // 112 entries × 32 bytes = 3584 bytes = 7 sectors
            totalSectors16 = 1440;  // 720 KB / 512 bytes
            mediaDescriptor = 0xF9; // Standard for 720 KB FAT12
            sectorsPerFAT = 3;      // FAT12 size for 720 KB
            sectorsPerTrack = 9;    // Standard for 720 KB FAT12
            numHeads = 2;           // Double‑sided
            hiddenSectors = 0;      // Floppies are unpartitioned
            totalSectors32 = 0;     // Only used if >65535 sectors
            driveNumber = 0x00;     // Default drive number
            reserved1 = 0x00;
            bootSignature = 0x29;   // Indicates extended BPB is present
            volumeID = 12345678;    // Example volume ID
            std::copy_n("NO NAME    ", 11, volumeLabel);
            std::copy_n("FAT12   ", 8, fsType);
        }
    }

}
