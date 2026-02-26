#include <boost/test/unit_test.hpp>
#include "../core/cpu.h"
#include "../run68000/ataribios.h"
using namespace mc68000;

BOOST_AUTO_TEST_SUITE(bios)

BOOST_AUTO_TEST_CASE(bios_arg_1word)
{
    unsigned char code[] = {
        0x3f,0x3c, 0x00,0x2a,  //      move.w  #42,-(sp)
        0x3f,0x3c, 0x00,0x02,  //      move.w  #2,-(sp)
        0x3f,0x3c, 0x00,0x0d,  //      move.w  #13,-(sp)
        0x4e,0x4d,             //      trap    #13
        0x5c,0x8f,             //      addq.l  #4, sp
        0xff,0xff };

    // Arrange
    Memory memory(256, 0, code, sizeof(code));
    Cpu cpu(memory);
    AtariBios bios;
    bios.registerTrapHandlers(&cpu);

    // Act
    cpu.reset();
    cpu.start(0, 256, 128);

    // Assert
    BOOST_CHECK_EQUAL(42, cpu.d0);
}

BOOST_AUTO_TEST_CASE(bios_arg_1long)
{
    unsigned char code[] = {
        0x2f,0x3c, 0x12, 0xab, 0x34,0xcd,   //      move.l  #12ab34cd,-(sp)
        0x3f,0x3c, 0x00,0x02,               //      move.w  #2,-(sp)
        0x3f,0x3c, 0x00,0x0e,               //      move.w  #14,-(sp)
        0x4e,0x4d,                          //      trap    #13
        0x5c,0x8f,                          //      addq.l  #6, sp
        0xff,0xff };

    // Arrange
    Memory memory(256, 0, code, sizeof(code));
    Cpu cpu(memory);
    AtariBios bios;
    bios.registerTrapHandlers(&cpu);

    // Act
    cpu.reset();
    cpu.start(0, 256, 128);

    // Assert
    BOOST_CHECK_EQUAL(0x12ab34cd, cpu.d0);
}

BOOST_AUTO_TEST_CASE(bios_arg_2long2)
{
    unsigned char code[] = {
        0x2f,0x3c, 0x12,0xab, 0x34,0xcd,    //      move.l  #12ab34cd,-(sp)
        0x2f,0x3c, 0x34,0xbc, 0x56,0xde,    //      move.l  #34bc56de,-(sp)
        0x3f,0x3c, 0x00,0x04,               //      move.w  #4,-(sp)
        0x3f,0x3c, 0x00,0x0e,               //      move.w  #14,-(sp)
        0x4e,0x4d,                          //      trap    #13
        0x5c,0x8f,                          //      addq.l  #6, sp
        0xff,0xff };

    // Arrange
    Memory memory(256, 0, code, sizeof(code));
    Cpu cpu(memory);
    AtariBios bios;
    bios.registerTrapHandlers(&cpu);

    // Act
    cpu.reset();
    cpu.start(0, 256, 128);

    // Assert
    BOOST_CHECK_EQUAL(0x12ab34cd, cpu.d0);
}
BOOST_AUTO_TEST_CASE(bios_arg_multiple)
{
    unsigned char code[] = {
        0x2f,0x3c, 0x12,0xab, 0x34,0xcd,    //      move.l  #$12ab34cd,-(sp)
        0x2f,0x3c, 0x34,0xbc, 0x56,0xde,    //      move.l  #$34bc56de,-(sp)
        0x3f,0x3c, 0x00,0x04,               //      move.w  #4,-(sp)
        0x3f,0x3c, 0x00,0x0e,               //      move.w  #14,-(sp)
        0x4e,0x4d,                          //      trap    #13
        0x5c,0x8f,                          //      addq.l  #6, sp
        0x22,0x00,                          //      move.l  d0,d1
        0x2f,0x3c, 0x12,0xab, 0x34,0xcd,    //      move.l  #$12ab34cd,-(sp)
        0x2f,0x3c, 0x34,0xbc, 0x56,0xde,    //      move.l  #$34bc56de,-(sp)
        0x3f,0x3c, 0x00,0x02,               //      move.w  #2,-(sp)
        0x3f,0x3c, 0x00,0x0e,               //      move.w  #14,-(sp)
        0x4e,0x4d,                          //      trap    #13
        0x5c,0x8f,                          //      addq.l  #6, sp

        0xff,0xff };

    // Arrange
    Memory memory(256, 0, code, sizeof(code));
    Cpu cpu(memory);
    AtariBios bios;
    bios.registerTrapHandlers(&cpu);

    // Act
    cpu.reset();
    cpu.start(0, 256, 128);

    // Assert
    BOOST_CHECK_EQUAL(0x12ab34cd, cpu.d1);
    BOOST_CHECK_EQUAL(0x34bc56de, cpu.d0);
}
BOOST_AUTO_TEST_SUITE_END()
