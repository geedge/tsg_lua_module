/*************************************************************************
    > File Name: elua_func.c
    > Author: pxz
    > Created Time: Wed 08 Jul 2020 03:45:55 PM CST
 ************************************************************************/
extern "C"
{
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<setjmp.h>
#include<time.h>
#include "lualib.h"
#include "luajit.h"
#include "lauxlib.h"
}
#include"elua.h"

#ifndef MIN
#define MIN(a,b) (((a) < (b))?(a):(b))
#endif


#define MAX_BEGIN_CODE_SIZE 23
#define MAX_END_CODE_SIZE 22

#define TEXT_BEGIN_CODE    "return function()"
#define TEXT_BEGIN_SIZE    (sizeof(TEXT_BEGIN_CODE) - 1)
#define TEXT_INIT_CODE     "\nlocal newglobaltable = {}\nsetmetatable(newglobaltable, {__index = _G})\nsetfenv(1, newglobaltable)"
#define TEXT_INIT_SIZE     (sizeof(TEXT_INIT_CODE) - 1)
#define TEXT_END_CODE      "\nend"
#define TEXT_END_SIZE      (sizeof(TEXT_END_CODE) - 1)

/* bytecode for luajit 2.0 */
#define    LJ20_BYTECODE_END_STRIPPED                                  \
    "\x14\x03\x00\x01\x00\x01\x00\x03"                                       \
    "\x31\x00\x00\x00\x30\x00\x00\x80\x48\x00\x02\x00"                       \
    "\x00\x00"
#define LJ20_BYTECODE_END_SIZE  (sizeof(LJ20_BYTECODE_END_STRIPPED) - 1)

/* bytecode for luajit 2.1 */
#define    LJ21_BYTECODE_END_STRIPPED                                  \
    "\x14\x03\x00\x01\x00\x01\x00\x03"                                       \
    "\x33\x00\x00\x00\x32\x00\x00\x80\x4c\x00\x02\x00"                       \
    "\x00\x00"

#define LJ21_BYTECODE_END_SIZE  (sizeof(LJ21_BYTECODE_END_STRIPPED) - 1)

/* bytecode for both */
#define    LJ_BYTECODE_LEN_STRIPPED     22
#define    LJ_HEADERSIZE            5
#define    LJ_BCDUMP_F_BE           0x01
#define    LJ_BCDUMP_F_STRIP        0x02
#define    LJ21_BCDUMP_VERSION        2
#define    LJ20_BCDUMP_VERSION        1
#define    LJ_SIGNATURE             "\x1b\x4c\x4a"

// #define EXEC_ERROR_INFO_NUM ((int)(sizeof(exec_error) / sizeof(exec_error_massage_t)))
// #define SYNTAX_ERROR_INFO_NUM ((int)(sizeof(syntax_error) / sizeof(char *)))


// typedef struct{
//     const char *err_info1;
//     const char *err_info2;
// }exec_error_massage_t;

enum{
    elua_READER_BUFFSIZE = 4096
};

typedef enum{
    elua_TEXT_FILE,
    elua_BC_LUA,
    elua_BC_LJ
}elua_clfactory_file_type_t;

typedef struct{
    const char *s;
    elua_clfactory_file_type_t file_type;
    int sent_begin;
    int sent_end;
    int size;

    size_t begin_code_len;
    size_t end_code_len;
    size_t rest_len;
    const char *begin_code_ptr;
    const char *end_code_ptr;
}elua_clfactory_buffer_t;

typedef struct{
    FILE *f;
    elua_clfactory_file_type_t file_type;
    int extraline;
    int sent_begin;
    int sent_end;

    size_t begin_code_len;
    size_t end_code_len;
    size_t rest_len;
    union{
        const char *ptr;
        char str[MAX_BEGIN_CODE_SIZE];
    }begin_code;
    const char *end_code_ptr;

    char buff[elua_READER_BUFFSIZE];
}elua_clfactory_file_t;

typedef struct elua_private_info
{
    jmp_buf *elua_exception;
    char elua_name[1024];
    void *userdata;
    int errcode;
    char errmsg[1024];
    int elua_mode;
#define elua_JIT_ON 0
#define elua_JIT_OFF 1
    long time_now;
    long time_limit;
    bool is_expired;    //not return error from C, when the script was ran in C and time out, 
                        // keep this status and not return error until the script run in LUA
    int lua_stack_top;
}elua_private_info_t;

typedef struct elua_context
{
    int context_id;
    char *context_name;
    lua_State *el;
}elua_context_t;

// static const char *syntax_error[] =
// {
//     "'end' expected",       /* ERR_SCRIPT_END_EXPECTED */
//     "'then' expected",      /* ERR_SCRIPT_THEN_EXPECTED */
//     "'do' expected",        /* ERR_SCRIPT_DO_EXPECTED */
//     "'<eof>' expected",     /* ERR_SCRIPT_EOF_EXPECTED */
//     "'=' or 'in' expected", /* ERR_SCRIPT_EQUAL_IN_EXPECTED */
//     "unexpected symbol",    /* ERR_SCRIPT_UNEXPECTED_SYMBOL */
//     "'<name>' expected",    /* ERR_SCRIPT_NAME_EXPECT */
//     "')' expected",         /* ERR_SCRIPT_RIGHT_CURVES_BRACKET */
//     "'}' expected",         /* ERR_SCRIPT_RIGHT_CURLY_BRACKET */
// };

// static exec_error_massage_t exec_error[] =
// {
//     {"bad argument", "(string expected, got nil)"},     /* ERR_SCRIPT_STRING_EXPECTED_BUT_NIL */
//     {"bad argument", "(string expected, got boolean)"}, /* ERR_SCRIPT_STRING_EXPECTED_BUT_BOOL */
//     {"bad argument", "(string expected, got table)"},   /* ERR_SCRIPT_STRING_EXPECTED_BUT_TABLE */
//     {"attempt to call global", "(a nil value)"},        /* ERR_SCRIPT_CALL_GLOBAL_BUT_NIL */
// };

