/*************************************************************************
    > File Name: gtest_elua_create_vm.cpp
    > Author: pxz
    > Created Time: Sun 19 Jul 2020 01:27:13 PM CST
 ************************************************************************/
#include <gtest/gtest.h>
#include "elua.h"

/* elua_destroy_vm */
TEST(elua_destroy_vm, normal)
{
    elua_vm *L = NULL;
    L = elua_create_vm(NULL);
    int ret = elua_destroy_vm(L);

    EXPECT_EQ(0, ret);
}

TEST(elua_destroy_vm, lua_vm_isnull)
{
    elua_vm *L = NULL;

    int ret = elua_destroy_vm(L);

    EXPECT_EQ(-1, ret);
    EXPECT_STREQ(elua_get_last_error_string(L), "The vm is invalid.");
}
