cmake_minimum_required(VERSION 3.5)
project (single_thread)
project (multithread)

link_directories("${CMAKE_BINARY_DIR}/src")
link_libraries(tsglua pthread)

add_definitions(-g -W -Wall)

add_executable(single_thread single_thread.cpp)
add_executable(multithread multithread.cpp)
add_dependencies(single_thread ${lib_name}_shared)
add_dependencies(multithread ${lib_name}_shared)

add_subdirectory(luatest)