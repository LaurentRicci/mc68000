# CMakeLists.txt : CMake project for run68000lib 

add_library (run68000lib
	"emulator.cpp" "emulator.h"
	"simplebios.cpp" "simplebios.h"
	"ataribios.cpp" "atarixbios.cpp" "atarigemdos.cpp" "ataribios.h"
	"osbios.cpp" "osbios.h" "biosparameterblock.h" "biosparameterblock.cpp"
	"ibios.h" "trapargs.h"
	)
target_include_directories(run68000lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(run68000lib PUBLIC core) 