static long mstime()
{
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    return now.tv_sec * 1000 + now.tv_nsec/1000000;
}

static long elua_clfactory_file_size(FILE *f)
{
    long cur_pos, len;
    cur_pos = ftell(f);
    if (cur_pos == -1)
    {
        return -1;
    }

    if (fseek(f, 0, SEEK_END) != 0)
    {
        return -1;
    }

    len = ftell(f);
    if (len == -1)
    {
        return -1;
    }

    if (fseek(f, cur_pos, SEEK_SET) != 0)
    {
        return -1;
    }

    return len;
}

static void elua_clfactory_file_text_prepare(elua_clfactory_file_t *lf)
{
    lf->begin_code.ptr = TEXT_BEGIN_CODE;
    lf->begin_code_len = TEXT_BEGIN_SIZE;
    lf->end_code_ptr = TEXT_END_CODE;
    lf->end_code_len = TEXT_END_SIZE;
}

static int elua_clfactory_bytecode_buffer_prepare(elua_clfactory_buffer_t *ls, char *errmsg)
{
    int little_endian, version, stripped;

    if (ls == NULL)
        return -1;

    if (ls->file_type == elua_BC_LJ)
    {
        /* get bytecode header */
        if (ls->size <= LJ_HEADERSIZE)
        {
            snprintf(errmsg, 1023, "[%s:%d] script is bad.", __FUNCTION__, __LINE__);            
            return -1;
        }
        
        ls->size--;
        /* get bytecode  version */
        version = (int)(ls->s[3]);

        /* compare bytecode header */
        if (strncmp(ls->s, LJ_SIGNATURE, sizeof(LJ_SIGNATURE) - 1))
        {
            snprintf(errmsg, 1023, "[%s:%d] bad byte-code header.", __FUNCTION__, __LINE__);
            return -1;
        }

        /* little endian or big little endian */
        little_endian = !((ls->s[4]) & LJ_BCDUMP_F_BE);
        if (little_endian == 0)
        {
            snprintf(errmsg, 1023, "[%s:%d] not support byte-code coding by big-endian.", __FUNCTION__, __LINE__);
            return -1;
        }

        /* stripped or debug */
        stripped = (ls->s[4]) & LJ_BCDUMP_F_STRIP;
        if (!stripped)
        {
            snprintf(errmsg, 1023, "[%s:%d] not support byte-code include debug-info.", __FUNCTION__, __LINE__);
            return -1;
        }

        ls->end_code_len = LJ_BYTECODE_LEN_STRIPPED;
        if (version == LJ21_BCDUMP_VERSION)
        {
            ls->end_code_ptr = LJ21_BYTECODE_END_STRIPPED;
        }
#if 0
        else if (version == LJ20_BCDUMP_VERSION)
        {
            ls->end_code_ptr = LJ20_BYTECODE_END_STRIPPED;
        }
#endif
        else
        {
            snprintf(errmsg, 1023, "[%s:%d] bytecode format version unsupported.", __FUNCTION__, __LINE__);
            return -1;
        }
    }
    return 0;
}

static int elua_clfactory_bytecode_file_prepare(elua_clfactory_file_t *lf, char *errmsg)
{
    *lf->begin_code.str = LUA_SIGNATURE[0];
    if (lf->file_type == elua_BC_LJ)
    {
        /* get bytecode header */
        size_t size = fread(lf->begin_code.str + 1, 1, LJ_HEADERSIZE - 1, lf->f);
        if (size != LJ_HEADERSIZE - 1)
        {
            snprintf(errmsg, 1023, "[%s:%d] can not read header.", __FUNCTION__, __LINE__);
            return -1;
        }
        
        /* get bytecode  version */
        int version = *(lf->begin_code.str + 3);

        /* compare bytecode header */
        if (strncmp(lf->begin_code.str, LJ_SIGNATURE, sizeof(LJ_SIGNATURE) - 1))
        {
            snprintf(errmsg, 1023, "[%s:%d] bad byte-code header.", __FUNCTION__, __LINE__);
            return -1;
        }

        lf->begin_code_len =  LJ_HEADERSIZE;

        /* little endian or big little endian */
        int little_endian = !((*(lf->begin_code.str + 4)) & LJ_BCDUMP_F_BE);
        if (little_endian == 0)
        {
            snprintf(errmsg, 1023, "[%s:%d] not support byte-code coding by big-endian.", __FUNCTION__, __LINE__);
            return -1;
        }

        /* stripped or debug */
        int stripped = (*(lf->begin_code.str + 4)) & LJ_BCDUMP_F_STRIP;
        if (!stripped)
        {
            snprintf(errmsg, 1023, "[%s:%d] not support byte-code include debug-info.", __FUNCTION__, __LINE__);
            return -1;
        }

        if (version == LJ21_BCDUMP_VERSION)
        {
            lf->end_code_ptr = LJ21_BYTECODE_END_STRIPPED;
            lf->end_code_len = LJ_BYTECODE_LEN_STRIPPED;
        }else if (version == LJ20_BCDUMP_VERSION)
        {
            lf->end_code_ptr = LJ20_BYTECODE_END_STRIPPED;
            lf->end_code_len = LJ_BYTECODE_LEN_STRIPPED;
        }else
        {
            snprintf(errmsg, 1023, "[%s:%d] bytecode format version unsupported.", __FUNCTION__, __LINE__);
            return -1;
        }
        long fsize = elua_clfactory_file_size(lf->f);
        if (fsize < 0)
        {
            snprintf(errmsg, 1023, "[%s:%d] script is bad.", __FUNCTION__, __LINE__);
            return -1;
        }
        lf->rest_len = fsize - LJ_HEADERSIZE;
    }
    return 0;
}

