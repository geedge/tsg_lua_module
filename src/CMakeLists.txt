cmake_minimum_required(VERSION 3.5)
SET(CMAKE_CXX_COMPILER /usr/bin/g++)
aux_source_directory(. DIR_LIB_SRCS)
include_directories("../3rd/include/" "/usr/local/include/gperftools")
link_directories("../3rd/lib/")
link_libraries("libluajit.a")

add_definitions(-g -W -Wall -O0 -DTSG_LUA_DEBUG)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -shared -fPIC")
add_library(tsglua_static ${DIR_LIB_SRCS})
add_library(tsglua SHARED ${DIR_LIB_SRCS})
set_target_properties(tsglua_static PROPERTIES OUTPUT_NAME "tsglua")
target_link_libraries(tsglua libluajit.a dl)

