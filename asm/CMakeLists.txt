# LAURENT:
# taken from https://github.com/antlr/antlr4/discussions/4418
#
cmake_minimum_required(VERSION 3.14)
project(asm)

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


set(ANTLR4_WITH_STATIC_CRT OFF) # using /MD flag for antlr4_runtime (for Visual C++ compilers only)
set(ANTLR_BUILD_SHARED ON) # in my case I built the shared version
set(ANTLR_TAG 4.13.1) # to specify the GitHub tag for the jar too
set(DISABLE_WARNINGS ON) # Antlr gives a few deprecation warnings for me
set(ANTLR_BUILD_CPP_TESTS OFF) # to avoid creating dist folder

set(ANTLR_EXECUTABLE "./antlr-${ANTLR_TAG}-complete.jar") # a local path for antlr.jar
if(NOT EXISTS "${ANTLR_EXECUTABLE}")
    file(
        DOWNLOAD
        "https://www.antlr.org/download/antlr-${ANTLR_TAG}-complete.jar"
        "${ANTLR_EXECUTABLE}"
    )
endif()
include(FetchContent)
FetchContent_Declare(
    antlr
    GIT_REPOSITORY https://github.com/antlr/antlr4
    GIT_TAG cb850120378d000e97da38afb852aa3c554b81f5
    SOURCE_SUBDIR "runtime/Cpp"
)
FetchContent_MakeAvailable(antlr) # populates antlr_SOURCE_DIR
include(${antlr_SOURCE_DIR}/runtime/Cpp/cmake/FindANTLR.cmake) # populates ANTLR_TARGET

antlr_target(lexer  lexer68000.g4 OUTPUT_DIRECTORY generated LEXER)
antlr_target(parser parser68000.g4 OUTPUT_DIRECTORY generated DEPENDS ${ANTLR_lexer_OUTPUTS} PARSER LISTENER VISITOR)

# add generated grammar to demo binary target
add_library(asm68000lib
    asmparser.cpp
    asmparser.h
    listener.cpp
    listener.h
    visitor.cpp
    visitor.h
    errors.cpp
    errors.h
    ${ANTLR_lexer_CXX_OUTPUTS}
    ${ANTLR_parser_CXX_OUTPUTS}
    )

target_link_libraries(asm68000lib PRIVATE antlr4_static)
target_include_directories(asm68000lib PRIVATE "${antlr_SOURCE_DIR}/runtime/Cpp/runtime/src" ${ANTLR_parser68000_OUTPUT_DIR} generated)
target_compile_features(asm68000lib PRIVATE cxx_std_20)

# if you want to copy them to a different location and haven't specified
# CMAKE_LIBRARY_OUTPUT_DIRECTORY
# CMAKE_ARCHIVE_OUTPUT_DIRECTORY
# CMAKE_RUNTIME_OUTPUT_DIRECTORY
# set(ANTLR_RUNTIME_LIBRARIES $<TARGET_FILE:antlr4_shared> $<TARGET_LINKER_FILE:antlr4_shared>)

# Include sub-projects.
add_subdirectory ("asm68000")
add_subdirectory ("asm68000test")