static const char * elua_gets(lua_State *L, void *ud, size_t *size)
{
    (void)L;
    elua_clfactory_buffer_t *ls = (elua_clfactory_buffer_t *)ud;
    if (ls->file_type == elua_TEXT_FILE)
    {
        if (ls->sent_begin == 0)
        {
            ls->sent_begin = 1;
            *size = ls->begin_code_len;
            return ls->begin_code_ptr;
        }
    }
    if (ls->size == 0)
    {
        if (ls->sent_end == 0)
        {
            ls->sent_end = 1;
            *size = ls->end_code_len;
            return ls->end_code_ptr;
        }
        return NULL;
    }

    if (ls->file_type == elua_BC_LJ)
    {
#if 0
        lf->rest_len -= ls->size;
        if (lf->rest_len == 0)
        {
            if (--ls->size == 0 && lf->sent_end == 0)
            {
                lf->sent_end = 1;
                *size = lf->end_code_len;

                return lf->end_code_ptr;
            }
        }
#endif
    }

    *size = ls->size;
    ls->size = 0;

    return ls->s;
}

static const char * elua_getf(lua_State *L, void *ud, size_t *size)
{
    (void)L;
    elua_clfactory_file_t *lf = (elua_clfactory_file_t *)ud;
    if (lf->extraline == 1)
    {
        lf->extraline = 0;
        *size = 1;
        return "\n";
    }

    if (lf->sent_begin == 0)
    {
        lf->sent_begin = 1;
        *size = lf->begin_code_len;
        if (lf->file_type == elua_TEXT_FILE)
        {
            return lf->begin_code.ptr;
        }else
        {
            return lf->begin_code.str;
        }
    }
    size_t num = fread(lf->buff, 1, sizeof(lf->buff), lf->f);
    if (num == 0)
    {
        if (lf->sent_end == 0)
        {
            lf->sent_end = 1;
            *size = lf->end_code_len;
            return lf->end_code_ptr;
        }
        return NULL;
    }

    if (lf->file_type == elua_BC_LJ)
    {
        lf->rest_len -= num;
        if (lf->rest_len == 0)
        {
            if (--num == 0 && lf->sent_end == 0)
            {
                lf->sent_end = 1;
                *size = lf->end_code_len;

                return lf->end_code_ptr;
            }
        }
    }
    *size = num;
    return lf->buff;
}

static int c_lua_atpanic(lua_State *L)
{
    char        *s = NULL;
    size_t      len = 0;
    jmp_buf     *elua_exception;
    elua_private_info_t *elua_info = NULL;

    elua_info = (elua_private_info_t *)lua_getexdata(L);
    elua_exception = elua_info->elua_exception;
    if (lua_type(L, -1) == LUA_TSTRING)
    {
        s = (char *)lua_tolstring(L, -1, &len);
    }

    if (s == NULL)
    {
        s = (char *)"unknow reason";
        len = strlen(s);
    }
    snprintf(elua_info->errmsg, 1023, "[%s:%d] lua atpanic:lua VM creashed, reson:%*s.", __FUNCTION__, __LINE__, (int)len, s);
    longjmp(*elua_exception, 1);
}

int elua_memmem(lua_State *L)
{
    const char *src;
    const char *dest;
    size_t src_len, dest_len;
    const char *ret;
    int start, end;
    int parameter_num = lua_gettop(L);
    if (parameter_num != 4)
    {
        return 0;
    }

    src = lua_tostring(L, -4);
    src_len = lua_tointeger(L, -3);
    dest = lua_tostring(L, -2);
    dest_len = lua_tointeger(L, -1);

    if (!src || !dest || !src_len || !dest_len)
    {
        return 0;
    }

    ret = (const char *)memmem(src, src_len, dest, dest_len);
    if (ret == NULL)
    {
        return 0;
    }
    start = ret - src;
    end = start + dest_len;
    lua_pushinteger(L, start);
    lua_pushinteger(L, end);

    return 2;
}

struct elua_vm
{
    lua_State *L;
};

static void luaMaskCountHook(struct elua_vm *vm, lua_Debug *ar)
{
    lua_State *L = (lua_State *)vm;
    elua_private_info_t *elua_info = (elua_private_info_t *)lua_getexdata(L);

    if (elua_info->is_expired == true)
    {
        if (ar->what && memcmp(ar->what, "C", 1) == 0)
        {
            return;
        }
        else
        {
            lua_pushstring(L, "Lua script killed by time out.");
            lua_error(L);
            return;
        }
    }

    long elapsed = mstime() - elua_info->time_now;
    if ( elua_info->time_limit > 0 && elapsed > elua_info->time_limit)
    {
        //printf("elasped:%ld\n", elapsed);
        if (ar->what && memcmp(ar->what, "C", 1) == 0)
        {
            elua_info->is_expired = true;
            // reset the interval to a shorter time
            int hook_granularity = elua_info->time_limit * 1000000 / 1000; //about time_limt / 1000
            lua_sethook(L, (lua_Hook)luaMaskCountHook, LUA_MASKCOUNT, hook_granularity);
        }
        else
        {
            lua_pushstring(L, "Lua script killed by time out.");
            lua_error(L);
        }
    }
}

static int elua_set_script_timeout(struct lua_State *L, int ms)
{
    if (L == NULL)
    {
        return -1;
    }

    elua_private_info_t *elua_info = (elua_private_info_t *)lua_getexdata(L);
    if (ms < 0)
    {
        snprintf(elua_info->errmsg, 1023, "[%s:%d] Parameter error.", __FUNCTION__, __LINE__);
        return -1;
    }

    elua_info->time_limit = ms;
    if (ms == 0)
    {
        elua_info->elua_mode = elua_JIT_ON;
        luaJIT_setmode(L, 1, LUAJIT_MODE_FUNC | LUAJIT_MODE_ON);
    }
    else
    {
        elua_info->elua_mode = elua_JIT_OFF;
        luaJIT_setmode(L, 0,  LUAJIT_MODE_ENGINE | LUAJIT_MODE_OFF);
        int hook_granularity = ms * 1000000 / 20; // about ms / 20
        lua_sethook(L, (lua_Hook)luaMaskCountHook, LUA_MASKCOUNT, hook_granularity);
    }

    return 0;
}

