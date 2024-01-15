# CMakeList.txt : CMake project for cputest, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Add source to this project's executable.
add_executable (simple "main.cpp" "emulator.h" "emulator.cpp" "../core/cpu.h")

target_include_directories(simple PUBLIC ${Boost_INCLUDE_DIRS}) 
target_link_libraries(simple PUBLIC core)