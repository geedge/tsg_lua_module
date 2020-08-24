/*************************************************************************
    > File Name: gtest_tsg_lua_exec.cpp
    > Author: pxz
    > Mail: 1500428751@qq.com 
    > Created Time: Sun 19 Jul 2020 01:28:20 PM CST
 ************************************************************************/
#include <gtest/gtest.h>
#include "tsg_lua_interface.h"
static const char *data = "hello world";

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
            return buffer;
        }
    }
    return NULL;
}

/* tsg_lua_exec */
TEST(tsg_lua_exec, normal_return_string)
{
    tsg_lua_handle L = NULL;
    char out[255];
    size_t out_len = 0;
    size_t out_type = 0;

    L = tsg_lua_vm_create();
    memset(out, 0, 255);
    const char *script = get_script_buffer("./script/return_string_normal.lua");
    int ret = tsg_lua_exec(L, script, strlen(script), data, strlen(data), out, &out_len, &out_type);
    
    /* check */
    EXPECT_EQ(0, ret);
    EXPECT_EQ(11, out_len);
    EXPECT_EQ(STRING, out_type);
    EXPECT_STREQ("hello world", out);
}

TEST(tsg_lua_exec, normal_return_boolean)
{
    tsg_lua_handle L = NULL;
    char out[255];
    size_t out_len = 0;
    size_t out_type = 0;
    L = tsg_lua_vm_create();
    const char *script = get_script_buffer("./script/return_boolean_normal.lua");
    memset(out, 0, 255);
    int ret = tsg_lua_exec(L, script, strlen(script), data, strlen(data), out, &out_len, &out_type);
    
    /* check */
    EXPECT_EQ(0, ret);
    EXPECT_EQ(1, out_len);
    EXPECT_EQ(BOOLEAN, out_type);
    EXPECT_EQ(true, out[0]);
}

TEST(tsg_lua_exec, normal_return_integer)
{
    tsg_lua_handle L = NULL;
    char out[255];
    size_t out_len = 0;
    size_t out_type = 0;
    L = tsg_lua_vm_create();
    const char *script = get_script_buffer("./script/return_integer_normal.lua");
    memset(out, 0, 255);
    int ret = tsg_lua_exec(L, script, strlen(script), data, strlen(data), out, &out_len, &out_type);
    
    /* check */
    EXPECT_EQ(0, ret);
    EXPECT_EQ(8, out_len);
    EXPECT_EQ(INTEGER, out_type);
    EXPECT_EQ(7, *(long int *)out);
}

TEST(tsg_lua_exec, lua_vm_isnull)
{
    tsg_lua_handle L = NULL;
    char out[255];
    size_t out_len = 0;
    size_t out_type = 0;
    
    const char *script = get_script_buffer("./script/return_string_normal.lua");
    memset(out, 0, sizeof(out));
    int ret = tsg_lua_exec(L, script, strlen(script), data, strlen(data), out, &out_len, &out_type);
    
    /* check */
    EXPECT_EQ(ERR_LUAVM_ISNULL, ret);
    EXPECT_EQ(0, out_len);
    EXPECT_EQ(0, out_type);
    EXPECT_STREQ("", out);
}

TEST(tsg_lua_exec, script_isnull)
{
    tsg_lua_handle L = NULL;
    char out[255];
    size_t out_len = 0;
    size_t out_type = 0;
    L = tsg_lua_vm_create();
    const char *script = NULL;
    memset(out, 0, 255);
    int ret = tsg_lua_exec(L, script, 0, data, strlen(data), out, &out_len, &out_type);
    
    /* check */
    EXPECT_EQ(ERR_SCRIPT_ISNULL, ret);
    EXPECT_EQ(0, out_len);
    EXPECT_EQ(0, out_type);
    EXPECT_STREQ("", out);
}

TEST(tsg_lua_exec, input_isnull)
{
    tsg_lua_handle L = NULL;
    char out[255];
    size_t out_len = 0;
    size_t out_type = 0;
    char *input = NULL;

    L = tsg_lua_vm_create();
    const char *script = get_script_buffer("./script/return_string_normal.lua");
    memset(out, 0, 255);
    int ret = tsg_lua_exec(L, script, strlen(script), input, 0, out, &out_len, &out_type);
    
    /* check */
    EXPECT_EQ(ERR_INPUT_ISNULL, ret);
    EXPECT_EQ(0, out_len);
    EXPECT_EQ(0, out_type);
    EXPECT_STREQ("", out);
}

