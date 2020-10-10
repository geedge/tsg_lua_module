/*************************************************************************
    > File Name: gtest_tsg_lua_exec_file.cpp
    > Author: pxz
    > Created Time: Sun 19 Jul 2020 01:28:20 PM CST
 ************************************************************************/
#include <gtest/gtest.h>
#include "tsg_lua_interface.h"
static const char *data = "hello world";

/* tsg_lua_exec_file */
TEST(tsg_lua_exec_bytecode_file, normal_return_string)
{
    tsg_lua_handle L = NULL;
    char out[255];
    size_t out_len = 0;
    size_t out_type = 0;

    L = tsg_lua_vm_create();
    const char *script = "./script/return_string_normal.luajit";
    memset(out, 0, 255);
    int ret = tsg_lua_exec_file(L, script, data, strlen(data), out, &out_len, &out_type);
    
    /* check */
    EXPECT_EQ(0, ret);
    EXPECT_EQ(11, out_len);
    EXPECT_EQ(STRING, out_type);
    EXPECT_STREQ("hello world", out);
}

TEST(tsg_lua_exec_bytecode_file, normal_return_boolean)
{
    tsg_lua_handle L = NULL;
    char out[255];
    size_t out_len = 0;
    size_t out_type = 0;
    L = tsg_lua_vm_create();
    const char *script = "./script/return_boolean_normal.luajit";
    memset(out, 0, 255);
    int ret = tsg_lua_exec_file(L, script, data, strlen(data), out, &out_len, &out_type);
    
    /* check */
    EXPECT_EQ(0, ret);
    EXPECT_EQ(1, out_len);
    EXPECT_EQ(BOOLEAN, out_type);
    EXPECT_EQ(true, out[0]);
}

TEST(tsg_lua_exec_bytecode_file, normal_return_integer)
{
    tsg_lua_handle L = NULL;
    char out[255];
    size_t out_len = 0;
    size_t out_type = 0;
    L = tsg_lua_vm_create();
    const char *script = "./script/return_integer_normal.luajit";
    memset(out, 0, 255);
    int ret = tsg_lua_exec_file(L, script, data, strlen(data), out, &out_len, &out_type);
    
    /* check */
    EXPECT_EQ(0, ret);
    EXPECT_EQ(8, out_len);
    EXPECT_EQ(INTEGER, out_type);
    EXPECT_EQ(7, *(long int *)out);
}

TEST(tsg_lua_exec_bytecode_file, lua_vm_isnull)
{
    tsg_lua_handle L = NULL;
    char out[255];
    size_t out_len = 0;
    size_t out_type = 0;
    
    const char *script = "./script/return_string_normal.luajit";
    memset(out, 0, sizeof(out));
    int ret = tsg_lua_exec_file(L, script, data, strlen(data), out, &out_len, &out_type);
    
    /* check */
    EXPECT_EQ(ERR_LUAVM_ISNULL, ret);
    EXPECT_EQ(0, out_len);
    EXPECT_EQ(0, out_type);
    EXPECT_STREQ("", out);
}

TEST(tsg_lua_exec_bytecode_file, script_isnull)
{
    tsg_lua_handle L = NULL;
    char out[255];
    size_t out_len = 0;
    size_t out_type = 0;
    L = tsg_lua_vm_create();
    const char *script = NULL;
    memset(out, 0, 255);
    int ret = tsg_lua_exec_file(L, script, data, strlen(data), out, &out_len, &out_type);
    
    /* check */
    EXPECT_EQ(ERR_SCRIPT_ISNULL, ret);
    EXPECT_EQ(0, out_len);
    EXPECT_EQ(0, out_type);
    EXPECT_STREQ("", out);
}

TEST(tsg_lua_exec_bytecode_file, input_isnull)
{
    tsg_lua_handle L = NULL;
    char out[255];
    size_t out_len = 0;
    size_t out_type = 0;
    char *input = NULL;

    L = tsg_lua_vm_create();
    const char *script = "./script/return_string_normal.luajit";
    memset(out, 0, 255);
    int ret = tsg_lua_exec_file(L, script, input, 0, out, &out_len, &out_type);
    
    /* check */
    EXPECT_EQ(ERR_INPUT_ISNULL, ret);
    EXPECT_EQ(0, out_len);
    EXPECT_EQ(0, out_type);
    EXPECT_STREQ("", out);
}

TEST(tsg_lua_exec_bytecode_file, in_len_invaild)
{
    tsg_lua_handle L = NULL;
    char out[255];
    size_t out_len = 0;
    size_t out_type = 0;

    L = tsg_lua_vm_create();
    const char *script = "./script/return_string_normal.luajit";
    memset(out, 0, 255);
    int ret = tsg_lua_exec_file(L, script, data, 0, out, &out_len, &out_type);
    
    /* check */
    EXPECT_EQ(ERR_IN_LEN_INVAILD, ret);
    EXPECT_EQ(0, out_len);
    EXPECT_EQ(0, out_type);
    EXPECT_STREQ("", out);
}

