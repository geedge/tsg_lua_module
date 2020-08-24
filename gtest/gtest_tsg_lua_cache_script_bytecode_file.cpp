/*************************************************************************
    > File Name: gtest_tsg_lua_cache_script_file.cpp
    > Author: pxz
    > Mail: 1500428751@qq.com 
    > Created Time: Sun 19 Jul 2020 01:28:20 PM CST
 ************************************************************************/
#include <gtest/gtest.h>
#include "tsg_lua_interface.h"

/* tsg_lua_cache_script_file */
TEST(tsg_lua_cache_script_bytecode_file, normal)
{
    tsg_lua_handle L = NULL;

    L = tsg_lua_vm_create();
    const char *script = "./script/return_string_normal.lua";
    int script_id = tsg_lua_cache_script_file(L, script);
    
    /* check */
    EXPECT_EQ(1, script_id);
}

TEST(tsg_lua_cache_script_bytecode_file, lua_vm_isnull)
{
    tsg_lua_handle L = NULL;

    const char *script = "./script/return_string_normal.lua";
    int script_id = tsg_lua_cache_script_file(L, script);
    
    /* check */
    EXPECT_EQ(ERR_LUAVM_ISNULL, script_id);
}

TEST(tsg_lua_cache_script_bytecode_file, script_isnull)
{
    tsg_lua_handle L = NULL;

    L = tsg_lua_vm_create();
    const char *script = NULL;
    int script_id = tsg_lua_cache_script_file(L, script);
    
    /* check */
    EXPECT_EQ(ERR_SCRIPT_ISNULL, script_id);
}