struct elua_vm *elua_create_vm(const char *name)
{
    if (name == NULL)
    {
        name = "TSG";
    }
    lua_State *L;
    jmp_buf *elua_exception = (jmp_buf *)malloc(sizeof(jmp_buf));
    if (elua_exception == NULL)
    {
        return NULL;
    }
    
    elua_private_info_t *elua_info = (elua_private_info_t *)calloc(1, sizeof(elua_private_info_t));
    if (elua_info == NULL)
    {
        free(elua_exception);
        return NULL;
    }

    L = luaL_newstate();
    if (L == NULL)
    {
        free(elua_exception);
        free(elua_info);
        return NULL;
    }
    elua_info->elua_exception = elua_exception;
    int len = strlen(name);
    memcpy(elua_info->elua_name, name, MIN(1023, len));
    elua_info->elua_name[len] = '\0';
    elua_info->userdata = NULL;
    lua_setexdata(L, elua_info);
    lua_atpanic(L, c_lua_atpanic);
    luaL_openlibs(L);
    lua_newtable(L);
    lua_pushcfunction(L, elua_memmem);
    lua_setfield(L, -2, "memmem");
    lua_setglobal(L, name);

    return (struct elua_vm *)L;
}

static int elua_script_check_and_clfactory(struct elua_vm *vm, const char *script, int script_len, elua_clfactory_buffer_t *ls, char *errmsg)
{
    int  i = 0;
    int sharp = 0;

    if (script[0] == '#')
    {
        for (i = 0; i < ls->size; i++)
        {
            if (script[i] == '\n')
            {
                /* skip extra line */
                ls->s = &script[i];
                ls->size = script_len - i;
                break;
            }
        }
        if (i == ls->size)
        {
            snprintf(errmsg, 1023, "[%s:%d] Script syntax error.", __FUNCTION__, __LINE__);
            return -1;
        }
        sharp = 1;
    }
    if (script[i] == LUA_SIGNATURE[0])
    {
        /* use luajit virtual machine rather then la virtual machine */
        ls->file_type = elua_BC_LJ;
        if (sharp)
        {
            snprintf(errmsg, 1023, "[%s:%d] Script is bad.", __FUNCTION__, __LINE__);
            return -1;
        }

        int status = elua_clfactory_bytecode_buffer_prepare(ls, errmsg);
        if (status != 0)
        {
            return status;
        }

    }else
    {
        ls->file_type = elua_TEXT_FILE;
        ls->begin_code_ptr = TEXT_BEGIN_CODE;
        ls->begin_code_len = TEXT_BEGIN_SIZE;
        ls->end_code_ptr = TEXT_END_CODE;
        ls->end_code_len = TEXT_END_SIZE;
    }

    return 0;
}

static int elua_load_buff(lua_State *L, elua_clfactory_buffer_t *ls, char *errmsg)
{
    int ret = lua_load(L, elua_gets, ls, "string");
    if (ret != 0)
    {
        if (ret == LUA_ERRMEM)
        {
            snprintf(errmsg, 1023, "[%s:%d] Lua memory is not enough.", __FUNCTION__, __LINE__);
            return -1;
        }
        else if (lua_isstring(L, -1))
        {
            const char *err = lua_tostring(L, -1);
            snprintf(errmsg, 1023, "[%s:%d] %s.", __FUNCTION__, __LINE__, err);
            return -1;
        }
        else 
        {
            snprintf(errmsg, 1023, "[%s:%d] Lua load failed.", __FUNCTION__, __LINE__);
            return -1;
        }
    }

    return 0;
}

struct elua_script
{
    size_t script_id;
    size_t timeout_ms;
    elua_vm *vm;
};

struct elua_script *elua_cache_script(struct elua_vm *vm, const char *script, size_t script_len, size_t timeout_ms)
{
    size_t script_id = 0;
    const char *err = NULL;
    int ret = 0;
    elua_clfactory_buffer_t ls;
    lua_State *L = (lua_State *)vm;

    if (L == NULL)
    {
        return NULL;
    }

    elua_private_info_t * elua_info = (elua_private_info_t *)lua_getexdata(L);
    if (script == NULL || script_len == 0)
    {
        snprintf(elua_info->errmsg, 1023, "[%s:%d] Parameter error.", __FUNCTION__, __LINE__);
        return NULL;
    }

    memset(&ls, 0, sizeof(elua_clfactory_buffer_t));
    ls.s =  script;
    ls.size = script_len;
    ls.sent_begin = 0;
    ls.sent_end = 0;

    lua_settop(L, 0);
    ret = elua_script_check_and_clfactory(vm, script, script_len, &ls, elua_info->errmsg);
    if (ret < 0)
    {
        return NULL;
    }

    ret = elua_load_buff(L, &ls, elua_info->errmsg);
    if (ret < 0)
    {
        return NULL;
    }

    if (lua_pcall(L, 0, LUA_MULTRET, 0))
    {
        err = lua_tostring(L, -1);
        snprintf(elua_info->errmsg, 1023, "[%s:%d] %s.", __FUNCTION__, __LINE__, err);
        return NULL;
    }

    script_id = luaL_ref(L, LUA_REGISTRYINDEX);
    if ((script_id == (size_t)LUA_REFNIL) || (script_id == (size_t)LUA_NOREF))
    {
        snprintf(elua_info->errmsg, 1023, "[%s:%d] Lua cache failed.", __FUNCTION__, __LINE__);
        return NULL;
    }

