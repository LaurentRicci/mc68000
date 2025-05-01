#pragma once
#include <any>
#include <vector>

class asmparser
{
public:
	bool parseFile(const char* filename);
	std::any parseText(const char* text);
	size_t checkSyntax(const char* text);
	const std::vector<uint16_t>& getCode() const { return code; }
private:
	std::vector<uint16_t> code;
};