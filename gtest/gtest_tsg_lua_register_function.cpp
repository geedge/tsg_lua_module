/*************************************************************************
    > File Name: gtest_elua_create_vm.cpp
    > Author: pxz
    > Created Time: Sun 19 Jul 2020 01:27:13 PM CST
 ************************************************************************/
#include <gtest/gtest.h>
#include "elua.h"

int get_hello(elua_vm *L)
{
    struct elua_data ldata;
    ldata.type = STRING;
    ldata.buff = (char *)"hello";
    ldata.len = 5;
    elua_cbinding_append_output_params(L, &ldata, 1);
    return 1;
}

int set_world(elua_vm *L)
{
    int argc = 0;
    
    argc = elua_cbinding_get_input_params_num(L);
    if (argc <= 0)
    {
        return 0;
    }

    struct elua_data *argv = (struct elua_data *)calloc(argc, sizeof(struct elua_data));

    for (int i = 0; i < argc; i++)
    {
        elua_cbinding_get_input_param(L, i+1, &argv[i]);
    }

    if (argc != 2 || argv[0].type != STRING || argv[1].type != STRING)
    {
        return 0;
    }

    struct elua_data retvalue;
    retvalue.type = STRING;
    retvalue.len = argv[0].len + argv[1].len + 2;
    retvalue.buff = (char *)calloc(1, retvalue.len);
    sprintf(retvalue.buff, "%s %s.", argv[0].buff, argv[1].buff);
    elua_cbinding_append_output_params(L, &retvalue, 1);
    free(retvalue.buff);

    return 1;
}

int get_req_header(elua_vm *L)
{
    const char *key[] = {"token", "User-Agent", "Accept", "Host", "Accept-Encoding", 
                    "Connection", "Content-Length", "Content-Type"};
    const char *value[] = {"c21f969b5f03d33d43e04f8f136e7682", " PostmanRuntime/7.26.8","*/*", "192.168.40.223:9098",
                    "gzip, deflate, br", "keep-alive", "7030", "text/markdown"};

    elua_table *etab = elua_create_table(L);
    struct elua_data ekey, evalue;
    ekey.type = STRING;
    evalue.type = STRING;

    for (int i = 0; i < (int)(sizeof(key) / sizeof(key[0])); i++)
    {
        ekey.len = strlen(key[i]);
        ekey.buff = (char *)key[i];
        evalue.len = strlen(value[i]);
        evalue.buff = (char *)value[i];
        elua_add_table(etab, &ekey, &evalue);
    }

    struct elua_data retvalue;
    retvalue.type = TABLE;
    retvalue.table = etab;
    retvalue.len = sizeof(struct elua_data);

    elua_cbinding_append_output_params(L, &retvalue, 1);
    elua_destroy_table(etab);

    return 1;
}

int get_rsp_header(elua_vm *L)
{
    const char *key[] = {"Content-Type", "Content-Length", "Data"};
    const char *value[] = {"application/xml;charset=UTF-8", "0", "Wed, 16 Dec 2020 08:45:55 GMT"};

    elua_table *etab = elua_create_table(L);
    struct elua_data ekey, evalue;
    ekey.type = STRING;
    evalue.type = STRING;

    for (int i = 0; i < (int)(sizeof(key) / sizeof(key[0])); i++)
    {
        ekey.len = strlen(key[i]);
        ekey.buff = (char *)key[i];
        evalue.len = strlen(value[i]);
        evalue.buff = (char *)value[i];
        elua_add_table(etab, &ekey, &evalue);
    }

    struct elua_data retvalue;
    retvalue.type = TABLE;
    retvalue.table = etab;
    retvalue.len = sizeof(struct elua_data);

    elua_cbinding_append_output_params(L, &retvalue, 1);
    elua_destroy_table(etab);

    return 1;
}

int get_result(elua_vm *L)
{
    struct elua_data retvalue;
    retvalue.type = BOOLEAN;
    retvalue.len = sizeof(bool);
    retvalue.true_or_false = false;
    elua_cbinding_append_output_params(L, &retvalue, 1);
    return 1;
}