    struct elua_script *escript = (struct elua_script *)malloc(sizeof(elua_script));
    escript->script_id = script_id;
    escript->timeout_ms = timeout_ms;
    escript->vm = vm;

    return escript;
}

struct elua_script *elua_cache_script_file(struct elua_vm *vm, const char *script, size_t timeout_ms)
{
    size_t script_id;
    const char *err = NULL;
    int ret = 0;
    int sharp = 0;
    elua_clfactory_file_t lf;
    lua_State *L = (lua_State *)vm;


    if (L == NULL)
    {
        return NULL;
    }
    elua_private_info_t * elua_info = (elua_private_info_t *)lua_getexdata(L);
    if (script == NULL)
    {
        snprintf(elua_info->errmsg, 1023, "[%s:%d] Parameter error.", __FUNCTION__, __LINE__);
        return NULL;
    }
    lf.extraline = 0;
    lf.sent_begin = 0;
    lf.sent_end = 0;
    lf.file_type = elua_TEXT_FILE;
    
    lf.f = fopen(script, "r");
    if (lf.f == NULL)
    {
        snprintf(elua_info->errmsg, 1023, "[%s:%d] script %s not exist.", __FUNCTION__, __LINE__, script);
        return NULL;
    }
    int c = getc(lf.f);
    if (c == '#')
    {
        lf.extraline = 1;
        while ((c = getc(lf.f)) != EOF && c != '\n') 
        {
            /* skip first line */
        }
        if (c == '\n')
        {
            c = getc(lf.f);
        }
        sharp = 1;
    }

    if (c == LUA_SIGNATURE[0])
    {
        lf.f = freopen(script, "rb", lf.f);
        if (lf.f == NULL)
        {
            snprintf(elua_info->errmsg, 1023, "[%s:%d] script %s not exist.", __FUNCTION__, __LINE__, script);
            return NULL;
        }
        lf.file_type = elua_BC_LJ;
        if (sharp)
        {
            /*
             * Loading bytecode with an extra header is disabled for security
             * reasons. This may circumvent the usual check for bytecode vs.
             * Lua code by looking at the first char. Since this is a potential
             * security violation no attempt is made to echo the chunkname either.
             */
            fclose(lf.f);
            snprintf(elua_info->errmsg, 1023, "[%s:%d] script %s is bad.", __FUNCTION__, __LINE__, script);
            return NULL;
        }

        while((c = getc(lf.f)) != EOF && c != LUA_SIGNATURE[0])
        {
            /* skip eventual "#! ..." */
        }
        int status = elua_clfactory_bytecode_file_prepare(&lf, elua_info->errmsg);
        if(status != 0)
        {
            return NULL;
        }
        lf.extraline = 0;
    }else
    {
        elua_clfactory_file_text_prepare(&lf);
        ungetc(c, lf.f);
    }
    lua_settop(L, 0);
    ret = lua_load(L, elua_getf, &lf, script);
    fclose(lf.f);
    if (ret != 0)
    {
        if (ret == LUA_ERRMEM)
        {
            snprintf(elua_info->errmsg, 1023, "[%s:%d] Lua memory is not enough.", __FUNCTION__, __LINE__);
            return NULL;
        }
        else if (lua_isstring(L, -1))
        {
            err = lua_tostring(L, -1);
            snprintf(elua_info->errmsg, 1023, "[%s:%d] %s.", __FUNCTION__, __LINE__, err);
            return NULL;
        }
        else 
        {
            snprintf(elua_info->errmsg, 1023, "[%s:%d] lua load failed.", __FUNCTION__, __LINE__);
            return NULL;
        }
    }
    /* set new globaltable */
    lua_createtable(L, 0, 1);
    lua_pushvalue(L, -1);
    lua_pushvalue(L, LUA_GLOBALSINDEX);
    lua_setfield(L, -2, "__index");
    lua_setmetatable(L, -2);
    lua_setfenv(L, -2);

    if (lua_pcall(L, 0, LUA_MULTRET, 0))
    {
        err = lua_tostring(L, -1);
        snprintf(elua_info->errmsg, 1023, "[%s:%d] %s.", __FUNCTION__, __LINE__, err);
        return NULL;
    }

    /* cache script */
    script_id = luaL_ref(L, LUA_REGISTRYINDEX);
    if ((script_id == (size_t)LUA_REFNIL) || (script_id == (size_t)LUA_NOREF))
    {
        snprintf(elua_info->errmsg, 1023, "[%s:%d] luaL_ref failed.", __FUNCTION__, __LINE__);
        return NULL;
    }

    struct elua_script *escript = (struct elua_script *)malloc(sizeof(elua_script));
    escript->script_id = script_id;
    escript->timeout_ms = timeout_ms;
    escript->vm = vm;

    return escript;
}

int elua_cleanup_script(struct elua_script *script)
{
    if (script == NULL)
    {
        return -1;
    }

    lua_State *L = (lua_State *)script->vm;
    if (L == NULL)
    {
        return -1;
    }

    luaL_unref(L, LUA_REGISTRYINDEX, script->script_id);

    return 0;
}

int elua_destroy_vm(struct elua_vm *vm)
{
    elua_private_info_t *elua_info = NULL;
    lua_State *L = (lua_State *)vm;

    if (L == NULL)
    {
        return -1;
    }
    elua_info = (elua_private_info_t *)lua_getexdata(L);
    if (elua_info != NULL)
    {
        if (elua_info->elua_exception != NULL)
        {
            free(elua_info->elua_exception);
        }
        free(elua_info);
    }

    lua_close(L);
    return 0;
}


void *elua_get_execute_userdata(struct elua_vm *vm)
{
    lua_State *L = (lua_State *)vm;
    if (vm == NULL)
    {
        return NULL;
    }
    elua_private_info_t * elua_info = (elua_private_info_t *)lua_getexdata(L);
    
    if (elua_info != NULL)
    {
        return elua_info->userdata;
    }
    else
    {
        return NULL;
    }
}

