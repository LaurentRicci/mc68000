#pragma once
#include <cstdint>
#include <stddef.h>
#include <fstream>

namespace mc68000
{
	class Memory
	{
	public:
		Memory(uint32_t size, uint32_t baseAddress) :
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

		Memory(uint32_t size, uint32_t baseAddress, const uint8_t* content, uint32_t contentSize) :
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

		Memory(const char* binaryFile)
		{
			std::ifstream inputFile(binaryFile, std::ios::binary);
			if (!inputFile)
			{
				throw "cannot open file";
			}

			// Read the header
			uint32_t magicNumber;
			inputFile.read(reinterpret_cast<char*>(&magicNumber), sizeof(uint32_t));
			if (magicNumber != 0x69344059)
			{
				throw "invalid magic number";
			}
			uint32_t start;
			inputFile.read(reinterpret_cast<char*>(&start), sizeof(uint32_t));

			uint32_t blocksCount;
			inputFile.read(reinterpret_cast<char*>(&blocksCount), sizeof(uint32_t));

			// For now, only one block is supported
			if (blocksCount != 1)
			{
				throw "only one block supported";
			}

			for (uint32_t i = 0; i < blocksCount; i++)
			{
				uint32_t codeSize;
				inputFile.read(reinterpret_cast<char*>(&codeSize), sizeof(uint32_t));
				size = codeSize * 2; // Each instruction is 2 bytes

				inputFile.read(reinterpret_cast<char*>(&baseAddress), sizeof(uint32_t));

				// Read the content
				rawMemory = new unsigned char[size + 1024];		// need some extra space for the stack
				char* p = reinterpret_cast<char*>(rawMemory);
				for (uint32_t i = 0; i < size; ++i, ++p)
				{
					inputFile.read(p, sizeof(uint8_t));
				}
                size += 1024;
			}
		}
		Memory() :
			rawMemory(nullptr),
			size(0),
			baseAddress(0)
		{
		}

		Memory(const Memory& rhs) :
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

		Memory& operator=(const Memory& rhs)
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

        std::pair<uint32_t, uint32_t> getMemoryRange() const
		{
			return { baseAddress, size };
		}

		~Memory()
		{
			delete[] rawMemory;
		}

	private:
		void verifyAddress(uint32_t address, uint32_t size) const
		{
			if (address < baseAddress || address > (baseAddress + this->size) || (address + size) >(baseAddress + this->size))
			{
				throw "memory:verifyAddress: illegal address";
			}
		}
	private:
		uint8_t* rawMemory;
		uint32_t size;
		uint32_t baseAddress;
	};

	template<> uint8_t Memory::get<uint8_t>(uint32_t address) const;
	template<> uint16_t Memory::get<uint16_t>(uint32_t address) const;
	template<> uint32_t Memory::get<uint32_t>(uint32_t address) const;
	template<> void* Memory::get<void*>(uint32_t address) const;

	template<> void Memory::set<uint8_t>(uint32_t address, uint8_t data);
	template<> void Memory::set<uint16_t>(uint32_t address, uint16_t data);
	template<> void Memory::set<uint32_t>(uint32_t address, uint32_t data);

}
