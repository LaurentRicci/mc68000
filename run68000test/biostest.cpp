#include <boost/test/unit_test.hpp>
#include "cpu.h"
#include "simplebios.h"
#include "ataribios.h"
#include "trapargs.h"

using namespace mc68000;

BOOST_AUTO_TEST_SUITE(bios)

BOOST_AUTO_TEST_CASE(bios_arg_1word)
{
    unsigned char code[] = {
        0x3f,0x3c, 0x00,0x2a,  //      move.w  #42,-(sp)
        0x3f,0x3c, 0x00,0x02,  //      move.w  #2,-(sp)
        0x3f,0x3c, 0x00,0x65,  //      move.w  #101,-(sp)
        0x4e,0x4f,             //      trap    #15
        0x58,0x8f,             //      addq.l  #4, sp
        0x32,0x1f,             //      move.w  (sp)+,d1
        0xff,0xff };

    // Arrange
    Memory memory(256, 0, code, sizeof(code));
    Cpu cpu(memory);
    SimpleBios bios;
    bios.setup();
    bios.registerTrapHandlers(&cpu);

    // Act
    cpu.reset();
    cpu.start(0, 256, 128);

    // Assert
    BOOST_CHECK_EQUAL(2, cpu.d0);
    BOOST_CHECK_EQUAL(42, cpu.d1);
}

BOOST_AUTO_TEST_CASE(bios_arg_1long)
{
    unsigned char code[] = {
        0x3f,0x3c, 0x00, 0x2a,              //      move.w #42, -(sp)
        0x2f,0x3c, 0x12,0xab, 0x34, 0xcd,   //      move.l #$12ab34cd, -(sp)
        0x3f,0x3c, 0x00,0x66,               //      move.w #102, -(sp)
        0x4e,0x4f,                          //      trap   #15
        0x5c,0x8f,                          //      addq.l #6, sp
        0x32, 0x1f,                         //      move.w (sp)+,d1
        0xff,0xff };

    // Arrange
    Memory memory(256, 0, code, sizeof(code));
    Cpu cpu(memory);
    SimpleBios bios;
    bios.setup();
    bios.registerTrapHandlers(&cpu);

    // Act
    cpu.reset();
    cpu.start(0, 256, 128);

    // Assert
    BOOST_CHECK_EQUAL(0x12ab34cd, cpu.d0);
    BOOST_CHECK_EQUAL(42, cpu.d1);
}

BOOST_AUTO_TEST_CASE(bios_arg_1w_1l_1w)
{
    unsigned char code[] = {
        0x3f,0x3c, 0x00,0x2a,               //      move.w #42, -(sp)
        0x3f,0x3c ,0x00,0x02,               //      move.w #2, -(sp)
        0x2f,0x3c ,0x12,0xab,0x34,0xcd,     //      move.l #$12ab34cd, -(sp)
        0x3f,0x3c ,0x00,0xfe,               //      move.w #254, -(sp)
        0x3f,0x3c ,0x00,0x67,               //      move.w #103, -(sp)
        0x4e,0x4f,                          //      trap   #15
        0xdf,0xfc ,0x00,0x00,0x00,0x0a,     //      add.l  #10, sp
        0x32,0x1f,                          //      move.w (sp)+ ,d1
        0xff,0xff };

    // Arrange
    Memory memory(256, 0, code, sizeof(code));
    Cpu cpu(memory);
    SimpleBios bios;
    bios.setup();
    bios.registerTrapHandlers(&cpu);

    // Act
    cpu.reset();
    cpu.start(0, 256, 128);

    // Assert
    BOOST_CHECK_EQUAL(0x12ab35cd, cpu.d0);
    BOOST_CHECK_EQUAL(42, cpu.d1);
}
BOOST_AUTO_TEST_CASE(bios_arg_multiple)
{
    unsigned char code[] = {
        0x3f,0x3c, 0x00,0x02,               //      move.w  #2,-(sp)
        0x3f,0x3c, 0x00,0x65,               //      move.w  #101,-(sp)
        0x4e,0x4f,                          //      trap    #15
        0x58,0x8f,                          //      addq.l  #4, sp
        0x22,0x00,                          //      move.l  d0,d1
        0x2f,0x3c, 0x12,0xab, 0x34, 0xcd,   //      move.l #$12ab34cd, -(sp)
        0x3f,0x3c, 0x00,0x66,               //      move.w #102, -(sp)
        0x4e,0x4f,                          //      trap   #15
        0x5c,0x8f,                          //      addq.l #6, sp
        0xff,0xff };

    // Arrange
    Memory memory(256, 0, code, sizeof(code));
    Cpu cpu(memory);
    SimpleBios bios;
    bios.setup();
    bios.registerTrapHandlers(&cpu);

    // Act
    cpu.reset();
    cpu.start(0, 256, 128);

    // Assert
    BOOST_CHECK_EQUAL(2, cpu.d1);
    BOOST_CHECK_EQUAL(0x12ab34cd, cpu.d0);
}

BOOST_AUTO_TEST_CASE(settings_one)
{
    // Arrange
    std::ofstream configFile("ataribios.conf");
    configFile << "key1 = value1" << std::endl;

    std::unordered_map<std::string, std::string> settings;
    AtariBios bios;

    // Act
    getConfig("ataribios.conf", settings);

    // Assert
    BOOST_CHECK_EQUAL(settings.size(), 1);
    BOOST_CHECK_EQUAL(settings["key1"], "value1");
}

BOOST_AUTO_TEST_CASE(settings_empty_line)
{
    // Arrange
    std::ofstream configFile("ataribios.conf");
    configFile << "key1 = value1" << std::endl;
    configFile << " " << std::endl;
    configFile << "key2 = value2" << std::endl;

    std::unordered_map<std::string, std::string> settings;

    // Act
    getConfig("ataribios.conf", settings);

    // Assert
    BOOST_CHECK_EQUAL(settings.size(), 2);
    BOOST_CHECK_EQUAL(settings["key1"], "value1");
    BOOST_CHECK_EQUAL(settings["key2"], "value2");
}

BOOST_AUTO_TEST_CASE(settings_comment_lines)
{
    // Arrange
    std::ofstream configFile("ataribios.conf");
    configFile << "key1 = value1  # this is a comment" << std::endl;
    configFile << "  # this line should be ignored " << std::endl;
    configFile << "key2 = value2" << std::endl;
    configFile << ";key3 = value3" << std::endl;
    configFile << "     key4 = 4" << std::endl;

    std::unordered_map<std::string, std::string> settings;

    // Act
    getConfig("ataribios.conf", settings);

    // Assert
    BOOST_CHECK_EQUAL(settings.size(), 3);
    BOOST_CHECK_EQUAL(settings["key1"], "value1");
    BOOST_CHECK_EQUAL(settings["key2"], "value2");
    BOOST_CHECK_EQUAL(settings["key4"], "4");
}
BOOST_AUTO_TEST_SUITE_END()
