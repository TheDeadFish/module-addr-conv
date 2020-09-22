project(win32-res-lib)
cmake_minimum_required(VERSION 3.12)
set( EXECUTABLE_OUTPUT_PATH ../bin)
add_link_options(-s -Wl,-gc-sections -mwindows)

add_executable(mod-addr-conv src/main.cc src/peFile.cc src/resource.rc)
target_link_libraries(mod-addr-conv 
	-lwin32hlp -lstdshit 
	-limagehlp
	
	)
