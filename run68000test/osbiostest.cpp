#include <boost/test/unit_test.hpp>
#include "fileCleanup.h"
#include "osbios.h"
void create_fat12_disk(const std::string& filename);

using namespace mc68000;
static const char* configFileName = "osbios.conf";
static const char* diskFileName = "drivea.dsk";

BOOST_AUTO_TEST_SUITE(osbios)

BOOST_AUTO_TEST_CASE(osbios)
{
    unsigned char code[] = {
    0x3f,0x3c, 0x00,0x01,               //      move.w #1, -(sp)
    0x4e,0x42,                          //      trap   #2
    0x5c,0x8f,                          //      addq.l #2, sp
    0xff,0xff };

    // Arrange
    FileCleanup configFileCleanup(configFileName);
    std::ofstream configFile(configFileName);
    configFile << "drivea = drivea.dsk" << std::endl;

    Memory memory(256, 0, code, sizeof(code));
    Cpu cpu(memory);
    OSBios bios;
    bios.registerTrapHandlers(&cpu);

    // Act
    cpu.reset();
    cpu.start(0, 256, 128);

    // Assert
    BOOST_CHECK_EQUAL(2, cpu.d1);
    BOOST_CHECK_EQUAL(0x12ab34cd, cpu.d0);

}

// ======================================================================
// READ related tests
// ======================================================================
BOOST_AUTO_TEST_CASE(sector_read)
{
    unsigned char code[] = {
    0x3f,0x3c, 0x00,0x01,               // move.w #1, -(sp) sectorCount
    0x3f,0x3c, 0x00,0x00,               // move.w #0, -(sp) sectorNumber
    0x3f,0x3c, 0x00,0x01,               // move.w #1, -(sp) device
    0x2f,0x3c, 0x00,0x00, 0x01, 0x00,   // move.l #256, -(sp)  buffer
    0x3f,0x3c, 0x00,0x01,               // move.w #1, -(sp) diskread
    0x4e,0x42,                          // trap   #2
    0xdf,0xfc, 0x00, 0x00, 0x00, 0x0c,  // add.l  #12, sp
    0xff,0xff };

    // Arrange the configuration file
    FileCleanup configFileCleanup(configFileName);
    std::ofstream configFile(configFileName);
    configFile << "disk1 = drivea.dsk" << std::endl;

    // Arrange the disk file
    FileCleanup diskFileCleanup(diskFileName);
    std::ofstream disk(diskFileName, std::ios::binary);
    BiosParameterBlock bpb(false);
    bpb.jump[0] = 42;
    bpb.jump[1] = 24;
    bpb.jump[2] = 66;
    disk.write(reinterpret_cast<char*>(&bpb), sizeof(BiosParameterBlock));

    char buffer[512 - sizeof(BiosParameterBlock)];
    disk.write(buffer, sizeof(buffer));
    disk.close();

    // Arrange the memory and CPU
    Memory memory(1024, 0, code, sizeof(code));
    Cpu cpu(memory);
    OSBios bios;
    bios.setup();
    bios.registerTrapHandlers(&cpu);

    // Act
    cpu.reset();
    cpu.start(0, 256, 128);

    // Assert
    BOOST_CHECK_EQUAL(512, cpu.d0);
    BiosParameterBlock* pbpb = static_cast<BiosParameterBlock*>(cpu.mem.get<void*>(256));
    BOOST_CHECK_EQUAL(pbpb->jump[0], 42);
    BOOST_CHECK_EQUAL(pbpb->jump[1], 24);
    BOOST_CHECK_EQUAL(pbpb->jump[2], 66);
    BOOST_CHECK_EQUAL(pbpb->jump[2], 66);
    BOOST_CHECK_EQUAL(pbpb->bytesPerSector, 512);
    BOOST_CHECK_EQUAL(pbpb->volumeID, 12345678);
}

