cmake_minimum_required(VERSION 2.6)
project(gtest_tsg_lua)
aux_source_directory(. SRCS)

include_directories("../src")
include_directories("${CMAKE_BINARY_DIR}/support/GoogleTest/include/")
#link_directories("/opt/MESA/lib")
link_directories("${CMAKE_BINARY_DIR}/src/")
link_directories("${CMAKE_BINARY_DIR}/support/GoogleTest/lib/")
link_directories("${CMAKE_BINARY_DIR}/support/luajit2/src/luajit2/src/")

#link_libraries(libluajit.a tsglua gtest gtest_main pthread)

add_definitions(-g -W -Wall)
add_executable(gtest_tsg_lua ${SRCS})
if (BUILD_STATIC_LIB)
	add_dependencies(gtest_tsg_lua ${lib_name}_static gtest)
	target_link_libraries(gtest_tsg_lua ${lib_name}_static gtest gtest_main pthread)
else()
	add_dependencies(gtest_tsg_lua ${lib_name}_shared gtest)
	target_link_libraries(gtest_tsg_lua ${lib_name}_shared gtest gtest_main pthread)
endif()
#target_link_libraries(gtest_tsg_lua tsglua gtest gtest_main pthread)

add_test(NAME SOFT_LINK COMMAND sh -c "ln -sf ${CMAKE_CURRENT_SOURCE_DIR}/script" "${CMAKE_CURRENT_BINARY_DIR}/script")
add_test(NAME LUA_TEST COMMAND gtest_tsg_lua)

