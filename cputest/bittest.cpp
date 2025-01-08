#include <boost/test/unit_test.hpp>
#include "../core/cpu.h"
#include "../core/memory.h"
#include "verifyexecution.h"


BOOST_AUTO_TEST_SUITE(cpuSuite_bit)

// ===================================================
// BCHG tests
// ===================================================
BOOST_AUTO_TEST_CASE(bchg_register_set)
{
	unsigned char code[] =
	{
		0x01, 0x41,    // bchg d0, d1
		0x4e, 0x40 };  // trap #0

	verifyExecution(code, sizeof(code), [](const Cpu& cpu)
		{
			BOOST_CHECK_EQUAL(0x0, cpu.d0);
			BOOST_CHECK_EQUAL(0x1, cpu.d1);
			BOOST_CHECK_EQUAL(0x1, cpu.sr.z);
		});
}

BOOST_AUTO_TEST_CASE(bchg_register_reset)
{
	unsigned char code[] =
	{
		0x70, 0x09,    // moveq #9, d0
		0x72, 0xff,    // moveq #$ff, d1
		0x01, 0x41,    // bchg d0, d1
		0x4e, 0x40 };  // trap #0

	verifyExecution(code, sizeof(code), [](const Cpu& cpu)
		{
			BOOST_CHECK_EQUAL(0x9, cpu.d0);
			BOOST_CHECK_EQUAL(0xFFFFFDFF, cpu.d1);
			BOOST_CHECK_EQUAL(0x0, cpu.sr.z);
		});
}

BOOST_AUTO_TEST_CASE(bchg_register_indirect)
{
	unsigned char code[] =
	{
		0x70, 0x03,                // moveq #3, d0
		0x43, 0xfa, 0x00, 0x0a,    // lea data(pc), a1
		0x01, 0x51,                // bchg d0, (a1)
		0x4e, 0x40,                // trap #0
		0xff, 0xff, 0xff, 0xff,
		0x12, 0x34 };              // data: dc.b $12,$34


	verifyExecution(code, sizeof(code), 0x1000, [](const Cpu& cpu)
		{
			BOOST_CHECK_EQUAL(0x3, cpu.d0);
			BOOST_CHECK_EQUAL(0x100e, cpu.a1);
			BOOST_CHECK_EQUAL(0x1, cpu.sr.z);
			BOOST_CHECK_EQUAL(0x1a, cpu.mem.get<uint8_t>(0x100e));
		});
}

BOOST_AUTO_TEST_CASE(bchg_register_indirect_increment)
{
	unsigned char code[] =
	{
		0x70, 0x03,                // moveq #3, d0
		0x43, 0xfa, 0x00, 0x0a,    // lea data(pc), a1
		0x01, 0x59,                // bchg d0, (a1)+
		0x4e, 0x40,                // trap #0
		0xff, 0xff, 0xff, 0xff,
		0x12, 0x34 };              // data: dc.b $12,$34


	verifyExecution(code, sizeof(code), 0x1000, [](const Cpu& cpu)
		{
			BOOST_CHECK_EQUAL(0x3, cpu.d0);
			BOOST_CHECK_EQUAL(0x100f, cpu.a1);
			BOOST_CHECK_EQUAL(0x1, cpu.sr.z);
			BOOST_CHECK_EQUAL(0x1a, cpu.mem.get<uint8_t>(0x100e));
			BOOST_CHECK_EQUAL(0x34, cpu.mem.get<uint8_t>(0x100f));
		});
}

BOOST_AUTO_TEST_CASE(bchg_immediate)
{
	unsigned char code[] =
	{
		0x08, 0x40, 0x00, 0x03,    // bchg #3, d0
		0x4e, 0x40 };  // trap #0

	verifyExecution(code, sizeof(code), [](const Cpu& cpu)
		{
			BOOST_CHECK_EQUAL(0x8, cpu.d0);
			BOOST_CHECK_EQUAL(0x1, cpu.sr.z);
		});
}

BOOST_AUTO_TEST_CASE(bchg_immediate_word)
{
	unsigned char code[] =
	{
		0x08, 0x40, 0x00, 0x0b,    // bchg #11, d0
		0x4e, 0x40 };              // trap #0

	verifyExecution(code, sizeof(code), [](const Cpu& cpu)
		{
			BOOST_CHECK_EQUAL(0x800, cpu.d0);
			BOOST_CHECK_EQUAL(0x1, cpu.sr.z);
		});
}