BOOST_AUTO_TEST_CASE(sector_read_2_out_of_3)
{
    unsigned char code[] = {
    0x3f,0x3c, 0x00,0x02,               // move.w #2, -(sp) sectorCount
    0x3f,0x3c, 0x00,0x01,               // move.w #1, -(sp) sectorNumber
    0x3f,0x3c, 0x00,0x01,               // move.w #1, -(sp) device
    0x2f,0x3c, 0x00,0x00, 0x01, 0x00,   // move.l #256, -(sp)  buffer
    0x3f,0x3c, 0x00,0x01,               // move.w #1, -(sp) diskread
    0x4e,0x42,                          // trap   #2
    0xdf,0xfc, 0x00, 0x00, 0x00, 0x0c,  // add.l  #12, sp
    0xff,0xff };

    // Arrange the configuration file
    FileCleanup configFileCleanup(configFileName);
    std::ofstream configFile(configFileName);
    configFile << "disk1 = drivea.dsk" << std::endl;

    // Arrange the disk file
    FileCleanup diskFileCleanup(diskFileName);
    std::ofstream disk(diskFileName, std::ios::binary);
    BiosParameterBlock bpb(false);
    bpb.jump[0] = 42;
    bpb.jump[1] = 24;
    bpb.jump[2] = 66;
    disk.write(reinterpret_cast<char*>(&bpb), sizeof(BiosParameterBlock));

    char buffer[512 - sizeof(BiosParameterBlock)];
    disk.write(buffer, sizeof(buffer));
    char sector[512];
    std::fill(std::begin(sector), std::end(sector), 0x22);
    disk.write(sector, sizeof(sector));
    std::fill(std::begin(sector), std::end(sector), 0x33);
    disk.write(sector, sizeof(sector));
    disk.close();

    // Arrange the memory and CPU
    Memory memory(256 + 2 * 512, 0, code, sizeof(code));
    Cpu cpu(memory);
    OSBios bios;
    bios.setup();
    bios.registerTrapHandlers(&cpu);

    // Act
    cpu.reset();
    cpu.start(0, 256, 128);

    // Assert
    BOOST_CHECK_EQUAL(2*512, cpu.d0);
    uint8_t* pcontent = static_cast<uint8_t*>(cpu.mem.get<void*>(256));
    BOOST_CHECK_EQUAL(pcontent[0], 0x22);
    BOOST_CHECK_EQUAL(pcontent[511], 0x22);
    BOOST_CHECK_EQUAL(pcontent[512], 0x33);
    BOOST_CHECK_EQUAL(pcontent[1023], 0x33);
}

BOOST_AUTO_TEST_CASE(sector_read_no_setup)
{
    unsigned char code[] = {
    0x3f,0x3c, 0x00,0x01,               // move.w #1, -(sp) sectorCount
    0x3f,0x3c, 0x00,0x00,               // move.w #0, -(sp) sectorNumber
    0x3f,0x3c, 0x00,0x01,               // move.w #1, -(sp) device
    0x2f,0x3c, 0x00,0x00, 0x01, 0x00,   // move.l #256, -(sp)  buffer
    0x3f,0x3c, 0x00,0x01,               // move.w #1, -(sp) diskread
    0x4e,0x42,                          // trap   #2
    0xdf,0xfc, 0x00, 0x00, 0x00, 0x0c,  // add.l  #12, sp
    0xff,0xff };

    // Arrange the configuration file
    FileCleanup configFileCleanup(configFileName);
    std::ofstream configFile(configFileName);
    configFile << "dummy = nothing" << std::endl;
    configFile.close();

    // Arrange the memory and CPU
    Memory memory(1024, 0, code, sizeof(code));
    Cpu cpu(memory);
    OSBios bios;
    bios.setup();
    bios.registerTrapHandlers(&cpu);

    // Act
    cpu.reset();
    cpu.start(0, 256, 128);

    // Assert
    BOOST_CHECK_EQUAL(0, cpu.d0);
}

BOOST_AUTO_TEST_CASE(sector_read_no_disk)
{
    unsigned char code[] = {
    0x3f,0x3c, 0x00,0x01,               // move.w #1, -(sp) sectorCount
    0x3f,0x3c, 0x00,0x00,               // move.w #0, -(sp) sectorNumber
    0x3f,0x3c, 0x00,0x01,               // move.w #1, -(sp) device
    0x2f,0x3c, 0x00,0x00, 0x01, 0x00,   // move.l #256, -(sp)  buffer
    0x3f,0x3c, 0x00,0x01,               // move.w #1, -(sp) diskread
    0x4e,0x42,                          // trap   #2
    0xdf,0xfc, 0x00, 0x00, 0x00, 0x0c,  // add.l  #12, sp
    0xff,0xff };

    // Arrange the configuration file
    FileCleanup configFileCleanup(configFileName);
    std::ofstream configFile(configFileName);
    configFile << "disk1 = nodisk.dsk" << std::endl;
    configFile.close();

    // Arrange the memory and CPU
    Memory memory(1024, 0, code, sizeof(code));
    Cpu cpu(memory);
    OSBios bios;
    bios.setup();
    bios.registerTrapHandlers(&cpu);

    // Act
    cpu.reset();
    cpu.start(0, 256, 128);

    // Assert
    BOOST_CHECK_EQUAL(0, cpu.d0);
}

