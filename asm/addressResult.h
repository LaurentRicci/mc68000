#pragma once
#include "relocation.h"

struct AddressResult
{
    bool isResolved;       // true if the address is local (aka coming from a label in the same file), false if external (aka coming from an imported symbol)
    uint32_t value;        // value if local
    Relocation relocation; // relocation if external
};
