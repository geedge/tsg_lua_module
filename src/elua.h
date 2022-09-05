/*************************************************************************
    > File Name: elua.h
    > Author: pxz
    > Created Time: Wed 01 Sep 2022 
 ************************************************************************/
#pragma once

#include <stddef.h>

enum elua_type
{
    NIL = 0,
    STRING = 1,
    INTEGER,
    BOOLEAN,
    TABLE,
};

struct elua_table;

struct elua_data
{
	enum elua_type type;
	size_t len;
    union{
        char *string;
        long integer;
        bool true_or_false;
		struct elua_table *table;
    };
};

struct elua_vm;

/* function name:     elua_create_vm
 * input:             char *name          the name of virtual machine, default: "TSG"
 * return value:      elua_handle      successed,return a virtual machine; failed, return NULL*/
struct elua_vm *elua_create_vm(const char *vm_name);

int elua_remove_function(struct elua_vm *vm, const char *elua_function);

const char *elua_get_last_error_string(struct elua_vm *vm);

/* function name:     destroy_lua         
 * input:             elua_handle L         a virtual machine
 * return value:      int                      successed, return 0; failed, return error code */
int elua_destroy_vm(struct elua_vm *vm);

int elua_cbinding_get_input_params_num(struct elua_vm *vm);
int elua_cbinding_get_input_param(struct elua_vm *vm, int param_index, struct elua_data *param);
int elua_cbinding_set_output_params(struct elua_vm *vm, struct elua_data *params, int params_num);  // only call once 

struct elua_table *elua_create_table(struct elua_vm *vm);
int elua_add_table(struct elua_table *table, struct elua_data *key, struct elua_data *value);
int elua_update_table(struct elua_table *table, struct elua_data *key, struct elua_data *value);
int elua_delete_table(struct elua_table *table, struct elua_data *key, struct elua_data *value);
struct elua_data *elua_search_table(struct elua_table *table, struct elua_data *key);
void elua_destroy_table(struct elua_table *table);

/* function name:     elua_get_userdata         
 * description:       get C userdata
 * input:             elua_handle L           a virtual machine
 * return value:     int                         successed, return 0; failed, return error code */
void *elua_get_execute_userdata(struct elua_vm *vm);

typedef int (*elua_cbinding_func_ptr)(struct elua_vm *vm);
/* function name:     elua_register_function         
 * description:       Regist a c function to lua
 * input:   elua_handle L            a virtual machine
 * input:   const char *function_set    The function set to which the function belongs
 * input:   const char *function_name   function name
 * input:   elua_function_ptr function   function pointer
 * return value:     int                successed, return 0; failed return 1 */
int elua_register_cbinding(struct elua_vm *vm, const char *func_space_name, const char *elua_func_name, elua_cbinding_func_ptr c_func_ptr);

struct elua_context;

/* function name:     elua_context_malloc         
 * input:             elua_handle L  
 * return value:      elua_context           failed, return NULL; successed, return a context*/
struct elua_context *elua_create_context(struct elua_vm *vm, const char *ctx_name);

/* function name:     elua_context_free         
 * input:             elua_handle L  
 * input:             elua_context context          context waiting to free        
 * return value:      int                                 successed, return 0; failed, return -1 */
int elua_destroy_context(struct elua_context *ctx);

struct elua_script;

/* function name:     elua_cache_script 
 * input:             elua_handle L       a virtual machine
 *                    char *script           script waiting to be cached, which must be script`s content
 *                    size_t *script_len     the length of script waiting to be cached
 *              
 * return value:     int                     successed, return a script id which is bigger than 1; failed, return error code */
struct elua_script *elua_cache_script(struct elua_vm *vm, const char *script, size_t script_len, size_t timeout_ms);
struct elua_script *elua_cache_script_file(struct elua_vm *vm, const char *script, size_t timeout_ms);

/* function name:     elua_execute_script   
 * input:             elua_handle L              a virtual machine
 *                    size_t script_id              a script id
 *                    struct elua_data_t in          data waiting to be handled
 *                    void *userdata                can get the userdata with elua_get_userdata(L)
 *                    elua_context context    Can accessed with lua-name.context in lua script
 *                    size_t timeout_ms             Maximum time to run the script.if timeout_ms > 0, jit.off; timeout_ms == 0, jit.on, not expired.
 * output:            struct estruct elua_data *outvalue    Requires input of an expected type that is not NIL, the type of the result that will be output after the call is complete
 * return value:      int                           successed, return 0; failed, return error code */
int elua_execute_script(struct elua_script *escript, const char *input, size_t input_len, void *userdata, struct elua_context *ctx, struct elua_data *output);

/* function name:     unchache_script 
 * input:             elua_handle L       a virtual machine
 *                    size_t script_id       a script id
 * return value:      int                    successed, return 0; failed, return error code */
int elua_cleanup_script(struct elua_script *escript);

