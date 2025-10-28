// commenter.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <string>
#include <regex>

int main(int argc, char** argv) 
{
    std::ifstream input(argv[1]);
    std::ofstream output(argv[2]);

    if (!input || !output) {
        std::cerr << "Error opening files.\n";
        return 1;
    }



    std::string line;
    // Regex:
    // ^(\s*)(\S.*?\S)(\s{4,})(\S.*)$
    // (\s*)       -> leading spaces
    // (\S.*?\S)   -> code part (label + instruction)
    // (\s{4,})    -> big gap before comment
    // (\S.*)$     -> comment text
    std::regex pattern(R"(^(\s*)(\S.*?\S)(\s{6,})(\S.*)$)");

    while (std::getline(input, line)) {
        std::smatch match;
        if (std::regex_match(line, match, pattern)) {
            std::string leading = match[1].str();
            std::string code = match[2].str();
            std::string gap = match[3].str();
            std::string comment = match[4].str();
            // Insert semicolon before comment
            line = leading + code + gap + ";" + comment;
        }
        output << line << "\n";
    }

    std::cout << "Processing complete. Output written to " << argv[2] << "\n";
    return 0;
}