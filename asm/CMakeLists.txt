# LAURENT:
# taken from https://github.com/antlr/antlr4/discussions/4418
#
cmake_minimum_required(VERSION 3.14)
project(asm)

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
    GIT_TAG ${ANTLR_TAG}
    SOURCE_SUBDIR "runtime/Cpp"
)
FetchContent_MakeAvailable(antlr) # populates antlr_SOURCE_DIR
include(${antlr_SOURCE_DIR}/runtime/Cpp/cmake/FindANTLR.cmake) # populates ANTLR_TARGET

#antlr_target(SampleGrammarLexer TLexer.g4 LEXER PACKAGE antlrcpptest)
antlr_target(asm68000 asm68000.g4 LEXER PARSER LISTENER VISITOR antlrcpptest)

# add generated grammar to demo binary target
add_executable(asm68000 main.cpp ${ANTLR_asm68000_CXX_OUTPUTS})

target_link_libraries(asm68000 PRIVATE antlr4_static)
target_include_directories(asm68000 PRIVATE "${antlr_SOURCE_DIR}/runtime/Cpp/runtime/src" ${ANTLR_asm68000_OUTPUT_DIR})
target_compile_features(asm68000 PRIVATE cxx_std_20)

# if you want to copy them to a different location and haven't specified
# CMAKE_LIBRARY_OUTPUT_DIRECTORY
# CMAKE_ARCHIVE_OUTPUT_DIRECTORY
# CMAKE_RUNTIME_OUTPUT_DIRECTORY
# set(ANTLR_RUNTIME_LIBRARIES $<TARGET_FILE:antlr4_shared> $<TARGET_LINKER_FILE:antlr4_shared>)