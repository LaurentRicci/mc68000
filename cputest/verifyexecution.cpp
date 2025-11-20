#include <boost/test/unit_test.hpp>
#include "../core/cpu.h"
#include "../core/memory.h"

using namespace mc68000;

void verifyExecution(const uint8_t* code, uint32_t size, void (*assertFunctor)(const Cpu& c))
{
	// Arrange
	Memory memory(256, 0, code, size);
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(0);

	// Assert
	assertFunctor(cpu);
}

void verifyExecution(const uint8_t* code, uint32_t size, uint32_t baseAddress, void (*assertFunctor)(const Cpu& c))
{
	// Arrange
	Memory memory(256, baseAddress, code, size);
	Cpu cpu(memory);

	// Act
	cpu.reset();
	cpu.start(baseAddress);

	// Assert
	assertFunctor(cpu);
}


void validateSR(const Cpu& cpu, int x, int n, int z, int v, int c)
{
	if (x != -1) BOOST_CHECK_EQUAL(x, cpu.sr.x);
	if (n != -1) BOOST_CHECK_EQUAL(n, cpu.sr.n);
	if (z != -1) BOOST_CHECK_EQUAL(z, cpu.sr.z);
	if (v != -1) BOOST_CHECK_EQUAL(v, cpu.sr.v);
	if (c != -1) BOOST_CHECK_EQUAL(c, cpu.sr.c);
}

