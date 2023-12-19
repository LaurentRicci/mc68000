#pragma once
#include <cstdint>
#include <stddef.h>

namespace mc68000
{
	class memory
	{
	public:
		memory(uint32_t size, uint32_t baseAddress) :
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
		memory(uint32_t size, uint32_t baseAddress, const uint8_t* content, uint32_t contentSize) :
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

		template<typename T> T get(uint32_t address) const
		{
			return 0;
		}

		template<typename T> void set(uint32_t address, T data) 
		{
		}

		uint16_t getWord(uint32_t address)
		{
			uint8_t* p8 = rawMemory + (address - baseAddress);

			uint16_t word = (*p8 << 8) | *(p8 + 1);

			return word;
		}

		~memory()
		{
			delete[] rawMemory;
		}

	private:
		void verifyAddress(uint32_t address, uint32_t size) const
		{
			if (address < baseAddress || (address + size) >(baseAddress + this->size))
			{
				throw "memory:verifyAddress: illegal address";
			}
		}
	private:
		uint8_t* rawMemory;
		uint32_t size;
		uint32_t baseAddress;
	};

	template<> uint8_t memory::get<uint8_t>(uint32_t address) const;
	template<> uint16_t memory::get<uint16_t>(uint32_t address) const;
	template<> uint32_t memory::get<uint32_t>(uint32_t address) const;

	template<> void memory::set<uint8_t>(uint32_t address, uint8_t data);
	template<> void memory::set<uint16_t>(uint32_t address, uint16_t data);
	template<> void memory::set<uint32_t>(uint32_t address, uint32_t data);

}