#include <fstream>
#include <sstream>
#include "asmResult.h"

bool asmResult::saveBinary(const char* filename) const
{
	std::ofstream outputFile(filename, std::ios::binary);
	if (!outputFile)
	{
		return false;
	}
	// Save the header first
	const uint32_t magicNumber = 0x69344059;
	outputFile.write(reinterpret_cast<const char*>(&magicNumber), sizeof(uint32_t)); // Magic number
	outputFile.write(reinterpret_cast<const char*>(&start), sizeof(uint32_t));       // Start
	uint32_t blocksCount = code.size();
	outputFile.write(reinterpret_cast<const char*>(&blocksCount), sizeof(uint32_t)); // Number of blocks

	// then save the content
	for (const auto& block : code)
	{
		uint32_t codeSize = static_cast<uint32_t>(block.code.size() * sizeof(uint16_t));
		outputFile.write(reinterpret_cast<const char*>(&codeSize), sizeof(uint32_t)); // Code size

		outputFile.write(reinterpret_cast<const char*>(&block.origin), sizeof(uint32_t)); // Origin

		for (const auto word : block.code)
		{
			uint16_t beWord = (word >> 8) | (word << 8); // Convert to big-endian
			outputFile.write(reinterpret_cast<const char*>(&beWord), sizeof(beWord));
		}
	}
	return true;
}

bool asmResult::saveSymbols(const char* filename) const
{
    std::ofstream outputFile(filename);
    if (!outputFile)
    {
        return false;
    }
    // Save labels
    outputFile << "# Labels" << std::endl;
    for (const auto& [label, address] : labels)
    {
        outputFile << label << " " << address << std::endl;
    }
    // Save symbols
    outputFile << "# Symbols" << std::endl;
    for (const auto& [symbol, value] : symbols)
    {
        if (value.type() == typeid(int32_t))
        {
            outputFile << symbol << " " << any_cast<int32_t>(value) << std::endl;
        }
        else if (value.type() == typeid(uint32_t))
        {
            outputFile << symbol << " " << any_cast<uint32_t>(value) << std::endl;
        }
        else if (value.type() == typeid(std::string))
        {
            outputFile << symbol << " " << any_cast<std::string>(value) << std::endl;
        }
        else
        {
            outputFile << symbol << " <unsupported type>" << std::endl;
        }
    }
    return true;
}

bool asmResult::loadBinary(const char* filename)
{
	std::ifstream inputFile(filename, std::ios::binary);
	if (!inputFile)
	{
		return false;
	}

	// Read the header
	uint32_t magicNumber;
	inputFile.read(reinterpret_cast<char*>(&magicNumber), sizeof(uint32_t));
	if (magicNumber != 0x69344059)
	{
		return false;
	}
	inputFile.read(reinterpret_cast<char*>(&start), sizeof(uint32_t));

	uint32_t blocksCount;
	inputFile.read(reinterpret_cast<char*>(&blocksCount), sizeof(uint32_t));
	code.clear();
	code.reserve(blocksCount);

	for (uint32_t i = 0; i < blocksCount; i++)
	{
		code.push_back(codeBlock());
		auto currentBlock = &code.back();
		uint32_t codeSize;
		inputFile.read(reinterpret_cast<char*>(&codeSize), sizeof(uint32_t));
		uint32_t origin;
		inputFile.read(reinterpret_cast<char*>(&origin), sizeof(uint32_t));
		currentBlock->origin = origin;

		// Read the content
		currentBlock->code.clear();
		currentBlock->code.reserve(codeSize / sizeof(uint16_t));
		for (uint32_t i = 0; i < codeSize / sizeof(uint16_t); ++i)
		{
			uint16_t beWord;
			inputFile.read(reinterpret_cast<char*>(&beWord), sizeof(beWord));
			currentBlock->code.push_back((beWord >> 8) | (beWord << 8)); // Convert from big-endian
		}
	}
	return true;
}

bool asmResult::loadSymbols(const char* filename)
{
    std::ifstream inputFile(filename);
    if (!inputFile)
    {
        return false;
    }
    labels.clear();
    symbols.clear();
    std::string line;
    enum class Section { None, Labels, Symbols };
    Section currentSection = Section::None;
    while (std::getline(inputFile, line))
    {
        if (line.empty() || line[0] == '#')
        {
            if (line == "# Labels")
            {
                currentSection = Section::Labels;
            }
            else if (line == "# Symbols")
            {
                currentSection = Section::Symbols;
            }
            continue;
        }
        std::istringstream iss(line);
        std::string name;
        iss >> name;
        if (currentSection == Section::Labels)
        {
            uint32_t address;
            iss >> address;
            labels[name] = address;
        }
        else if (currentSection == Section::Symbols)
        {
            std::string valueStr;
            iss >> valueStr;
            try
            {
                int32_t intValue = std::stoi(valueStr);
                symbols[name] = intValue;
            }
            catch (const std::invalid_argument&)
            {
                try
                {
                    uint32_t uintValue = static_cast<uint32_t>(std::stoul(valueStr));
                    symbols[name] = uintValue;
                }
                catch (const std::invalid_argument&)
                {
                    symbols[name] = valueStr; // treat as string
                }
            }
        }
    }
    return true;
}
