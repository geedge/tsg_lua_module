cmake_minimum_required(VERSION 3.5)
project (single_thread)
project (multithread)

link_directories("../lib")
link_libraries(tsglua pthread)

add_definitions(-g -W -Wall)

add_executable(single_thread single_thread.cpp)
add_executable(multithread multithread.cpp)
target_link_libraries(single_thread tsglua)
target_link_libraries(multithread tsglua)

