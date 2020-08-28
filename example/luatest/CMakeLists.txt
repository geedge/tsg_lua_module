cmake_minimum_required(VERSION 3.5)
project (luatest)

include_directories("/usr/local/include/gperftools")
include_directories("../../include")
link_directories("../../lib")
link_libraries(tsglua pthread dl tcmalloc)

add_definitions(-g -W -Wall -O0  -rdynamic)

add_executable(luatest luatest.cpp)
target_link_libraries(luatest pcap tsglua pthread dl)