TEST(tsg_lua_exec, in_len_invaild)
{
    tsg_lua_handle L = NULL;
    char out[255];
    size_t out_len = 0;
    size_t out_type = 0;

    L = tsg_lua_vm_create();
    const char *script = get_script_buffer("./script/return_string_normal.lua");
    memset(out, 0, 255);
    int ret = tsg_lua_exec(L, script, strlen(script), data, 0, out, &out_len, &out_type);
    
    /* check */
    EXPECT_EQ(ERR_IN_LEN_INVAILD, ret);
    EXPECT_EQ(0, out_len);
    EXPECT_EQ(0, out_type);
    EXPECT_STREQ("", out);
}

TEST(tsg_lua_exec, end_expected)
{
    tsg_lua_handle L = NULL;
    char out[255];
    size_t out_len = 0;
    size_t out_type = 0;

    L = tsg_lua_vm_create();
    const char *script = get_script_buffer("./script/end_expected.lua");
    memset(out, 0, 255);
    int ret = tsg_lua_exec(L, script, strlen(script), data, strlen(data), out, &out_len, &out_type);
    
    /* check */
    EXPECT_EQ(ERR_SCRIPT_END_EXPECTED, ret);
    EXPECT_EQ(0, out_len);
    EXPECT_EQ(0, out_type);
    EXPECT_STREQ("", out);
}

TEST(tsg_lua_exec, then_expected)
{
    tsg_lua_handle L = NULL;
    char out[255];
    size_t out_len = 0;
    size_t out_type = 0;

    L = tsg_lua_vm_create();
    const char *script = get_script_buffer("./script/then_expected.lua");
    memset(out, 0, 255);
    int ret = tsg_lua_exec(L, script, strlen(script), data, strlen(data), out, &out_len, &out_type);
    
    /* check */
    EXPECT_EQ(ERR_SCRIPT_THEN_EXPECTED, ret);
    EXPECT_EQ(0, out_len);
    EXPECT_EQ(0, out_type);
    EXPECT_STREQ("", out);
}

TEST(tsg_lua_exec, do_expected)
{
    tsg_lua_handle L = NULL;
    char out[255];
    size_t out_len = 0;
    size_t out_type = 0;

    L = tsg_lua_vm_create();
    const char *script = get_script_buffer("./script/do_expected.lua");
    memset(out, 0, 255);
    int ret = tsg_lua_exec(L, script, strlen(script), data, strlen(data), out, &out_len, &out_type);
    
    /* check */
    EXPECT_EQ(ERR_SCRIPT_DO_EXPECTED, ret);
    EXPECT_EQ(0, out_len);
    EXPECT_EQ(0, out_type);
    EXPECT_STREQ("", out);
}

TEST(tsg_lua_exec, eof_expected)
{
    tsg_lua_handle L = NULL;
    char out[255];
    size_t out_len = 0;
    size_t out_type = 0;

    L = tsg_lua_vm_create();
    const char *script = get_script_buffer("./script/eof_expected.lua");
    memset(out, 0, 255);
    int ret = tsg_lua_exec(L, script, strlen(script), data, strlen(data), out, &out_len, &out_type);
    
    /* check */
    EXPECT_EQ(ERR_SCRIPT_EOF_EXPECTED, ret);
    EXPECT_EQ(0, out_len);
    EXPECT_EQ(0, out_type);
    EXPECT_STREQ("", out);
}

TEST(tsg_lua_exec, equal_in_expected)
{
    tsg_lua_handle L = NULL;
    char out[255];
    size_t out_len = 0;
    size_t out_type = 0;

    L = tsg_lua_vm_create();
    const char *script = get_script_buffer("./script/equal_in_expected.lua");
    memset(out, 0, 255);
    int ret = tsg_lua_exec(L, script, strlen(script), data, strlen(data), out, &out_len, &out_type);
    
    /* check */
    EXPECT_EQ(ERR_SCRIPT_EQUAL_IN_EXPECTED, ret);
    EXPECT_EQ(0, out_len);
    EXPECT_EQ(0, out_type);
    EXPECT_STREQ("", out);
}

TEST(tsg_lua_exec, unexpected_symbol)
{
    tsg_lua_handle L = NULL;
    char out[255];
    size_t out_len = 0;
    size_t out_type = 0;

    L = tsg_lua_vm_create();
    const char *script = get_script_buffer("./script/unexpected_symbol.lua");
    memset(out, 0, 255);
    int ret = tsg_lua_exec(L, script, strlen(script), data, strlen(data), out, &out_len, &out_type);
    
    /* check */
    EXPECT_EQ(ERR_SCRIPT_UNEXPECTED_SYMBOL, ret);
    EXPECT_EQ(0, out_len);
    EXPECT_EQ(0, out_type);
    EXPECT_STREQ("", out);
}

