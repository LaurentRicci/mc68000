#pragma once

#include "antlr4-runtime.h"
#include <vector>
#include <string>

using namespace antlr4;

class ErrorListener : public antlr4::BaseErrorListener {
public:
    struct Error {
        int line;
        int column;
        std::string message;
    };

    std::vector<Error> errors;

    void syntaxError(antlr4::Recognizer* recognizer,
        Token* offendingSymbol,
        size_t line,
        size_t charPositionInLine,
        const std::string& msg,
        std::exception_ptr e) override
    {
        errors.push_back({ (int)line, (int)charPositionInLine, msg });
    }

    bool hasErrors() const {
        return !errors.empty();
    }

    int count() const {
        return (int)errors.size();
    }
};
