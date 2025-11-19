#pragma once
#include <any>
#include <vector>
#include <map>
#include <string>
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
		: start(0)
	{
		code.push_back(codeBlock());
	}
	bool save(const char* filename) const;
	bool load(const char* filename);

	std::vector<codeBlock>		    code;
	uint32_t                        start;

	std::map<std::string, uint32_t>	labels;
	std::map<std::string, std::any> symbols;
	mc68000::errors                 errors;
private:
	asmResult(const asmResult&)            = delete;
	asmResult& operator=(const asmResult&) = delete;
};

