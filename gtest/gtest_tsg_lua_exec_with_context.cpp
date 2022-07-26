#include <gtest/gtest.h>
#include "tsg_lua_interface.h"

static char buffer[4096];
static char *get_script_buffer(const char *script)
{
    FILE *fp = fopen(script, "r");
    if (fp)
    {
        int num = fread(buffer, 1, 4096, fp);
        if (num != 0)
        {
            buffer[num] = 0;
            fclose(fp);
            return buffer;
        }
        fclose(fp);
    }
    return NULL;
}

int get_userdata(tsg_lua_handle L)
{
	void *ud = lua_get_userdata(L);
	c_push_string_into_lua(L, (const char *)ud, strlen((const char *)ud));
	return 1;
}

TEST(lua_exec_file, normal)
{
	tsg_lua_handle L = tsg_lua_vm_create_with_name("TEST");
	
	const char *ud = "hello world.";
    	const char *script = "./script/exec_with_context.lua";
	lua_data_t in;
	in.data = (char *)"This is a test";
	in.len = strlen(in.data);
	lua_arg_t out;
	out.type = STRING;
	out.len = 1024;
	out.str = (char *)calloc(1, 1024);

	lua_register_function(L, "get", "get_userdata", get_userdata);
	lua_script_context context = NULL;
	context = lua_script_context_malloc(L);
	int ret = lua_exec_file(L, script, in, (void *)ud, context, 0, &out);
	const char *str = "userdata:hello world., context.count:1, context.message:This is first called";
	EXPECT_EQ(0, ret);
	EXPECT_EQ(strlen(str), out.len);
	EXPECT_EQ(STRING, out.type);
	EXPECT_STREQ(str, out.str);
	bzero(out.str, 1024);

	ud = "hello lua.";
	ret = lua_exec_file(L, script, in, (void *)ud, context, 0, &out);
	str = "userdata:hello lua., context.count:2, context.message:This is not first called";
	EXPECT_EQ(0, ret);
	EXPECT_EQ(strlen(str), out.len);
	EXPECT_EQ(STRING, out.type);
	EXPECT_STREQ(str, out.str);
	bzero(out.str, 1024);

	lua_script_context context2 = NULL;
	context2 = lua_script_context_malloc(L);
	ret = lua_exec_file(L, script, in, (void *)ud, context2, 0, &out);
	str = "userdata:hello lua., context.count:1, context.message:This is first called";
	EXPECT_EQ(0, ret);
	EXPECT_EQ(strlen(str), out.len);
	EXPECT_EQ(STRING, out.type);
	EXPECT_STREQ(str, out.str);
	bzero(out.str, 1024);

	lua_script_context_free(L, context);
	lua_script_context_free(L, context2);
	tsg_destory_lua(L);
	free(out.str);
}

TEST(lua_exec, normal)
{
	tsg_lua_handle L = tsg_lua_vm_create_with_name("TEST");
	
	const char *ud = "hello world.";
	lua_data_t in;
	in.data = (char *)"This is a test";
	in.len = strlen(in.data);
	lua_arg_t out;
	out.type = STRING;
	out.len = 1024;
	out.str = (char *)calloc(1, 1024);
	lua_data_t script;
	script.data = get_script_buffer("./script/exec_with_context.lua");
	script.len = strlen(script.data);

	lua_register_function(L, "get", "get_userdata", get_userdata);
	lua_script_context context = NULL;
	context = lua_script_context_malloc(L);
	int ret = lua_exec(L, script, in, (void *)ud, context, 0, &out);
	const char *str = "userdata:hello world., context.count:1, context.message:This is first called";
	EXPECT_EQ(0, ret);
	EXPECT_EQ(strlen(str), out.len);
	EXPECT_EQ(STRING, out.type);
	EXPECT_STREQ(str, out.str);
	bzero(out.str, 1024);

	ud = "hello lua.";
	ret = lua_exec(L, script, in, (void *)ud, context, 0, &out);
	str = "userdata:hello lua., context.count:2, context.message:This is not first called";
	EXPECT_EQ(0, ret);
	EXPECT_EQ(strlen(str), out.len);
	EXPECT_EQ(STRING, out.type);
	EXPECT_STREQ(str, out.str);
	bzero(out.str, 1024);

	lua_script_context context2 = NULL;
	context2 = lua_script_context_malloc(L);
	ret = lua_exec(L, script, in, (void *)ud, context2, 0, &out);
	str = "userdata:hello lua., context.count:1, context.message:This is first called";
	EXPECT_EQ(0, ret);
	EXPECT_EQ(strlen(str), out.len);
	EXPECT_EQ(STRING, out.type);
	EXPECT_STREQ(str, out.str);
	bzero(out.str, 1024);

	lua_script_context_free(L, context);
	lua_script_context_free(L, context2);
	tsg_destory_lua(L);
	free(out.str);
}

TEST(lua_cache_exec, normal)
{
	tsg_lua_handle L = tsg_lua_vm_create_with_name("TEST");
	
	const char *ud = "hello world.";
	lua_data_t in;
	in.data = (char *)"This is a test";
	in.len = strlen(in.data);
	lua_arg_t out;
	out.type = STRING;
	out.len = 1024;
	out.str = (char *)calloc(1, 1024);
	const char *script = get_script_buffer("./script/exec_with_context.lua");
	int script_id = tsg_lua_cache_script(L, script, strlen(script));

	lua_register_function(L, "get", "get_userdata", get_userdata);
	lua_script_context context = NULL;
	context = lua_script_context_malloc(L);
	int ret = lua_cache_exec(L, script_id, in, (void *)ud, context, 0, &out);
	const char *str = "userdata:hello world., context.count:1, context.message:This is first called";
	EXPECT_EQ(0, ret);
	EXPECT_EQ(strlen(str), out.len);
	EXPECT_EQ(STRING, out.type);
	EXPECT_STREQ(str, out.str);
	bzero(out.str, 1024);

	ud = "hello lua.";
	ret = lua_cache_exec(L, script_id, in, (void *)ud, context, 0, &out);
	str = "userdata:hello lua., context.count:2, context.message:This is not first called";
	EXPECT_EQ(0, ret);
	EXPECT_EQ(strlen(str), out.len);
	EXPECT_EQ(STRING, out.type);
	EXPECT_STREQ(str, out.str);
	bzero(out.str, 1024);

	lua_script_context context2 = NULL;
	context2 = lua_script_context_malloc(L);
	ret = lua_cache_exec(L, script_id, in, (void *)ud, context2, 0, &out);
	str = "userdata:hello lua., context.count:1, context.message:This is first called";
	EXPECT_EQ(0, ret);
	EXPECT_EQ(strlen(str), out.len);
	EXPECT_EQ(STRING, out.type);
	EXPECT_STREQ(str, out.str);
	bzero(out.str, 1024);

	lua_script_context_free(L, context);
	lua_script_context_free(L, context2);
	tsg_destory_lua(L);
	free(out.str);
}
