#pragma once
#include <any>
#include <vector>
#include <map>

#include "asmResult.h"
#include "errors.h"

class asmparser
{
public:
	bool parseFile(const char* filename, bool showTree=false);
	std::any parseText(const char* text);
	size_t checkSyntax(const char* text);
    bool saveBinary(const char* filename);
    bool saveSymbols(const char* filename);
    void listingFileName(const char* filename);

	const std::vector<codeBlock>& getCodeBlocks() const { return result.code; }
	const std::map<std::string, uint32_t>& getLabels() const { return result.labels; }
    const std::map<std::string, std::any>& getSymbols() const { return result.symbols; }
    const mc68000::errors& getErrors() const { return result.errors; }
	const asmResult& getCode68000() const { return result; }
private:
	asmResult result;
    const char* listingFile = nullptr;
};
