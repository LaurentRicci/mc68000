# CMakeList.txt : CMake project for cpu, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.31)
cmake_policy(SET CMP0076 NEW)
set (CMAKE_CXX_STANDARD 20)

# Add source to this project's executable.
add_library (core 
	"noopcpu.cpp" "instructions.cpp" "disasm.cpp" "setup.cpp" "cpu_utils.cpp" "disasm_utils.cpp"
	"core.h" "noopcpu.h" "statusregister.h" "instructions.h" "disasm.h" "exceptions.h")
target_sources(core PRIVATE "cpu.cpp" "memory.cpp")
target_sources(core PUBLIC "cpu.h" "memory.h" "statusregister.h" "exceptions.h")

# TODO: Add tests and install targets if needed.