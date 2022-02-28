cmake_minimum_required(VERSION 3.5)
SET(CMAKE_CXX_COMPILER /usr/bin/g++)
aux_source_directory(. DIR_LIB_SRCS)
include_directories("${LUAJIT_INSTALL_PREFIX}/include/luajit-2.1")

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -shared -fPIC")
SET(CMAKE_CXX_DEBUG_FLAGS "${CMAKE_CXX_DEBUG_FLAGS} -DTSG_LUA_DEBUG")
SET(CMAKE_BUILD_TYPE "Release")

option(TSG_LUA_DEBUG "If enabled, the SDK will be print debuginfo." OFF)

if (TSG_LUA_DEBUG)
    add_definitions(-DTSG_LUA_DEBUG)
endif()


add_library(${lib_name}_shared SHARED ${DIR_LIB_SRCS})
target_link_libraries(${lib_name}_shared dl luajit2-static)
set_target_properties(${lib_name}_shared PROPERTIES OUTPUT_NAME ${lib_name})

install(TARGETS ${lib_name}_shared LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib COMPONENT LIBRARIES)
install(FILES ${CMAKE_SOURCE_DIR}/src/tsg_lua_interface.h DESTINATION ${CMAKE_INSTALL_PREFIX}/include/MESA COMPONENT HEADER)
install(PROGRAMS ${CMAKE_SOURCE_DIR}/example/checktools/checktools.lua DESTINATION ${LUAJIT_INSTALL_PREFIX}/bin COMPONENT LIBRARIES)