BOOST_AUTO_TEST_CASE(bchg_immediate_too_long)
{
	unsigned char code[] =
	{
		0x08, 0x40, 0x00, 0x23,    // bchg #11, d0
		0x4e, 0x40 };              // trap #0

	verifyExecution(code, sizeof(code), [](const Cpu& cpu)
		{
			BOOST_CHECK_EQUAL(0x8, cpu.d0);
			BOOST_CHECK_EQUAL(0x1, cpu.sr.z);
		});
}

// ===================================================
// BCLR tests
// ===================================================
BOOST_AUTO_TEST_CASE(bclr_register_set)
{
	unsigned char code[] =
	{
		0x72, 0xff,		// moveq #$ff, d1
		0x01, 0x81,    // bclr d0, d1
		0x4e, 0x40 };  // trap #0

	verifyExecution(code, sizeof(code), [](const Cpu& cpu)
		{
			BOOST_CHECK_EQUAL(0x0, cpu.d0);
			BOOST_CHECK_EQUAL(0xfffffffe, cpu.d1);
			BOOST_CHECK_EQUAL(0x0, cpu.sr.z);
		});
}

BOOST_AUTO_TEST_CASE(bclr_register_indirect)
{
	unsigned char code[] =
	{
		0x70, 0x03,                // moveq #3, d0
		0x43, 0xfa, 0x00, 0x0a,    // lea data(pc), a1
		0x01, 0x91,                // bclr d0, (a1)
		0x4e, 0x40,                // trap #0
		0xff, 0xff, 0xff, 0xff,
		0x12, 0x34 };              // data: dc.b $12,$34


	verifyExecution(code, sizeof(code), 0x1000, [](const Cpu& cpu)
		{
			BOOST_CHECK_EQUAL(0x3, cpu.d0);
			BOOST_CHECK_EQUAL(0x100e, cpu.a1);
			BOOST_CHECK_EQUAL(0x1, cpu.sr.z);
			BOOST_CHECK_EQUAL(0x12, cpu.mem.get<uint8_t>(0x100e));
		});
}

BOOST_AUTO_TEST_CASE(bclr_immediate)
{
	unsigned char code[] =
	{
		0x7a, 0x05,                // moveq #5, d5
		0x08, 0x85, 0x00, 0x02,    // bclr d0, (a1)
		0x4e, 0x40,                // trap #0
		0xff, 0xff, 0xff, 0xff };


	verifyExecution(code, sizeof(code), 0x1000, [](const Cpu& cpu)
		{
			BOOST_CHECK_EQUAL(0x1, cpu.d5);
			BOOST_CHECK_EQUAL(0x0, cpu.sr.z);
		});
}
// ===================================================
// BSET tests
// ===================================================
BOOST_AUTO_TEST_CASE(bset_register_set)
{
	unsigned char code[] =
	{
		0x72, 0xfe,	   // moveq #$fe, d1
		0x01, 0xC1,    // bset d0, d1
		0x4e, 0x40 };  // trap #0

	verifyExecution(code, sizeof(code), [](const Cpu& cpu)
		{
			BOOST_CHECK_EQUAL(0x0, cpu.d0);
			BOOST_CHECK_EQUAL(0xffffffff, cpu.d1);
			BOOST_CHECK_EQUAL(0x1, cpu.sr.z);
		});
}

BOOST_AUTO_TEST_CASE(bset_register_indirect)
{
	unsigned char code[] =
	{
		0x70, 0x03,                // moveq #3, d0
		0x43, 0xfa, 0x00, 0x0a,    // lea data(pc), a1
		0x01, 0xd1,                // bset d0, (a1)
		0x4e, 0x40,                // trap #0
		0xff, 0xff, 0xff, 0xff,
		0x12, 0x34 };              // data: dc.b $12,$34


	verifyExecution(code, sizeof(code), 0x1000, [](const Cpu& cpu)
		{
			BOOST_CHECK_EQUAL(0x3, cpu.d0);
			BOOST_CHECK_EQUAL(0x100e, cpu.a1);
			BOOST_CHECK_EQUAL(0x1, cpu.sr.z);
			BOOST_CHECK_EQUAL(0x1a, cpu.mem.get<uint8_t>(0x100e));
		});
}

