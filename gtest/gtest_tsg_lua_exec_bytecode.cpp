/*************************************************************************
    > File Name: gtest_tsg_lua_exec.cpp
    > Author: pxz
    > Created Time: Sun 19 Jul 2020 01:28:20 PM CST
 ************************************************************************/
#include <gtest/gtest.h>
#include "tsg_lua_interface.h"

static const char *data = "hello world";
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
            script_len = num;
            fclose(fp);
            return buffer;
        }
        fclose(fp);
    }
    return NULL;
}

/* tsg_lua_exec */
TEST(tsg_lua_exec_bytecode, normal_return_string)
{
    tsg_lua_handle L = NULL;
    char out[255];
    size_t out_len = 0;
    size_t out_type = 0;

    L = tsg_lua_vm_create();
    memset(out, 0, 255);
    const char *script = get_script_buffer("./script/return_string_normal.luajit");
    int ret = tsg_lua_exec(L, script, script_len, data, strlen(data), out, &out_len, &out_type);
    
    /* check */
    EXPECT_EQ(0, ret);
    EXPECT_EQ(11, out_len);
    EXPECT_EQ(STRING, out_type);
    EXPECT_STREQ("hello world", out);
}

TEST(tsg_lua_exec_bytecode, normal_return_boolean)
{
    tsg_lua_handle L = NULL;
    char out[255];
    size_t out_len = 0;
    size_t out_type = 0;
    L = tsg_lua_vm_create();
    const char *script = get_script_buffer("./script/return_boolean_normal.luajit");
    memset(out, 0, 255);
    int ret = tsg_lua_exec(L, script, script_len, data, strlen(data), out, &out_len, &out_type);
    
    /* check */
    EXPECT_EQ(0, ret);
    EXPECT_EQ(1, out_len);
    EXPECT_EQ(BOOLEAN, out_type);
    EXPECT_EQ(true, out[0]);
}

TEST(tsg_lua_exec_bytecode, normal_return_integer)
{
    tsg_lua_handle L = NULL;
    char out[255];
    size_t out_len = 0;
    size_t out_type = 0;
    L = tsg_lua_vm_create();
    const char *script = get_script_buffer("./script/return_integer_normal.luajit");
    memset(out, 0, 255);
    int ret = tsg_lua_exec(L, script, script_len, data, strlen(data), out, &out_len, &out_type);
    
    /* check */
    EXPECT_EQ(0, ret);
    EXPECT_EQ(8, out_len);
    EXPECT_EQ(INTEGER, out_type);
    EXPECT_EQ(7, *(long int *)out);
}

TEST(tsg_lua_exec_bytecode, lua_vm_isnull)
{
    tsg_lua_handle L = NULL;
    char out[255];
    size_t out_len = 0;
    size_t out_type = 0;
    
    const char *script = get_script_buffer("./script/return_string_normal.luajit");
    memset(out, 0, sizeof(out));
    int ret = tsg_lua_exec(L, script, script_len, data, strlen(data), out, &out_len, &out_type);
    
    /* check */
    EXPECT_EQ(ERR_LUAVM_ISNULL, ret);
    EXPECT_EQ(0, out_len);
    EXPECT_EQ(0, out_type);
    EXPECT_STREQ("", out);
}

TEST(tsg_lua_exec_bytecode, script_isnull)
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

TEST(tsg_lua_exec_bytecode, input_isnull)
{
    tsg_lua_handle L = NULL;
    char out[255];
    size_t out_len = 0;
    size_t out_type = 0;
    char *input = NULL;

    L = tsg_lua_vm_create();
    const char *script = get_script_buffer("./script/return_string_normal.luajit");
    memset(out, 0, 255);
    int ret = tsg_lua_exec(L, script, script_len, input, 0, out, &out_len, &out_type);
    
    /* check */
    EXPECT_EQ(ERR_INPUT_ISNULL, ret);
    EXPECT_EQ(0, out_len);
    EXPECT_EQ(0, out_type);
    EXPECT_STREQ("", out);
}

TEST(tsg_lua_exec_bytecode, in_len_invaild)
{
    tsg_lua_handle L = NULL;
    char out[255];
    size_t out_len = 0;
    size_t out_type = 0;

    L = tsg_lua_vm_create();
    const char *script = get_script_buffer("./script/return_string_normal.luajit");
    memset(out, 0, 255);
    int ret = tsg_lua_exec(L, script, script_len, data, 0, out, &out_len, &out_type);
    
    /* check */
    EXPECT_EQ(ERR_IN_LEN_INVAILD, ret);
    EXPECT_EQ(0, out_len);
    EXPECT_EQ(0, out_type);
    EXPECT_STREQ("", out);
}

TEST(tsg_lua_exec_bytecode, string_expected_but_nil)
{
    tsg_lua_handle L = NULL;
    char out[255];
    size_t out_len = 0;
    size_t out_type = 0;

    L = tsg_lua_vm_create();
    const char *script = get_script_buffer("./script/string_expected_but_nil.luajit");
    memset(out, 0, 255);
    int ret = tsg_lua_exec(L, script, script_len, data, strlen(data), out, &out_len, &out_type);
    
    /* check */
    EXPECT_EQ(ERR_SCRIPT_STRING_EXPECTED_BUT_NIL, ret);
    EXPECT_EQ(0, out_len);
    EXPECT_EQ(0, out_type);
    EXPECT_STREQ("", out);
}