TEST(tsg_lua_exec, name_expected)
{
    tsg_lua_handle L = NULL;
    char out[255];
    size_t out_len = 0;
    size_t out_type = 0;

    L = tsg_lua_vm_create();
    const char *script = get_script_buffer("./script/name_expected.lua");
    memset(out, 0, 255);
    int ret = tsg_lua_exec(L, script, strlen(script), data, strlen(data), out, &out_len, &out_type);
    
    /* check */
    EXPECT_EQ(ERR_SCRIPT_NAME_EXPECTED, ret);
    EXPECT_EQ(0, out_len);
    EXPECT_EQ(0, out_type);
    EXPECT_STREQ("", out);
}

TEST(tsg_lua_exec, right_curves_bracket_expected)
{
    tsg_lua_handle L = NULL;
    char out[255];
    size_t out_len = 0;
    size_t out_type = 0;

    L = tsg_lua_vm_create();
    const char *script = get_script_buffer("./script/right_curves_bracket_expected.lua");
    memset(out, 0, 255);
    int ret = tsg_lua_exec(L, script, strlen(script), data, strlen(data), out, &out_len, &out_type);
    
    /* check */
    EXPECT_EQ(ERR_SCRIPT_RIGHT_CURVES_BRACKET_EXPECTED, ret);
    EXPECT_EQ(0, out_len);
    EXPECT_EQ(0, out_type);
    EXPECT_STREQ("", out);
}

TEST(tsg_lua_exec, right_curly_bracket_expected)
{
    tsg_lua_handle L = NULL;
    char out[255];
    size_t out_len = 0;
    size_t out_type = 0;

    L = tsg_lua_vm_create();
    const char *script = get_script_buffer("./script/right_curly_bracket_expected.lua");
    memset(out, 0, 255);
    int ret = tsg_lua_exec(L, script, strlen(script), data, strlen(data), out, &out_len, &out_type);
    
    /* check */
    EXPECT_EQ(ERR_SCRIPT_RIGHT_CURLY_BRACKET_EXPECTED, ret);
    EXPECT_EQ(0, out_len);
    EXPECT_EQ(0, out_type);
    EXPECT_STREQ("", out);
}

TEST(tsg_lua_exec, string_expected_but_nil)
{
    tsg_lua_handle L = NULL;
    char out[255];
    size_t out_len = 0;
    size_t out_type = 0;

    L = tsg_lua_vm_create();
    const char *script = get_script_buffer("./script/string_expected_but_nil.lua");
    memset(out, 0, 255);
    int ret = tsg_lua_exec(L, script, strlen(script), data, strlen(data), out, &out_len, &out_type);
    
    /* check */
    EXPECT_EQ(ERR_SCRIPT_STRING_EXPECTED_BUT_NIL, ret);
    EXPECT_EQ(0, out_len);
    EXPECT_EQ(0, out_type);
    EXPECT_STREQ("", out);
}

TEST(tsg_lua_exec, string_expected_but_boolean)
{
    tsg_lua_handle L = NULL;
    char out[255];
    size_t out_len = 0;
    size_t out_type = 0;

    L = tsg_lua_vm_create();
    const char *script = get_script_buffer("./script/string_expected_but_boolean.lua");
    memset(out, 0, 255);
    int ret = tsg_lua_exec(L, script, strlen(script), data, strlen(data), out, &out_len, &out_type);
    
    /* check */
    EXPECT_EQ(ERR_SCRIPT_STRING_EXPECTED_BUT_BOOL, ret);
    EXPECT_EQ(0, out_len);
    EXPECT_EQ(0, out_type);
    EXPECT_STREQ("", out);
}

TEST(tsg_lua_exec, string_expected_but_table)
{
    tsg_lua_handle L = NULL;
    char out[255];
    size_t out_len = 0;
    size_t out_type = 0;

    L = tsg_lua_vm_create();
    const char *script = get_script_buffer("./script/string_expected_but_table.lua");
    memset(out, 0, 255);
    int ret = tsg_lua_exec(L, script, strlen(script), data, strlen(data), out, &out_len, &out_type);
    
    /* check */
    EXPECT_EQ(ERR_SCRIPT_STRING_EXPECTED_BUT_TABLE, ret);
    EXPECT_EQ(0, out_len);
    EXPECT_EQ(0, out_type);
    EXPECT_STREQ("", out);
}

