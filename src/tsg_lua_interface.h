/*************************************************************************
    > File Name: tsg_lua_interface.h
    > Author: pxz
    > Mail: 1500428751@qq.com 
    > Created Time: Wed 08 Jul 2020 02:50:42 PM CST
 ************************************************************************/
#ifndef __TSG_LUA_INTERFACE__
#define __TSG_LUA_INTERFACE__

enum type
{
    STRING = 1,
    INTEGER,
    BOOLEAN,
};

enum err_value
{
    ERR_LUAVM_ISNULL                            = -1,       /* 输入虚拟机参数为空 */
    ERR_SCRIPT_ISNULL                           = -2,       /* 输入脚本为空 */
    ERR_INPUT_ISNULL                            = -3,       /* 输入数据为空 */
    ERR_IN_LEN_INVAILD                          = -4,       /* 输入数据长度小于1 */
    ERR_SCRIPT_FUNCTION_INVAILD                 = -5,       /* 脚本函数名无效 */
    ERR_SCRIPT_RETURN_LEN_INVAILD               = -6,       /* 脚本返回值长度为0 */
    ERR_SCRIPT_RETURN_TOO_MUCH                  = -7,       /* 脚本返回值太多 */
    ERR_SCRIPT_RETURN_TOO_FEW                   = -8,       /* 脚本返回值太少 */
    ERR_SCRIPT_RETURN_TYPE_INVAILD              = -9,       /* 脚本返回值类型错误 */
    ERR_SCRIPT_SYNTAX_ERROR                     = -10,      /* 脚本语法错误 */
    ERR_SCRIPT_END_EXPECTED                     = -11,      /* 脚本缺少end */
    ERR_SCRIPT_THEN_EXPECTED                    = -12,      /* 脚本缺少then */
    ERR_SCRIPT_DO_EXPECTED                      = -13,      /* 脚本缺少do */
    ERR_SCRIPT_EOF_EXPECTED                     = -14,      /* 脚本缺少eof */
    ERR_SCRIPT_EQUAL_IN_EXPECTED                = -15,      /* 脚本缺少‘=’或者‘in’ */
    ERR_SCRIPT_UNEXPECTED_SYMBOL                = -16,      /* 脚本中出现不可预期的错误符号 */
    ERR_SCRIPT_NAME_EXPECTED                    = -17,      /* 脚本中for后面没有表达式 */
    ERR_SCRIPT_RIGHT_CURVES_BRACKET_EXPECTED    = -18,      /* 脚本中缺少')' */
    ERR_SCRIPT_RIGHT_CURLY_BRACKET_EXPECTED     = -19,      /* 脚本中缺少'}' */
    ERR_SCRIPT_EXEC_ERROR                       = -20,      /* 脚本运行错误 */
    ERR_SCRIPT_STRING_EXPECTED_BUT_NIL          = -21,      /* 脚本参数错误，尝试访问string类型变量，但该变量是nil类型 */
    ERR_SCRIPT_STRING_EXPECTED_BUT_BOOL         = -22,      /* 脚本参数错误，尝试访问string类型变量，但该变量是bool类型 */
    ERR_SCRIPT_STRING_EXPECTED_BUT_TABLE        = -23,      /* 脚本参数错误，尝试访问string类型变量，但该变量是table类型 */
    ERR_SCRIPT_CALL_GLOBAL_BUT_NIL              = -24,      /* 脚本尝试访问一个不存在的变量 */
    ERR_SCRIPT_NOT_EXIT                         = -25,      /* 脚本不存在 */
    ERR_SCRIPT_BYTECODE_NO_HEADER               = -26,      /* 字节码脚本没有header */
    ERR_SCRIPT_BYTECODE_BAD_HEADER              = -27,      /* 字节码脚本header错误 */
    ERR_SCRIPT_BYTECODE_VERSION_UNSUPPORTED     = -28,      /* 字节码脚本的版本不支持 */
    ERR_SCRIPT_BYTECODE_ONLY_SUPPORT_LUAJIT     = -29,      /* 只支持luajit的字节码脚本 */
    ERR_SCRIPT_BYTECODE_NOT_SUPPORT_BIG_ENDIAN  = -30,      /* 不支持字节码脚本大端模式 */
    ERR_SCRIPT_BYTECODE_NOT_SUPPORT_DEBUG       = -31,      /* 不支持字节码脚本debug模式 */
    ERR_SCRIPT_IS_BAD                           = -32,      /* 脚本错误 */
    ERR_SCRIPT_ID_INVAILD                       = -33,      /* 脚本ID无效 */
    ERR_LUA_CACHE_FAILED                        = -34,      /* 脚本缓存失败 */
    ERR_MEM_NOT_ENOUGH                          = -35,      /* 空间不足 */
    ERR_LUA_ID					= -36,      /* lua id 错误*/
    ERR_UNKNOWN                                 = -37,      /* 未知错误 */
};

