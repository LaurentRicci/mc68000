#include <boost/test/unit_test.hpp>
#include "../core/disasm.h"

using namespace mc68000;

BOOST_AUTO_TEST_SUITE(Tutorial)

BOOST_AUTO_TEST_CASE(tutorial2)
{

	// Arrange
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
		0xffff
	};
	std::string expected =
		"move.b #$e,d0\n"
		"lea $1074.l,a1\n"
		"trap #15\n"
		"move.b #$8,d0\n"
		"trap #15\n"
		"and.l #$5fffff,d1\n"
		"divu #$64,d1\n"
		"swap d1\n"
		"addq.w #1,d1\n"
		"move.w d1,d2\n"
		"move.b #$e,d0\n"
		"lea $10d9.l,a1\n"
		"trap #15\n"
		"move.b #$4,d0\n"
		"trap #15\n"
		"cmp.w #$0,d1\n"
		"beq $1070\n"
		"cmp.w d2,d1\n"
		"bhi $1054\n"
		"bcs $1062\n"
		"move.b #$e,d0\n"
		"lea $10ee.l,a1\n"
		"trap #15\n"
		"bra $1000\n"
		"move.b #$e,d0\n"
		"lea $110f.l,a1\n"
		"trap #15\n"
		"bra $1022\n"
		"move.b #$e,d0\n"
		"lea $1129.l,a1\n"
		"trap #15\n"
		"bra $1022\n"
		"end\n";
	// Act
	DisAsm d;
	auto result = d.dasm(code, 0x1000);

	// Assert
	BOOST_CHECK_EQUAL(expected.c_str(), result.c_str());
}
BOOST_AUTO_TEST_SUITE_END()