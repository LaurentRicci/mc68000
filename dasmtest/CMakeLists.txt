# CMakeList.txt : CMake project for coretest, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.28)

# Add source to this project's executable.
add_executable (dasmtest "module.cpp" "disasmtest.cpp" "tutorial2.cpp"  "offset.cpp"
	"../core/core.h" "../core/disasm.h" "../core/cpu.h" "../core/statusregister.h")

target_include_directories(dasmtest PUBLIC ${Boost_INCLUDE_DIRS}) 
target_link_libraries(dasmtest PUBLIC core)