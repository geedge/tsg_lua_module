#include <gtest/gtest.h>
#include "elua.h"

TEST(lua_remove_cmd, normal)
{
	elua_vm *L = elua_create_vm(NULL);
	ASSERT_TRUE(L);
	char buff[1024];
	elua_data out;
	out.type = STRING;
	out.integer = 1024;
	out.string = buff;

	elua_remove_function(L, "print");

	struct elua_script *escript = elua_cache_script_file(L, "./script/remove_print.lua", 0);
	int ret = elua_execute_script(escript, "remove print", strlen("remove print"), NULL, NULL, &out);
	EXPECT_EQ(ret, -1);

	const char *errmsg = "[elua_call_script:1074] [string \"./script/remove_print.lua\"]:3: attempt to call global \'print\' (a nil value).";
	EXPECT_STREQ(elua_get_last_error_string(L), errmsg);
}