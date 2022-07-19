/*************************************************************************
    > File Name: tsg_lua_interface.h
    > Author: pxz
    > Created Time: Wed 08 Jul 2020 02:50:42 PM CST
 ************************************************************************/
#ifndef __TSG_LUA_INTERFACE__
#define __TSG_LUA_INTERFACE__
#include <stddef.h>

enum type
{
    NIL = 0,
    STRING = 1,
    INTEGER,
    BOOLEAN,
};

struct lua_arg_t
{
    size_t type;
    size_t len; //仅当type == STRING时有效
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
    ERR_LUAVM_ISNULL                            = -1,       /*The lua vm is not existed */
    ERR_SCRIPT_ISNULL                           = -2,       /* The entered script is null */
    ERR_INPUT_ISNULL                            = -3,       /* The entered data is null */
    ERR_IN_LEN_INVAILD                          = -4,       /* The length of entered data less then 1*/
    ERR_SCRIPT_RETURN_LEN_INVAILD               = -5,       /* The return value of the script has a length of 0 */
    ERR_SCRIPT_RETURN_TOO_MUCH                  = -6,       /* Too many return values of the script */
    ERR_SCRIPT_RETURN_TOO_FEW                   = -7,       /* Too few return values of the script */
    ERR_SCRIPT_RETURN_TYPE_INVAILD              = -8,       /* The script return a value which type is error */
    ERR_SCRIPT_SYNTAX_ERROR                     = -9,      /* Script syntax error */
    ERR_SCRIPT_END_EXPECTED                     = -10,      /* Script "end" expected */
    ERR_SCRIPT_THEN_EXPECTED                    = -11,      /* Script "then" expected */
    ERR_SCRIPT_DO_EXPECTED                      = -12,      /* Script "do" expected */
    ERR_SCRIPT_EOF_EXPECTED                     = -13,      /* Script "eof" expected */
    ERR_SCRIPT_EQUAL_IN_EXPECTED                = -14,      /* Scprit "=" or "in" expected */
    ERR_SCRIPT_UNEXPECTED_SYMBOL                = -15,      /* Script funexpected symbol */
    ERR_SCRIPT_NAME_EXPECTED                    = -16,      /* Script "<name>" expected */
    ERR_SCRIPT_RIGHT_CURVES_BRACKET_EXPECTED    = -17,      /* Script ")" expected */
    ERR_SCRIPT_RIGHT_CURLY_BRACKET_EXPECTED     = -18,      /* Script "}" expected */
    ERR_SCRIPT_EXEC_ERROR                       = -19,      /* Script run error */
    ERR_SCRIPT_STRING_EXPECTED_BUT_NIL          = -20,      /* Script bad argument, string expected, got nil */
    ERR_SCRIPT_STRING_EXPECTED_BUT_BOOL         = -21,      /* Script bad argument, string expected, got boolean */
    ERR_SCRIPT_STRING_EXPECTED_BUT_TABLE        = -22,      /* Script bad argument, string expected, got table */
    ERR_SCRIPT_CALL_GLOBAL_BUT_NIL              = -23,      /* Script attempt to call global value which is a nil value */
    ERR_SCRIPT_NOT_EXIT                         = -24,      /* The script is not existed */
    ERR_SCRIPT_BYTECODE_NO_HEADER               = -25,      /* The bytecode script has no header */
    ERR_SCRIPT_BYTECODE_BAD_HEADER              = -26,      /* The bytecode script has a bad header */
    ERR_SCRIPT_BYTECODE_VERSION_UNSUPPORTED     = -27,      /* Not support the bytecode version */
    ERR_SCRIPT_BYTECODE_ONLY_SUPPORT_LUAJIT     = -28,      /* Only support the bytecode script of luajit */
    ERR_SCRIPT_BYTECODE_NOT_SUPPORT_BIG_ENDIAN  = -29,      /* Not support the bytecoode script building with BIG ENDIAN */
    ERR_SCRIPT_BYTECODE_NOT_SUPPORT_DEBUG       = -30,      /* Not support the bytecode script building with Debug mode */
    ERR_SCRIPT_IS_BAD                           = -31,      /* The script is bad */
    ERR_SCRIPT_ID_INVAILD                       = -32,      /* The script id is invaild*/
    ERR_LUA_CACHE_FAILED                        = -33,      /* Cache script failed */
    ERR_MEM_NOT_ENOUGH                          = -34,      /* The memory of lua is not enough */
    ERR_EXPECT_TYPE_IS_NIL                      = -35,      /* The caller expect script return a nil value */
    ERR_LUA_FUNCTION_ARGC                       = -36,      /* lua cFunction, the number of input parameters does not match expection */
    ERR_LUA_FUNCTION_ARGV                       = -37,      /* lua cFunction, the type of input paramters does not match expection */
    ERR_PARAMETER                               = -38,      /* Paramter error */
    ERR_LUA_PRIVATE_INFO_IS_NIL                 = -39,      /* The private info of lua is nil */
    ERR_LUA_SET_CONTEXT_FAILED                  = -40,      /* lua malloc context failed */
    ERR_UNKNOWN                                 = -41,      /* Unkown */
    ERR_RETUNR_NIL                              = 1,        /* Script run successed, but return a nil value*/
    ERR_RETURN_TYPE_NOT_MATCH_EXPECT            = 2,        /* Script run successed，but the type of return value does not match expection */
};

