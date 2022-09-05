/*************************************************************************
    > File Name: gtest_elua_execute_script.cpp
    > Author: pxz
    > Created Time: Sun 19 Jul 2020 01:28:20 PM CST
 ************************************************************************/
#include <gtest/gtest.h>
#include "elua.h"
static const char *data = "hello world";

/* elua_execute_script */
TEST(elua_execute_script, normal_return_string)
{
    elua_vm *L = NULL;

    L = elua_create_vm(NULL);
    const char *script = "./script/return_string_normal.lua";
     
    int len = strlen(data);
    struct elua_data out;
    char buff[1024];
    out.type = STRING;
    out.integer = 1204;
    out.string = buff;

    struct elua_script *escript = elua_cache_script_file(L, script, 0);
    int ret = elua_execute_script(escript, data, len, NULL, NULL, &out);
    
    /* check */
    EXPECT_EQ(0, ret);
    EXPECT_EQ(11, out.len);
    EXPECT_EQ(STRING, out.type);
    EXPECT_STREQ("hello world", out.string);
}

TEST(elua_execute_script, normal_return_boolean)
{
    elua_vm *L = NULL;
    L = elua_create_vm(NULL);
    const char *script = "./script/return_boolean_normal.lua";
     
    struct elua_data out;

    char *buff = (char *)data;
    int len = strlen(data);
    out.type = BOOLEAN;

    struct elua_script *escript = elua_cache_script_file(L, script, 0);
    int ret = elua_execute_script(escript, buff, len, NULL, NULL, &out);
    
    /* check */
    EXPECT_EQ(0, ret);
    EXPECT_EQ(4, out.len);
    EXPECT_EQ(BOOLEAN, out.type);
    EXPECT_EQ(true, out.true_or_false);
}

TEST(elua_execute_script, normal_return_integer)
{
    elua_vm *L = NULL;
    L = elua_create_vm(NULL);
    const char *script = "./script/return_integer_normal.lua";
     
    struct elua_data out;

    char *buff = (char *)data;
    int len = strlen(data);
    out.type = INTEGER;

    struct elua_script *escript = elua_cache_script_file(L, script, 0);
    int ret = elua_execute_script(escript, buff, len, NULL, NULL, &out);
    
    /* check */
    EXPECT_EQ(0, ret);
    EXPECT_EQ(8, out.len);
    EXPECT_EQ(INTEGER, out.type);
    EXPECT_EQ(7, out.integer);
}

TEST(elua_execute_script, lua_vm_isnull)
{
    elua_vm *L = NULL;
    
    L = elua_create_vm(NULL);
    const char *script = "./script/return_string_normal.lua";
    struct elua_script *escript = elua_cache_script_file(L, script, 0);
    L = NULL;
     
    struct elua_data out;

    char *buff = (char *)data;
    int len = strlen(data);
    out.type = INTEGER;
    
    int ret = elua_execute_script(escript, buff, len, NULL, NULL, &out);
    /* check */
    EXPECT_EQ(-1, ret);
    EXPECT_STREQ("The vm is invalid.", elua_get_last_error_string(L));
}

TEST(elua_execute_script, script_id_invaild)
{
    elua_vm *L = NULL;
    L = elua_create_vm(NULL);
    struct elua_script *escript = 0;
     
    struct elua_data out;

    char *buff = (char *)data;
    int len = strlen(data);
    out.type = INTEGER;
    int ret = elua_execute_script(escript, buff, len, NULL, NULL, &out);
    
    /* check */
    EXPECT_EQ(-1, ret);
    EXPECT_STREQ(elua_get_last_error_string(L), "");
}

TEST(elua_execute_script, input_isnull)
{
    elua_vm *L = NULL;

    L = elua_create_vm(NULL);
    const char *script = "./script/return_string_normal.lua";
    struct elua_script *escript = elua_cache_script_file(L, script, 0);
    struct elua_data out;
    out.type = INTEGER;
    int ret = elua_execute_script(escript, NULL, strlen(data), NULL, NULL, &out);
    
    /* check */
    EXPECT_EQ(-1, ret);
    EXPECT_STREQ(elua_get_last_error_string(L), "[elua_execute_script:1168] input or input_len is invaild.");
}

TEST(elua_execute_script, in_len_invaild)
{
    elua_vm *L = NULL;

    L = elua_create_vm(NULL);
    const char *script = "./script/return_string_normal.lua";
    struct elua_script *escript = elua_cache_script_file(L, script, 0);

     
    struct elua_data out;

    char *buff = (char *)data;
    int len = 0;
    out.type = INTEGER;
    int ret = elua_execute_script(escript, buff, len, NULL, NULL, &out);
    
    /* check */
    EXPECT_EQ(-1, ret);
    EXPECT_STREQ(elua_get_last_error_string(L), "[elua_execute_script:1168] input or input_len is invaild.");
}

