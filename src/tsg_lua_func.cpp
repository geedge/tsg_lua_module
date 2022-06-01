/*************************************************************************
    > File Name: tsg_lua_func.c
    > Author: pxz
    > Created Time: Wed 08 Jul 2020 03:45:55 PM CST
 ************************************************************************/
extern "C"
{
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<setjmp.h>
#include "lualib.h"
#include "luajit.h"
#include "lauxlib.h"
}
#include"tsg_lua_interface.h"

#ifndef MIN
#define MIN(a,b) (((a) < (b))?(a):(b))
#endif

#ifndef TSG_MAX_LUA_ID
#define TSG_MAX_LUA_ID  256
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

#define EXEC_ERROR_INFO_NUM ((int)(sizeof(exec_error) / sizeof(exec_error_massage_t)))
#define SYNTAX_ERROR_INFO_NUM ((int)(sizeof(syntax_error) / sizeof(char *)))


typedef struct{
    const char *err_info1;
    const char *err_info2;
}exec_error_massage_t;

enum{
    TSG_LUA_READER_BUFFSIZE = 4096
};

typedef enum{
    TSG_LUA_TEXT_FILE,
    TSG_LUA_BC_LUA,
    TSG_LUA_BC_LJ
}tsg_lua_clfactory_file_type_t;

typedef struct{
    const char *s;
    tsg_lua_clfactory_file_type_t file_type;
    int sent_begin;
    int sent_end;
    int size;

    size_t begin_code_len;
    size_t end_code_len;
    size_t rest_len;
    const char *begin_code_ptr;
    const char *end_code_ptr;
}tsg_lua_clfactory_buffer_t;

typedef struct{
    FILE *f;
    tsg_lua_clfactory_file_type_t file_type;
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

    char buff[TSG_LUA_READER_BUFFSIZE];
}tsg_lua_clfactory_file_t;

//static jmp_buf lua_exception[TSG_MAX_LUA_ID];

static const char *syntax_error[] =
{
    "'end' expected",       /* ERR_SCRIPT_END_EXPECTED */
    "'then' expected",      /* ERR_SCRIPT_THEN_EXPECTED */
    "'do' expected",        /* ERR_SCRIPT_DO_EXPECTED */
    "'<eof>' expected",     /* ERR_SCRIPT_EOF_EXPECTED */
    "'=' or 'in' expected", /* ERR_SCRIPT_EQUAL_IN_EXPECTED */
    "unexpected symbol",    /* ERR_SCRIPT_UNEXPECTED_SYMBOL */
    "'<name>' expected",    /* ERR_SCRIPT_NAME_EXPECT */
    "')' expected",         /* ERR_SCRIPT_RIGHT_CURVES_BRACKET */
    "'}' expected",         /* ERR_SCRIPT_RIGHT_CURLY_BRACKET */
};

static exec_error_massage_t exec_error[] =
{
    {"bad argument", "(string expected, got nil)"},     /* ERR_SCRIPT_STRING_EXPECTED_BUT_NIL */
    {"bad argument", "(string expected, got boolean)"}, /* ERR_SCRIPT_STRING_EXPECTED_BUT_BOOL */
    {"bad argument", "(string expected, got table)"},   /* ERR_SCRIPT_STRING_EXPECTED_BUT_TABLE */
    {"attempt to call global", "(a nil value)"},        /* ERR_SCRIPT_CALL_GLOBAL_BUT_NIL */
};

static void debuginfo(const char *info, const char *file, size_t line) 
{   
#ifdef TSG_LUA_DEBUG    
    printf("error: [%s:%ld]%s\n", file, line, info); 
#else
    (void)info;
    (void)file;
    (void)line;
#endif 
    return ;    
}

static long tsg_lua_clfactory_file_size(FILE *f)
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

static void tsg_lua_clfactory_file_text_prepare(tsg_lua_clfactory_file_t *lf)
{
    lf->begin_code.ptr = TEXT_BEGIN_CODE;
    lf->begin_code_len = TEXT_BEGIN_SIZE;
    lf->end_code_ptr = TEXT_END_CODE;
    lf->end_code_len = TEXT_END_SIZE;
}