struct lua_State;
typedef lua_State* tsg_lua_handle;

/* function name：  tsg_lua_vm_create
 * return value：   tsg_lua_handle      successed,return a virtual machine; failed, return NULL*/
tsg_lua_handle tsg_lua_vm_create();

/* function name:    tsg_lua_exec
 * input:            tsg_lua_handle L        a virtual machine
 *                   char *script            script waiting to be executed, which must be script`s content
 *                   size_t *script_len      the length of script waiting to be executed
 *                   char *in                data waiting to be handled
 *                   size_t in_len           the length of data waiting to be handled
 * output:           char *out               out value
 *                   size_t *out_len         the length of out value
 * input-output:     size_t *out_type        Requires input of an expected type that is not NIL, the type of the result that will be output after the call is complete
 * return value:     int                     successed return 0; failed return error code */
int tsg_lua_exec(tsg_lua_handle L, const char *script, size_t script_len, const char *in, size_t in_len, char *out, size_t *out_len, size_t *out_type);

/* function name:    tsg_lua_exec_file
 * input:            tsg_lua_handle L        a virtual machine
 *                   char *script            script waiting to be executed, which mush be script`s path
 *                   char *in                data waiting to be handled
 *                   size_t in_len           the length of data waiting to be handled
 * output:           char *out               out value
 *                   size_t *out_len         the length of out value
 * input-output:     size_t *out_type        Requires input of an expected type that is not NIL, the type of the result that will be output after the call is complete
 * return value:     int                     successed return 0; failed return error code */
int tsg_lua_exec_file(tsg_lua_handle L, const char *script, const char *in, size_t in_len, char *out, size_t *out_len, size_t *out_type);

/* function name:     tsg_lua_cache_script 
 * input:             tsg_lua_handle L       a virtual machine
 *                    char *script           script waiting to be cached, which must be script`s content
 *                    size_t *script_len     the length of script waiting to be cached
 *              
 * return value:     int                     successed, return a script id which is bigger than 1; failed, return error code */
int tsg_lua_cache_script(tsg_lua_handle L, const char *script, size_t script_len);

/* function name:     tsg_lua_cache_script_file
 * input:             tsg_lua_handle L       a virtual machine
 *                    char *script           script waiting to be cached, which must be script`s path
 *                    size_t *script_len     the length of script waiting to be cached
 *              
 * return value:     int                     successed, return a script id which is bigger than 1; failed, return error code */
int tsg_lua_cache_script_file(tsg_lua_handle L, const char *script);

/* function name:     tsg_unchache_script 
 * input:             tsg_lua_handle L       a virtual machine
 *                    size_t script_id       a script id
 * return value:      int                    successed, return 0; failed, return error code */
int tsg_lua_uncache_script(tsg_lua_handle L, size_t script_id);

/* function name:     tsg_cache_exec   
 * input:             tsg_lua_handle L        a virtual machine
 *                    size_t script_id        a script id
 *                    char *in                data waiting to be handled
 *                    size_t in_len           the length of data waiting to be handled
 * output:            char *out               out value
 *                    size_t *out_len         the length of out value
 * input-output:      size_t *out_type        Requires input of an expected type that is not NIL, the type of the result that will be output after the call is complete
 * return value:      int                     successed, return 0; failed, return error code */
int tsg_lua_cache_exec(tsg_lua_handle L, size_t script_id, const char *in, size_t in_len, char *out, size_t *out_len, size_t *out_type);

/* function name:     tsg_destory_lua         
 * input:             tsg_lua_handle L         a virtual machine
 * return value:      int                      successed, return 0; failed, return error code */
int tsg_destory_lua(tsg_lua_handle L);

