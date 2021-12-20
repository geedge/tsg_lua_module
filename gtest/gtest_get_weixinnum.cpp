#include <gtest/gtest.h>
#include "tsg_lua_interface.h"

static char *get_file_to_buffer(const char *file, int *len)
{
    FILE *fp = fopen(file, "r");
    if (fp)
    {
	    fseek(fp, 0, 2);
        *len = ftell(fp);
        fseek(fp, 0, 0);
        if (*len <= 0)
        {
            fclose(fp);
            return NULL;
        }
        char *buff = (char *)malloc((*len) + 1);
        int num = fread(buff, 1, *len, fp);
        if (num != 0)
        {
            buff[num] = 0;
            *len = num - 1;
            fclose(fp);
            return buff;
        }
        free(buff);
        fclose(fp);
    }
    return NULL;
}

TEST(tsg_lua_get_weixinnum, exec_cache_script)
{
	tsg_lua_handle L = NULL;
    int script_len;
    int data_len;
    char out[255];
    size_t out_len = 0;
    size_t out_type = 0;
    memset(out, 0, 255);

	L = tsg_lua_vm_create();
	const char *script = get_file_to_buffer("./script/handle_weixinnum.lua", &script_len);
	int script_id = tsg_lua_cache_script(L, script, script_len);
	const char *data = get_file_to_buffer("./script/weixin_id_ios_20200423_004.pcap", &data_len);
	int ret = tsg_lua_cache_exec(L, script_id, data, data_len, out, &out_len, &out_type);
    free((void *)data);
    free((void *)script);

    EXPECT_EQ(0, ret);
    EXPECT_EQ(10, out_len);
    EXPECT_EQ(STRING, out_type);
    EXPECT_STREQ("1955740780", out);
}

TEST(tsg_lua_get_weixinnum, exec_cache_file)
{
	tsg_lua_handle L = NULL;
    int data_len;
    char out[255];
    size_t out_len = 0;
    size_t out_type = 0;

    memset(out, 0, 255);
	L = tsg_lua_vm_create();
	int script_id = tsg_lua_cache_script_file(L, "./script/handle_weixinnum.lua");
	const char *data = get_file_to_buffer("./script/weixin_id_ios_20200423_004.pcap", &data_len);
	int ret = tsg_lua_cache_exec(L, script_id, data, data_len, out, &out_len, &out_type);
    free((void *)data);

    EXPECT_EQ(0, ret);
    EXPECT_EQ(10, out_len);
    EXPECT_EQ(STRING, out_type);
    EXPECT_STREQ("1955740780", out);
}

TEST(tsg_lua_get_weixinnum, exec_script)
{
	tsg_lua_handle L = NULL;
    int script_len;
    int data_len;
    char out[255];
    size_t out_len = 0;
    size_t out_type = 0;

    memset(out, 0, 255);
	L = tsg_lua_vm_create();
	const char *script = get_file_to_buffer("./script/handle_weixinnum.lua", &script_len);
	const char *data = get_file_to_buffer("./script/weixin_id_ios_20200423_004.pcap", &data_len);
	int ret = tsg_lua_exec(L, script, script_len, data, data_len, out, &out_len, &out_type);
    free((void *)data);
    free((void *)script);

    EXPECT_EQ(0, ret);
    EXPECT_EQ(10, out_len);
    EXPECT_EQ(STRING, out_type);
    EXPECT_STREQ("1955740780", out);
}

TEST(tsg_lua_get_weixinnum, exec_file)
{
	tsg_lua_handle L = NULL;
    int data_len;
    char out[255];
    size_t out_len = 0;
    size_t out_type = 0;

    memset(out, 0, 255);
	L = tsg_lua_vm_create();
	const char *script = "./script/handle_weixinnum.lua";
	const char *data = get_file_to_buffer("./script/weixin_id_ios_20200423_004.pcap", &data_len);
	int ret = tsg_lua_exec_file(L, script, data, data_len, out, &out_len, &out_type);
    free((void *)data);

    EXPECT_EQ(0, ret);
    EXPECT_EQ(10, out_len);
    EXPECT_EQ(STRING, out_type);
    EXPECT_STREQ("1955740780", out);
}