static int tsg_lua_clfactory_bytecode_buffer_prepare(tsg_lua_clfactory_buffer_t *ls)
{
    int little_endian, version, stripped;

    if (ls == NULL)
        return -1;

    if (ls->file_type == TSG_LUA_BC_LJ)
    {
        /* get bytecode header */
        if (ls->size <= LJ_HEADERSIZE)
        {
            debuginfo("script is bad", __FILE__, __LINE__);
            return ERR_SCRIPT_IS_BAD;
        }
        
        ls->size--;
        /* get bytecode  version */
        version = (int)(ls->s[3]);

        /* compare bytecode header */
        if (strncmp(ls->s, LJ_SIGNATURE, sizeof(LJ_SIGNATURE) - 1))
        {
            debuginfo("bad byte-code header", __FILE__, __LINE__);
            return ERR_SCRIPT_BYTECODE_BAD_HEADER;
        }

        /* little endian or big little endian */
        little_endian = !((ls->s[4]) & LJ_BCDUMP_F_BE);
        if (little_endian == 0)
        {
            debuginfo("not support byte-code coding by big-endian", __FILE__, __LINE__);
            return ERR_SCRIPT_BYTECODE_NOT_SUPPORT_BIG_ENDIAN;
        }

        /* stripped or debug */
        stripped = (ls->s[4]) & LJ_BCDUMP_F_STRIP;
        if (!stripped)
        {
            debuginfo("not support byte-code include debug-info", __FILE__, __LINE__);
            return ERR_SCRIPT_BYTECODE_NOT_SUPPORT_DEBUG;
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
            debuginfo("bytecode format version unsupported", __FILE__, __LINE__);
            return ERR_SCRIPT_BYTECODE_VERSION_UNSUPPORTED;
        }
    }
    return 0;
}

static int tsg_lua_clfactory_bytecode_file_prepare(tsg_lua_clfactory_file_t *lf)
{
    *lf->begin_code.str = LUA_SIGNATURE[0];
    if (lf->file_type == TSG_LUA_BC_LJ)
    {
        /* get bytecode header */
        size_t size = fread(lf->begin_code.str + 1, 1, LJ_HEADERSIZE - 1, lf->f);
        if (size != LJ_HEADERSIZE - 1)
        {
            debuginfo("can not read header", __FILE__, __LINE__);
            return ERR_SCRIPT_BYTECODE_NO_HEADER;
        }
        
        /* get bytecode  version */
        int version = *(lf->begin_code.str + 3);

        /* compare bytecode header */
        if (strncmp(lf->begin_code.str, LJ_SIGNATURE, sizeof(LJ_SIGNATURE) - 1))
        {
            debuginfo("bad byte-code header", __FILE__, __LINE__);
            return ERR_SCRIPT_BYTECODE_BAD_HEADER;
        }

        lf->begin_code_len =  LJ_HEADERSIZE;

        /* little endian or big little endian */
        int little_endian = !((*(lf->begin_code.str + 4)) & LJ_BCDUMP_F_BE);
        if (little_endian == 0)
        {
            debuginfo("not support byte-code coding by big-endian", __FILE__, __LINE__);
            return ERR_SCRIPT_BYTECODE_NOT_SUPPORT_BIG_ENDIAN;
        }

        /* stripped or debug */
        int stripped = (*(lf->begin_code.str + 4)) & LJ_BCDUMP_F_STRIP;
        if (!stripped)
        {
            debuginfo("not support byte-code include debug-info", __FILE__, __LINE__);
            return ERR_SCRIPT_BYTECODE_NOT_SUPPORT_DEBUG;
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
            debuginfo("bytecode format version unsupported", __FILE__, __LINE__);
            return ERR_SCRIPT_BYTECODE_VERSION_UNSUPPORTED;
        }
        long fsize = tsg_lua_clfactory_file_size(lf->f);
        if (fsize < 0)
        {
            return ERR_SCRIPT_IS_BAD;
        }
        lf->rest_len = fsize - LJ_HEADERSIZE;
    }
    return 0;
}

