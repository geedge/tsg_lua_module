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
        char *buff;
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

/* input:           vm              a virtual machine
 *                  elua_function   the name of lua function 
 * return value :   0, successed; other failed.*/
int elua_remove_function(struct elua_vm *vm, const char *elua_function);

const char *elua_get_last_error_string(struct elua_vm *vm);

/* input:             vm             a virtual machine
 * return value:      successed, return 0; failed, return other */
int elua_destroy_vm(struct elua_vm *vm);

int elua_cbinding_get_input_params_num(struct elua_vm *vm);
int elua_cbinding_get_input_param(struct elua_vm *vm, int param_index, struct elua_data *param);
int elua_cbinding_append_output_params(struct elua_vm *vm, struct elua_data *params, int params_num);

struct elua_table *elua_create_table(struct elua_vm *vm);
int elua_add_table(struct elua_table *table, struct elua_data *key, struct elua_data *value);
int elua_update_table(struct elua_table *table, struct elua_data *key, struct elua_data *value);
int elua_delete_table(struct elua_table *table, struct elua_data *key);
struct elua_data *elua_search_table(struct elua_table *table, struct elua_data *key);
void elua_destroy_table(struct elua_table *table);

/* description:       get C userdata
 * input:             elua_handle L           a virtual machine
 * return value:      successed, return 0; failed, return other */
void *elua_get_execute_userdata(struct elua_vm *vm);

typedef int (*elua_cbinding_func_ptr)(struct elua_vm *vm);
/* description:       Regist a c function to lua
 * input:             vm                  a virtual machine
 * input:             func_space_name     The function space to which the function belongs
 * input:             elua_func_name      function name
 * input:             c_func_ptr          function pointer
 * return value:      int                 successed, return 0; failed, return other */
int elua_register_cbinding(struct elua_vm *vm, const char *func_space_name, const char *elua_func_name, elua_cbinding_func_ptr c_func_ptr);

struct elua_context;

/* input:             vm                  a virtual machine  
 * return value:      ctx_name            failed, return NULL; successed, return a context*/
struct elua_context *elua_create_context(struct elua_vm *vm, const char *ctx_name);

/* input:             context               context waiting to free        
 * return value:      int                   successed, return 0; failed, return other */
int elua_destroy_context(struct elua_context *ctx);

struct elua_script;

/* input:             vm                     a virtual machine
 *                    script                 script waiting to be cached, which must be script`s content
 *                    script_len             the length of script waiting to be cached
 *              
 * return value:     int                     successed, return a elua script; failed, return NULL */
struct elua_script *elua_cache_script(struct elua_vm *vm, const char *script, size_t script_len, size_t timeout_ms);
struct elua_script *elua_cache_script_file(struct elua_vm *vm, const char *script, size_t timeout_ms);

/* input:             vm                     a virtual machine
 *                    escript                elua script
 *                    input                  data waiting to be handled
 *                    input_len              input len
 *                    userdata               can get the userdata with elua_get_userdata(L)
 *                    context                can accessed with lua-name.context in lua script
 *                    timeout_ms             Maximum time to run the script.if timeout_ms > 0, jit.off; timeout_ms == 0, jit.on, not expired.
 * output:            outvalue               Requires input of an expected type, the type of the result that will be output after the call is complete
 * return value:      int                    successed, return 0; failed, return other */
int elua_execute_script(struct elua_script *escript, const char *input, size_t input_len, void *userdata, struct elua_context *ctx, struct elua_data *output);

/* input:             vm                     a virtual machine
 *                    escript                elua script
 * return value:      int                    successed, return 0; failed, return other */
int elua_cleanup_script(struct elua_script *escript);

