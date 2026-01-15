# CMakeList.txt : CMake project for cputest, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.28)

# Add source to this project's executable.
add_executable (run68000 "main.cpp" "emulator.h" "emulator.cpp" "ibios.h" "simplebios.h" "simplebios.cpp" "../core/cpu.h")

target_include_directories(run68000 PUBLIC ${Boost_INCLUDE_DIRS}) 
target_link_libraries(run68000 PUBLIC core)