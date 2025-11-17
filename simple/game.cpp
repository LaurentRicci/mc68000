#include <cstring>
#include <iostream>
#include "emulator.h"

uint16_t code[] = {
	0x103C, 0x000E,           // 16  start:   move.b  #14,d0      Display instructions
	0x43F9, 0x0000, 0x1074,   // 17      lea     instrMsg,a1
	0x4E4F,                   // 18      trap    #15

	0x103C, 0x0008,           // 25      move.b  #8,d0
	0x4E4F,                   // 26      trap    #15
	0xC2BC, 0x005F, 0xFFFF,   // 27      and .l   #$5FFFFF,d1     prevent overflow in divu
	0x82FC, 0x0064,           // 28      divu    #100,d1         time count / 100
	0x4841,                   // 29      swap    d1              swap upper and lower words of D1 to put remainder in low word
	0x5241,                   // 30      addq.w  #1,d1           d1.w contains number from 1 to 100
	0x3401,                   // 31      move    d1,d2           d2 = d1

								// 33  again:
	0x103C, 0x000E,           // 34      move.b  #14,d0          Display 'enter guess' prompt
	0x43F9, 0x0000, 0x10D9,   // 35      lea     enterMsg,a1
	0x4E4F,                   // 36      trap    #15

	0x103C, 0x0004,           // 38      move.b  #4,d0           get user guess in D1.L
	0x4E4F,                   // 39      trap    #15

	0xB27C, 0x0000,           // 41      cmp     #0,d1           if exit
	0x6700, 0x0036,           // 42      beq     end

	0xB242,                   // 44      cmp     d2,d1           compare the user guess in d1 to the number in d2
	0x6200, 0x0014,           // 45      bhi     tooBig          if the guess is too big
	0x6500, 0x001E,           // 46      blo     tooSmall        if the guess is too small

	0x103C, 0x000E,           // 49      move.b  #14,d0          Display 'Good Guess' message
	0x43F9, 0x0000, 0x10EE,   // 50      lea     goodMsg,a1
	0x4E4F,                   // 51      trap    #15

	0x60AC,                   // 53      bra     start           goto start

	0x103C, 0x000E,           // 55  tooBig  move.b  #14,d0      Display 'Too Big'
	0x43F9, 0x0000, 0x110F,   // 56      lea     bigMsg,a1
	0x4E4F,                   // 57      trap    #15

	0x60C0,                   // 59      bra     again           guess again

	0x103C, 0x000E,           // 61  tooSmall    move.b  #14,d0  Display 'Too Small'
	0x43F9, 0x0000, 0x1129,   // 62      lea     smallMsg,a1
	0x4E4F,                   // 63      trap    #15

	0x60B2,                   // 65      bra     again           guess again
	0xffff, 0xffff
};

char instrMsg[] =
	"\r\n"
	"----- Number guessing game -----\r\n"
	" Guess the number from 1 to 100\r\n"
	" Enter a 0 to quit the game\r\n\r\n"
	;

char enterMsg[] = "\r\nEnter your guess: ";
char goodMsg[] = "\r\nGood guess, that is correct!\r\n";
char bigMsg[] = "\r\nYour guess is too big\r\n";
char smallMsg[] = "\r\nYour guess is too small\r\n";


uint8_t* swap(uint16_t* code, size_t length)
{
	for (int i = 0; i < length/2; i++)
	{
		uint16_t word = code[i];
		code[i] = (word >> 8) | (word << 8);
	}
	return (uint8_t*) code;
}

uint8_t* assemble()
{
	size_t size = 4096;
	uint8_t* memory = new uint8_t[size];
	memcpy(memory, swap(code, sizeof(code)), sizeof(code));
	size_t offset = sizeof(code);
	memcpy(memory + offset, instrMsg, sizeof(instrMsg));
	offset += sizeof(instrMsg);

	memcpy(memory + offset, enterMsg, sizeof(enterMsg));
	offset += sizeof(enterMsg);

	memcpy(memory + offset, goodMsg, sizeof(goodMsg));
	offset += sizeof(goodMsg);

	memcpy(memory + offset, bigMsg, sizeof(bigMsg));
	offset += sizeof(bigMsg);

	memcpy(memory + offset, smallMsg, sizeof(smallMsg));
	offset += sizeof(smallMsg);

	return memory;
}

void game(bool debugMode)
{
	uint8_t* code = assemble();

	Emulator emulator(4096, 0x1000, code, 4096);
	emulator.debug(debugMode);
	emulator.run();
}

void game(const char* binaryFile, bool debugMode)
{
	Emulator emulator(binaryFile);
	emulator.debug(debugMode);
	emulator.run(0, 1024, 1024);
}