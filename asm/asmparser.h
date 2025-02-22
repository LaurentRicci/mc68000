#pragma once
#include <any>

bool parseFile(const char* filename);
std::any parseText(const char* text);
size_t checkSyntax(const char* text);