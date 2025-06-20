#include "listener.h"
#include <iostream>


void listener::enterInstructionSection(parser68000::InstructionSectionContext* ctx)
{
	std::cout << "enter instruction section" << std::endl;
}
void listener::exitInstructionSection(parser68000::InstructionSectionContext* ctx)
{
	std::cout << "exit instruction section" << std::endl;
}


