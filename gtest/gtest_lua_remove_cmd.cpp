#include <gtest/gtest.h>
#include "tsg_lua_interface.h"

TEST(lua_remove_cmd, normal)
{
	tsg_lua_handle L = tsg_lua_vm_create_with_name("TEST");
	ASSERT_TRUE(L);

	lua_remove_cmd(L, "print");

	char out[1204];
	size_t len = 1024;
	size_t type = STRING;
	int ret = tsg_lua_exec_file(L, "./script/remove_print.lua", "remove print", strlen("remove print"), out, &len, &type);
	EXPECT_EQ(ret, ERR_SCRIPT_EXEC_ERROR);
	
}