cmake_minimum_required(VERSION 3.5)
project (luatest)

include_directories("../../src/")
link_directories("${CMAKE_BINARY_DIR}/src")
#link_directories("${CMAKE_BINARY_DIR}/support/luajit2/src/luajit2/src/")

add_definitions(-g -W -Wall -O0  -rdynamic)

add_executable(luatest luatest.cpp)
if (BUILD_STATIC_LIB)
	add_dependencies(luatest ${lib_name}_static)
	target_link_libraries(luatest ${lib_name}_static pthread dl pcap)
else()
	add_dependencies(luatest ${lib_name}_shared)
	target_link_libraries(luatest ${lib_name}_shared pthread dl pcap)
endif()
