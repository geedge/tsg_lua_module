cmake_minimum_required (VERSION 2.8)

set(lib_name tsglua)
project(${lib_name})

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
include(Version)

set(CMAKE_MACOSX_RPATH 0)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

set(CMAKE_INSTALL_PREFIX /opt/MESA)

add_subdirectory(support) 
add_subdirectory(src) 

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/src/libtsglua.so DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/ COMPONENT LIBRARY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/src/libtsglua.a DESTINATION ${CMAKE_INSTALL_PREFIX}/lib COMPONENT LIBRARY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/src/tsg_lua_interface.h DESTINATION ${CMAKE_INSTALL_PREFIX}/include/MESA COMPONENT HEADER)

include(Package)