static const char * tsg_lua_gets(lua_State *L, void *ud, size_t *size)
{
    (void)L;
    tsg_lua_clfactory_buffer_t *ls = (tsg_lua_clfactory_buffer_t *)ud;
    if (ls->file_type == TSG_LUA_TEXT_FILE)
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

    if (ls->file_type == TSG_LUA_BC_LJ)
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

static const char * tsg_lua_getf(lua_State *L, void *ud, size_t *size)
{
    (void)L;
    tsg_lua_clfactory_file_t *lf = (tsg_lua_clfactory_file_t *)ud;
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
        if (lf->file_type == TSG_LUA_TEXT_FILE)
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

    if (lf->file_type == TSG_LUA_BC_LJ)
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
    char        err[255];
    size_t      len = 0;
    jmp_buf     *lua_exception;

    lua_exception = (jmp_buf *)lua_getexdata(L);
    if (lua_type(L, -1) == LUA_TSTRING)
    {
        s = (char *)lua_tolstring(L, -1, &len);
    }

    if (s == NULL)
    {
        s = (char *)"unknow reason";
        len = strlen(s);
    }
    sprintf(err, "lua atpanic:lua VM creashed, reason:%*s\n", (int)len, s);
    debuginfo(err, __FILE__, __LINE__);
    longjmp(*lua_exception, 1);
}

int tsg_lua_memmem(lua_State *L)
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

tsg_lua_handle tsg_lua_vm_create()
{
    lua_State *L;
    jmp_buf *lua_exception = (jmp_buf *)malloc(sizeof(jmp_buf));
    if (lua_exception == NULL)
    {
        return NULL;
    }

    L = luaL_newstate();
    if (L == NULL)
    {
        free(lua_exception);
        return NULL;
    }
    lua_setexdata(L, lua_exception);
    lua_atpanic(L, c_lua_atpanic);
    luaL_openlibs(L);
    lua_createtable(L, 0, 2);
    lua_pushcfunction(L, tsg_lua_memmem);
    lua_setfield(L, -2, "memmem");
    lua_setglobal(L, "TSG");
#if 1
    const char *code = "local ffi = require(\"ffi\")\n"\
                  "ffi.cdef[[char *memmem(const char *haystack, size_t haystacklen, const char *needle, size_t needlelen);]]\n"\
                  "TSG.ffi = ffi\n"\
                  "TSG.C = ffi.C"; 
    if (luaL_dostring(L, code))
    {
        const char *err = lua_tostring(L, -1);
        debuginfo(err, __FILE__, __LINE__);
        lua_close(L);
        return NULL;
    }
#endif
    return L;
}

int tsg_lua_exec_file(tsg_lua_handle lua, const char *script, const char *in, size_t in_len, char *out, size_t *out_len, size_t *out_type)
{
    lua_State *L = (lua_State *)lua;
    const char *err = NULL;
    int ret = 0;
    int i = 0;
    int sharp = 0;
    tsg_lua_clfactory_file_t lf;
    jmp_buf *lua_exception;

    if (L == NULL)
    {
        debuginfo("lua VM is null.", __FILE__, __LINE__);
        return ERR_LUAVM_ISNULL;
    }
    if (script == NULL)
    {
        debuginfo("script is null.", __FILE__, __LINE__);
        return ERR_SCRIPT_ISNULL;
    }

    if (in == NULL)
    {
        debuginfo("input is null.", __FILE__, __LINE__);
        return ERR_INPUT_ISNULL; 
    }

    if (in_len < 1)
    {
        debuginfo("in_len is invailed", __FILE__, __LINE__);
        return ERR_IN_LEN_INVAILD;
    }

    lua_exception = (jmp_buf *)lua_getexdata(L);

    lf.extraline = 0;
    lf.sent_begin = 0;
    lf.sent_end = 0;
    lf.file_type = TSG_LUA_TEXT_FILE;
    
    lf.f = fopen(script, "r");
    if (lf.f == NULL)
    {
        return ERR_SCRIPT_NOT_EXIT;
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
            return ERR_SCRIPT_NOT_EXIT;
        }

        lf.file_type = TSG_LUA_BC_LJ; 
        if (sharp)
        {
            /* 出于对安全的考虑，禁用字节码文件首行不是lua代码的文件
             * 这是因为，字节码文件首行不是lua代码会规避掉一些常规字节码检查 */
            fclose(lf.f);
            return ERR_SCRIPT_IS_BAD;
        }

        while((c = getc(lf.f)) != EOF && c != LUA_SIGNATURE[0])
        {
            /* skip eventual "#! ..." */
        }
        int status = tsg_lua_clfactory_bytecode_file_prepare(&lf);
        if(status != 0)
        {
            fclose(lf.f);
            return status;
        }
        lf.extraline = 0;
    }else
    {
        tsg_lua_clfactory_file_text_prepare(&lf);
        ungetc(c, lf.f);
    }
    lua_settop(L, 0);
    ret = lua_load(L, tsg_lua_getf, &lf, script);
    fclose(lf.f);
    if (ret != 0)
    {
        if (ret == LUA_ERRMEM)
        {
            return ERR_MEM_NOT_ENOUGH;
        }
        else if (lua_isstring(L, -1))
        {
            err = lua_tostring(L, -1);
            debuginfo(err, __FILE__, __LINE__);
            for (i = 0; i < SYNTAX_ERROR_INFO_NUM; i++)
            {
                if (strstr(err, syntax_error[i]))
                {
                    return ERR_SCRIPT_SYNTAX_ERROR - (i + 1);
                }
            }
            return ERR_SCRIPT_SYNTAX_ERROR;
        }
        else 
        {
            return ERR_UNKNOWN;
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
        debuginfo(err, __FILE__, __LINE__);
        for (i = 0; i < SYNTAX_ERROR_INFO_NUM; i++)
        {
            if (strstr(err, syntax_error[i]))
            {
                return ERR_SCRIPT_SYNTAX_ERROR - (i + 1);
            }
        }
        return ERR_SCRIPT_SYNTAX_ERROR;
    }

    /* 输入待处理数据 */
    lua_getglobal(L, "TSG");
    lua_pushlstring(L, in, in_len);
    lua_setfield(L, -2, "data");
    lua_settop(L, 1);

    if (setjmp(*lua_exception) == 0)
    {
        if (lua_pcall(L, 0, LUA_MULTRET, 0))
        {
            err = lua_tostring(L, -1);
            debuginfo(err, __FILE__, __LINE__);
            for (i = 0; i < EXEC_ERROR_INFO_NUM; i++)
            {
                if ((strstr(err, exec_error[i].err_info1)) && (strstr(err, exec_error[i].err_info2))) 
                    return ERR_SCRIPT_EXEC_ERROR - (i + 1);
            }
            return ERR_SCRIPT_EXEC_ERROR;
        }
        int num = lua_gettop(L);
        if (num < 2)
        {
            char err_buf[255];
            sprintf((char *)err_buf,"num:%d", num);
            debuginfo(err_buf, __FILE__, __LINE__);
            return ERR_SCRIPT_RETURN_TOO_FEW;
        }
        if (num > 2)
        {
            char err_buf[255];
            sprintf((char *)err_buf,"num:%d", num);
            debuginfo(err_buf, __FILE__, __LINE__);
            return ERR_SCRIPT_RETURN_TOO_MUCH;
        }
        *out_len = lua_tonumber(L, -2);
        if (*out_len < 1)
        {
            debuginfo("script out_len is 0", __FILE__, __LINE__);
            return ERR_SCRIPT_RETURN_LEN_INVAILD;
        }
        switch(lua_type(L, -1))
        {
            case LUA_TSTRING:
                memcpy(out, lua_tostring(L, -1), *out_len);
                *out_type = STRING;
                break;
            case LUA_TBOOLEAN:
                out[0] = lua_toboolean(L, -1);
                //*out_len = 1;
                *out_type = BOOLEAN;
                break;
            case LUA_TNUMBER:
                *(long *)out = lua_tointeger(L, -1);
                //*out_len = 8;
                *out_type = INTEGER;
                break;
            default:
                char err_buf[255];
                sprintf((char *)err_buf, "out_type:%d", (lua_type(L, -1)));
                debuginfo(err_buf, __FILE__, __LINE__);
                *out_len = 0;
                return ERR_SCRIPT_RETURN_TYPE_INVAILD;
        }
    }else
    {
        return ERR_SCRIPT_EXEC_ERROR;
    }

    return 0;
}


int tsg_lua_exec(tsg_lua_handle lua, const char *script, size_t script_len, const char *in, size_t in_len, char *out, size_t *out_len, size_t *out_type)
{
    lua_State *L = (lua_State *)lua;
    const char *err = NULL;
    int ret = 0;
    int i = 0;
    int sharp = 0;
    tsg_lua_clfactory_buffer_t ls;
    jmp_buf *lua_exception;

    memset(&ls, 0, sizeof(tsg_lua_clfactory_buffer_t));

    if (L == NULL)
    {
        debuginfo("lua VM is null.", __FILE__, __LINE__);
        return ERR_LUAVM_ISNULL;
    }
    if (script == NULL)
    {
        debuginfo("script is null.", __FILE__, __LINE__);
        return ERR_SCRIPT_ISNULL;
    }

    if (in == NULL)
    {
        debuginfo("input is null.", __FILE__, __LINE__);
        return ERR_INPUT_ISNULL; 
    }

    if (in_len < 1)
    {
        debuginfo("in_len is invailed", __FILE__, __LINE__);
        return ERR_IN_LEN_INVAILD;
    }

    lua_exception = (jmp_buf *)lua_getexdata(L);

    ls.s =  script;
    ls.size = script_len;
    ls.sent_begin = 0;
    ls.sent_end = 0;

    if (script[0] == '#')
    {
        for (i = 0; i < ls.size; i++)
        {
            if (script[i] == '\n')
            {
                /* skip extra line */
                ls.s = &script[i];
                ls.size = script_len - i;
                break;
            }
        }
        if (i == ls.size)
        {
            return ERR_SCRIPT_SYNTAX_ERROR;
        }
        sharp = 1;
    }
    if (script[i] == LUA_SIGNATURE[0])
    {
        /* 不会使用lua虚拟机， 使用的是luajit虚拟机 */
        ls.file_type = TSG_LUA_BC_LJ;
        if (sharp)
        {
            return ERR_SCRIPT_IS_BAD;
        }
        int status = tsg_lua_clfactory_bytecode_buffer_prepare(&ls);
        if (status != 0)
        {
            return status;
        }

    }else
    {
        ls.file_type = TSG_LUA_TEXT_FILE;
        ls.begin_code_ptr = TEXT_BEGIN_CODE;
        ls.begin_code_len = TEXT_BEGIN_SIZE;
        ls.end_code_ptr = TEXT_END_CODE;
        ls.end_code_len = TEXT_END_SIZE;
    }
    lua_settop(L, 0);
    ret = lua_load(L, tsg_lua_gets, &ls, "main");
    if (ret != 0)
    {
        if (ret == LUA_ERRMEM)
        {
            return ERR_MEM_NOT_ENOUGH;
        }
        else if (lua_isstring(L, -1))
        {
            err = lua_tostring(L, -1);
            debuginfo(err, __FILE__, __LINE__);
            for (i = 0; i < SYNTAX_ERROR_INFO_NUM; i++)
            {
                if (strstr(err, syntax_error[i]))
                {
                    return ERR_SCRIPT_SYNTAX_ERROR - (i + 1);
                } 
            }
            return ERR_SCRIPT_SYNTAX_ERROR;
        }
        else 
        {
            return ERR_UNKNOWN;
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
        debuginfo(err, __FILE__, __LINE__);
        for (i = 0; i < SYNTAX_ERROR_INFO_NUM; i++)
        {
            if (strstr(err, syntax_error[i]))
            {
                return ERR_SCRIPT_SYNTAX_ERROR - (i + 1);
            }
        }
        return ERR_SCRIPT_SYNTAX_ERROR;
    }

    /* 输入待处理数据 */
    lua_getglobal(L, "TSG");
    lua_pushlstring(L, in, in_len);
    lua_setfield(L, -2, "data");
    lua_settop(L, 1);
    if (setjmp(*lua_exception) == 0)
    {
        if (lua_pcall(L, 0, LUA_MULTRET, 0))
        {
            err = lua_tostring(L, -1);
            debuginfo(err, __FILE__, __LINE__);
            for (i = 0; i < EXEC_ERROR_INFO_NUM; i++)
            {
                if ((strstr(err, exec_error[i].err_info1)) && (strstr(err, exec_error[i].err_info2))) 
                    return ERR_SCRIPT_EXEC_ERROR - (i + 1);
            }
            return ERR_SCRIPT_EXEC_ERROR;
        }
        int num = lua_gettop(L);
        if (num < 2)
        {
            char err_buf[255];
            sprintf((char *)err_buf, "num:%d", num);
            debuginfo(err_buf, __FILE__, __LINE__);
            return ERR_SCRIPT_RETURN_TOO_FEW;
        }
        if (num > 2)
        {
            char err_buf[255];
            sprintf((char *)err_buf, "num:%d", num);
            debuginfo(err_buf, __FILE__, __LINE__);
            return ERR_SCRIPT_RETURN_TOO_MUCH;
        }
        *out_len = lua_tonumber(L, -2);
        if (*out_len < 1)
        {
            debuginfo("script out_len is 0", __FILE__, __LINE__);
            return ERR_SCRIPT_RETURN_LEN_INVAILD;
        }
        switch(lua_type(L, -1))
        {
            case LUA_TSTRING:
                memcpy(out, lua_tostring(L, -1), *out_len);
                *out_type = STRING;
                break;
            case LUA_TBOOLEAN:
                out[0] = lua_toboolean(L, -1);
                //*out_len = 1;
                *out_type = BOOLEAN;
                break;
            case LUA_TNUMBER:
                *(size_t *)out = lua_tointeger(L, -1);
                //*out_len = 8;
                *out_type = INTEGER;
                break;
            default:
                char err_buf[255];
                sprintf((char *)err_buf, "out_type:%d", (lua_type(L, -1)));
                debuginfo(err_buf, __FILE__, __LINE__);
                *out_len = 0;
                return ERR_SCRIPT_RETURN_TYPE_INVAILD;
        }
    }else
    {
        return ERR_SCRIPT_EXEC_ERROR;
    }

    return 0;
}

int tsg_lua_cache_script(tsg_lua_handle lua, const char *script, size_t script_len)
{
    size_t script_id = 0;
    const char *err = NULL;
    int ret = 0;
    int i = 0;
    int sharp = 0;
    lua_State *L = (lua_State *)lua;
    tsg_lua_clfactory_buffer_t ls;

    if (L == NULL)
    {
        debuginfo("lua VM is null.", __FILE__, __LINE__);
        return ERR_LUAVM_ISNULL;
    }
    if (script == NULL)
    {
        debuginfo("script is null.", __FILE__, __LINE__);
        return ERR_SCRIPT_ISNULL;
    }

    memset(&ls, 0, sizeof(tsg_lua_clfactory_buffer_t));
    ls.s =  script;
    ls.size = script_len;
    ls.sent_begin = 0;
    ls.sent_end = 0;

    if (script[0] == '#')
    {
        for (i = 0; i < ls.size; i++)
        {
            if (script[i] == '\n')
            {
                /* skip extra line */
                ls.s = &script[i];
                ls.size = script_len - i;
                break;
            }
        }
        if (i == ls.size)
        {
            return ERR_SCRIPT_SYNTAX_ERROR;
        }
        sharp = 1;
    }
    if (script[i] == LUA_SIGNATURE[0])
    {
        /* 不会使用lua虚拟机， 使用的是luajit虚拟机 */
        ls.file_type = TSG_LUA_BC_LJ;
        if (sharp)
        {
            return ERR_SCRIPT_IS_BAD;
        }

        int status = tsg_lua_clfactory_bytecode_buffer_prepare(&ls);
        if (status != 0)
        {
            return status;
        }

    }else
    {
        ls.file_type = TSG_LUA_TEXT_FILE;
        ls.begin_code_ptr = TEXT_BEGIN_CODE;
        ls.begin_code_len = TEXT_BEGIN_SIZE;
        ls.end_code_ptr = TEXT_END_CODE;
        ls.end_code_len = TEXT_END_SIZE;
    }

    lua_settop(L, 0);
    ret = lua_load(L, tsg_lua_gets, &ls, "string");
    if (ret != 0)
    {
        if (ret == LUA_ERRMEM)
        {
            return ERR_MEM_NOT_ENOUGH;
        }
        else if (lua_isstring(L, -1))
        {
            err = lua_tostring(L, -1);
            debuginfo(err, __FILE__, __LINE__);
            for (i = 0; i < SYNTAX_ERROR_INFO_NUM; i++)
            {
                if (strstr(err, syntax_error[i]))
                {
                    return ERR_SCRIPT_SYNTAX_ERROR - (i + 1);
                }
            }
            return ERR_SCRIPT_SYNTAX_ERROR;
        }
        else 
        {
            return ERR_UNKNOWN;
        }
    }
    if (lua_pcall(L, 0, LUA_MULTRET, 0))
    {
        err = lua_tostring(L, -1);
        debuginfo(err, __FILE__, __LINE__);
        for (i = 0; i < SYNTAX_ERROR_INFO_NUM; i++)
        {
            if (strstr(err, syntax_error[i]))
            {
                return ERR_SCRIPT_SYNTAX_ERROR - (i + 1);
            }
        }
        return ERR_SCRIPT_SYNTAX_ERROR;
    }

    script_id = luaL_ref(L, LUA_REGISTRYINDEX);
    if ((script_id == (size_t)LUA_REFNIL) || (script_id == (size_t)LUA_NOREF))
    {
        debuginfo("lua cache failed.", __FILE__, __LINE__);
        return ERR_LUA_CACHE_FAILED;
    }

    return script_id;
}

int tsg_lua_cache_script_file(tsg_lua_handle lua, const char *script)
{
    size_t script_id;
    const char *err = NULL;
    int ret = 0;
    int sharp = 0;
    tsg_lua_clfactory_file_t lf;
    lua_State *L = (lua_State *)lua;


    if (L == NULL)
    {
        debuginfo("lua VM is null.", __FILE__, __LINE__);
        return ERR_LUAVM_ISNULL;
    }
    if (script == NULL)
    {
        debuginfo("script is null.", __FILE__, __LINE__);
        return ERR_SCRIPT_ISNULL;
    }
    lf.extraline = 0;
    lf.sent_begin = 0;
    lf.sent_end = 0;
    lf.file_type = TSG_LUA_TEXT_FILE;
    
    lf.f = fopen(script, "r");
    if (lf.f == NULL)
    {
        return ERR_SCRIPT_NOT_EXIT;
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
            return ERR_SCRIPT_NOT_EXIT;
        }
        lf.file_type = TSG_LUA_BC_LJ;
        if (sharp)
        {
            /* 出于对安全的考虑，禁用字节码文件首行不是lua代码的文件
             * 这是因为，字节码文件首行不是lua代码会规避掉一些常规字节码检查 */
            fclose(lf.f);
            return ERR_SCRIPT_IS_BAD;
        }

        while((c = getc(lf.f)) != EOF && c != LUA_SIGNATURE[0])
        {
            /* skip eventual "#! ..." */
        }
        int status = tsg_lua_clfactory_bytecode_file_prepare(&lf);
        if(status != 0)
        {
            return status;
        }
        lf.extraline = 0;
    }else
    {
        tsg_lua_clfactory_file_text_prepare(&lf);
        ungetc(c, lf.f);
    }
    lua_settop(L, 0);
    ret = lua_load(L, tsg_lua_getf, &lf, script);
    fclose(lf.f);
    if (ret != 0)
    {
        if (ret == LUA_ERRMEM)
        {
            return ERR_MEM_NOT_ENOUGH;
        }
        else if (lua_isstring(L, -1))
        {
            err = lua_tostring(L, -1);
            debuginfo(err, __FILE__, __LINE__);
            for (int i = 0; i < SYNTAX_ERROR_INFO_NUM; i++)
            {
                if (strstr(err, syntax_error[i]))
                {
                    return ERR_SCRIPT_SYNTAX_ERROR - (i + 1);
                }
            }
            return ERR_SCRIPT_SYNTAX_ERROR;
        }
        else 
        {
            return ERR_UNKNOWN;
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
        debuginfo(err, __FILE__, __LINE__);
        for (int i = 0; i < SYNTAX_ERROR_INFO_NUM; i++)
        {
            if (strstr(err, syntax_error[i]))
            {
                return ERR_SCRIPT_SYNTAX_ERROR - (i + 1);
            }
        }
        return ERR_SCRIPT_SYNTAX_ERROR;
    }

    /* cache script */
    script_id = luaL_ref(L, LUA_REGISTRYINDEX);
    if ((script_id == (size_t)LUA_REFNIL) || (script_id == (size_t)LUA_NOREF))
    {
        debuginfo("lua cache failed.", __FILE__, __LINE__);
        return ERR_LUA_CACHE_FAILED;
    }

    return script_id;
}

int tsg_lua_uncache_script(tsg_lua_handle lua, size_t script_id)
{
    lua_State *L = (lua_State *)lua;

    if (L == NULL)
    {
        debuginfo("lua VM is null.", __FILE__, __LINE__);
        return ERR_LUAVM_ISNULL;
    }

    luaL_unref(L, LUA_REGISTRYINDEX, script_id);

    return 0;
}

int tsg_lua_cache_exec(tsg_lua_handle lua, size_t script_id, const char *in, size_t in_len, char *out, size_t *out_len, size_t *out_type)
{
    lua_State *L = (lua_State *)lua;
    jmp_buf *lua_exception;

    if (L == NULL)
    {
        debuginfo("lua VM is null.", __FILE__, __LINE__);
        return ERR_LUAVM_ISNULL;
    }
    if (script_id < 1)
    {
        debuginfo("script_id is invaild.", __FILE__, __LINE__);
        return ERR_SCRIPT_ID_INVAILD;
    }
    if (in == NULL)
    {
        debuginfo("input is null.", __FILE__, __LINE__);
        return ERR_INPUT_ISNULL;
    }
    if (in_len < 1)
    {
        debuginfo("in_len is invailed", __FILE__, __LINE__);
        return ERR_IN_LEN_INVAILD;
    }

    lua_settop(L, 0);
    lua_exception = (jmp_buf *)lua_getexdata(L);

    lua_rawgeti(L, LUA_REGISTRYINDEX, script_id);
    lua_getglobal(L, "TSG");
    lua_pushlstring(L, in, in_len);
    lua_setfield(L, -2, "data");
    lua_settop(L, 1);

    if (setjmp(*lua_exception) == 0)
    {
        if (lua_pcall(L, 0, LUA_MULTRET, 0))
        {
            const char *err = lua_tostring(L, -1);
            debuginfo(err, __FILE__, __LINE__);
            for (int i = 0; i < EXEC_ERROR_INFO_NUM; i++)
            {
                if ((strstr(err, exec_error[i].err_info1)) && (strstr(err, exec_error[i].err_info2))) 
                    return ERR_SCRIPT_EXEC_ERROR - (i + 1);
            }
            return ERR_SCRIPT_EXEC_ERROR;
        }
        int num = lua_gettop(L);
        if (num < 2)
        {
            char err_buf[255];
            sprintf((char *)err_buf,"num:%d", num);
            debuginfo(err_buf, __FILE__, __LINE__);
            return ERR_SCRIPT_RETURN_TOO_FEW;
        }
        if (num > 2)
        {
            char err_buf[255];
            sprintf((char *)err_buf,"num:%d", num);
            debuginfo(err_buf, __FILE__, __LINE__);
            return ERR_SCRIPT_RETURN_TOO_MUCH;
        }

        *out_len = lua_tonumber(L, -2);
        if (*out_len < 1)
        {
            debuginfo("script out_len is 0", __FILE__, __LINE__);
            return ERR_SCRIPT_RETURN_LEN_INVAILD;
        }
        switch(lua_type(L, -1))
        {
            case LUA_TSTRING:
                memcpy(out, lua_tostring(L, -1), *out_len);
                *out_type = STRING;
                break;   
            case LUA_TBOOLEAN:
                out[0] =  lua_toboolean(L, -1);
                //*out_len = 1;
                *out_type = BOOLEAN;
                break;
            case LUA_TNUMBER:
                *(size_t *)out = lua_tointeger(L, -1);
                //*out_len = 8;
                *out_type = INTEGER;
                break; 
            default:
                char err_buf[255];
                sprintf((char *)err_buf,"out_type:%d", lua_type(L, -1));
                debuginfo(err_buf, __FILE__, __LINE__);
                *out_len = 0;
                return ERR_SCRIPT_RETURN_TYPE_INVAILD;
        }
    }else
    {
        return ERR_SCRIPT_EXEC_ERROR;
    }

    return 0;
}

int tsg_destory_lua(tsg_lua_handle lua)
{
    lua_State *L = (lua_State *)lua;
    jmp_buf *lua_exception = NULL;

    if (L == NULL)
    {
        debuginfo("lua VM is null.", __FILE__, __LINE__);
        return ERR_LUAVM_ISNULL;
    }
    lua_exception = (jmp_buf *)lua_getexdata(L);
    if (lua_exception != NULL)
    {
        free(lua_exception);
    }

    lua_close(L);
    return 0;
}

