#include <gtest/gtest.h>
#include <time.h>
#include "tsg_lua_interface.h"

static long mstime()
{
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    return now.tv_sec * 1000 + now.tv_nsec/1000000;
}

static int count_set(tsg_lua_handle L)
{
	int num = 0;
	for (int i = 0; i < 10000; i++)
	{
		for (int j = 0; j < 1000; j++)
		{
			num++;
		}
	}
	int *count = (int *)lua_get_userdata(L);
	*count = num;
	return 0;
}

TEST(lua_time_out, lua)
{
	tsg_lua_handle L = tsg_lua_vm_create_with_name("TEST");
	
	const char *ud = "hello world.";
    	const char *script = "./script/lua_time_out.lua";
	lua_data_t in;
	in.data = (char *)"This is a test";
	in.len = strlen(in.data);
	lua_arg_t out;
	out.type = STRING;
	out.len = 1024;
	out.str = (char *)calloc(1, 1024);
	long tt, elapsed;

	int ret = lua_exec_file(L, script, in, (void *)ud, NULL, 10, &out);
	EXPECT_EQ(ERR_SCRIPT_TIMEOUT, ret);
	bzero(out.str, 1024);

	tt = mstime();
	ret = lua_exec_file(L, script, in, (void *)ud, NULL, 0, &out);
	elapsed = mstime() - tt;
	EXPECT_EQ(0, ret);
	EXPECT_LE(10, elapsed);
	EXPECT_EQ(in.len, out.len);
	EXPECT_EQ(STRING, out.type);
	EXPECT_STREQ(in.data, out.str);
	bzero(out.str, 1024);

	ret = lua_exec_file(L, script, in, (void *)ud, NULL, 10, &out);
	EXPECT_EQ(ERR_SCRIPT_TIMEOUT, ret);
	bzero(out.str, 1024);

	tsg_destory_lua(L);
	free(out.str);
}

TEST(lua_time_out, c)
{
	tsg_lua_handle L = tsg_lua_vm_create_with_name("TEST");
	
	int count = 10;
    	const char *script = "./script/c_time_out.lua";
	lua_data_t in;
	in.data = (char *)"This is a test";
	in.len = strlen(in.data);
	lua_arg_t out;
	out.type = STRING;
	out.len = 1024;
	out.str = (char *)calloc(1, 1024);
	long tt, elapsed;

	lua_register_function(L, NULL, "set_count", count_set);
	tt = mstime();
	int ret = lua_exec_file(L, script, in, (void *)&count, NULL, 10, &out);
	elapsed = mstime() - tt;
	EXPECT_EQ(ERR_SCRIPT_TIMEOUT, ret);
	EXPECT_LE(10, elapsed);
	EXPECT_EQ(10000000, count);
	bzero(out.str, 1024);

	tt = mstime();
	ret = lua_exec_file(L, script, in, (void *)&count, NULL, 0, &out);
	elapsed = mstime() - tt;
	EXPECT_EQ(0, ret);
	EXPECT_LE(10, elapsed);
	EXPECT_EQ(in.len, out.len);
	EXPECT_EQ(STRING, out.type);
	EXPECT_STREQ(in.data, out.str);
	EXPECT_EQ(10000000, count);
	bzero(out.str, 1024);

	tt = mstime();
	ret = lua_exec_file(L, script, in, (void *)&count, NULL, 10, &out);
	elapsed = mstime() - tt;
	EXPECT_EQ(ERR_SCRIPT_TIMEOUT, ret);
	EXPECT_LE(10, elapsed);
	EXPECT_EQ(10000000, count);
	bzero(out.str, 1024);

	tsg_destory_lua(L);
	free(out.str);
}