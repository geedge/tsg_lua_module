/*************************************************************************
    > File Name: gtest_tsg_lua_vm_create.cpp
    > Author: pxz
    > Mail: 1500428751@qq.com 
    > Created Time: Sun 19 Jul 2020 01:27:13 PM CST
 ************************************************************************/
#include <gtest/gtest.h>
#include "tsg_lua_interface.h"

/* tsg_destory_lua */
TEST(tsg_destory_lua, normal)
{
    tsg_lua_handle L = NULL;
    L = tsg_lua_vm_create();
    int ret = tsg_destory_lua(L);

    EXPECT_EQ(0, ret);
}

TEST(tsg_destory_lua, lua_vm_isnull)
{
    tsg_lua_handle L = NULL;

    int ret = tsg_destory_lua(L);

    EXPECT_EQ(ERR_LUAVM_ISNULL, ret);
}