TEST(tsg_lua_exec, call_global_but_nil)
{
    tsg_lua_handle L = NULL;
    char out[255];
    size_t out_len = 0;
    size_t out_type = 0;

    L = tsg_lua_vm_create();
    const char *script = get_script_buffer("./script/call_global_but_nil.lua");
    memset(out, 0, 255);
    int ret = tsg_lua_exec(L, script, strlen(script), data, strlen(data), out, &out_len, &out_type);
    
    /* check */
    EXPECT_EQ(ERR_SCRIPT_CALL_GLOBAL_BUT_NIL, ret);
    EXPECT_EQ(0, out_len);
    EXPECT_EQ(0, out_type);
    EXPECT_STREQ("", out);
}


TEST(tsg_lua_exec, return_too_few)
{
    tsg_lua_handle L = NULL;
    char out[255];
    size_t out_len = 0;
    size_t out_type = 0;

    L = tsg_lua_vm_create();
    const char *script = get_script_buffer("./script/return_too_few.lua");
    memset(out, 0, 255);
    int ret = tsg_lua_exec(L, script, strlen(script), data, strlen(data), out, &out_len, &out_type);
    
    /* check */
    EXPECT_EQ(ERR_SCRIPT_RETURN_TOO_FEW, ret);
    EXPECT_EQ(0, out_len);
    EXPECT_EQ(0, out_type);
    EXPECT_STREQ("", out);
}

TEST(tsg_lua_exec, return_too_much)
{
    tsg_lua_handle L = NULL;
    char out[255];
    size_t out_len = 0;
    size_t out_type = 0;

    L = tsg_lua_vm_create();
    const char *script = get_script_buffer("./script/return_too_much.lua");
    memset(out, 0, 255);
    int ret = tsg_lua_exec(L, script, strlen(script), data, strlen(data), out, &out_len, &out_type);
    
    /* check */
    EXPECT_EQ(ERR_SCRIPT_RETURN_TOO_MUCH, ret);
    EXPECT_EQ(0, out_len);
    EXPECT_EQ(0, out_type);
    EXPECT_STREQ("", out);
}

TEST(tsg_lua_exec, return_type_invaild)
{
    tsg_lua_handle L = NULL;
    char out[255];
    size_t out_len = 0;
    size_t out_type = 0;

    L = tsg_lua_vm_create();
    const char *script = get_script_buffer("./script/return_type_invaild.lua");
    memset(out, 0, 255);
    int ret = tsg_lua_exec(L, script, strlen(script), data, strlen(data), out, &out_len, &out_type);
    
    /* check */
    EXPECT_EQ(ERR_SCRIPT_RETURN_TYPE_INVAILD, ret);
    EXPECT_EQ(0, out_len);
    EXPECT_EQ(0, out_type);
    EXPECT_STREQ("", out);
}

TEST(tsg_lua_exec, return_length_iszero)
{
    tsg_lua_handle L = NULL;
    char out[255];
    size_t out_len = 0;
    size_t out_type = 0;

    L = tsg_lua_vm_create();
    const char *script = get_script_buffer("./script/return_length_iszero.lua");
    memset(out, 0, 255);
    int ret = tsg_lua_exec(L, script, strlen(script), data, strlen(data), out, &out_len, &out_type);
    
    /* check */
    EXPECT_EQ(ERR_SCRIPT_RETURN_LEN_INVAILD, ret);
    EXPECT_EQ(0, out_len);
    EXPECT_EQ(0, out_type);
    EXPECT_STREQ("", out);
}
#if 0
TEST(tsg_lua_exec, function_invaild)
{
    tsg_lua_handle L = NULL;
    char out[255];
    size_t out_len = 0;
    size_t out_type = 0;

    L = tsg_lua_vm_create();
    const char *script = get_script_buffer("./script/function_invaild.lua");
    memset(out, 0, 255);
    int ret = tsg_lua_exec(L, script, strlen(script), data, strlen(data), out, &out_len, &out_type);
    
    /* check */
    EXPECT_EQ(ERR_SCRIPT_FUNCTION_INVAILD, ret);
    EXPECT_EQ(0, out_len);
    EXPECT_EQ(0, out_type);
    EXPECT_STREQ("", out);
}
#endif
