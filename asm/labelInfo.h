#pragma once
#include <cstdint>
struct labelInfo
{
    labelInfo() : offset(0), defined(false), imported(false), exported(false) {}
    labelInfo(uint32_t address) : offset(address), defined(true), imported(false), exported(false) {}


    uint32_t offset;       // The address of the label
    bool defined;           // True if the label is defined in this module
    bool imported;          // True if the label is imported from another module
    bool exported;          // True if the label is exported to other modules

};