int elua_register_cbinding(struct elua_vm *vm, const char *elua_func_namespace, const char *elua_func_name, elua_cbinding_func_ptr const function)
{
    lua_State *L = (lua_State *)vm;
    if (L == NULL)
    {
        return -1;
    }
    elua_private_info_t *elua_info = (elua_private_info_t *)lua_getexdata(L);
    if (elua_func_name == NULL || function == NULL)
    {
        snprintf(elua_info->errmsg, 1023, "[%s:%d] Parameter error.", __FUNCTION__, __LINE__);
        return -1;
    }

    lua_getglobal(L, elua_info->elua_name);
    if (elua_func_namespace != NULL)
    {
        lua_getfield(L, -1, elua_func_namespace);
        int ret = lua_type(L, -1);
        if (ret != LUA_TTABLE)
        {
            lua_pop(L, 1);
            lua_newtable(L);
            lua_pushcfunction(L, (lua_CFunction)function);
            lua_setfield(L, -2, elua_func_name);
            lua_setfield(L, -2, elua_func_namespace);
        }
        else
        {
            lua_pushcfunction(L, (lua_CFunction)function);
            lua_setfield(L, -2, elua_func_name);
        }
    }
    else
    {
        lua_pushcfunction(L, (lua_CFunction)function);
        lua_setfield(L, -2, elua_func_name);
    }
    lua_settop(L, 0);

    return 0;
}

elua_context_t *elua_create_context(struct elua_vm *vm, const char *ctx_name)
{
    lua_State *L = (lua_State *)vm;
    if (L == NULL)
    {
        return NULL;
    }

    elua_private_info_t *elua_info = (elua_private_info_t *)lua_getexdata(L);
    if (ctx_name == NULL)
    {
        snprintf(elua_info->errmsg, 1023, "[%s:%d] Parameter error.", __FUNCTION__, __LINE__);
        return NULL;
    }

    lua_newtable(L);
    int context_id = luaL_ref(L, LUA_REGISTRYINDEX);
    if (context_id == LUA_REFNIL ||  context_id == LUA_NOREF)
    {
        snprintf(elua_info->errmsg, 1023, "[%s:%d] luaL_ref failed.", __FUNCTION__, __LINE__);
        return NULL;
    }
    lua_settop(L, 0);

    struct elua_context *context = (struct elua_context *)malloc(sizeof(struct elua_context));
    context->context_id = context_id;
    int len = strlen(ctx_name);
    context->context_name = (char *)malloc(len+1);
    memcpy(context->context_name, ctx_name, len);
    context->context_name[len] = '\0';
    context->el = (lua_State *)vm;

    return context;
}

int elua_destroy_context(struct elua_context *context)
{
    if (context == NULL)
    {
        return -1;
    }

    lua_pushnil(context->el);
    lua_setglobal(context->el, context->context_name);
    luaL_unref(context->el, LUA_REGISTRYINDEX, context->context_id);

    if (context->context_name != NULL)
    {
        free(context->context_name);
        context->context_name = NULL;
    }
    free(context);

    return 0;
}

static void elua_set_context(lua_State *L, const char *elua_name, int context_id, const char *context_name)
{
    if (L != NULL && elua_name != NULL && context_id != LUA_REFNIL && context_id != LUA_NOREF && context_name != NULL)
    {
        lua_getglobal(L, elua_name);
        lua_rawgeti(L, LUA_REGISTRYINDEX, context_id);
        lua_setfield(L, -2, context_name);
        lua_pop(L, 1);
    }
}

static void elua_set_data(lua_State *L, const char *elua_name, const char *data, int data_len)
{
    if (L != NULL && elua_name != NULL && data != NULL && data_len > 0)
    {
        int top = lua_gettop(L);
        lua_getglobal(L, elua_name);
        lua_pushlstring(L, data, data_len);
        lua_setfield(L, -2, "data");
        lua_settop(L, top);
    }
}

static int elua_call_script(lua_State *L, int script_id, char *errmsg)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, script_id);

    if (lua_pcall(L, 0, LUA_MULTRET, 0))
    {
        const char *err = lua_tostring(L, -1);
        snprintf(errmsg, 1023, "[%s:%d] %s.", __FUNCTION__, __LINE__, err);
        return -1;
    }
    return 0;
}

static int elua_vm_return_value(lua_State *L, struct elua_data *output, char *errmsg)
{
    if (output == NULL)
    {
        //Do not care about return value
        lua_settop(L, 0);
    }
    else
    {
        int num = lua_gettop(L);
        if (num == 0)
        {
            // return nothing
            snprintf(errmsg, 1023, "[%s:%d] script return nothing.", __FUNCTION__, __LINE__);
            return -1;
        }
        else if (num == 2)
        {
            output->len = lua_tonumber(L, -2);
            if (output->len < 1)
            {
                snprintf(errmsg, 1023, "[%s:%d] script out_len is %zu.", __FUNCTION__, __LINE__, output->len);
                return -1;
            }
        }
        else if (num > 2)
        {
            snprintf(errmsg, 1023, "[%s:%d] the script return too many values, the num of value is %d, %s.", __FUNCTION__, __LINE__, num, lua_tostring(L, 0-num));
            return -1;
        }

        size_t lua_ret_type = lua_type(L, -1);
        elua_type actual_ret_type;

        switch (lua_ret_type)
        {
        case LUA_TNIL:
            actual_ret_type = NIL;
            break;
        case LUA_TSTRING:
            if (output->type == STRING && output->buff != NULL)
            {
                memcpy(output->buff, lua_tostring(L, -1), output->len);
            }
            actual_ret_type = STRING;
            break;
        case LUA_TBOOLEAN:
            output->true_or_false = lua_toboolean(L, -1);
            output->len = sizeof(int);
            actual_ret_type = BOOLEAN;
            break;
        case LUA_TNUMBER:
            output->integer = lua_tointeger(L, -1);
            output->len = sizeof(lua_Integer);
            actual_ret_type = INTEGER;
            break;
        default:
            snprintf(errmsg, 1023, "[%s:%d] the out_type of return value is invalid.", __FUNCTION__, __LINE__);
            output->len = 0;
            return -1;
        }

        if (actual_ret_type != output->type)
        {
            snprintf(errmsg, 1023, "[%s:%d] expect out_type is %d, actual out_type is:%d.", __FUNCTION__, __LINE__, output->type, actual_ret_type);
            output->type = actual_ret_type;
            return -1;
        }
    }
    return 0;
}

