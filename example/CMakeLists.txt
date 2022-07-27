cmake_minimum_required(VERSION 3.5)
project (single_thread)
project (multithread)

link_directories("${CMAKE_BINARY_DIR}/src")
link_directories("${CMAKE_BINARY_DIR}/support/luajit2/src/luajit2/src/")

add_definitions(-g -W -Wall)

add_executable(single_thread single_thread.cpp)
add_executable(multithread multithread.cpp)
if (BUILD_STATIC_LIB)
	add_dependencies(single_thread ${lib_name}_static)
	add_dependencies(multithread ${lib_name}_static)
	target_link_libraries(single_thread ${lib_name}_static pthread)
	target_link_libraries(multithread ${lib_name}_static pthread)
else()
	add_dependencies(single_thread ${lib_name}_shared)
	add_dependencies(multithread ${lib_name}_shared)
	target_link_libraries(single_thread ${lib_name}_shared pthread)
	target_link_libraries(multithread ${lib_name}_shared pthread)
endif()

add_subdirectory(luatest)