// ======================================================================
// WRITE related tests
// ======================================================================
void prepare_disk(size_t size)
{
    // Arrange the configuration file
    std::ofstream configFile(configFileName);
    configFile << "disk1 = drivea.dsk" << std::endl;

    // Arrange the disk file
    std::ofstream disk(diskFileName, std::ios::binary);
    BiosParameterBlock bpb(false);
    disk.write(reinterpret_cast<char*>(&bpb), sizeof(BiosParameterBlock));

    for (size_t i = sizeof(BiosParameterBlock); i < 512; i++)
    {
        disk.put(0x00);
    }
    for (size_t i = 512; i < size; i++)
    {
        disk.put(0x55);
    }
    disk.close();
}

BOOST_AUTO_TEST_CASE(sector_write)
{
    unsigned char code[] = {
    0x3f,0x3c, 0x00,0x01,               // move.w #1, -(sp) sectorCount
    0x3f,0x3c, 0x00,0x01,               // move.w #1, -(sp) sectorNumber
    0x3f,0x3c, 0x00,0x01,               // move.w #1, -(sp) device
    0x2f,0x3c, 0x00,0x00, 0x01, 0x00,   // move.l #256, -(sp)  buffer
    0x3f,0x3c, 0x00,0x02,               // move.w #2, -(sp) diskwrite
    0x4e,0x42,                          // trap   #2
    0xdf,0xfc, 0x00, 0x00, 0x00, 0x0c,  // add.l  #12, sp
    0xff,0xff };

    // Arrange the disk
    FileCleanup configFileCleanup(configFileName);
    FileCleanup diskFileCleanup(diskFileName);
    prepare_disk(1024); // 2 sectors

    // Arrange the memory and CPU
    Memory memory(1024, 0, code, sizeof(code));
    uint8_t data1 = 0x5B;
    uint8_t data2 = 0x6C;
    int start = 256;
    for (int i = start; i < start + 512; i+=2)
    {
        memory.set(i, data1);
        memory.set(i + 1, data2);
    }
    Cpu cpu(memory);
    OSBios bios;
    bios.setup();
    bios.registerTrapHandlers(&cpu);

    // Act
    cpu.reset();
    cpu.start(0, 256, 128);

    // Assert
    BOOST_CHECK_EQUAL(512, cpu.d0);

    char sector[512];
    std::ifstream diskIn("drivea.dsk", std::ios::binary);

    // Validate the first sector is unchanged
    diskIn.read(sector, sizeof(sector));
    BiosParameterBlock* pbpb = reinterpret_cast<BiosParameterBlock*>(sector);
    BOOST_CHECK_EQUAL(pbpb->jump[0], 0);
    BOOST_CHECK_EQUAL(pbpb->jump[1], 0);
    BOOST_CHECK_EQUAL(pbpb->jump[2], 0);
    BOOST_CHECK_EQUAL(pbpb->bytesPerSector, 512);
    BOOST_CHECK_EQUAL(pbpb->volumeID, 12345678);
    
    // validate the second sector is updated with the pattern
    diskIn.read(sector, sizeof(sector));
    for (int i = 0; i < 512; i += 2)
    {
        uint8_t res1 = sector[i];
        uint8_t res2 = sector[i + 1];
        BOOST_CHECK_EQUAL(data1, res1);
        BOOST_CHECK_EQUAL(data2, res2);
    }
    diskIn.close();
}