int elua_execute_script(struct elua_script *escript, const char *input, size_t input_len, void *userdata, struct elua_context *ctx, struct elua_data *output)
{
    if (escript == NULL)
    {
        return -1;
    }

    lua_State *L = (lua_State *)escript->vm;
    if (L == NULL)
    {
        return -1;
    }
    elua_private_info_t *elua_info = (elua_private_info_t *)lua_getexdata(L);

    size_t timeout_ms = escript->timeout_ms;
    elua_info->userdata = userdata;
    if (ctx != NULL)
    {
        elua_set_context(L, elua_info->elua_name, ctx->context_id, ctx->context_name);
    }

    elua_set_script_timeout(L, timeout_ms);

    jmp_buf *elua_exception;

    elua_info = (elua_private_info_t *)lua_getexdata(L);
    elua_exception = elua_info->elua_exception;

    if (input != NULL && input_len != 0)
    {
        elua_set_data(L, elua_info->elua_name, input, input_len);
    }

    if (setjmp(*elua_exception) == 0)
    {
        if (elua_info->time_limit > 0)
        {
            elua_info->time_now = mstime();
        }
        lua_settop(L, 0);
        int ret = elua_call_script(L, escript->script_id, elua_info->errmsg);
        if (ret < 0)
        {
            return ret;
        }
        ret = elua_vm_return_value(L, output, elua_info->errmsg);
        if (ret < 0)
        {
            return ret;
        }
    }
    else
    {
        snprintf(elua_info->errmsg, 1023, "[%s:%d] exec script failed.", __FUNCTION__, __LINE__);
        return -1;
    }

    return 0;
}

const char *elua_get_last_error_string(struct elua_vm *vm)
{
    lua_State *L = (lua_State *)vm;
    if (L == NULL)
    {
        return "The vm is invalid.";
    }

    elua_private_info_t *elua_info = (elua_private_info_t *)lua_getexdata(L);

    return (const char *)elua_info->errmsg;
}

int elua_remove_function(struct elua_vm *vm, const char *elua_function)
{
    lua_State *L = (lua_State *)vm;
    if (L == NULL)
    {
        return -1;
    }

    elua_private_info_t *elua_info = (elua_private_info_t *)lua_getexdata(L);
    if (elua_function == NULL)
    {
        snprintf(elua_info->errmsg, 1023, "[%s:%d] Parameter error.", __FUNCTION__, __LINE__);
        return -1;    
    }

    lua_pushnil(L);
    lua_setglobal(L, elua_function);
    lua_settop(L, 0);

    return 0;
}

int elua_cbinding_get_input_params_num(struct elua_vm *vm)
{
    lua_State *L = (lua_State *)vm;
    if (L == NULL)
    {
        return -1;
    }
    return lua_gettop(L);
}

int elua_cbinding_get_input_param(struct elua_vm *vm, int param_index, struct elua_data *param)
{
    lua_State *L = (lua_State *)vm;
    if (L == NULL)
    {
        return -1;
    }

    elua_private_info_t *elua_info = (elua_private_info_t *)lua_getexdata(L);
    if (param_index < 1 || param == NULL)
    {
        snprintf(elua_info->errmsg, 1023, "[%s:%d] Parameter error.", __FUNCTION__, __LINE__);
        return -1;
    }

    int num = lua_gettop(L);
    if (param_index > num)
    {
        return 0;
    }

    int type = lua_type(L, param_index);
    switch (type)
    {
    case LUA_TBOOLEAN:
        param->type = BOOLEAN;
        param->true_or_false = lua_toboolean(L, param_index);
        param->len = sizeof(int);
        break;
    case LUA_TNUMBER:
        param->type = INTEGER;
        param->integer = lua_tonumber(L, param_index);
        param->len = sizeof(lua_Integer);
        break;
    case LUA_TSTRING:
        param->type = STRING;
        param->buff = (char *)lua_tostring(L, param_index);
        param->len = strlen(param->buff);
        break;
    default:
        snprintf(elua_info->errmsg, 1023, "[%s:%d] the out_type of param value is invalid.", __FUNCTION__, __LINE__);
        return -1;
    }

    return 1;
}

struct elua_table
{
    int index;
    lua_State *el;
    bool is_table_value;
};

struct elua_table *elua_create_table(struct elua_vm *vm)
{
    struct lua_State *L = (struct lua_State *)vm;
    if (L == NULL)
    {
        return NULL;
    }

    struct elua_table *etab = (struct elua_table *)malloc(sizeof(struct elua_table));
    lua_newtable(L);
    etab->index = lua_gettop(L);
    etab->is_table_value = false;
    etab->el = L;
    
    return etab;
}