int get_nil(elua_vm *L)
{
    struct elua_data retvalue;
    retvalue.type = NIL;
    elua_cbinding_append_output_params(L, &retvalue, 1);
    return 1;   
}

int get_time(elua_vm *L)
{
    struct elua_data retvalue;
    retvalue.type = INTEGER;
    retvalue.len = sizeof(long);
    retvalue.integer = 0x12345678;
    elua_cbinding_append_output_params(L, &retvalue, 1);
    return 1;
}

int set_repeat(elua_vm *L)
{
    struct elua_data retvalue;
    retvalue.type = INTEGER;
    retvalue.len = sizeof(long);
    retvalue.integer = 0x12345678;
    elua_cbinding_append_output_params(L, &retvalue, 1);
    retvalue.type = STRING;
    retvalue.len = 5;
    retvalue.buff = (char *)"hello";
    elua_cbinding_append_output_params(L, &retvalue, 1);

    return 1;
}

int set_multi_type_retvalue(elua_vm *L)
{
    elua_table *etab = elua_create_table(L);
    struct elua_data ekey, evalue;

    char buff[1024];
    bzero(buff, 1024);
    for (int i = 0; i < 3; i++)
    {
        snprintf(buff, 1024, "test%d", i);
        ekey.type = STRING;
        evalue.type = INTEGER;
        ekey.len = 5;
        ekey.buff = buff;
        evalue.len = sizeof(int);
        evalue.integer = i;
        elua_add_table(etab, &ekey, &evalue);

        snprintf(buff, 1024, "test%d", i);
        ekey.type = INTEGER;
        evalue.type = STRING;
        ekey.len = sizeof(int);
        ekey.integer = i;
        evalue.len = 5;
        evalue.buff = buff;
        elua_add_table(etab, &ekey, &evalue);
    }

    struct elua_data retvalue;
    retvalue.type = TABLE;
    retvalue.table = etab;
    retvalue.len = sizeof(struct elua_data);

    elua_cbinding_append_output_params(L, &retvalue, 1);
    elua_destroy_table(etab);

    return 1;
}

TEST(elua_register_cbinding, normal)
{
    elua_vm *L = NULL;
    
    L = elua_create_vm(NULL);
    EXPECT_NE((void *)NULL, L);
    elua_register_cbinding(L, "get", "get_hello", get_hello);
    elua_register_cbinding(L, "set", "set_world", set_world);

    char buff[1024];
    const char *script = "./script/call_cfunction.lua";
    struct elua_script *escript = elua_cache_script_file(L, script, 0);
    struct elua_data output;
    output.type = STRING;
    output.buff = buff;
    output.len = 1024;
    memset(buff, 0, 1024);
    int ret = elua_execute_script(escript, "hello", strlen("hello"), NULL, NULL, &output);

    EXPECT_EQ(0, ret);
    EXPECT_EQ(12, output.len);
    EXPECT_EQ(STRING, output.type);
    EXPECT_STREQ("hello world.", output.buff);
	elua_destroy_vm(L);
}

TEST(elua_register_cbinding, get_header)
{
    elua_vm *L = NULL;

    L = elua_create_vm(NULL);
    EXPECT_NE((void *)NULL, L);
    elua_register_cbinding(L, "req", "get_header", get_req_header);
    elua_register_cbinding(L, "rsp", "get_header", get_rsp_header);

    char buff[1024];
    const char *script = "./script/get_header.lua";
    struct elua_script *escript = elua_cache_script_file(L, script, 0);
    struct elua_data output;
    output.type = STRING;
    output.buff = buff;
    output.len = 1024;
    memset(buff, 0, 1024);
    int ret = elua_execute_script(escript, "hello", strlen("hello"), NULL, NULL, &output);

    const char *result = "req_header:Host:192.168.40.223:9098,Content-Type:text/markdown,Content-Length:7030,Connection:keep-alive,Accept:*/*,User-Agent: PostmanRuntime/7.26.8,token:c21f969b5f03d33d43e04f8f136e7682,Accept-Encoding:gzip, deflate, br; rsp_header:Content-Length:0,Content-Type:application/xml;charset=UTF-8,Data:Wed, 16 Dec 2020 08:45:55 GMT";
    EXPECT_EQ(0, ret);
    EXPECT_EQ(strlen(result), output.len);
    EXPECT_EQ(STRING, output.type);
    EXPECT_STREQ(result, output.buff);
	elua_destroy_vm(L);
}

