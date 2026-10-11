#pragma once
#include <cstdint>
#include <string>

enum class RelocationType
{
    ABSOLUTE_32,
    ABSOLUTE_16,
    ABSOLUTE_8,
    PCREL_16
};

struct Relocation
{
    uint32_t offset;      // where to patch the code
    std::string symbol;   // symbol name
    int32_t addend;       // to handle expression linke jmp symbol+4 (normaly 0)
    RelocationType type;  // ABSOLUTE_32, PCREL_16, etc.

    Relocation() : offset(0), symbol(""), addend(0), type(RelocationType::ABSOLUTE_32) {}
    Relocation(const std::string name) : offset(0), symbol(name), addend(0), type(RelocationType::ABSOLUTE_32) {}
};
inline bool operator==(RelocationType a, RelocationType b)
{
    return static_cast<int>(a) == static_cast<int>(b);
}
inline std::ostream& operator<<(std::ostream& os, RelocationType t)
{
    switch (t) 
    {
    case RelocationType::ABSOLUTE_32: 
        os << "ABSOLUTE_32"; 
        break;
    case RelocationType::ABSOLUTE_16: 
        os << "ABSOLUTE_16"; 
        break;
    case RelocationType::PCREL_16:    
        os << "PCREL_16"; 
        break;
    default:                          
        os << "UNKNOWN"; 
        break;
    }
    return os;
}
