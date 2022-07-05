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

int get_req_header(tsg_lua_handle L)
{
    const char *key[] = {"token", "User-Agent", "Accept", "Host", "Accept-Encoding", 
                    "Connection", "Content-Length", "Content-Type"};
    const char *value[] = {"c21f969b5f03d33d43e04f8f136e7682", " PostmanRuntime/7.26.8","*/*", "192.168.40.223:9098",
                    "gzip, deflate, br", "keep-alive", "7020", "text/markdown"};
    c_push_table_into_lua(L, key, value, 8);

    return 1;
}

int get_rsp_header(tsg_lua_handle L)
{
    const char *key[] = {"Content-Type", "Content-Length", "Data"};
    const char *value[] = {"application/xml;charset=UTF-8", "0", "Wed, 16 Dec 2020 08:45:55 GMT"};
    c_push_table_into_lua(L, key, value, 3);

    return 1;
}

int get_result(tsg_lua_handle L)
{
    c_push_bool_into_lua(L, false);
    return 1;
}

int get_nil(tsg_lua_handle L)
{
    c_push_nil_into_lua(L);
    return 1;   
}

int get_time(tsg_lua_handle L)
{
    c_push_num_into_lua(L, 0x12345678);
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
	tsg_destory_lua(L);
}

TEST(lua_register_function, get_header)
{
    tsg_lua_handle L = NULL;
    char out[1024];
    size_t out_len = 0;
    size_t out_type = STRING;

    L = tsg_lua_vm_create();
    EXPECT_NE((void *)NULL, L);
    lua_register_function(L, "req", "get_header", get_req_header);
    lua_register_function(L, "rsp", "get_header", get_rsp_header);

    const char *script = "./script/get_header.lua";
    memset(out, 0, 1024);
    int ret = tsg_lua_exec_file(L, script, "hello", strlen("hello"), out, &out_len, &out_type);

    const char *result = "req_header:Host:192.168.40.223:9098,Content-Type:text/markdown,Content-Length:7020,Connection:keep-alive,Accept:*/*,User-Agent: PostmanRuntime/7.26.8,token:c21f969b5f03d33d43e04f8f136e7682,Accept-Encoding:gzip, deflate, br; rsp_header:nil,Content-Length:0,Content-Type:application/xml;charset=UTF-8,Data:Wed, 16 Dec 2020 08:45:55 GMT";
    EXPECT_EQ(0, ret);
    EXPECT_EQ(strlen(result), out_len);
    EXPECT_EQ(STRING, out_type);
    EXPECT_STREQ(result, out);
	tsg_destory_lua(L);
}

TEST(lua_register_function, other)
{
    tsg_lua_handle L = NULL;
    char out[1024];
    size_t out_len = 0;
    size_t out_type = STRING;

    L = tsg_lua_vm_create();
    EXPECT_NE((void *)NULL, L);
    lua_register_function(L, "get", "get_result", get_result);
    lua_register_function(L, "get", "get_nil", get_nil);
    lua_register_function(L, "get", "get_time", get_time);

    const char *script = "./script/get_other.lua";
    memset(out, 0, 1024);
    int ret = tsg_lua_exec_file(L, script, "hello", strlen("hello"), out, &out_len, &out_type);

    const char *result = "result:false, time:305419896, is nil";
    EXPECT_EQ(0, ret);
    EXPECT_EQ(strlen(result), out_len);
    EXPECT_EQ(STRING, out_type);
    EXPECT_STREQ(result, out);
	tsg_destory_lua(L);
}
