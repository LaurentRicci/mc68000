# CMakeList.txt : CMake project for cputest, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Add source to this project's executable.
add_executable (coretest "coretest.cpp" "../core/core.h")

target_include_directories(coretest PUBLIC ${Boost_INCLUDE_DIRS}) 
target_link_libraries(coretest PUBLIC core)