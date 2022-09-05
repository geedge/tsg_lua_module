/*************************************************************************
    > File Name: gtest_elua_create_vm.cpp
    > Author: pxz
    > Created Time: Sun 19 Jul 2020 01:27:13 PM CST
 ************************************************************************/
#include <gtest/gtest.h>
#include "elua.h"

/* elua_create_vm */
TEST(elua_create_vm, create_normal)
{
    elua_vm *L = NULL;
    L = elua_create_vm(NULL);
    EXPECT_NE((void *)NULL, L);
}
