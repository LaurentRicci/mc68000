# CMakeList.txt : CMake project for run68000test.
#
cmake_minimum_required (VERSION 3.28)

# Add source to this project's executable.
add_executable (run68000test "module.cpp" "ataribiostest.cpp"
	"../run68000/ataribios.cpp"
	"../run68000/atarixbios.cpp"
	"../run68000/atarigemdos.cpp"
	"../core/core.h" "../core/cpu.h" )

target_include_directories(run68000test PUBLIC ${Boost_INCLUDE_DIRS}) 
target_link_libraries(run68000test PUBLIC core)