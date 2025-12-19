#include <iostream>
#include <iomanip>

#include "listingGenerator.h"

listingGenerator::listingGenerator(const char* filename)
{
    if (filename)
    {
        listingFile.open(filename);
        if (!listingFile.is_open())
        {
            doListing = false;
            cerr << "Could not open listing file: " << filename << endl;
        }
    }
}
void listingGenerator::enable()
{
    if (listingFile.is_open())
    {
        doListing = true;
    }
}

void listingGenerator::setAddress(uint32_t address)
{
    if (doListing)
    {
        this->address = address;
    }
}

void listingGenerator::setBinary(uint16_t opcode, const vector<uint16_t>& extensions)
{
    if (doListing)
    {
        this->extensions.push_back(opcode);
        this->extensions.insert(this->extensions.end(), extensions.begin(), extensions.end());
        extensionsFormat = Word;
    }
}
void listingGenerator::setBinary(vector<uint8_t>::const_iterator begin, vector<uint8_t>::const_iterator end)
{
    if (doListing)
    {
        for (auto it = begin; it != end; ++it)
        {
            extensions.push_back(static_cast<uint16_t>(*it));
        }
        extensionsFormat = Byte;
    }
}
void listingGenerator::setBinary(vector<uint16_t>::const_iterator begin, vector<uint16_t>::const_iterator end, uint16_t size)
{
    if (doListing)
    {
        for (auto it = begin; it != end; ++it)
        {
            extensions.push_back(static_cast<uint16_t>(*it));
        }
        extensionsFormat = (size == 2) ? Long : Word;
    }
}

void listingGenerator::setLabel(const string& label)
{
    if (doListing)
    {
        this->label = label;
    }
}

void listingGenerator::setInstruction(const string& instruction)
{
    if (doListing)
    {
        this->instruction = instruction;
    }
}

void listingGenerator::setComment(const string& comment)
{
    if (doListing)
    {
        this->comment = comment;
    }
}

const string listingGenerator::sourceText(antlr4::ParserRuleContext* ctx)
{
    antlr4::Token* start = ctx->getStart();
    antlr4::Token* stop = ctx->getStop();

    antlr4::misc::Interval interval(start->getStartIndex(), stop->getStopIndex());
    const string source = start->getTokenSource()->getInputStream()->getText(interval);
    return source;
}

void listingGenerator::startLine()
{
    if (doListing)
    {
        extensionsFormat = None;
        extensions.clear();
        label.clear();
        instruction.clear();
        comment.clear();
    }
}

void listingGenerator::endLine()
{
    if (doListing)
    {
        listingFile << right << hex << uppercase << setw(8) << setfill('0') << address;
        switch (extensionsFormat)
        {
            case None:
                listingFile << left << ":   " << setw(21) << setfill(' ') << '-';
                break;
            case Byte:
            {
                ostringstream text;
                text << hex << uppercase;
                size_t end = extensions.size() > 6 ? 6 : extensions.size();
                for (size_t i = 0; i < end; i++)
                {
                    text << ' ' << setw(2) << setfill('0') << extensions[i];
                }
                if (extensions.size() > 6)
                {
                    text << " ...";
                }
                listingFile << left << ":  " << setw(22) << setfill(' ') << text.str();
                break;
            }
            case Word:
            {
                ostringstream text;
                text << hex << uppercase;
                for (auto ext : extensions)
                {
                    text << ' ' << setw(4) << setfill('0') << ext;
                }
                listingFile << left << ":  " << setw(22) << setfill(' ') << text.str();
                break;
            }
            case Long:
            {
                ostringstream text;
                text << hex << uppercase;
                for (size_t i = 0; i < extensions.size(); i += 2)
                {
                    uint32_t value = (uint32_t(extensions[i]) << 16) + extensions[i + 1];
                    text << ' ' << setw(8) << setfill('0') << value;
                }
                listingFile << left << ":  " << setw(22) << setfill(' ') << text.str();
                break;
            }
        }

        listingFile << left << " " << setw(8) << setfill(' ') << label;
        listingFile << left << " " << setw(24) << setfill(' ') << instruction;
        listingFile << left << setw(16) << setfill(' ') << comment;

        listingFile << endl;
    }
}

