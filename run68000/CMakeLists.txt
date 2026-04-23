# CMakeList.txt : CMake project for cputest, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.28)

# Add source to this project's executable.
add_executable (run68000 
	"main.cpp" 
)

target_link_libraries(run68000 PUBLIC run68000lib)