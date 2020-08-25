cmake_minimum_required(VERSION 3.5)
SET(CMAKE_CXX_COMPILER /usr/bin/g++)
project (c_handle_weixinnum)

add_definitions(-g -W -Wall -O2)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -shared -fPIC")
add_library(${PROJECT_NAME}_static handle_weixinnum.cpp)
add_library(${PROJECT_NAME} SHARED handle_weixinnum.cpp)
set_target_properties(${PROJECT_NAME}_static PROPERTIES OUTPUT_NAME ${PROJECT_NAME})

add_library(handle_fasta_find_static handle_fasta_find.cpp)
add_library(handle_fasta_find SHARED handle_fasta_find.cpp)
set_target_properties(handle_fasta_find_static PROPERTIES OUTPUT_NAME "handle_fasta_find")

add_library(handle_fasta_count_static handle_fasta_count.cpp)
add_library(handle_fasta_count SHARED handle_fasta_count.cpp)
set_target_properties(handle_fasta_count_static PROPERTIES OUTPUT_NAME "handle_fasta_count")