TEST(tsg_lua_exec_bytecode, string_expected_but_boolean)
{
    tsg_lua_handle L = NULL;
    char out[255];
    size_t out_len = 0;
    size_t out_type = 0;

    L = tsg_lua_vm_create();
    const char *script = get_script_buffer("./script/string_expected_but_boolean.luajit");
    memset(out, 0, 255);
    int ret = tsg_lua_exec(L, script, script_len, data, strlen(data), out, &out_len, &out_type);
    
    /* check */
    EXPECT_EQ(ERR_SCRIPT_STRING_EXPECTED_BUT_BOOL, ret);
    EXPECT_EQ(0, out_len);
    EXPECT_EQ(0, out_type);
    EXPECT_STREQ("", out);
}

TEST(tsg_lua_exec_bytecode, string_expected_but_table)
{
    tsg_lua_handle L = NULL;
    char out[255];
    size_t out_len = 0;
    size_t out_type = 0;

    L = tsg_lua_vm_create();
    const char *script = get_script_buffer("./script/string_expected_but_table.luajit");
    memset(out, 0, 255);
    int ret = tsg_lua_exec(L, script, script_len, data, strlen(data), out, &out_len, &out_type);
    
    /* check */
    EXPECT_EQ(ERR_SCRIPT_STRING_EXPECTED_BUT_TABLE, ret);
    EXPECT_EQ(0, out_len);
    EXPECT_EQ(0, out_type);
    EXPECT_STREQ("", out);
}

TEST(tsg_lua_exec_bytecode, call_global_but_nil)
{
    tsg_lua_handle L = NULL;
    char out[255];
    size_t out_len = 0;
    size_t out_type = 0;

    L = tsg_lua_vm_create();
    const char *script = get_script_buffer("./script/call_global_but_nil.luajit");
    memset(out, 0, 255);
    int ret = tsg_lua_exec(L, script, script_len, data, strlen(data), out, &out_len, &out_type);
    
    /* check */
    EXPECT_EQ(ERR_SCRIPT_CALL_GLOBAL_BUT_NIL, ret);
    EXPECT_EQ(0, out_len);
    EXPECT_EQ(0, out_type);
    EXPECT_STREQ("", out);
}


TEST(tsg_lua_exec_bytecode, return_too_few)
{
    tsg_lua_handle L = NULL;
    char out[255];
    size_t out_len = 0;
    size_t out_type = 0;

    L = tsg_lua_vm_create();
    const char *script = get_script_buffer("./script/return_too_few.luajit");
    memset(out, 0, 255);
    int ret = tsg_lua_exec(L, script, script_len, data, strlen(data), out, &out_len, &out_type);
    
    /* check */
    EXPECT_EQ(ERR_SCRIPT_RETURN_TOO_FEW, ret);
    EXPECT_EQ(0, out_len);
    EXPECT_EQ(0, out_type);
    EXPECT_STREQ("", out);
}

TEST(tsg_lua_exec_bytecode, return_too_much)
{
    tsg_lua_handle L = NULL;
    char out[255];
    size_t out_len = 0;
    size_t out_type = 0;

    L = tsg_lua_vm_create();
    const char *script = get_script_buffer("./script/return_too_much.luajit");
    memset(out, 0, 255);
    int ret = tsg_lua_exec(L, script, script_len, data, strlen(data), out, &out_len, &out_type);
    
    /* check */
    EXPECT_EQ(ERR_SCRIPT_RETURN_TOO_MUCH, ret);
    EXPECT_EQ(0, out_len);
    EXPECT_EQ(0, out_type);
    EXPECT_STREQ("", out);
}

TEST(tsg_lua_exec_bytecode, return_type_invaild)
{
    tsg_lua_handle L = NULL;
    char out[255];
    size_t out_len = 0;
    size_t out_type = 0;

    L = tsg_lua_vm_create();
    const char *script = get_script_buffer("./script/return_type_invaild.luajit");
    memset(out, 0, 255);
    int ret = tsg_lua_exec(L, script, script_len, data, strlen(data), out, &out_len, &out_type);
    
    /* check */
    EXPECT_EQ(ERR_SCRIPT_RETURN_TYPE_INVAILD, ret);
    EXPECT_EQ(0, out_len);
    EXPECT_EQ(0, out_type);
    EXPECT_STREQ("", out);
}

TEST(tsg_lua_exec_bytecode, return_length_iszero)
{
    tsg_lua_handle L = NULL;
    char out[255];
    size_t out_len = 0;
    size_t out_type = 0;

    L = tsg_lua_vm_create();
    const char *script = get_script_buffer("./script/return_length_iszero.luajit");
    memset(out, 0, 255);
    int ret = tsg_lua_exec(L, script, script_len, data, strlen(data), out, &out_len, &out_type);
    
    /* check */
    EXPECT_EQ(ERR_SCRIPT_RETURN_LEN_INVAILD, ret);
    EXPECT_EQ(0, out_len);
    EXPECT_EQ(0, out_type);
    EXPECT_STREQ("", out);
}

TEST(tsg_lua_exec_bytecode, normal_return_string_sharp)
{
    tsg_lua_handle L = NULL;
    char out[255];
    size_t out_len = 0;
    size_t out_type = 0;

    L = tsg_lua_vm_create();
    memset(out, 0, 255);
    const char *script = get_script_buffer("./script/return_string_normal_sharp.luajit");
    int ret = tsg_lua_exec(L, script, script_len, data, strlen(data), out, &out_len, &out_type);
    
    /* check */
    EXPECT_EQ(0, ret);
    EXPECT_EQ(11, out_len);
    EXPECT_EQ(STRING, out_type);
    EXPECT_STREQ("hello world", out);
}