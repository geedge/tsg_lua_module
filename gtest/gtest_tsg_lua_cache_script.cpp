/*************************************************************************
    > File Name: gtest_tsg_lua_cache_script.cpp
    > Author: pxz
    > Created Time: Sun 19 Jul 2020 01:28:20 PM CST
 ************************************************************************/
#include <gtest/gtest.h>
#include "tsg_lua_interface.h"

static size_t script_len;
static char buffer[4096];
static char *get_script_buffer(const char *script)
{
    FILE *fp = fopen(script, "r");
    if (fp)
    {
        int num = fread(buffer, 1, 4096, fp);
        if (num != 0)
        {
            buffer[num] = 0;
            script_len = num - 1;
            return buffer;
        }
    }
    return NULL;
}
/* tsg_lua_cache_script */
TEST(tsg_lua_cache_script, normal)
{
    tsg_lua_handle L = NULL;

    L = tsg_lua_vm_create();
    const char *script = get_script_buffer("./script/return_string_normal.lua");
    int script_id = tsg_lua_cache_script(L, script, script_len);
    
    /* check */
    EXPECT_EQ(1, script_id);
}

TEST(tsg_lua_cache_script, lua_vm_isnull)
{
    tsg_lua_handle L = NULL;

    const char *script = get_script_buffer("./script/return_string_normal.lua");
    int script_id = tsg_lua_cache_script(L, script, script_len);
    
    /* check */
    EXPECT_EQ(ERR_LUAVM_ISNULL, script_id);
}

TEST(tsg_lua_cache_script, script_isnull)
{
    tsg_lua_handle L = NULL;

    L = tsg_lua_vm_create();
    const char *script = NULL;
    int script_id = tsg_lua_cache_script(L, script, script_len);
    
    /* check */
    EXPECT_EQ(ERR_SCRIPT_ISNULL, script_id);
}
TEST(tsg_lua_cache_script, do_expected)
{
    tsg_lua_handle L = NULL;

    L = tsg_lua_vm_create();
    const char *script = get_script_buffer("./script/do_expected.lua");
    int script_id = tsg_lua_cache_script(L, script, script_len);
    
    /* check */
    EXPECT_EQ(ERR_SCRIPT_DO_EXPECTED, script_id);
}

TEST(tsg_lua_cache_script, then_expected)
{
    tsg_lua_handle L = NULL;

    L = tsg_lua_vm_create();
    const char *script = get_script_buffer("./script/then_expected.lua");
    int script_id = tsg_lua_cache_script(L, script, script_len);
    
    /* check */
    EXPECT_EQ(ERR_SCRIPT_THEN_EXPECTED, script_id);
}

TEST(tsg_lua_cache_script, end_expected)
{
    tsg_lua_handle L = NULL;

    L = tsg_lua_vm_create();
    const char *script = get_script_buffer("./script/end_expected.lua");
    int script_id = tsg_lua_cache_script(L, script, script_len);
    
    /* check */
    EXPECT_EQ(ERR_SCRIPT_END_EXPECTED, script_id);
}

TEST(tsg_lua_cache_script, eof_expected)
{
    tsg_lua_handle L = NULL;

    L = tsg_lua_vm_create();
    const char *script = get_script_buffer("./script/eof_expected.lua");
    int script_id = tsg_lua_cache_script(L, script, script_len);
    
    /* check */
    EXPECT_EQ(ERR_SCRIPT_EOF_EXPECTED, script_id);
}

TEST(tsg_lua_cache_script, equal_in_expected)
{
    tsg_lua_handle L = NULL;

    L = tsg_lua_vm_create();
    const char *script = get_script_buffer("./script/equal_in_expected.lua");
    int script_id = tsg_lua_cache_script(L, script, script_len);
    
    /* check */
    EXPECT_EQ(ERR_SCRIPT_EQUAL_IN_EXPECTED, script_id);
}

TEST(tsg_lua_cache_script, unexpected_symbol)
{
    tsg_lua_handle L = NULL;

    L = tsg_lua_vm_create();
    const char *script = get_script_buffer("./script/unexpected_symbol.lua");
    int script_id = tsg_lua_cache_script(L, script, script_len);
    
    /* check */
    EXPECT_EQ(ERR_SCRIPT_UNEXPECTED_SYMBOL, script_id);
}

TEST(tsg_lua_cache_script, name_expected)
{
    tsg_lua_handle L = NULL;

    L = tsg_lua_vm_create();
    const char *script = get_script_buffer("./script/name_expected.lua");
    int ret = tsg_lua_cache_script(L, script, script_len);
    
    /* check */
    EXPECT_EQ(ERR_SCRIPT_NAME_EXPECTED, ret);
}

TEST(tsg_lua_cache_script, right_curves_bracket_expected)
{
    tsg_lua_handle L = NULL;

    L = tsg_lua_vm_create();
    const char *script = get_script_buffer("./script/right_curves_bracket_expected.lua");
    int ret = tsg_lua_cache_script(L, script, script_len);
    
    /* check */
    EXPECT_EQ(ERR_SCRIPT_RIGHT_CURVES_BRACKET_EXPECTED, ret);
}

TEST(tsg_lua_cache_script, right_curly_bracket_expected)
{
    tsg_lua_handle L = NULL;

    L = tsg_lua_vm_create();
    const char *script = get_script_buffer("./script/right_curly_bracket_expected.lua");
    int ret = tsg_lua_cache_script(L, script, script_len);
    
    /* check */
    EXPECT_EQ(ERR_SCRIPT_RIGHT_CURLY_BRACKET_EXPECTED, ret);
}
