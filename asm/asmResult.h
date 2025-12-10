#pragma once
#include <any>
#include <vector>
#include <map>
#include <string>
#include <cstdint>

#include "errors.h"

struct codeBlock
{
	std::vector<uint16_t>	code;
	uint32_t				origin;
	codeBlock(uint32_t org = 0) : origin(org) 
	{
	}
};

class asmResult
{
public:
	asmResult()
	{
		code.push_back(codeBlock());
	}
	bool saveBinary(const char* filename) const;
    bool saveSymbols(const char* filename) const;
    bool loadBinary(const char* filename);
    bool loadSymbols(const char* filename);

	std::vector<codeBlock>		    code;
	uint32_t                        start = 0;
    uint32_t                        memoryStart = 0;
    uint32_t                        memoryEnd = 0;

	std::map<std::string, uint32_t>	labels;
	std::map<std::string, std::any> symbols;
	mc68000::errors                 errors;
private:
	asmResult(const asmResult&)            = delete;
	asmResult& operator=(const asmResult&) = delete;
};

