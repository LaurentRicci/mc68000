#include <boost/test/unit_test.hpp>
#include "../core/cpu.h"
#include "../core/memory.h"
#include "coretest.h"


BOOST_AUTO_TEST_SUITE(cpuSuite_bit)

// ===================================================
// BCHG tests
// ===================================================
BOOST_AUTO_TEST_CASE(bit_bchg_register_set)
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

BOOST_AUTO_TEST_CASE(bit_bchg_register_reset)
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

BOOST_AUTO_TEST_CASE(bit_bchg_register_indirect)
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

BOOST_AUTO_TEST_CASE(bit_bchg_register_indirect_increment)
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

BOOST_AUTO_TEST_CASE(bit_bchg_immediate)
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

BOOST_AUTO_TEST_CASE(bit_bchg_immediate_word)
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

BOOST_AUTO_TEST_CASE(bit_bchg_immediate_too_long)
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


BOOST_AUTO_TEST_SUITE_END()