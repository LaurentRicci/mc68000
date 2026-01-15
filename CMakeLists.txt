# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.28)
set (CMAKE_CXX_STANDARD 20)
set (CMAKE_BUILD_TYPE debug)
project ("mc68000")

# ------------------------------------------------------------
# Global output directories (executables, libs)
# ------------------------------------------------------------
# For single-config generators (Ninja, Makefiles)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# For multi-config generators (Visual Studio, Xcode)
foreach(OUTPUTCONFIG IN LISTS CMAKE_CONFIGURATION_TYPES)
    string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG_UPPER)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG_UPPER} ${CMAKE_SOURCE_DIR}/bin)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG_UPPER} ${CMAKE_BINARY_DIR}/lib)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG_UPPER} ${CMAKE_BINARY_DIR}/lib)
endforeach()


if (WIN32)
    if (EXISTS C:/code/lib/boost)
        set(Boost_INCLUDE_DIRS "C:/code/lib/boost")
    elseif (EXISTS D:/code/lib/boost)
        set(Boost_INCLUDE_DIRS "D:/code/lib/boost")
    elseif (EXISTS Q:/code/lib/boost)
        set(Boost_INCLUDE_DIRS "Q:/code/lib/boost")
    else()
        message(FATAL_ERROR "*** boost library not found ***")
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
add_subdirectory("run68000")
add_subdirectory("dasmconsole")
