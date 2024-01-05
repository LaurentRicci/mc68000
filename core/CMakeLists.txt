# CMakeList.txt : CMake project for cpu, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)
cmake_policy(SET CMP0076 NEW)
set (CMAKE_CXX_STANDARD 20)

# Add source to this project's executable.
add_library (core "noopcpu.cpp" "core.h" "noopcpu.h" "instructions.cpp" "instructions.h" "disasm.h" "disasm.cpp" "setup.cpp" "cpu_utils.cpp")
target_sources(core PRIVATE "cpu.cpp" "memory.cpp")
target_sources(core PUBLIC "cpu.h" "memory.h")

# TODO: Add tests and install targets if needed.