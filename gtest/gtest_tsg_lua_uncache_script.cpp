/*************************************************************************
    > File Name: gtest_tsg_lua_cache_script_file.cpp
    > Author: pxz
    > Created Time: Sun 19 Jul 2020 01:28:20 PM CST
 ************************************************************************/
#include <gtest/gtest.h>
#include "tsg_lua_interface.h"

/* tsg_lua_uncache_script */
TEST(tsg_lua_uncache_script, normal)
{
    tsg_lua_handle L = NULL;

    L = tsg_lua_vm_create();
    const char *script = "./script/return_string_normal.lua";
    int script_id = tsg_lua_cache_script_file(L, script);
    int ret = tsg_lua_uncache_script(L, script_id);
    
    /* check */
    EXPECT_EQ(0, ret);
}

TEST(tsg_lua_uncache_script, lua_vm_isnull)
{
    tsg_lua_handle L = NULL;

    L = tsg_lua_vm_create();
    const char *script = "./script/return_string_normal.lua";
    int script_id = tsg_lua_cache_script_file(L, script);
    L = NULL;
    int ret = tsg_lua_uncache_script(L, script_id);
    
    /* check */
    EXPECT_EQ(ERR_LUAVM_ISNULL, ret);
}

