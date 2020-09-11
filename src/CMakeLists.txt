cmake_minimum_required(VERSION 3.5)
SET(CMAKE_CXX_COMPILER /usr/bin/g++)
aux_source_directory(. DIR_LIB_SRCS)
include_directories("${LUAJIT_INSTALL_PREFIX}/include/luajit-2.1")

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -shared -fPIC")
SET(CMAKE_CXX_DEBUG_FLAGS "${CMAKE_CXX_DEBUG_FLAGS} -DTSG_LUA_DEBUG")
SET(CMAKE_BUILD_TYPE "Release")

add_library(${lib_name}_shared SHARED ${DIR_LIB_SRCS})
target_link_libraries(${lib_name}_shared dl luajit2-static)
set_target_properties(${lib_name}_shared PROPERTIES OUTPUT_NAME ${lib_name})
#set_target_properties(${lib_name}  PROPERTIES PREFIX "")
#set_target_properties(${lib_name}_shared PROPERTIES OUTPUT_NAME ${lib_name})

#add_library(${lib_name}_static ${DIR_LIB_SRCS})
#add_dependencies(${lib_name}_static luajit2-static)
#set_target_properties(${lib_name}_static  PROPERTIES PREFIX "")


install(TARGETS ${lib_name}_shared LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib COMPONENT LIBRARIES)
#install(TARGETS ${lib_name}_static  LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib COMPONENT LIBRARY)
