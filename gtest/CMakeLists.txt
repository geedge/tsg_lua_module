cmake_minimum_required(VERSION 2.6)
project(gtest_tsg_lua)
aux_source_directory(. SRCS)

include_directories("../include")
link_directories("/root/workspace/tsg_lua_0.99/lib")
link_libraries(tsglua gtest gtest_main pthread)

add_definitions(-g -W -Wall)
add_executable(../gtest_tsg_lua ${SRCS})
target_link_libraries(../gtest_tsg_lua tsglua gtest gtest_main pthread)