int c_pull_param_from_lua(tsg_lua_handle lua, int *argc, lua_arg_t **argv);
int free_param_form_lua(int argc, lua_arg_t *argv);
int c_push_num_into_lua(tsg_lua_handle lua, long num);
int c_push_bool_into_lua(tsg_lua_handle lua, bool flag);
int c_push_nil_into_lua(tsg_lua_handle lua);
int c_push_string_into_lua(tsg_lua_handle lua, const char *str, size_t len);
int c_push_table_into_lua(tsg_lua_handle lua, const char **key_list, const char **value_list, size_t list_len);

/* function name:     lua_get_userdata         
 * description:       get C userdata
 * input:             tsg_lua_handle L           a virtual machine
 * return value:     int                         successed, return 0; failed, return error code */
void *lua_get_userdata(tsg_lua_handle L);

typedef int (*lua_function_ptr)(tsg_lua_handle L);
/* function name:     lua_register_function         
 * description:       Regist a c function to lua
 * input:   tsg_lua_handle L            a virtual machine
 * input:   const char *function_set    The function set to which the function belongs
 * input:   const char *function_name   function name
 * input:   lua_function_ptr function   function pointer
 * return value:     int                successed, return 0; failed return 1 */
int lua_register_function(tsg_lua_handle L, const char *function_set, const char *function_name, lua_function_ptr function);

struct lua_script_context_t;
typedef lua_script_context_t * lua_script_context;

/* function name:     lua_script_context_malloc         
 * input:             tsg_lua_handle L  
 * return value:      lua_script_context           failed, return NULL; successed, return a context*/
lua_script_context lua_script_context_malloc(tsg_lua_handle L);

/* function name:     lua_script_context_free         
 * input:             tsg_lua_handle L  
 * input:             lua_script_context context          context waiting to free        
 * return value:      int                                 successed, return 0; failed, return -1 */
int lua_script_context_free(tsg_lua_handle L, lua_script_context context);

/* function name:     lua_cache_exec   
 * input:             tsg_lua_handle L              a virtual machine
 *                    size_t script_id              a script id
 *                    struct lua_data_t in          data waiting to be handled
 *                    void *userdata                can get the userdata with lua_get_userdata(L)
 *                    lua_script_context context    Can accessed with lua-name.context in lua script
 * output:            struct lua_arg_t *outvalue    Requires input of an expected type that is not NIL, the type of the result that will be output after the call is complete
 * return value:      int                           successed, return 0; failed, return error code */
int lua_cache_exec(tsg_lua_handle L, size_t script_id, struct lua_data_t in, void *userdata, lua_script_context context, struct lua_arg_t *outvalue);

/* function name:     lua_exec   
 * input:             tsg_lua_handle L              a virtual machine
 *                    struct lua_data_t script      script waiting to be executed, which must be script`s content
 *                    struct lua_data_t in          data waiting to be handled
 *                    void *userdata                can get the userdata with lua_get_userdata(L)
 *                    lua_script_context context    Can accessed with lua-name.context in lua script
 * output:            struct lua_arg_t *outvalue    Requires input of an expected type that is not NIL, the type of the result that will be output after the call is complete
 * return value:      int                           successed, return 0; failed, return error code */
int lua_exec(tsg_lua_handle L, struct lua_data_t script, struct lua_data_t in, void *userdata, lua_script_context context, struct lua_arg_t *outvalue);

/* function name:     lua_exec_file   
 * input:             tsg_lua_handle L              a virtual machine
 *                    struct lua_data_t script      script waiting to be executed, which must be script`s path
 *                    struct lua_data_t in          data waiting to be handled
 *                    void *userdata                can get the userdata with lua_get_userdata(L)
 *                    lua_script_context context    Can accessed with lua-name.context in lua script
 * output:            struct lua_arg_t *outvalue    Requires input of an expected type that is not NIL, the type of the result that will be output after the call is complete
 * return value:      int                           successed, return 0; failed, return error code */
int lua_exec_file(tsg_lua_handle L, const char *script, struct lua_data_t in, void *userdata, lua_script_context context, struct lua_arg_t *outvalue);

/* function name：      tsg_lua_vm_create
 * input:               char *name          the name of virtual machine, default: "TSG"
 * return value：       tsg_lua_handle      successed,return a virtual machine; failed, return NULL*/
tsg_lua_handle tsg_lua_vm_create_with_name(const char *name);

int lua_get_error_code(tsg_lua_handle L);

int lua_remove_cmd(tsg_lua_handle L, const char *cmd);
#endif