typedef void* tsg_lua_handle;

/* 函数名：tsg_lua_vm_create
 * 返回值：tsg_lua_handle  成功,返回一个虚拟机，
 *                          失败,返回NULL */
tsg_lua_handle tsg_lua_vm_create();

/* 函数名：     tsg_lua_exec
 * 输入参数：   tsg_lua_handle L        指定虚拟机
 *              char *script            待执行的lua脚本
 *              size_t *script_len          待执行的lua脚本大小
 *              char *in                输入待处理的数据
 *              size_t in_len              输入待处理的数据的长度
 * 输出参数：
 *              char *out               输出结果
 *              size_t *out_len            输出结果的长度
 *              size_t *out_type           输出结果的类型
 * 返回值：     int                     成功，返回0；失败返回错误值err_value */
int tsg_lua_exec(tsg_lua_handle L, const char *script, size_t script_len, const char *in, size_t in_len, char *out, size_t *out_len, size_t *out_type);

/* 函数名：     tsg_lua_exec_script
 * 输入参数：   tsg_lua_handle L        指定虚拟机
 *              char *script            待执行的lua脚本
 *              char *in                输入待处理的数据
 *              size_t in_len              输入待处理的数据的长度
 * 输出参数：
 *              char *out               输出结果
 *              size_t *out_len            输出结果的长度
 *              size_t *out_type           输出结果的类型
 * 返回值：     int                     成功，返回0；失败返回错误值err_value */
int tsg_lua_exec_file(tsg_lua_handle L, const char *script, const char *in, size_t in_len, char *out, size_t *out_len, size_t *out_type);

/* 函数名：     tsg_lua_cache_script 
 * 输入参数：   tsg_lua_handle L            指定虚拟机
 *              char *script            待缓存的lua脚本
 *              size_t *script_len          待缓存的lua脚本大小
 *              
 * 返回值：     int                     成功，返回一个大于1的值，是脚本在当前虚拟机的唯一标识；失败，返回错误值err_value */
int tsg_lua_cache_script(tsg_lua_handle L, const char *script, size_t script_len);

/* 函数名：     tsg_lua_cache_script_file 
 * 输入参数：   tsg_lua_handle L            指定虚拟机
 *              char *script            待缓存的lua脚本路径名
 *              
 * 返回值：     int                     成功，返回一个大于1的值，是脚本在当前虚拟机的唯一标识；失败，返回错误值err_value */
int tsg_lua_cache_script_file(tsg_lua_handle L, const char *script);

/* 函数名：     tsg_unchache_script 
 * 输入参数：   tsg_lua_handle L            指定虚拟机
 *              size_t script_id               当前虚拟机内指定脚本的唯一标识
 * 返回值：     int                         成功，返回0；失败，返回错误码err_value */
int tsg_lua_uncache_script(tsg_lua_handle L, size_t script_id);

/* 函数名：     tsg_cache_exec   
 * 输入参数：   tsg_lua_handle L            指定虚拟机
 *              size_t script_id               当前虚拟机内指定脚本的唯一标识
 *              char *in                    输入待处理的数据
 *              size_t in_len                  输入待处理的数据的长度
 * 输出参数：   char *out                   输出结果
 *              size_t *out_len                输出结果的长度
 *              size_t *out_type               输出结果的类型
 * 返回值：     int                         成功，返回0；失败,返回错误值err_value */
int tsg_lua_cache_exec(tsg_lua_handle L, size_t script_id, const char *in, size_t in_len, char *out, size_t *out_len, size_t *out_type);

/* 函数名：     tsg_destory_lua         
 * 输入参数：   tsg_lua_handle L           指定虚拟机
 * 返回值：     int                         成功，返回0；失败返回错误值err_value */
int tsg_destory_lua(tsg_lua_handle L);

#endif