BOOST_AUTO_TEST_CASE(sector_write_read)
{
    unsigned char code[] = {
    0x3f,0x3c, 0x00,0x01,               // move.w #1, -(sp) sectorCount
    0x3f,0x3c, 0x00,0x01,               // move.w #1, -(sp) sectorNumber
    0x3f,0x3c, 0x00,0x01,               // move.w #1, -(sp) device
    0x2f,0x3c, 0x00,0x00, 0x01, 0x00,   // move.l #256, -(sp)  buffer
    0x3f,0x3c, 0x00,0x02,               // move.w #2, -(sp) diskwrite
    0x4e,0x42,                          // trap   #2
    0xdf,0xfc, 0x00, 0x00, 0x00, 0x0c,  // add.l  #12, sp

    0x3f,0x3c, 0x00,0x01,               // move.w #1, -(sp) sectorCount
    0x3f,0x3c, 0x00,0x01,               // move.w #1, -(sp) sectorNumber
    0x3f,0x3c, 0x00,0x01,               // move.w #1, -(sp) device
    0x2f,0x3c, 0x00,0x00, 0x03, 0x00,   // move.l #768, -(sp)  buffer
    0x3f,0x3c, 0x00,0x01,               // move.w #1, -(sp) diskread
    0x4e,0x42,                          // trap   #2
    0xdf,0xfc, 0x00, 0x00, 0x00, 0x0c,  // add.l  #12, sp

    0xff,0xff };

    // Arrange the disk
    FileCleanup configFileCleanup(configFileName);
    FileCleanup diskFileCleanup(diskFileName);
    prepare_disk(1024); // 2 sectors

    // Arrange the memory and CPU
    Memory memory(2048, 0, code, sizeof(code));
    uint8_t data1 = 0x5B;
    uint8_t data2 = 0x6C;
    int start = 256;
    for (int i = start; i < start + 512; i += 2)
    {
        data1 = i & 0xFF;
        memory.set(i, data1);
        memory.set(i + 1, data2);
    }
    Cpu cpu(memory);
    OSBios bios;
    bios.setup();
    bios.registerTrapHandlers(&cpu);

    // Act
    cpu.reset();
    cpu.start(0, 256, 128);

    // Assert
    BOOST_CHECK_EQUAL(512, cpu.d0);

    // Validate the value read back is the same as the one written
    const Memory& cpuMemory = cpu.mem;
    for (int i = 0; i < 512; i++)
    {
        uint8_t dataWritten = cpuMemory.get<uint8_t>(256 + i);
        uint8_t dataRead = cpuMemory.get<uint8_t>(768 + i);
        BOOST_CHECK_EQUAL(dataWritten, dataRead);
    }
}

// ======================================================================
// FILE related tests
// ======================================================================
BOOST_AUTO_TEST_CASE(file_find)
{
    unsigned char code[] = {
    0x3f,0x3c, 0x00,0x03,               // move.w #3, -(sp) findfile
    0x4e,0x42,                          // trap   #2
    0xdf,0xfc, 0x00, 0x00, 0x00, 0x0c,  // add.l  #2, sp
    0xff,0xff };

    // Arrange the configuration file
    FileCleanup configFileCleanup(configFileName);
    std::ofstream configFile(configFileName);
    configFile << "disk1 = drivea.dsk" << std::endl;

    // Arrange the disk file
    FileCleanup diskFileCleanup(diskFileName);
    create_fat12_disk(diskFileName);

    // Arrange the memory and CPU
    Memory memory(1024, 0, code, sizeof(code));
    Cpu cpu(memory);
    OSBios bios;
    bios.setup();
    bios.registerTrapHandlers(&cpu);

    // Act
    cpu.reset();
    cpu.start(0, 256, 128);

    // Assert
    BOOST_CHECK_EQUAL(2, cpu.d0);
}

BOOST_AUTO_TEST_CASE(trap_not_handled)
{
    unsigned char code[] = {
        // At address 1024 is the code handling TRAP #2
        0x42, 0x80,   // clr.l, d0  
        0x2f, 0x08,   // move.l a0, -(sp)
        0x4e, 0x68,   // move.l usp, a0
        0x30, 0x10,   // move.w (a0), d0
        0x20, 0x5f,   // move.l (sp)+, a0
        0x4e,0x73,    // rte
        0x4e,0x71,    // nop
        0x4e,0x71,    // nop
        0x4e,0x71,    // nop
        // Trigger a trap #2 with a value which is not handled by the trap handler (e.g. #3) to force emulator execution
        0x3f,0x3c, 0x00,0x63,               // move.w #99, -(sp) not handled
        0x4e,0x42,                          // trap   #2
        0xdf,0xfc, 0x00, 0x00, 0x00, 0x0c,  // add.l  #2, sp
        0xff,0xff };

    // Arrange the memory and CPU
    const uint32_t codeAddress = 1024;
    Memory memory(2048, 0);
    memory.set(0x88, codeAddress);            // The trap #2 vector is at 0x88 and point to the code at address 1024
    memory.set(codeAddress, code, sizeof(code));
    Cpu cpu(memory);
    OSBios bios;
    bios.setup();
    bios.registerTrapHandlers(&cpu);

    // Act
    cpu.reset();
    cpu.start(codeAddress + 18, codeAddress+ 256, codeAddress + 128);

    // Assert
    BOOST_CHECK_EQUAL(99, cpu.d0);
}
BOOST_AUTO_TEST_SUITE_END()
