#include <gtest/gtest.h>
#include <time.h>
#include "elua.h"

static long mstime()
{
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    return now.tv_sec * 1000 + now.tv_nsec/1000000;
}

static int count_set(elua_vm *L)
{
	int num = 0;
	for (int i = 0; i < 10000; i++)
	{
		for (int j = 0; j < 1000; j++)
		{
			num++;
		}
	}
	int *count = (int *)elua_get_execute_userdata(L);
	*count = num;
	return 0;
}

TEST(elua_time_out, lua)
{
	elua_vm *L = elua_create_vm("TEST");
	
	const char *ud = "hello world.";
    	const char *script = "./script/elua_time_out.lua";
	
	char *buff= (char *)"This is a test";
	int  len = strlen(buff);
	elua_data out;
	out.type = STRING;
	out.integer = 1024;
	out.string = (char *)calloc(1, 1024);
	long tt, elapsed;
	int ret = 0;

	struct elua_script *escript = elua_cache_script_file(L, script, 10);

	ret = elua_execute_script(escript, buff, len, (void *)ud, NULL, &out);
	EXPECT_EQ(ret, -1);
	EXPECT_STREQ(elua_get_last_error_string(L), "[elua_call_script:1074] Lua script killed by time out..");
	bzero(out.string, 1024);

	struct elua_script *escript0 = elua_cache_script_file(L, script, 0);
	tt = mstime();
	ret = elua_execute_script(escript0, buff, len, (void *)ud, NULL, &out);
	elapsed = mstime() - tt;
	EXPECT_EQ(0, ret);
	EXPECT_LE(10, elapsed);
	EXPECT_EQ(len, out.len);
	EXPECT_EQ(STRING, out.type);
	EXPECT_STREQ(buff, out.string);
	bzero(out.string, 1024);

	ret = elua_execute_script(escript, buff, len, (void *)ud, NULL, &out);
	EXPECT_EQ(ret, -1);
	EXPECT_STREQ(elua_get_last_error_string(L), "[elua_call_script:1074] Lua script killed by time out..");
	bzero(out.string, 1024);

	elua_cleanup_script(escript);
	elua_cleanup_script(escript0);
	elua_destroy_vm(L);
	free(out.string);
}

TEST(elua_time_out, c)
{
	elua_vm *L = elua_create_vm("TEST");
	
	int count = 10;
    	const char *script = "./script/c_time_out.lua";
	
	char *buff= (char *)"This is a test";
	int  len = strlen(buff);
	
	struct elua_data out;
	out.type = STRING;
	out.integer = 1024;
	out.string = (char *)calloc(1, 1024);
	long tt, elapsed;

	elua_register_cbinding(L, NULL, "set_count", count_set);
	struct elua_script *escript = elua_cache_script_file(L, script, 10);
	tt = mstime();
	int ret = elua_execute_script(escript, buff, len, (void *)&count, NULL, &out);
	elapsed = mstime() - tt;
	EXPECT_EQ(-1, ret);
	EXPECT_STREQ(elua_get_last_error_string(L), "[elua_call_script:1074] Lua script killed by time out..");
	EXPECT_LE(10, elapsed);
	EXPECT_EQ(10000000, count);
	bzero(out.string, 1024);

	struct elua_script *escript0 = elua_cache_script_file(L, script, 0);
	tt = mstime();
	ret = elua_execute_script(escript0, buff, len, (void *)&count, NULL, &out);
	elapsed = mstime() - tt;
	EXPECT_EQ(0, ret);
	EXPECT_LE(10, elapsed);
	EXPECT_EQ(len, out.len);
	EXPECT_EQ(STRING, out.type);
	EXPECT_STREQ(buff, out.string);
	EXPECT_EQ(10000000, count);
	bzero(out.string, 1024);

	tt = mstime();
	ret = elua_execute_script(escript, buff, len, (void *)&count, NULL, &out);
	elapsed = mstime() - tt;
	EXPECT_EQ(-1, ret);
	EXPECT_STREQ(elua_get_last_error_string(L), "[elua_call_script:1074] Lua script killed by time out..");
	EXPECT_LE(10, elapsed);
	EXPECT_EQ(10000000, count);
	bzero(out.string, 1024);

	elua_cleanup_script(escript0);
	elua_cleanup_script(escript);
	elua_destroy_vm(L);
	free(out.string);
}