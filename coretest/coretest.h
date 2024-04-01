#pragma once
#include <cstdint>
#include "../core/cpu.h"

using namespace mc68000;
void verifyExecution(const uint8_t* code, uint32_t size, void (*assertFunctor)(const Cpu& c));
void verifyExecution(const uint8_t* code, uint32_t size, uint32_t baseAddress, void (*assertFunctor)(const Cpu& c));