//int elua_table_add_element(struct elua_table *table, struct elua_data *key, struct elua_data *value)
int elua_add_table(struct elua_table *table, struct elua_data *key, struct elua_data *value)
{
    if (table == NULL)
    {
        return -2;
    }

    lua_State *L = table->el;
    if (L == NULL)
    {
        return -1;
    }

    elua_private_info_t *elua_info = (elua_private_info_t *)lua_getexdata(table->el);
    if(key->type != STRING && key->type != INTEGER)
    {
        snprintf(elua_info->errmsg, 1023, "[%s:%d] the type of key is invalid.", __FUNCTION__, __LINE__);
        return -1;
    }

    int top = lua_gettop(L);
    if (key->type == STRING)
    {
        lua_pushlstring(L, key->buff, key->len);
    }
    else
    {
        lua_pushinteger(L, key->integer);
    }

    switch (value->type)
    {
    case NIL:
        lua_pushnil(L);
        break;
    case STRING:
        lua_pushlstring(L, value->buff, value->len);
        break;
    case INTEGER:
        lua_pushinteger(L, value->integer);
        break;
    case BOOLEAN:
        lua_pushboolean(L, value->true_or_false);
        break;
    case TABLE:
        lua_pushvalue(L, value->table->index);
    default:
        lua_settop(L, top);
        snprintf(elua_info->errmsg, 1023, "[%s:%d] the type of value is not invalid.", __FUNCTION__, __LINE__);
        return -1;
        break;
    }

    lua_rawset(L, table->index);
    return 0;
}

//int elua_table_update_element(struct elua_table *table, struct elua_data *key, struct elua_data *value)
int elua_update_table(struct elua_table *table, struct elua_data *key, struct elua_data *value)
{
    return elua_add_table(table, key, value);
}

//int elua_table_delet_element(struct elua_table *table, struct elua_data *key, struct elua_data *value)
int elua_delete_table(struct elua_table *table, struct elua_data *key)
{
    struct elua_data value;
    value.type = NIL;
    return elua_add_table(table, key, &value);
}

//struct elua_data *elua_table_search_element(struct elua_table *table, struct elua_data *key)
int elua_search_table(struct elua_table *table, struct elua_data *key, struct elua_data *value)
{
    if (table == NULL)
    {
        return -1;
    }

    lua_State *L = table->el;
    if (L == NULL)
    {
        return -1;
    }

    elua_private_info_t *elua_info = (elua_private_info_t *)lua_getexdata(table->el);
    if(key->type != STRING && key->type != INTEGER)
    {
        snprintf(elua_info->errmsg, 1023, "[%s:%d] the type of key is invalid.", __FUNCTION__, __LINE__);
        return -1;
    }

    if (key->type == STRING)
    {
        lua_pushlstring(L, key->buff, key->len);
    }
    else
    {
        lua_pushinteger(L, key->integer);
    }

    lua_rawget(L, table->index);
    int type = lua_type(L, -1);
    switch(type)
    {
        case LUA_TBOOLEAN:
        value->type = BOOLEAN;
        value->true_or_false = lua_toboolean(L, -1);
        break;
        case LUA_TNIL:
        value->type = NIL;
        break;
        case LUA_TSTRING:
        value->type = STRING;
        value->buff = (char *)lua_tostring(L, -1);
        break;
        case LUA_TNUMBER:
        value->type = INTEGER;
        value->integer = lua_tointeger(L, -1);
        break;
        case LUA_TTABLE:
        value->type = TABLE;
        //TODO 需要提供table 遍历函数 
        break;
    }

    return 0;
}

void elua_destroy_table(struct elua_table *table)
{
    if (table == NULL)
    {
        return;
    }

    lua_State *L = table->el;
    if (L == NULL)
    {
        return;
    }

    lua_remove(L, table->index);
    free(table);
    return;
}

int elua_cbinding_append_output_params(struct elua_vm *vm, struct elua_data *params, int params_num)
{
    lua_State *L = (lua_State *)vm;
    if (L == NULL)
    {
        return -1;
    }

    elua_private_info_t *elua_info = (elua_private_info_t *)lua_getexdata(L);
    if(params == NULL || params_num <= 0)
    {
        snprintf(elua_info->errmsg, 1023, "[%s:%d] Parameters error.", __FUNCTION__, __LINE__);
        return -1;
    }

    int top = lua_gettop(L);
    if (top != 0)
    {
        if (top == elua_info->lua_stack_top)
        {
            snprintf(elua_info->errmsg, 1023, "[%s:%d] You have called this function already.", __FUNCTION__, __LINE__);
            return -1;
        }
    }

    int i;
    for (i = 0; i < params_num; i++)
    {
        switch(params[i].type)
        {
        case NIL:
            lua_pushnil(L);
            break;
        case STRING:
            lua_pushlstring(L, params[i].buff, params[i].len);
            break;
        case INTEGER:
            lua_pushinteger(L, params[i].integer);
            break;
        case BOOLEAN:
            lua_pushboolean(L, params[i].true_or_false);
            break;
        case TABLE:
            lua_pushvalue(L, params[i].table->index);
            break;
        default:
            snprintf(elua_info->errmsg, 1023, "[%s:%d] the type of param is invalid.", __FUNCTION__, __LINE__);
            return i;
            break;
        }
    }

    return 0;
}

int elua_cbinding_return(struct elua_vm *vm, struct elua_data *data)
{
    lua_State *L = (lua_State *)vm;
    elua_private_info_t *elua_info = (elua_private_info_t *)lua_getexdata(L);
    if (data == NULL)
    {
        snprintf(elua_info->errmsg, 1023, "[%s:%d] Parameter error.", __FUNCTION__, __LINE__);
        return -1;
    }

    switch(data->type)
    {
    case NIL:
        lua_pushnil(L);
        break;
    case STRING:
        lua_pushlstring(L, data->buff, data->len);
        break;
    case INTEGER:
        lua_pushinteger(L, data->integer);
        break;
    case BOOLEAN:
        if (data->true_or_false)
        {
            lua_pushnil(L);
        }
        else
        {
            lua_pushinteger(L, data->true_or_false);
        }
        break;
    default:
        snprintf(elua_info->errmsg, 1023, "[%s:%d] the out_type of param value is invalid.", __FUNCTION__, __LINE__);
        return -1;
        break;
    }

    return 0;
}
