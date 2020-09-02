cmake_minimum_required(VERSION 3.5)
SET(CMAKE_CXX_COMPILER /usr/bin/g++)
aux_source_directory(. DIR_LIB_SRCS)
include_directories("${CMAKE_CURRENT_BINARY_DIR}/../support/install/include/luajit-2.1")

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -shared -fPIC")
SET(CMAKE_CXX_DEBUG_FLAGS "${CMAKE_CXX_DEBUG_FLAGS} -DTSG_LUA_DEBUG")
SET(CMAKE_BUILD_TYPE "Release")
add_library(tsglua_static ${DIR_LIB_SRCS})
add_library(tsglua SHARED ${DIR_LIB_SRCS})
set_target_properties(tsglua_static PROPERTIES OUTPUT_NAME "tsglua")

target_link_libraries(tsglua dl luajit2-static)
#set_target_properties(tsglua  PROPERTIES PREFIX "")

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libtsglua.so DESTINATION ${PROJECT_SOURCE_DIR}/lib COMPONENT PROFILE)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libtsglua.a DESTINATION ${PROJECT_SOURCE_DIR}/lib COMPONENT PROFILE)
install(FILES ${PROJECT_SOURCE_DIR}/src/tsg_lua_interface.h DESTINATION ${PROJECT_SOURCE_DIR}/include COMPONENT PROFILE)

