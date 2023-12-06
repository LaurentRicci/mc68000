# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)
set (CMAKE_CXX_STANDARD 20)
project ("mc68000")

if (WIN32)
    set(Boost_INCLUDE_DIRS "C:/code/lib/boost")
else()
    set(Boost_INCLUDE_DIRS "/usr/include/boost")
endif()
# Include sub-projects.
add_subdirectory ("core")
add_subdirectory("coretest")