BOOST_AUTO_TEST_CASE(bset_immediate)
{
	unsigned char code[] =
	{
		0x7a, 0x05,	               // moveq #5, d5
		0x08, 0xC5, 0x00, 0x06,    // bset #6, d5
		0x4e, 0x40 };              // trap #0

	verifyExecution(code, sizeof(code), [](const Cpu& cpu)
		{
			BOOST_CHECK_EQUAL(0x45, cpu.d5);
			BOOST_CHECK_EQUAL(0x1, cpu.sr.z);
		});
}
// ===================================================
// BTST tests
// ===================================================
BOOST_AUTO_TEST_CASE(btst_register_set)
{
	unsigned char code[] =
	{
		0x72, 0xfe,	   // moveq #$fe, d1
		0x01, 0x01,    // btst d0, d1
		0x4e, 0x40 };  // trap #0

	verifyExecution(code, sizeof(code), [](const Cpu& cpu)
		{
			BOOST_CHECK_EQUAL(0x0, cpu.d0);
			BOOST_CHECK_EQUAL(0xfffffffe, cpu.d1);
			BOOST_CHECK_EQUAL(0x1, cpu.sr.z);
		});
}

BOOST_AUTO_TEST_CASE(btst_register_indirect)
{
	unsigned char code[] =
	{
		0x70, 0x03,                // moveq #3, d0
		0x43, 0xfa, 0x00, 0x0a,    // lea data(pc), a1
		0x01, 0x11,                // btst d0, (a1)
		0x4e, 0x40,                // trap #0
		0xff, 0xff, 0xff, 0xff,
		0x12, 0x34 };              // data: dc.b $12,$34


	verifyExecution(code, sizeof(code), 0x1000, [](const Cpu& cpu)
		{
			BOOST_CHECK_EQUAL(0x3, cpu.d0);
			BOOST_CHECK_EQUAL(0x100e, cpu.a1);
			BOOST_CHECK_EQUAL(0x1, cpu.sr.z);
			BOOST_CHECK_EQUAL(0x12, cpu.mem.get<uint8_t>(0x100e));
		});
}

BOOST_AUTO_TEST_CASE(btst_immediate)
{
	unsigned char code[] =
	{
		0x7a, 0x05,	               // moveq #5, d5
		0x44, 0xfc, 0x00, 0xff,    // move #$ff,ccr
		0x08, 0x05, 0x00, 0x02,    // btst #2, d5
		0x4e, 0x40 };              // trap #0

	verifyExecution(code, sizeof(code), [](const Cpu& cpu)
		{
			BOOST_CHECK_EQUAL(0x5, cpu.d5);
			BOOST_CHECK_EQUAL(0x0, cpu.sr.z);
		});
}

// ===================================================
// NOT Tests
// ===================================================
BOOST_AUTO_TEST_CASE(not_)
{
	unsigned char code[] =
	{
		0x70, 0x2a,	   // moveq #42, d0
		0x46, 0x00,    // not.b d0
		0x40, 0xc1,    // move sr,d1

		0x74, 0xd6,	   // moveq #-42, d2
		0x46, 0x42,    // not.w d2
		0x40, 0xc3,    // move sr,d3

		0x78, 0xff,	   // moveq #$ff, d4
		0x46, 0x84,    // not.l d4
		0x40, 0xc5,    // move sr,d5

		0x4e, 0x40 };  // trap #0

	verifyExecution(code, sizeof(code), [](const Cpu& cpu)
		{
			BOOST_CHECK_EQUAL(0xd5, cpu.d0); // 0010 1010 -> 1101 0101 -> 0xD5
			BOOST_CHECK_EQUAL(0x08, cpu.d1);

			BOOST_CHECK_EQUAL(0xffff0029, cpu.d2); // 1101 0110 -> 0010 1001 -> 0x29
			BOOST_CHECK_EQUAL(0x00, cpu.d3);

			BOOST_CHECK_EQUAL(0x00, cpu.d4); // 1111 1111 -> 0000 0000 -> 0x00
			BOOST_CHECK_EQUAL(0x04, cpu.d5);

			BOOST_CHECK_EQUAL(0x1, cpu.sr.z);
		});
}

// ===================================================
// SCC Tests
// ===================================================
BOOST_AUTO_TEST_CASE(scc)
{
	unsigned char code[] =
	{
		0x70, 0x00,	   // moveq #0, d0
		0x57, 0xc1,    // seq d1
		0x4e, 0x40,    // trap #0
	};
	verifyExecution(code, sizeof(code), [](const Cpu& cpu)
	{
		BOOST_CHECK_EQUAL(0x0, cpu.d0);
		BOOST_CHECK_EQUAL(0xff, cpu.d1);
		BOOST_CHECK_EQUAL(0x1, cpu.sr.z);
	});
}
BOOST_AUTO_TEST_SUITE_END()