project(module-addr-conv)
cmake_minimum_required(VERSION 3.12)
set( EXECUTABLE_OUTPUT_PATH ../bin)
if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
	add_link_options("-mwindows")
endif()

add_executable(mod-addr-conv src/main.cc src/peFile.cc src/resource.rc
	src/addr-list.cc
)

target_link_libraries(mod-addr-conv 
	-lwin32hlp -lstdshit 
	-limagehlp
	
	)
