/*************************************************************************
    > File Name: gtest_elua_cache_script_file.cpp
    > Author: pxz
    > Created Time: Sun 19 Jul 2020 01:28:20 PM CST
 ************************************************************************/
#include <gtest/gtest.h>
#include "elua.h"

/* elua_cache_script_file */
TEST(elua_cache_script_file, normal)
{
    elua_vm *L = NULL;

    L = elua_create_vm(NULL);
    const char *script = "./script/return_string_normal.lua";
    struct elua_script *escript = elua_cache_script_file(L, script, 0);
    
    /* check */
    EXPECT_NE((void *)NULL, escript);
}

TEST(elua_cache_script_file, lua_vm_isnull)
{
    elua_vm *L = NULL;

    const char *script = "./script/return_string_normal.lua";
    struct elua_script *escript = elua_cache_script_file(L, script, 0);
    
    /* check */
    EXPECT_EQ((void *)NULL, escript);
    EXPECT_STREQ(elua_get_last_error_string(L), "The vm is invalid.");
}

TEST(elua_cache_script_file, script_isnull)
{
    elua_vm *L = NULL;

    L = elua_create_vm(NULL);
    const char *script = NULL;
    struct elua_script *escript = elua_cache_script_file(L, script, 0);
    
    /* check */
    EXPECT_EQ((void *)NULL, escript);
    EXPECT_STREQ(elua_get_last_error_string(L), "[elua_cache_script_file:765] Parameter error.");
}

TEST(elua_cache_script_file, do_expected)
{
    elua_vm *L = NULL;

    L = elua_create_vm(NULL);
    const char *script = "./script/do_expected.lua";
    struct elua_script *escript = elua_cache_script_file(L, script, 0);
    
    /* check */
    EXPECT_EQ((void *)NULL, escript);
    EXPECT_STREQ(elua_get_last_error_string(L), "[elua_cache_script_file:844] [string \"./script/do_expected.lua\"]:3: 'do' expected near 'return'.");
}

TEST(elua_cache_script_file, then_expected)
{
    elua_vm *L = NULL;

    L = elua_create_vm(NULL);
    const char *script = "./script/then_expected.lua";
    struct elua_script *escript = elua_cache_script_file(L, script, 0);
    
    /* check */
    EXPECT_EQ((void *)NULL, escript);
    EXPECT_STREQ(elua_get_last_error_string(L), "[elua_cache_script_file:844] [string \"./script/then_expected.lua\"]:2: 'then' expected near 'str'.");
}

TEST(elua_cache_script_file, end_expected)
{
    elua_vm *L = NULL;

    L = elua_create_vm(NULL);
    const char *script = "./script/end_expected.lua";
    struct elua_script *escript = elua_cache_script_file(L, script, 0);
    
    /* check */
    EXPECT_EQ((void *)NULL, escript);
    EXPECT_STREQ(elua_get_last_error_string(L), "[elua_cache_script_file:844] [string \"./script/end_expected.lua\"]:5: 'end' expected (to close 'function' at line 1) near '<eof>'.");
}

TEST(elua_cache_script_file, eof_expected)
{
    elua_vm *L = NULL;

    L = elua_create_vm(NULL);
    const char *script = "./script/eof_expected.lua";
    struct elua_script *escript = elua_cache_script_file(L, script, 0);
    
    /* check */
    EXPECT_EQ((void *)NULL, escript);
    EXPECT_STREQ(elua_get_last_error_string(L), "[elua_cache_script_file:844] [string \"./script/eof_expected.lua\"]:6: '<eof>' expected near 'end'.");
}

TEST(elua_cache_script_file, equal_in_expected)
{
    elua_vm *L = NULL;

    L = elua_create_vm(NULL);
    const char *script = "./script/equal_in_expected.lua";
    struct elua_script *escript = elua_cache_script_file(L, script, 0);
    
    /* check */
    EXPECT_EQ((void *)NULL, escript);
    EXPECT_STREQ(elua_get_last_error_string(L), "[elua_cache_script_file:844] [string \"./script/equal_in_expected.lua\"]:3: '=' or 'in' expected near 'return'.");
}

TEST(elua_cache_script_file, unexpected_symbol)
{
    elua_vm *L = NULL;

    L = elua_create_vm(NULL);
    const char *script = "./script/unexpected_symbol.lua";
    struct elua_script *escript = elua_cache_script_file(L, script, 0);
    
    /* check */
    EXPECT_EQ((void *)NULL, escript);
    EXPECT_STREQ(elua_get_last_error_string(L), "[elua_cache_script_file:844] [string \"./script/unexpected_symbol.lua\"]:1: unexpected symbol near '-'.");
}

TEST(elua_cache_script_file, name_expected)
{
    elua_vm *L = NULL;

    L = elua_create_vm(NULL);
    const char *script = "./script/name_expected.lua";
    struct elua_script *escript = elua_cache_script_file(L, script, 0);
    
    /* check */
    EXPECT_EQ((void *)NULL, escript);
    EXPECT_STREQ(elua_get_last_error_string(L), "[elua_cache_script_file:844] [string \"./script/name_expected.lua\"]:2: '<name>' expected near 'do'.");
}

TEST(elua_cache_script_file, right_curves_bracket_expected)
{
    elua_vm *L = NULL;

    L = elua_create_vm(NULL);
    const char *script = "./script/right_curves_bracket_expected.lua";
    struct elua_script *escript = elua_cache_script_file(L, script, 0);
    
    /* check */
    EXPECT_EQ((void *)NULL, escript);
    EXPECT_STREQ(elua_get_last_error_string(L), "[elua_cache_script_file:844] [string \"./script/right_curves_bracket_expected.lua\"]:2: ')' expected (to close '(' at line 1) near 'return'.");
}

TEST(elua_cache_script_file, right_curly_bracket_expected)
{
    elua_vm *L = NULL;

    L = elua_create_vm(NULL);
    const char *script = "./script/right_curly_bracket_expected.lua";
    struct elua_script *escript = elua_cache_script_file(L, script, 0);
    
    /* check */
    EXPECT_EQ((void *)NULL, escript);
    EXPECT_STREQ(elua_get_last_error_string(L), "[elua_cache_script_file:844] [string \"./script/right_curly_bracket_expected.lua\"]:3: '}' expected near ')'.");
}

TEST(elua_cache_script_file, sharp)
{
    elua_vm *L = NULL;

    L = elua_create_vm(NULL);
    const char *script = "./script/return_string_normal_sharp.lua";
    struct elua_script *escript = elua_cache_script_file(L, script, 0);
    
    /* check */
    EXPECT_NE((void *)NULL, escript);
}