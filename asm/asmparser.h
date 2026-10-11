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
    void setGeneratePIC(bool generatePIC) { this->generatePIC = generatePIC; }

	const std::vector<codeBlock>& getCodeBlocks() const { return result.code; }
	const std::map<std::string, labelInfo>& getLabels() const { return result.labels; }
    const std::map<std::string, std::any>& getSymbols() const { return result.symbols; }
    const std::vector<Relocation>   getRelocations() const { return result.relocations; }
    const mc68000::errors& getErrors() const { return result.errors; }
    const asmResult& getCode68000() const { return result; }
private:
	asmResult result;
    const char* listingFile = nullptr;
    bool generatePIC = false; // true if the generated code should be position independent (PIC)
};
