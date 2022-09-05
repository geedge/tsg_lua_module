/*************************************************************************
    > File Name: gtest_elua_cache_script_file.cpp
    > Author: pxz
    > Created Time: Sun 19 Jul 2020 01:28:20 PM CST
 ************************************************************************/
#include <gtest/gtest.h>
#include "elua.h"

/* elua_cache_script_file */
TEST(elua_cache_script_bytecode_file, normal)
{
    elua_vm *L = NULL;

    L = elua_create_vm(NULL);
    const char *script = "./script/return_string_normal.luajit";
    struct elua_script *escript = elua_cache_script_file(L, script, 0);
    
    /* check */
    EXPECT_NE((void *)NULL, escript);
}

TEST(elua_cache_script_bytecode_file, lua_vm_isnull)
{
    elua_vm *L = NULL;

    const char *script = "./script/return_string_normal.luajit";
    struct elua_script *escript = elua_cache_script_file(L, script, 0);
    
    /* check */
    EXPECT_EQ(NULL, escript);
    EXPECT_STREQ(elua_get_last_error_string(L), "The vm is invalid.");
}

TEST(elua_cache_script_bytecode_file, script_isnull)
{
    elua_vm *L = NULL;

    L = elua_create_vm(NULL);
    const char *script = NULL;
    struct elua_script *escript = elua_cache_script_file(L, script, 0);
    
    /* check */
    EXPECT_EQ(NULL, escript);
    EXPECT_STREQ(elua_get_last_error_string(L), "[elua_cache_script_file:765] Parameter error.");
}
