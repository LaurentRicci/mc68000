# CMakeList.txt : CMake project for cputest, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.28)

# Add source to this project's executable.
add_executable (cputest 
	"module.cpp" "addtest.cpp" "andtest.cpp" "bittest.cpp" "comparetest.cpp"
	"controlflowtest.cpp" "cputest.cpp" "divtest.cpp" "eortest.cpp"
	"exceptiontest.cpp" "movetest.cpp" "multest.cpp" "ortest.cpp"   
	"roltest.cpp" "shifttest.cpp" "subtest.cpp" "various.cpp" 
	"verifyexecution.cpp"
	"../core/core.h" "../core/noopcpu.h" "../core/disasm.h" "../core/cpu.h" 
	"../core/statusregister.h" "verifyexecution.h" )

target_include_directories(cputest PUBLIC ${Boost_INCLUDE_DIRS}) 
target_link_libraries(cputest PUBLIC core)