TEST(elua_register_cbinding, other)
{
    elua_vm *L = NULL;

    L = elua_create_vm(NULL);
    EXPECT_NE((void *)NULL, L);
    elua_register_cbinding(L, "get", "get_result", get_result);
    elua_register_cbinding(L, "get", "get_nil", get_nil);
    elua_register_cbinding(L, "get", "get_time", get_time);

    const char *script = "./script/get_other.lua";
    char buff[1024];
    struct elua_script *escript = elua_cache_script_file(L, script, 0);
    struct elua_data output;
    output.type = STRING;
    output.buff = buff;
    output.len = 1024;
    memset(buff, 0, 1024);
    int ret = elua_execute_script(escript, "hello", strlen("hello"), NULL, NULL, &output);


    const char *result = "result:false, time:305419896, is nil";
    EXPECT_EQ(0, ret);
    EXPECT_EQ(strlen(result), output.len);
    EXPECT_EQ(STRING, output.type);
    EXPECT_STREQ(result, output.buff);
	elua_destroy_vm(L);
}

TEST(elua_register_cbinding, invalid_args)
{
    elua_vm *L = NULL;
    
    L = elua_create_vm(NULL);
    EXPECT_NE((void *)NULL, L);
    elua_register_cbinding(L, "get", "get_hello", get_hello);
    elua_register_cbinding(L, "set", "set_world", set_world);

    char buff[1024];
    const char *script = "./script/call_cfunction_invalid_args.lua";
    struct elua_script *escript = elua_cache_script_file(L, script, 0);
    struct elua_data output;
    output.type = STRING;
    output.buff = buff;
    output.len = 1024;
    memset(buff, 0, 1024);
    int ret = elua_execute_script(escript, "hello", strlen("hello"), NULL, NULL, &output);

    EXPECT_EQ(0, ret);
    EXPECT_EQ(12, output.len);
    EXPECT_EQ(STRING, output.type);
    EXPECT_STREQ("hello world.", output.buff);
	elua_destroy_vm(L);
}

TEST(elua_register_cbinding, set_repeat)
{
    elua_vm *L = NULL;
    
    L = elua_create_vm(NULL);
    EXPECT_NE((void *)NULL, L);
    elua_register_cbinding(L, NULL, "set_repeat", set_repeat);

    char buff[1024];
    const char *script = "./script/call_set_repeat.lua";
    struct elua_script *escript = elua_cache_script_file(L, script, 0);
    struct elua_data output;
    output.type = STRING;
    output.buff = buff;
    output.len = 1024;
    memset(buff, 0, 1024);
    int ret = elua_execute_script(escript, "hello", strlen("hello"), NULL, NULL, &output);

    EXPECT_EQ(0, ret);
    EXPECT_EQ(5, output.len);
    EXPECT_EQ(STRING, output.type);
    EXPECT_STREQ("hello", output.buff);
	elua_destroy_vm(L);
}

TEST(elua_register_cbinding, get_value)
{
    elua_vm *L = NULL;
    
    L = elua_create_vm(NULL);
    EXPECT_NE((void *)NULL, L);
    elua_register_cbinding(L, NULL, "get_value", set_multi_type_retvalue);

    char buff[1024];
    const char *script = "./script/call_cfunction_get_value.lua";
    struct elua_script *escript = elua_cache_script_file(L, script, 0);
    struct elua_data output;
    output.type = STRING;
    output.buff = buff;
    output.len = 1024;
    memset(buff, 0, 1024);
    int ret = elua_execute_script(escript, "hello", strlen("hello"), NULL, NULL, &output);

    EXPECT_EQ(0, ret);
    printf(elua_get_last_error_string(L));
    EXPECT_EQ(47, output.len);
    EXPECT_EQ(STRING, output.type);
    EXPECT_STREQ("0:test0,1:test1,2:test2,test1:1,test0:0,test2:2", output.buff);
	elua_destroy_vm(L);
}