#include "memory.h"

namespace mc68000
{
	template<> uint8_t memory::get<uint8_t>(uint32_t address) const
	{
		verifyAddress(address, sizeof(uint8_t));
		uint8_t* p8 = rawMemory + (address - baseAddress);
		return *(p8 + 0); // TODO: why was it +1
	}

	template<> uint16_t memory::get<uint16_t>(uint32_t address) const
	{
		verifyAddress(address, sizeof(uint16_t));
		uint8_t* p8 = rawMemory + (address - baseAddress);
		uint16_t word = (*p8 << 8) | *(p8 + 1);
		return word;
	}

	template<> uint32_t memory::get<uint32_t>(uint32_t address) const
	{
		verifyAddress(address, sizeof(uint32_t));
		uint8_t* p8 = rawMemory + (address - baseAddress);
		uint32_t longWord = (*p8 << 24) | (*(p8 + 1) << 16) | (*(p8 + 2) << 8) | *(p8 + 3);
		return longWord;
	}

	template<> void memory::set<uint8_t>(uint32_t address, uint8_t data)
	{
		verifyAddress(address, sizeof(uint8_t));
		uint8_t* p8 = rawMemory + (address - baseAddress);
		*p8 = data;
	}

	template<> void memory::set<uint16_t>(uint32_t address, uint16_t data)
	{
		verifyAddress(address, sizeof(uint16_t));
		uint8_t* p8 = rawMemory + (address - baseAddress);
		*p8++ = data >> 8;
		*p8 = data & 0xff;
	}

	template<> void memory::set<uint32_t>(uint32_t address, uint32_t data)
	{
		verifyAddress(address, sizeof(uint32_t));
		uint8_t* p8 = rawMemory + (address - baseAddress);
		*p8++ = (data >> 24) & 0xff;
		*p8++ = (data >> 16) & 0xff;
		*p8++ = (data >> 8) & 0xff;
		*p8   = data & 0xff;
	}
}