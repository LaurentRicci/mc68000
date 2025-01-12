#include <iostream>
#include "emulator.h"
extern void game();

int main()
{
    Emulator emulator;

    emulator.run();

    game();
}
