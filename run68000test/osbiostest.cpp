#include <boost/test/unit_test.hpp>
#include "osbios.h"
using namespace mc68000;

BOOST_AUTO_TEST_SUITE(osbios)

BOOST_AUTO_TEST_CASE(osbios)
{
    unsigned char code[] = {
    0x3f,0x3c, 0x00,0x01,               //      move.w #1, -(sp)
    0x4e,0x42,                          //      trap   #2
    0x5c,0x8f,                          //      addq.l #2, sp
    0xff,0xff };

    // Arrange
    std::ofstream configFile("osbios.conf");
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
    std::ofstream configFile("osbios.conf");
    configFile << "disk1 = drivea.dsk" << std::endl;

    // Arrange the disk file
    std::ofstream disk("drivea.dsk", std::ios::binary);
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
    std::ofstream configFile("osbios.conf");
    configFile << "disk1 = drivea.dsk" << std::endl;

    // Arrange the disk file
    std::ofstream disk("drivea.dsk", std::ios::binary);
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
    std::ofstream configFile("osbios.conf");
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
    std::ofstream configFile("osbios.conf");
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
BOOST_AUTO_TEST_SUITE_END()
