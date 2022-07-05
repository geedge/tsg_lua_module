cmake_minimum_required(VERSION 3.5)
project (luatest)

include_directories("../../src/")
link_directories("${CMAKE_BINARY_DIR}/src")
link_libraries(tsglua pthread dl)

add_definitions(-g -W -Wall -O0  -rdynamic)

add_executable(luatest luatest.cpp)
add_dependencies(luatest ${lib_name}_shared)
target_link_libraries(luatest pcap tsglua pthread dl)
