#pragma once
#include <cstdint>

namespace mc68000
{
	class memory
	{
	public:
		memory(unsigned int size, unsigned int baseAddress) :
			size(size),
			baseAddress(baseAddress)
		{
			rawMemory = new unsigned char[size];
			auto p1 = rawMemory;
			for (unsigned int i = 0; i < size; i++)
			{
				*p1++ = 0;
			}
		}
		memory(unsigned int size, unsigned int baseAddress, const unsigned char* content, unsigned int contentSize) :
			size(size),
			baseAddress(baseAddress)
		{
			if (contentSize > size)
			{
				throw "invalid size";
			}
			rawMemory = new unsigned char[size];
			auto p1 = rawMemory;
			auto p2 = content;
			unsigned int i = 0;
			for (; i < contentSize; i++)
			{
				*p1++ = *p2++;
			}
			for (; i < size; i++)
			{
				*p1++ = 0;
			}
		}
		memory() :
			rawMemory(nullptr),
			size(0),
			baseAddress(0)
		{
		}

		memory(const memory& rhs) :
			rawMemory(nullptr),
			size(rhs.size),
			baseAddress(rhs.baseAddress)
		{
			if (size)
			{
				rawMemory = new unsigned char[size];
				auto p1 = rawMemory;
				auto p2 = rhs.rawMemory;
				for (unsigned int i = 0; i < size; i++)
				{
					*p1++ = *p2++;
				}
			}
		}

		memory& operator=(const memory& rhs)
		{
			size = rhs.size;
			baseAddress = rhs.baseAddress;
			delete[] rawMemory;

			if (size)
			{
				rawMemory = new unsigned char[size];
				auto p1 = rawMemory;
				auto p2 = rhs.rawMemory;
				for (unsigned int i = 0; i < size; i++)
				{
					*p1++ = *p2++;
				}
			}
			else
			{
				rawMemory = nullptr;
			}
			return *this;
		}

		template<typename T> T get(uint32_t address)
		{
			return 0;
		}
		template<> uint8_t get<uint8_t>(uint32_t address);

		template<> uint16_t get<uint16_t>(uint32_t address);

		template<> uint32_t get<uint32_t>(uint32_t address);

		uint16_t getWord(uint32_t address)
		{
			uint8_t* p8 = rawMemory + (address - baseAddress);

			uint16_t word = (*p8++ << 8) | *p8;

			return word;
		}
		~memory()
		{
			delete[] rawMemory;
		}
	private:
		unsigned char* rawMemory;
		unsigned int size;
		unsigned int baseAddress;
	};
}