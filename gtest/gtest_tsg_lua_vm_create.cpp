/*************************************************************************
    > File Name: gtest_tsg_lua_vm_create.cpp
    > Author: pxz
    > Mail: 1500428751@qq.com 
    > Created Time: Sun 19 Jul 2020 01:27:13 PM CST
 ************************************************************************/
#include <gtest/gtest.h>
#include "tsg_lua_interface.h"

/* tsg_lua_vm_create */
TEST(tsg_lua_vm_create, create_normal)
{
    tsg_lua_handle L = NULL;
    L = tsg_lua_vm_create();
    EXPECT_NE((void *)NULL, L);
}
