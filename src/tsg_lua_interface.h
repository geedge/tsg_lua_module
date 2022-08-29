/*************************************************************************
    > File Name: tsg_lua_interface.h
    > Author: pxz
    > Created Time: Wed 08 Jul 2020 02:50:42 PM CST
 ************************************************************************/
#ifndef __TSG_LUA_INTERFACE__
#define __TSG_LUA_INTERFACE__
#include <stddef.h>

enum lua_type
{
    NIL = 0,
    STRING = 1,
    INTEGER,
    BOOLEAN,
};

struct lua_data
{
    enum lua_type type;
    size_t len;
    union{
        char *string;
        long num;
        bool flag;
    };
};

struct lua_vm;

/* function name:     tsg_lua_vm_create
 * input:             char *name          the name of virtual machine, default: "TSG"
 * return value:      tsg_lua_handle      successed,return a virtual machine; failed, return NULL*/
struct lua_vm *lua_vm_create(const char *vm_name);

int lua_remove_function(struct lua_vm *vm, const char *lua_function);

const char *lua_get_last_error_string(struct lua_vm *vm);

/* function name:     tsg_destory_lua         
 * input:             tsg_lua_handle L         a virtual machine
 * return value:      int                      successed, return 0; failed, return error code */
int lua_vm_destory(struct lua_vm *vm);

struct lua_cbinding_params;
struct lua_cbinding_params *lua_cbinding_get_last_params(struct lua_vm *vm);
struct lua_data *lua_cbinging_get_next_para(struct lua_cbinding_params *params);
void lua_cbinding_free_params(struct lua_cbinding_params *params);

int lua_cbinding_return(struct lua_vm *vm, struct lua_data *data);
int lua_cbinding_return_pair(struct lua_vm *vm, struct lua_data *data1, struct lua_data *data2);

/* function name:     lua_get_userdata         
 * description:       get C userdata
 * input:             tsg_lua_handle L           a virtual machine
 * return value:     int                         successed, return 0; failed, return error code */
void *lua_get_exec_userdata(struct lua_vm *vm);

typedef int (*lua_cbinding_func_ptr)(struct lua_vm *vm);
/* function name:     lua_register_function         
 * description:       Regist a c function to lua
 * input:   tsg_lua_handle L            a virtual machine
 * input:   const char *function_set    The function set to which the function belongs
 * input:   const char *function_name   function name
 * input:   lua_function_ptr function   function pointer
 * return value:     int                successed, return 0; failed return 1 */
int lua_register_cbinding(struct lua_vm *vm, const char *namespace, const char *lua_func_name, lua_cbinding_func_ptr c_func_ptr);

struct lua_context;

/* function name:     lua_script_context_malloc         
 * input:             tsg_lua_handle L  
 * return value:      lua_script_context           failed, return NULL; successed, return a context*/
struct lua_context *lua_context_create(struct lua_vm *vm, const char *ctx_name);

/* function name:     lua_script_context_free         
 * input:             tsg_lua_handle L  
 * input:             lua_script_context context          context waiting to free        
 * return value:      int                                 successed, return 0; failed, return -1 */
int lua_context_destroy(struct lua_vm *vm, struct lua_context *ctx);

/* function name:     tsg_lua_cache_script 
 * input:             tsg_lua_handle L       a virtual machine
 *                    char *script           script waiting to be cached, which must be script`s content
 *                    size_t *script_len     the length of script waiting to be cached
 *              
 * return value:     int                     successed, return a script id which is bigger than 1; failed, return error code */
size_t lua_cache_script(struct lua_vm *vm, const char *script, size_t script_len);

/* function name:     lua_cache_exec   
 * input:             tsg_lua_handle L              a virtual machine
 *                    size_t script_id              a script id
 *                    struct lua_data_t in          data waiting to be handled
 *                    void *userdata                can get the userdata with lua_get_userdata(L)
 *                    lua_script_context context    Can accessed with lua-name.context in lua script
 *                    size_t timeout_ms             Maximum time to run the script.if timeout_ms > 0, jit.off; timeout_ms == 0, jit.on, not expired.
 * output:            struct lua_arg_t *outvalue    Requires input of an expected type that is not NIL, the type of the result that will be output after the call is complete
 * return value:      int                           successed, return 0; failed, return error code */
int lua_cache_exec(struct lua_vm *vm, size_t script_id, struct lua_data *in, void *userdata, struct lua_context *ctx, size_t timeout_ms, struct lua_data *out);

/* function name:     tsg_unchache_script 
 * input:             tsg_lua_handle L       a virtual machine
 *                    size_t script_id       a script id
 * return value:      int                    successed, return 0; failed, return error code */
int lua_uncache_script(struct lua_vm *vm, size_t script_id);

#endif
