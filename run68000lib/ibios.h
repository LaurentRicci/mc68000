#pragma once
#include "../core/cpu.h"

namespace mc68000
{
	struct IBios
	{
        virtual void setup() = 0;
		virtual void registerTrapHandlers(Cpu* cpu) = 0;
        virtual ~IBios() = default;
	};
}
