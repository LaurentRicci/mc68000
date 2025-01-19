# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)
set (CMAKE_CXX_STANDARD 20)
set (CMAKE_BUILD_TYPE debug)
project ("mc68000")

if (WIN32)
    if (EXISTS C:/code/lib/boost)
        set(Boost_INCLUDE_DIRS "C:/code/lib/boost")
    elseif (EXISTS Q:/code/lib/boost)
        set(Boost_INCLUDE_DIRS "Q:/code/lib/boost")
    else()
        message(STATUS "boost library not found")
    endif()
else()
    set(Boost_INCLUDE_DIRS "/usr/include/boost")
endif()
message(STATUS "boost: ${Boost_INCLUDE_DIRS} ")
# Include sub-projects.
add_subdirectory ("core")
add_subdirectory("coretest")
add_subdirectory("dasmtest")
add_subdirectory("cputest")
add_subdirectory("simple")
