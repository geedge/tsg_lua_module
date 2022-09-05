/*************************************************************************
    > File Name: gtest_elua_cache_script.cpp
    > Author: pxz
    > Created Time: Sun 19 Jul 2020 01:28:20 PM CST
 ************************************************************************/
#include <gtest/gtest.h>
#include "elua.h"

static size_t script_len;
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
            script_len = num - 1;
            fclose(fp);
            return buffer;
        }
        fclose(fp);
    }
    return NULL;
}
/* elua_cache_script */
TEST(elua_cache_script, normal)
{
    elua_vm *L = NULL;

    L = elua_create_vm(NULL);
    const char *script = get_script_buffer("./script/return_string_normal.lua");
    struct elua_script *escript = elua_cache_script(L, script, script_len, 0);
    
    /* check */
    EXPECT_NE((void *)NULL, escript);
}

TEST(elua_cache_script, lua_vm_isnull)
{
    elua_vm *L = NULL;

    const char *script = get_script_buffer("./script/return_string_normal.lua");
    struct elua_script *escript = elua_cache_script(L, script, script_len, 0);
    
    /* check */
    EXPECT_EQ((void *)NULL, escript);
    EXPECT_STREQ(elua_get_last_error_string(L), "The vm is invalid.");
}

TEST(elua_cache_script, script_isnull)
{
    elua_vm *L = NULL;

    L = elua_create_vm(NULL);
    const char *script = NULL;
    struct elua_script *escript = elua_cache_script(L, script, script_len, 0);
    
    /* check */
    EXPECT_EQ((void *)NULL, escript);
    EXPECT_STREQ(elua_get_last_error_string(L), "[elua_cache_script:703] Parameter error.");
}
TEST(elua_cache_script, do_expected)
{
    elua_vm *L = NULL;

    L = elua_create_vm(NULL);
    const char *script = get_script_buffer("./script/do_expected.lua");
    struct elua_script *escript = elua_cache_script(L, script, script_len, 0);
    
    /* check */
    EXPECT_EQ((void *)NULL, escript);
    EXPECT_STREQ(elua_get_last_error_string(L), "[elua_load_buff:667] [string \"string\"]:3: 'do' expected near 'return'.");
}

TEST(elua_cache_script, then_expected)
{
    elua_vm *L = NULL;

    L = elua_create_vm(NULL);
    const char *script = get_script_buffer("./script/then_expected.lua");
    struct elua_script *escript = elua_cache_script(L, script, script_len, 0);
    
    /* check */
    EXPECT_EQ((void *)NULL, escript);
    EXPECT_STREQ(elua_get_last_error_string(L), "[elua_load_buff:667] [string \"string\"]:2: 'then' expected near 'str'.");
}

TEST(elua_cache_script, end_expected)
{
    elua_vm *L = NULL;

    L = elua_create_vm(NULL);
    const char *script = get_script_buffer("./script/end_expected.lua");
    struct elua_script *escript = elua_cache_script(L, script, script_len, 0);
    
    /* check */
    EXPECT_EQ((void *)NULL, escript);
    EXPECT_STREQ(elua_get_last_error_string(L), "[elua_load_buff:667] [string \"string\"]:4: 'end' expected (to close 'function' at line 1) near '<eof>'.");
}

TEST(elua_cache_script, eof_expected)
{
    elua_vm *L = NULL;

    L = elua_create_vm(NULL);
    const char *script = get_script_buffer("./script/eof_expected.lua");
    struct elua_script *escript = elua_cache_script(L, script, script_len, 0);
    
    /* check */
    EXPECT_EQ((void *)NULL, escript);
    EXPECT_STREQ(elua_get_last_error_string(L), "[elua_load_buff:667] [string \"string\"]:5: '<eof>' expected near 'end'.");
}

TEST(elua_cache_script, equal_in_expected)
{
    elua_vm *L = NULL;

    L = elua_create_vm(NULL);
    const char *script = get_script_buffer("./script/equal_in_expected.lua");
    struct elua_script *escript = elua_cache_script(L, script, script_len, 0);
    
    /* check */
    EXPECT_EQ((void *)NULL, escript);
    EXPECT_STREQ(elua_get_last_error_string(L), "[elua_load_buff:667] [string \"string\"]:3: '=' or 'in' expected near 'return'.");
}

TEST(elua_cache_script, unexpected_symbol)
{
    elua_vm *L = NULL;

    L = elua_create_vm(NULL);
    const char *script = get_script_buffer("./script/unexpected_symbol.lua");
    struct elua_script *escript = elua_cache_script(L, script, script_len, 0);
    
    /* check */
    EXPECT_EQ((void *)NULL, escript);
    EXPECT_STREQ(elua_get_last_error_string(L), "[elua_load_buff:667] [string \"string\"]:1: unexpected symbol near '-'.");
}

TEST(elua_cache_script, name_expected)
{
    elua_vm *L = NULL;

    L = elua_create_vm(NULL);
    const char *script = get_script_buffer("./script/name_expected.lua");
    struct elua_script *escript = elua_cache_script(L, script, script_len, 0);
    
    /* check */
    EXPECT_EQ((void *)NULL, escript);
    EXPECT_STREQ(elua_get_last_error_string(L), "[elua_load_buff:667] [string \"string\"]:2: '<name>' expected near 'do'.");
}

TEST(elua_cache_script, right_curves_bracket_expected)
{
    elua_vm *L = NULL;

    L = elua_create_vm(NULL);
    const char *script = get_script_buffer("./script/right_curves_bracket_expected.lua");
    struct elua_script *escript = elua_cache_script(L, script, script_len, 0);
    
    /* check */
    EXPECT_EQ((void *)NULL, escript);
    EXPECT_STREQ(elua_get_last_error_string(L), "[elua_load_buff:667] [string \"string\"]:2: ')' expected (to close '(' at line 1) near 'return'.");
}

TEST(elua_cache_script, right_curly_bracket_expected)
{
    elua_vm *L = NULL;

    L = elua_create_vm(NULL);
    const char *script = get_script_buffer("./script/right_curly_bracket_expected.lua");
    struct elua_script *escript = elua_cache_script(L, script, script_len, 0);
    
    /* check */
    EXPECT_EQ((void *)NULL, escript);
    EXPECT_STREQ(elua_get_last_error_string(L), "[elua_load_buff:667] [string \"string\"]:3: '}' expected near ')'.");
}

TEST(elua_cache_script, return_string_normal_sharp)
{
    elua_vm *L = NULL;

    L = elua_create_vm(NULL);
    const char *script = get_script_buffer("./script/return_string_normal_sharp.lua");
    struct elua_script *escript = elua_cache_script(L, script, script_len, 0);
    
    /* check */
    EXPECT_NE((void *)NULL, escript);
    EXPECT_STREQ(elua_get_last_error_string(L), "");
}