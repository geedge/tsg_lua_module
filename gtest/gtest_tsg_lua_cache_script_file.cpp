/*************************************************************************
    > File Name: gtest_tsg_lua_cache_script_file.cpp
    > Author: pxz
    > Mail: 1500428751@qq.com 
    > Created Time: Sun 19 Jul 2020 01:28:20 PM CST
 ************************************************************************/
#include <gtest/gtest.h>
#include "tsg_lua_interface.h"

/* tsg_lua_cache_script_file */
TEST(tsg_lua_cache_script_file, normal)
{
    tsg_lua_handle L = NULL;

    L = tsg_lua_vm_create();
    const char *script = "./script/return_string_normal.lua";
    int script_id = tsg_lua_cache_script_file(L, script);
    
    /* check */
    EXPECT_EQ(1, script_id);
}

TEST(tsg_lua_cache_script_file, lua_vm_isnull)
{
    tsg_lua_handle L = NULL;

    const char *script = "./script/return_string_normal.lua";
    int script_id = tsg_lua_cache_script_file(L, script);
    
    /* check */
    EXPECT_EQ(ERR_LUAVM_ISNULL, script_id);
}

TEST(tsg_lua_cache_script_file, script_isnull)
{
    tsg_lua_handle L = NULL;

    L = tsg_lua_vm_create();
    const char *script = NULL;
    int script_id = tsg_lua_cache_script_file(L, script);
    
    /* check */
    EXPECT_EQ(ERR_SCRIPT_ISNULL, script_id);
}
#if 0
TEST(tsg_lua_cache_script_file, function_invaild)
{
    tsg_lua_handle L = NULL;

    L = tsg_lua_vm_create();
    const char *script = "./script/function_invaild.lua";
    int script_id = tsg_lua_cache_script_file(L, script);
    
    /* check */
    EXPECT_EQ(ERR_SCRIPT_FUNCTION_INVAILD, script_id);
}
#endif
TEST(tsg_lua_cache_script_file, do_expected)
{
    tsg_lua_handle L = NULL;

    L = tsg_lua_vm_create();
    const char *script = "./script/do_expected.lua";
    int script_id = tsg_lua_cache_script_file(L, script);
    
    /* check */
    EXPECT_EQ(ERR_SCRIPT_DO_EXPECTED, script_id);
}

TEST(tsg_lua_cache_script_file, then_expected)
{
    tsg_lua_handle L = NULL;

    L = tsg_lua_vm_create();
    const char *script = "./script/then_expected.lua";
    int script_id = tsg_lua_cache_script_file(L, script);
    
    /* check */
    EXPECT_EQ(ERR_SCRIPT_THEN_EXPECTED, script_id);
}

TEST(tsg_lua_cache_script_file, end_expected)
{
    tsg_lua_handle L = NULL;

    L = tsg_lua_vm_create();
    const char *script = "./script/end_expected.lua";
    int script_id = tsg_lua_cache_script_file(L, script);
    
    /* check */
    EXPECT_EQ(ERR_SCRIPT_END_EXPECTED, script_id);
}

TEST(tsg_lua_cache_script_file, eof_expected)
{
    tsg_lua_handle L = NULL;

    L = tsg_lua_vm_create();
    const char *script = "./script/eof_expected.lua";
    int script_id = tsg_lua_cache_script_file(L, script);
    
    /* check */
    EXPECT_EQ(ERR_SCRIPT_EOF_EXPECTED, script_id);
}

TEST(tsg_lua_cache_script_file, equal_in_expected)
{
    tsg_lua_handle L = NULL;

    L = tsg_lua_vm_create();
    const char *script = "./script/equal_in_expected.lua";
    int script_id = tsg_lua_cache_script_file(L, script);
    
    /* check */
    EXPECT_EQ(ERR_SCRIPT_EQUAL_IN_EXPECTED, script_id);
}

TEST(tsg_lua_cache_script_file, unexpected_symbol)
{
    tsg_lua_handle L = NULL;

    L = tsg_lua_vm_create();
    const char *script = "./script/unexpected_symbol.lua";
    int script_id = tsg_lua_cache_script_file(L, script);
    
    /* check */
    EXPECT_EQ(ERR_SCRIPT_UNEXPECTED_SYMBOL, script_id);
}

TEST(tsg_lua_cache_script_file, name_expected)
{
    tsg_lua_handle L = NULL;

    L = tsg_lua_vm_create();
    const char *script = "./script/name_expected.lua";
    int ret = tsg_lua_cache_script_file(L, script);
    
    /* check */
    EXPECT_EQ(ERR_SCRIPT_NAME_EXPECTED, ret);
}

TEST(tsg_lua_cache_script_file, right_curves_bracket_expected)
{
    tsg_lua_handle L = NULL;

    L = tsg_lua_vm_create();
    const char *script = "./script/right_curves_bracket_expected.lua";
    int ret = tsg_lua_cache_script_file(L, script);
    
    /* check */
    EXPECT_EQ(ERR_SCRIPT_RIGHT_CURVES_BRACKET_EXPECTED, ret);
}

TEST(tsg_lua_cache_script_file, right_curly_bracket_expected)
{
    tsg_lua_handle L = NULL;

    L = tsg_lua_vm_create();
    const char *script = "./script/right_curly_bracket_expected.lua";
    int ret = tsg_lua_cache_script_file(L, script);
    
    /* check */
    EXPECT_EQ(ERR_SCRIPT_RIGHT_CURLY_BRACKET_EXPECTED, ret);
}
