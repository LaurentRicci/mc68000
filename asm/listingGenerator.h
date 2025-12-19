#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <fstream>
#include "parser68000.h"

using namespace std;

class listingGenerator
{
public:
    listingGenerator(const char* filename = nullptr);
    void enable();
    void setAddress(uint32_t address);
    void setBinary(uint16_t opcode, const vector<uint16_t>& extensions);
    void setBinary(vector<uint8_t>::const_iterator begin, vector<uint8_t>::const_iterator end);
    void setBinary(vector<uint16_t>::const_iterator begin, vector<uint16_t>::const_iterator end, uint16_t size);

    void setLabel(const string& label);
    void setInstruction(const string& label);
    void setComment(const string& comment);
    const string sourceText(antlr4::ParserRuleContext* ctx);

    void startLine();
    void endLine();
private:
    bool doListing = false;
    ofstream listingFile;

    uint32_t address{};
    enum extensionType
    {
        None,
        Byte,
        Word,
        Long
    } extensionsFormat = None;
    vector<uint16_t> extensions;

    string label;
    string instruction;
    string comment;
};