TEST(tsg_lua_exec_bytecode_file, string_expected_but_nil)
{
    tsg_lua_handle L = NULL;
    char out[255];
    size_t out_len = 0;
    size_t out_type = 0;

    L = tsg_lua_vm_create();
    const char *script = "./script/string_expected_but_nil.luajit";
    memset(out, 0, 255);
    int ret = tsg_lua_exec_file(L, script, data, strlen(data), out, &out_len, &out_type);
    
    /* check */
    EXPECT_EQ(ERR_SCRIPT_STRING_EXPECTED_BUT_NIL, ret);
    EXPECT_EQ(0, out_len);
    EXPECT_EQ(0, out_type);
    EXPECT_STREQ("", out);
}

TEST(tsg_lua_exec_bytecode_file, string_expected_but_boolean)
{
    tsg_lua_handle L = NULL;
    char out[255];
    size_t out_len = 0;
    size_t out_type = 0;

    L = tsg_lua_vm_create();
    const char *script = "./script/string_expected_but_boolean.luajit";
    memset(out, 0, 255);
    int ret = tsg_lua_exec_file(L, script, data, strlen(data), out, &out_len, &out_type);
    
    /* check */
    EXPECT_EQ(ERR_SCRIPT_STRING_EXPECTED_BUT_BOOL, ret);
    EXPECT_EQ(0, out_len);
    EXPECT_EQ(0, out_type);
    EXPECT_STREQ("", out);
}

TEST(tsg_lua_exec_bytecode_file, string_expected_but_table)
{
    tsg_lua_handle L = NULL;
    char out[255];
    size_t out_len = 0;
    size_t out_type = 0;

    L = tsg_lua_vm_create();
    const char *script = "./script/string_expected_but_table.luajit";
    memset(out, 0, 255);
    int ret = tsg_lua_exec_file(L, script, data, strlen(data), out, &out_len, &out_type);
    
    /* check */
    EXPECT_EQ(ERR_SCRIPT_STRING_EXPECTED_BUT_TABLE, ret);
    EXPECT_EQ(0, out_len);
    EXPECT_EQ(0, out_type);
    EXPECT_STREQ("", out);
}

TEST(tsg_lua_exec_bytecode_file, call_global_but_nil)
{
    tsg_lua_handle L = NULL;
    char out[255];
    size_t out_len = 0;
    size_t out_type = 0;

    L = tsg_lua_vm_create();
    const char *script = "./script/call_global_but_nil.luajit";
    memset(out, 0, 255);
    int ret = tsg_lua_exec_file(L, script, data, strlen(data), out, &out_len, &out_type);
    
    /* check */
    EXPECT_EQ(ERR_SCRIPT_CALL_GLOBAL_BUT_NIL, ret);
    EXPECT_EQ(0, out_len);
    EXPECT_EQ(0, out_type);
    EXPECT_STREQ("", out);
}


TEST(tsg_lua_exec_bytecode_file, return_too_few)
{
    tsg_lua_handle L = NULL;
    char out[255];
    size_t out_len = 0;
    size_t out_type = 0;

    L = tsg_lua_vm_create();
    const char *script = "./script/return_too_few.luajit";
    memset(out, 0, 255);
    int ret = tsg_lua_exec_file(L, script, data, strlen(data), out, &out_len, &out_type);
    
    /* check */
    EXPECT_EQ(ERR_SCRIPT_RETURN_TOO_FEW, ret);
    EXPECT_EQ(0, out_len);
    EXPECT_EQ(0, out_type);
    EXPECT_STREQ("", out);
}

TEST(tsg_lua_exec_bytecode_file, return_too_much)
{
    tsg_lua_handle L = NULL;
    char out[255];
    size_t out_len = 0;
    size_t out_type = 0;

    L = tsg_lua_vm_create();
    const char *script = "./script/return_too_much.luajit";
    memset(out, 0, 255);
    int ret = tsg_lua_exec_file(L, script, data, strlen(data), out, &out_len, &out_type);
    
    /* check */
    EXPECT_EQ(ERR_SCRIPT_RETURN_TOO_MUCH, ret);
    EXPECT_EQ(0, out_len);
    EXPECT_EQ(0, out_type);
    EXPECT_STREQ("", out);
}

TEST(tsg_lua_exec_bytecode_file, return_type_invaild)
{
    tsg_lua_handle L = NULL;
    char out[255];
    size_t out_len = 0;
    size_t out_type = 0;

    L = tsg_lua_vm_create();
    const char *script = "./script/return_type_invaild.luajit";
    memset(out, 0, 255);
    int ret = tsg_lua_exec_file(L, script, data, strlen(data), out, &out_len, &out_type);
    
    /* check */
    EXPECT_EQ(ERR_SCRIPT_RETURN_TYPE_INVAILD, ret);
    EXPECT_EQ(0, out_len);
    EXPECT_EQ(0, out_type);
    EXPECT_STREQ("", out);
}

TEST(tsg_lua_exec_bytecode_file, return_length_iszero)
{
    tsg_lua_handle L = NULL;
    char out[255];
    size_t out_len = 0;
    size_t out_type = 0;

    L = tsg_lua_vm_create();
    const char *script = "./script/return_length_iszero.luajit";
    memset(out, 0, 255);
    int ret = tsg_lua_exec_file(L, script, data, strlen(data), out, &out_len, &out_type);
    
    /* check */
    EXPECT_EQ(ERR_SCRIPT_RETURN_LEN_INVAILD, ret);
    EXPECT_EQ(0, out_len);
    EXPECT_EQ(0, out_type);
    EXPECT_STREQ("", out);
}
