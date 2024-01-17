# CMakeList.txt : CMake project for cputest, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Add source to this project's executable.
add_executable (coretest "noopcputest.cpp" "asmtest.cpp" "cputest.cpp" "addtest.cpp" "andtest.cpp" "ortest.cpp" "../core/core.h" "../core/noopcpu.h" "../core/disasm.h" "../core/cpu.h")

target_include_directories(coretest PUBLIC ${Boost_INCLUDE_DIRS}) 
target_link_libraries(coretest PUBLIC core)