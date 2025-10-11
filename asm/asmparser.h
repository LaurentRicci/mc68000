#pragma once
#include <any>
#include <vector>
#include <map>

#include "errors.h"

class asmparser
{
public:
	bool parseFile(const char* filename, bool showTree=false);
	std::any parseText(const char* text);
	size_t checkSyntax(const char* text);
	const std::vector<uint16_t>& getCode() const { return code; }
	const std::map<std::string, uint32_t>& getLabels() const { return labels; }
	const mc68000::errors& getErrors() const { return errorsList; }
private:
	std::vector<uint16_t> code;
	std::map<std::string, uint32_t> labels;
	std::vector<uint32_t> labelsLocation;
	mc68000::errors errorsList;
};