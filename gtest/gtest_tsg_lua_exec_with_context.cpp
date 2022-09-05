#include <gtest/gtest.h>
#include "elua.h"

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

int get_userdata(elua_vm *L)
{
	void *ud = elua_get_execute_userdata(L);
	struct elua_data edata;
	edata.type = STRING;
	edata.string = (char *)ud;
	edata.len = strlen((char *)ud);
	elua_cbinding_set_output_params(L, &edata, 1);
	return 1;
}

TEST(lua_execute_file, normal)
{
	elua_vm *L = elua_create_vm("TEST");
	
	const char *ud = "hello world.";
    	const char *script = "./script/exec_with_context.lua";
	char *input = (char *)"This is a test";
	int len = strlen(input);
	struct elua_data out;
	out.type = STRING;
	out.len = 1024;
	out.string = (char *)calloc(1, 1024);

	elua_register_cbinding(L, "get", "get_userdata", get_userdata);
	elua_context *context = NULL;
	context = elua_create_context(L, "context");
	struct elua_script *escript = elua_cache_script_file(L, script, 0);
	int ret = elua_execute_script(escript, input, len, (void *)ud, context, &out);
	const char *str = "userdata:hello world., context.count:1, context.message:This is first called";
	EXPECT_EQ(0, ret);
	EXPECT_EQ(strlen(str), out.len);
	EXPECT_EQ(STRING, out.type);
	EXPECT_STREQ(str, out.string);
	bzero(out.string, 1024);

	ud = "hello lua.";
	ret = elua_execute_script(escript, input, len, (void *)ud, context, &out);
	str = "userdata:hello lua., context.count:2, context.message:This is not first called";
	EXPECT_EQ(0, ret);
	EXPECT_EQ(strlen(str), out.len);
	EXPECT_EQ(STRING, out.type);
	EXPECT_STREQ(str, out.string);
	bzero(out.string, 1024);

	elua_context *context2 = NULL;
	context2 = elua_create_context(L, "context");
	ret = elua_execute_script(escript, input, len, (void *)ud, context2, &out);
	str = "userdata:hello lua., context.count:1, context.message:This is first called";
	EXPECT_EQ(0, ret);
	EXPECT_EQ(strlen(str), out.len);
	EXPECT_EQ(STRING, out.type);
	EXPECT_STREQ(str, out.string);
	bzero(out.string, 1024);

	elua_destroy_context(context);
	elua_destroy_context(context2);
	elua_destroy_vm(L);
	free(out.string);
}

TEST(lua_exec, normal)
{
	elua_vm *L = elua_create_vm("TEST");
	
	const char *ud = "hello world.";
	char *input = (char *)"This is a test";
	int len = strlen(input);
	struct elua_data out;
	out.type = STRING;
	out.integer = 1024;
	out.string = (char *)calloc(1, 1024);
	char *script = get_script_buffer("./script/exec_with_context.lua");
	int script_len = strlen(script);
	elua_script *escript = elua_cache_script(L, script, script_len, 0);

	elua_register_cbinding(L, "get", "get_userdata", get_userdata);
	elua_context *context = NULL;
	context = elua_create_context(L, "context");
	int ret = elua_execute_script(escript, input, len, (void *)ud, context, &out);
	const char *str = "userdata:hello world., context.count:1, context.message:This is first called";
	EXPECT_EQ(0, ret);
	EXPECT_EQ(strlen(str), out.len);
	EXPECT_EQ(STRING, out.type);
	EXPECT_STREQ(str, out.string);
	bzero(out.string, 1024);

	ud = "hello lua.";
	ret = elua_execute_script(escript, input, len, (void *)ud, context, &out);
	str = "userdata:hello lua., context.count:2, context.message:This is not first called";
	EXPECT_EQ(0, ret);
	EXPECT_EQ(strlen(str), out.len);
	EXPECT_EQ(STRING, out.type);
	EXPECT_STREQ(str, out.string);
	bzero(out.string, 1024);

	elua_context *context2 = NULL;
	context2 = elua_create_context(L, "context");
	ret = elua_execute_script(escript, input, len, (void *)ud, context2, &out);
	str = "userdata:hello lua., context.count:1, context.message:This is first called";
	EXPECT_EQ(0, ret);
	EXPECT_EQ(strlen(str), out.len);
	EXPECT_EQ(STRING, out.type);
	EXPECT_STREQ(str, out.string);
	bzero(out.string, 1024);

	elua_destroy_context(context);
	elua_destroy_context(context2);
	elua_destroy_vm(L);
	free(out.string);
}

TEST(elua_execute_script, normal)
{
	elua_vm *L = elua_create_vm("TEST");
	
	const char *ud = "hello world.";
	char *input = (char *)"This is a test";
	int len = strlen(input);
	struct elua_data out;
	out.type = STRING;
	out.integer = 1024;
	out.string = (char *)calloc(1, 1024);
	const char *script = get_script_buffer("./script/exec_with_context.lua");
	struct elua_script *escript = elua_cache_script(L, script, strlen(script), 0);

	elua_register_cbinding(L, "get", "get_userdata", get_userdata);
	elua_context *context = NULL;
	context = elua_create_context(L, "context");
	int ret = elua_execute_script(escript, input, len, (void *)ud, context, &out);
	const char *str = "userdata:hello world., context.count:1, context.message:This is first called";
	EXPECT_EQ(0, ret);
	EXPECT_EQ(strlen(str), out.len);
	EXPECT_EQ(STRING, out.type);
	EXPECT_STREQ(str, out.string);
	bzero(out.string, 1024);

	ud = "hello lua.";
	ret = elua_execute_script(escript, input, len, (void *)ud, context, &out);
	str = "userdata:hello lua., context.count:2, context.message:This is not first called";
	EXPECT_EQ(0, ret);
	EXPECT_EQ(strlen(str), out.len);
	EXPECT_EQ(STRING, out.type);
	EXPECT_STREQ(str, out.string);
	bzero(out.string, 1024);

	elua_context *context2 = NULL;
	context2 = elua_create_context(L, "context");
	ret = elua_execute_script(escript, input, len, (void *)ud, context2, &out);
	str = "userdata:hello lua., context.count:1, context.message:This is first called";
	EXPECT_EQ(0, ret);
	EXPECT_EQ(strlen(str), out.len);
	EXPECT_EQ(STRING, out.type);
	EXPECT_STREQ(str, out.string);
	bzero(out.string, 1024);

	elua_destroy_context(context);
	elua_destroy_context(context2);
	elua_destroy_vm(L);
	free(out.string);
}
