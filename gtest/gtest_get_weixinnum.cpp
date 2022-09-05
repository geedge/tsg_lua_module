#include <gtest/gtest.h>
#include "elua.h"

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

TEST(elua_get_weixinnum, exec_cache_script)
{
	elua_vm *L = NULL;
    int script_len;
    int data_len;
    
    char buff[1024];
    struct elua_data output;
    output.len = 1024;
    output.type = STRING;
    output.buff = buff; 

	L = elua_create_vm(NULL);
	const char *script = get_file_to_buffer("./script/handle_weixinnum.lua", &script_len);
	struct elua_script *escript = elua_cache_script(L, script, script_len, 0);
	const char *data = get_file_to_buffer("./script/weixin_id_ios_20200423_004.pcap", &data_len);
	int ret = elua_execute_script(escript, data, data_len, NULL, NULL, &output);
    free((void *)data);
    free((void *)script);

    EXPECT_EQ(0, ret);
    EXPECT_EQ(10, output.len);
    EXPECT_EQ(STRING, output.type);
    EXPECT_STREQ("1955740780", output.buff);
}

TEST(elua_get_weixinnum, exec_cache_script_file)
{
	elua_vm *L = NULL;
    int data_len;
    
    char buff[1024];
    memset(buff, 0, 255);
    struct elua_data output;
    output.len = 1024;
    output.type = STRING;
    output.buff = buff;
	L = elua_create_vm(NULL);
	struct elua_script *escript = elua_cache_script_file(L, "./script/handle_weixinnum.lua", 0);
	const char *data = get_file_to_buffer("./script/weixin_id_ios_20200423_004.pcap", &data_len);
	int ret = elua_execute_script(escript, data, data_len, NULL, NULL, &output);
    free((void *)data);

    EXPECT_EQ(0, ret);
    EXPECT_EQ(10, output.len);
    EXPECT_EQ(STRING, output.type);
    EXPECT_STREQ("1955740780", output.buff);
}