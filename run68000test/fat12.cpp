#include <osbios.h>

using namespace mc68000;

void create_fat12_disk(const std::string& filename)
{
    BiosParameterBlock bpb(false);
    std::fill(reinterpret_cast<char*>(&bpb), reinterpret_cast<char*>(&bpb) + sizeof(bpb), 0x00);
    bpb.jump[0] = 0xEB; // JMP instruction
    bpb.jump[1] = 0x3C; // NOP padding
    bpb.jump[2] = 0x90; // NOP padding
    std::copy_n("MINIMAL", 8, bpb.oem);
    bpb.bytesPerSector = 512;
    bpb.sectorsPerCluster = 1;
    bpb.reservedSectors = 1;
    bpb.numFATs = 1;            
    bpb.rootEntryCount = 16;
    bpb.totalSectors16 = 4;     
    bpb.mediaDescriptor = 0xF8;
    bpb.sectorsPerFAT = 1;

    std::ofstream disk(filename, std::ios::binary);

    // SECTOR 0: Boot sector with BPB
    disk.write(reinterpret_cast<char*>(&bpb), sizeof(BiosParameterBlock));
    char buffer0[512 - sizeof(BiosParameterBlock)];
    std::fill(std::begin(buffer0), std::end(buffer0), 0);
    disk.write(buffer0, sizeof(buffer0));

    // SECTOR 1: FAT table (all entries free)
    char buffer1[512];
    std::fill(std::begin(buffer1), std::end(buffer1), 0);
    buffer1[0] = 0xF8; // Media descriptor
    buffer1[1] = 0xFF; // (cluster 1 = reserved)
    buffer1[2] = 0xFF; //   
    buffer1[1] = 0xFF; // (cluster 2 = end of file)
    buffer1[2] = 0xFF; //   
    disk.write(buffer1, sizeof(buffer1));

    // SECTOR 2: Root directory (one entry for a file named "TEST.TXT")
    char buffer2[512];
    std::fill(std::begin(buffer2), std::end(buffer2), 0);
    std::copy_n("TEST    ", 8, buffer2); // Filename (padded to 8 bytes)
    std::copy_n("TXT", 3, buffer2 + 8);  // Extension (padded to 3 bytes)
    buffer2[11] = 0x20;                  // Attribute: archive
    buffer2[26] = 2;                     // Starting cluster (cluster 2)
    buffer2[28] = 5;                     // File size = 5 bytes
    disk.write(buffer2, sizeof(buffer2));

    // SECTOR 3: File data (5 bytes of content)
    char buffer3[512];
    std::fill(std::begin(buffer3), std::end(buffer3), 0);
    std::copy_n("Hello", 5, buffer3); // File content
    disk.write(buffer3, sizeof(buffer3));

    disk.close();
}
