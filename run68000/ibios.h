#pragma once
#include "../core/cpu.h"

namespace mc68000
{
	struct IBios
	{
		virtual void registerTrapHandlers(Cpu* cpu) = 0;
	};
}
