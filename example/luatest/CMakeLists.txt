cmake_minimum_required(VERSION 3.5)
project (luatest)

include_directories("/opt/MESA/include")
link_directories("/opt/MESA/lib")
link_libraries(tsglua pthread dl)

add_definitions(-g -W -Wall -O0  -rdynamic)

add_executable(luatest luatest.cpp)
target_link_libraries(luatest pcap tsglua pthread dl)
