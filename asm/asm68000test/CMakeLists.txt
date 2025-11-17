# CMakeList.txt : CMake project for cputest, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Add source to this project's executable.
add_executable (asm68000test 
	"instructionTest.cpp"  "module.cpp" "addressingModeTest.cpp" "directiveTest.cpp" "parserTest.cpp"
	"../asmparser.h" "util.h")

target_include_directories(asm68000test PUBLIC ${Boost_INCLUDE_DIRS}) 
target_include_directories(asm68000test PRIVATE "..")
target_compile_features(asm68000test PRIVATE cxx_std_20)
target_link_libraries(asm68000test PUBLIC asm68000lib)
