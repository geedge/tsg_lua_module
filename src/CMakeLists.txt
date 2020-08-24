cmake_minimum_required(VERSION 3.5)
SET(CMAKE_CXX_COMPILER /usr/bin/g++)
aux_source_directory(. DIR_LIB_SRCS)
include_directories("/usr/local/include/luajit-2.1/")
link_directories("/usr/local/lib")
link_libraries("libluajit-5.1.a")

add_definitions(-g -W -Wall -O0 -DTSG_LUA_DEBUG)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -shared -fPIC")
add_library(tsglua_static ${DIR_LIB_SRCS})
add_library(tsglua SHARED ${DIR_LIB_SRCS})
set_target_properties(tsglua_static PROPERTIES OUTPUT_NAME "tsglua")
target_link_libraries(tsglua libluajit-5.1.a dl)

