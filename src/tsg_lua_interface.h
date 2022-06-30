/*************************************************************************
    > File Name: tsg_lua_interface.h
    > Author: pxz
    > Created Time: Wed 08 Jul 2020 02:50:42 PM CST
 ************************************************************************/
#ifndef __TSG_LUA_INTERFACE__
#define __TSG_LUA_INTERFACE__

enum type
{
    NIL = 0,
    STRING = 1,
    INTEGER,
    BOOLEAN,
};

struct lua_arg_t
{
    int type;
    int len; //仅当type == STRING时有效
    union{
        char *str;
        long num;
        bool flag;
    };
};

struct lua_KV_t
{
    char *key;
    char *value;
};

struct lua_multi_KV_t
{
    int KV_num;
    struct lua_KV_t *KV;
};

struct lua_data_t
{
    size_t len;
    char *data;
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
    ERR_LUA_ID					                = -36,      /* lua id 错误*/
    ERR_EXPECT_TYPE_IS_NIL                      = -37,      /* 预期脚本返回类型为空 */
    ERR_UNKNOWN                                 = -38,      /* 未知错误 */
    ERR_RETUNR_NIL                              = 1,        /* 脚本执行成功，但是返回空*/
    ERR_RETURN_TYPE_NOT_MATCH_EXPECT            = 2,        /* 脚本执行成功，但是返回值类型与预期不符 */
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
 * 输入输出参数：
 *              size_t *out_type           要求输入一个不为NIL的预期类型， 调用完成后会输出结果的类型
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
 * 输入输出参数：
 *              size_t *out_type           要求输入一个不为NIL的预期类型， 调用完成后会输出结果的类型
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
 * 输入输出参数：
 *              size_t *out_type           要求输入一个不为NIL的预期类型， 调用完成后会输出结果的类型
 * 返回值：     int                         成功，返回0；失败,返回错误值err_value */
int tsg_lua_cache_exec(tsg_lua_handle L, size_t script_id, const char *in, size_t in_len, char *out, size_t *out_len, size_t *out_type);

/* 函数名：     tsg_destory_lua         
 * 输入参数：   tsg_lua_handle L           指定虚拟机
 * 返回值：     int                         成功，返回0；失败返回错误值err_value */
int tsg_destory_lua(tsg_lua_handle L);

/* 函数名：     lua_poll_interactive_stack_param         
 * 描述：       此函数用来获取当前C与LUA的交互栈的参数
 * 输入参数：   tsg_lua_handle L           指定虚拟机
 * 输入参数：   int  argc                   指定交互栈上应该返回多少个参数，参数个数不同，返回错误
 * 输入输出参数：lua_arg_t *argv             指定交互栈上返回的参数类型，参数类型不同，返回错误，参数类型相同，会记录该类型值
 * 返回值：     int                         成功，返回0；失败返回错误值err_value */
int lua_poll_interactive_stack_param(tsg_lua_handle L, int argc, lua_arg_t *argv);

/* 函数名：     lua_push_interactive_stack_param         
 * 描述：       此函数用来获取当前C与LUA的交互栈的参数
 * 输入参数：   tsg_lua_handle L            指定虚拟机
 * 输入参数：   int  argc                   指定交互栈上应该返回多少个参数，参数个数不同，返回错误
 * 输入参数：   lua_arg_t *argv             指定交互栈上返回的参数类型，参数类型不同，返回错误，参数类型相同，会记录该类型值
 * 输入参数：   enum TYPE lua_type          将输入参数转换为指定类型并入到C与lua的交互栈上
 * 返回值：     int                         成功，返回0；失败返回错误值err_value */
int lua_push_interactive_stack_param(tsg_lua_handle L, int argc, lua_arg_t *argv, enum type lua_type);

/* 函数名：     lua_get_userdata         
 * 描述：       此函数用来获取exec系列函数设置的userdata
 * 输入参数：   tsg_lua_handle L            指定虚拟机
 * 返回值：     int                         成功，返回0；失败返回错误值err_value */
void *lua_get_userdata(tsg_lua_handle L);

typedef int (*lua_function_ptr)(tsg_lua_handle L);
/* 函数名：     lua_register_function         
 * 描述：       此函数用来注册函数给LUA使用
 * 输入参数：   tsg_lua_handle L            指定虚拟机
 * 输入参数：   const char *function_set    注册函数所属函数集
 * 输入参数：   const char *function_name   注册函数的函数名
 * 输入参数：   lua_function_ptr function   注册函数的函数指针
 * 返回值：     int                         成功，返回0；失败返回 -1。*/
int lua_register_function(tsg_lua_handle L, const char *function_set, const char *function_name, lua_function_ptr function);

struct lua_script_context;
/* 函数名：     lua_set_script_context         
 * 输入参数：   tsg_lua_handle L  
 * 输出参数：   lua_script_context *context_id          成功时返回lua context        
 * 返回值：     int                                     成功，返回0；失败返回 -1*/
int lua_set_script_context(tsg_lua_handle L, struct lua_script_context *context);

/* 函数名：     lua_unset_script_context         
 * 输入参数：   tsg_lua_handle L  
 * 输出参数：   lua_script_context *context_id          成功时回收lua context        
 * 返回值：     int                                     成功，返回0；失败返回 -1*/
int lua_unset_script_context(tsg_lua_handle L, struct lua_script_context *context);

/* 函数名：     lua_cache_exec_with_context         
 * 输入参数：   tsg_lua_handle L  
                size_t script_id                      缓存的脚本id
                struct lua_data_t in                  待处理的数据
                void *userdata                        set_function的callpback 使用的userdata
                struct lua_script_context *context    lua上下文
 * 输出参数：   struct lua_arg_t *outvalue             脚本执行成功返回的内容      
 * 返回值：     int                                    成功，返回0；失败返回 -1*/
int lua_cache_exec_with_context(tsg_lua_handle L, size_t script_id, struct lua_data_t in, void *userdata, lua_script_context *context, struct lua_arg_t *outvalue);

/* 函数名：     lua_cache_exec_with_context         
 * 输入参数：   tsg_lua_handle L  
                struct lua_data_t script               lua脚本(lua脚本内容)
                struct lua_data_t in                   待处理的数据
                void *userdata                         set_function的callback 使用的userdata
                struct lua_script_context *context     lua上下文
 * 输出参数：   struct lua_arg_t *outvalue              脚本执行成功返回的内容      
 * 返回值：     int                                     成功，返回0；失败返回 -1*/
int lua_exec_with_context(tsg_lua_handle L, struct lua_data_t script, struct lua_data_t in, void *userdata, lua_script_context *context, struct lua_arg_t *outvalue);

/* 函数名：     lua_cache_exec_with_context         
 * 输入参数：   tsg_lua_handle L  
                struct script                           lua脚本(lua脚本路径)
                struct lua_data_t in                    待处理的数据
                void *userdata                          set_function的callback 使用的userdata
                struct lua_script_context *context      lua上下文文
 * 输出参数：   struct lua_arg_t *outvalue               脚本执行成功返回的内容      
 * 返回值：     int                                      成功，返回0；失败返回 -1*/
int lua_exec_file_with_context(tsg_lua_handle L, struct lua_data_t script, struct lua_data_t in, void *userdata, lua_script_context *context, struct lua_arg_t *outvalue);
#endif