TEST(elua_execute_script, string_expected_but_nil)
{
    elua_vm *L = NULL;

    L = elua_create_vm(NULL);
    const char *script = "./script/string_expected_but_nil.lua";
    struct elua_script *escript = elua_cache_script_file(L, script, 0);

     
    struct elua_data out;

    char *buff = (char *)data;
    int len = strlen(data);
    out.type = INTEGER;
    int ret = elua_execute_script(escript, buff, len, NULL, NULL, &out);
    
    /* check */
    EXPECT_EQ(-1, ret);
    EXPECT_STREQ(elua_get_last_error_string(L), "[elua_call_script:1074] [string \"./script/string_expected_but_nil.lua\"]:1: bad argument #1 to 'len' (string expected, got nil).");
}

TEST(elua_execute_script, string_expected_but_boolean)
{
    elua_vm *L = NULL;

    L = elua_create_vm(NULL);
    const char *script = "./script/string_expected_but_boolean.lua";
    struct elua_script *escript = elua_cache_script_file(L, script, 0);
     
    struct elua_data out;

    char *buff = (char *)data;
    int len = strlen(data);
    out.type = INTEGER;
    int ret = elua_execute_script(escript, buff, len, NULL, NULL, &out);
    
    /* check */
    EXPECT_EQ(-1, ret);
    EXPECT_STREQ(elua_get_last_error_string(L), "[elua_call_script:1074] [string \"./script/string_expected_but_boolean.lua\"]:2: bad argument #1 to 'len' (string expected, got boolean).");
}

TEST(elua_execute_script, call_global_but_nil)
{
    elua_vm *L = NULL;

    L = elua_create_vm(NULL);
    const char *script = "./script/call_global_but_nil.lua";
    struct elua_script *escript = elua_cache_script_file(L, script, 0);
     
    struct elua_data out;

    char *buff = (char *)data;
    int len = strlen(data);
    out.type = INTEGER;
    int ret = elua_execute_script(escript, buff, len, NULL, NULL, &out);
    
    /* check */
    EXPECT_EQ(-1, ret);
    EXPECT_STREQ(elua_get_last_error_string(L), "[elua_call_script:1074] [string \"./script/call_global_but_nil.lua\"]:1: attempt to call global 'tablefunc' (a nil value).");
}


TEST(elua_execute_script, return_too_much)
{
    elua_vm *L = NULL;

    L = elua_create_vm(NULL);
    const char *script = "./script/return_too_much.lua";
    struct elua_script *escript = elua_cache_script_file(L, script, 0);
     
    struct elua_data out;

    char *buff = (char *)data;
    int len = strlen(data);
    out.type = INTEGER;
    int ret = elua_execute_script(escript, buff, len, NULL, NULL, &out);
    
    /* check */
    EXPECT_EQ(-1, ret);
    EXPECT_STREQ(elua_get_last_error_string(L), "[elua_vm_return_value:1107] the script return too many values, the num of value is 3, 5.");
}

TEST(elua_execute_script, return_length_iszero)
{
    elua_vm *L = NULL;

    L = elua_create_vm(NULL);
    const char *script = "./script/return_length_iszero.lua";
    struct elua_script *escript = elua_cache_script_file(L, script, 0);
     
    struct elua_data out;

    char *buff = (char *)data;
    int len = strlen(data);
    out.type = INTEGER;
    int ret = elua_execute_script(escript, buff, len, NULL, NULL, &out);
    
    /* check */
    EXPECT_EQ(-1, ret);
    EXPECT_STREQ(elua_get_last_error_string(L), "[elua_vm_return_value:1101] script out_len is 0.");
}

TEST(elua_execute_script, string_expected_but_table)
{
    elua_vm *L = NULL;

    L = elua_create_vm(NULL);
    const char *script = "./script/string_expected_but_table.lua";
    struct elua_script *escript = elua_cache_script_file(L, script, 0);
     
    struct elua_data out;

    char *buff = (char *)data;
    int len = strlen(data);
    out.type = INTEGER;
    int ret = elua_execute_script(escript, buff, len, NULL, NULL, &out);
    
    /* check */
    EXPECT_EQ(-1, ret);
    EXPECT_STREQ(elua_get_last_error_string(L), "[elua_call_script:1074] [string \"./script/string_expected_but_table.lua\"]:2: bad argument #1 to 'len' (string expected, got table).");
}

TEST(elua_execute_script, normal_return_string_sharp)
{
    elua_vm *L = NULL;

    L = elua_create_vm(NULL);
    const char *script = "./script/return_string_normal_sharp.lua";
    struct elua_script *escript = elua_cache_script_file(L, script, 0);
     
    struct elua_data out;
    out.type = STRING;
    out.len = 1024;
    char buff[1024];
    out.string = buff;

    int len = strlen(data);
    int ret = elua_execute_script(escript, data, len, NULL, NULL, &out);
    
    /* check */
    EXPECT_EQ(0, ret);
    EXPECT_EQ(len, out.len);
    EXPECT_STREQ(data, out.string);
}