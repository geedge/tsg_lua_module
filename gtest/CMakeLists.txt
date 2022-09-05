cmake_minimum_required(VERSION 2.6)
project(gtest_elua)
# aux_source_directory(. SRCS)
file(GLOB SRCS
	"gtest_lua_remove_cmd.cpp"
	"gtest_lua_timeout.cpp"
	"gtest_tsg_destroy_lua.cpp"
	"gtest_tsg_lua_cache_exec.cpp"
	"gtest_tsg_lua_cache_script_bytecode_file.cpp"
	"gtest_tsg_lua_cache_script_file.cpp"
	"gtest_tsg_lua_cache_script.cpp"
	"gtest_tsg_lua_register_function.cpp"
	"gtest_tsg_lua_uncache_script.cpp"
	"gtest_tsg_lua_vm_create.cpp"
	"gtest_tsg_lua_exec_with_context.cpp"
	"gtest_get_weixinnum.cpp"
)


include_directories("../src")
include_directories("${CMAKE_BINARY_DIR}/support/GoogleTest/include/")
link_directories("${CMAKE_BINARY_DIR}/src/")
link_directories("${CMAKE_BINARY_DIR}/support/GoogleTest/lib/")
link_directories("${CMAKE_BINARY_DIR}/support/luajit2/src/luajit2/src/")

#link_libraries(libluajit.a tsglua gtest gtest_main pthread)

add_definitions(-g -W -Wall)
add_executable(gtest_elua ${SRCS})
if (BUILD_STATIC_LIB)
	add_dependencies(gtest_elua ${lib_name}_static gtest)
	target_link_libraries(gtest_elua ${lib_name}_static gtest gtest_main pthread)
else()
	add_dependencies(gtest_elua ${lib_name}_shared gtest)
	target_link_libraries(gtest_elua ${lib_name}_shared gtest gtest_main pthread)
endif()
#target_link_libraries(gtest_elua tsglua gtest gtest_main pthread)

add_test(NAME SOFT_LINK COMMAND sh -c "ln -sf ${CMAKE_CURRENT_SOURCE_DIR}/script" "${CMAKE_CURRENT_BINARY_DIR}/script")
add_test(NAME LUA_TEST COMMAND gtest_elua)

