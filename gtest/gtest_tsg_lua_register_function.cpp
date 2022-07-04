/*************************************************************************
    > File Name: gtest_tsg_lua_vm_create.cpp
    > Author: pxz
    > Created Time: Sun 19 Jul 2020 01:27:13 PM CST
 ************************************************************************/
#include <gtest/gtest.h>
#include "tsg_lua_interface.h"

int get_hello(tsg_lua_handle L)
{
    c_push_string_into_lua(L, "hello", strlen("hello"));
    return 1;
}

int set_world(tsg_lua_handle L)
{
    int argc = 0;
    lua_arg_t *argv = NULL;
    
    c_pull_param_from_lua(L, &argc, &argv);
    if (argc != 1 || argv == NULL || argv[0].type != STRING)
    {
        return 0;
    }

    char *buff = (char *)calloc(1, argv[0].len + strlen(" world.") + 1);
    sprintf(buff, "%s%s", argv[0].str, " world.");
    c_push_string_into_lua(L, buff, strlen(buff));
    free(buff);
    buff = NULL;
    free_param_form_lua(argc, &argv);

    return 1;
}

TEST(lua_register_function, normal)
{
    tsg_lua_handle L = NULL;
    char out[255];
    size_t out_len = 0;
    size_t out_type = STRING;

    L = tsg_lua_vm_create();
    EXPECT_NE((void *)NULL, L);
    lua_register_function(L, "get", "get_hello", get_hello);
    lua_register_function(L, "set", "set_world", set_world);

    const char *script = "./script/call_cfunction.lua";
    memset(out, 0, 255);
    int ret = tsg_lua_exec_file(L, script, "hello", strlen("hello"), out, &out_len, &out_type);

    EXPECT_EQ(0, ret);
    EXPECT_EQ(12, out_len);
    EXPECT_EQ(STRING, out_type);
    EXPECT_STREQ("hello world.", out);
}
