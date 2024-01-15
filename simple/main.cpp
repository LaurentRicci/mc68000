#include <iostream>
#include "emulator.h"

int main()
{
    Emulator emulator;

    std::cout << "--- starting ---\n";
    emulator.run();
    std::cout << "--- ending ---\n";
}
