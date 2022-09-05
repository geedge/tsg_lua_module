/*************************************************************************
    > File Name: gtest_elua_cache_script_file.cpp
    > Author: pxz
    > Created Time: Sun 19 Jul 2020 01:28:20 PM CST
 ************************************************************************/
#include <gtest/gtest.h>
#include "elua.h"

/* elua_cleanup_script */
TEST(elua_cleanup_script, normal)
{
    elua_vm *L = NULL;

    L = elua_create_vm(NULL);
    const char *script = "./script/return_string_normal.lua";
    struct elua_script *escript = elua_cache_script_file(L, script, 0);
    int ret = elua_cleanup_script(escript);
    
    /* check */
    EXPECT_EQ(0, ret);
}

TEST(elua_cleanup_script, script_isnull)
{
    int ret = elua_cleanup_script(NULL);
    
    /* check */
    EXPECT_EQ(-1, ret);
}

