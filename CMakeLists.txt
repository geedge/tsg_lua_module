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

find_program(CMAKE_CXX_CPPCHECK NAMES cppcheck)
if (CMAKE_CXX_CPPCHECK)
    list(
        APPEND CMAKE_CXX_CPPCHECK 
            "--enable=all"
            "--error-exitcode=1"
            "--suppress=unusedFunction"
            "--suppress=missingInclude"
            "--suppress=uselessAssignmentPtrArg"
            "--suppress=unmatchedSuppression"
    )
    set(CMAKE_C_CPPCHECK ${CMAKE_CXX_CPPCHECK})
else()
  message(FATAL_ERROR "Could not find the program cppcheck.")
endif()

add_subdirectory(support) 
add_subdirectory(src) 

enable_testing()
add_subdirectory(gtest)

install(FILES ./src/tsg_lua_interface.h DESTINATION ${CMAKE_INSTALL_PREFIX}/include/MESA COMPONENT HEADER)
install(PROGRAMS ./example/checktools/checktools.lua DESTINATION ${LUAJIT_INSTALL_PREFIX}/bin)

include(Package)
