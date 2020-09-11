cmake_minimum_required (VERSION 2.8)

set(lib_name tsglua)
project(${lib_name})

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
include(Version)

set(CMAKE_MACOSX_RPATH 0)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

set(CMAKE_INSTALL_PREFIX /opt/MESA)
set(LUAJIT_INSTALL_PREFIX /usr/local)

add_subdirectory(support) 
add_subdirectory(src) 

install(FILES ./src/tsg_lua_interface.h DESTINATION ${CMAKE_INSTALL_PREFIX}/include/MESA COMPONENT HEADER)
install(PROGRAMS ./example/checktools/checktools.lua DESTINATION ${LUAJIT_INSTALL_PREFIX}/bin)

include(Package)
