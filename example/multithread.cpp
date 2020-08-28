/*************************************************************************
    > File Name: main.c
    > Author: pxz
    > Mail: 1500428751@qq.com 
    > Created Time: Thu 09 Jul 2020 07:08:40 AM CST
 ************************************************************************/
extern "C"
{
    #include<stdio.h>
    #include<stdlib.h>
    #include<string.h>
    #include<pthread.h>
    #include<unistd.h>
    #include<time.h>
}
#include"../include/tsg_lua_interface.h"
#ifndef MAX_THREAD_NUM
#define MAX_THREAD_NUM 9
#endif
#define MAX_SCRIPT_NUM 3
#define SCRIPT_NO_CACHE -1

typedef struct lua_script_info_s{
    const char *script_data;
    int script_len;
    int script_id;
}lua_script_info_t;

typedef struct lua_data_s{
    char data[1024];
    int data_len;
}lua_data_t;

typedef struct{
    lua_data_t data;
    int thread_id;
    tsg_lua_handle lua;
}userdata_t;


/* 存储脚本以及脚本id */
#define SCRIPT_PROTOCOL_RECOGNITION     "./script/protocol_recognition.lua"
#define SCRIPT_COUNT_1                  "./script/count_1.lua"
#define SCRIPT_MATCH_12345              "./script/match_12345.lua"

void debuginfo(int thread_id, char *out, int out_len, int out_type) 
{ 
    printf("[C][thread_id:%d]output.len:%d\n", thread_id, out_len); 
    printf("[C][thread_id:%d]output.type:%d\n", thread_id, out_type);   
    switch(out_type)  
    {   
        case STRING:   
            printf("[C][thread_id:%d]output.data:%s\n", thread_id, out);    
            break;  
        case BOOLEAN:   
            printf("[C][thread_id:%d]outpt.data:%d\n", thread_id, out[0]);    
            break;  
        case INTEGER:   
            printf("[C][thread_id:%d]outpt.data:%ld\n", thread_id, *(long int *)out); 
            break;  
        default:

            break;
    } 
    return;
}

void *lua_function(void *ptr)
{
    int ret;
    char out[255];
    size_t out_len = 0;
    size_t out_type = 0;
    memset(out, 0, 255);
    tsg_lua_handle L = NULL;

    userdata_t *userdata = (userdata_t *)ptr;
    lua_data_t *data = &userdata->data;
    int thread_id = userdata->thread_id;

    lua_script_info_t script_info[MAX_SCRIPT_NUM] =
    {   
        {SCRIPT_PROTOCOL_RECOGNITION, (int)strlen(SCRIPT_PROTOCOL_RECOGNITION), SCRIPT_NO_CACHE},   
        {SCRIPT_COUNT_1, (int)strlen(SCRIPT_COUNT_1), SCRIPT_NO_CACHE},   
        {SCRIPT_MATCH_12345, (int)strlen(SCRIPT_MATCH_12345), SCRIPT_NO_CACHE},   
    };

    printf("[C][thread_id:%d]:tsg_lua_vm_create start...\n", thread_id);
    L = tsg_lua_vm_create();
    if (L == NULL)
    {
        printf("[C][thread_id:%d]:tsg_lua_vm_create failed...\n", thread_id);
        return (void *)"lua vm create failed";
    }
    printf("[C][thread_id:%d]:lua:%p ...\n", thread_id, L);
    printf("[C][thread_id:%d]:tsg_lua_vm_create success...\n", thread_id);

    printf("[C][thread_id:%d]:tsg_lua_exec_file start...\n", thread_id);
    ret = tsg_lua_exec_file(L, script_info[0].script_data, data->data, data->data_len, out, &out_len, &out_type);
    if(ret < 0)
    {
        printf("[C][thread_id:%d]:tsg_lua_exec_file failed...\n", thread_id);
        return (void *)"exec failed";
    }
    debuginfo(thread_id, out, out_len, out_type);
    printf("[C][thread_id:%d]:tsg_lua_exec_file success...\n", thread_id);

    printf("[C][thread_id:%d]:tsg_lua_cache_script_file start...\n", thread_id);
    script_info[1].script_id = tsg_lua_cache_script_file(L, script_info[1].script_data);
    if (script_info[1].script_id < 0)
    {
        printf("[C][thread_id:%d]:tsg_lua_cache_script_file failed...\n", thread_id);
        return (void *)"cache script failed";
    }
    printf("[C][thread_id:%d]:script_id:%d ...\n", thread_id, script_info[1].script_id);
    printf("[C][thread_id:%d]:tsg_lua_cache_script_file success...\n", thread_id);

    printf("[C][thread_id:%d]:tsg_lua_cache_exec start...\n", thread_id);
    ret = tsg_lua_cache_exec(L, script_info[1].script_id, data->data, data->data_len, out, &out_len, &out_type);
    if(ret < 0)
    {
        printf("[C][thread_id:%d]:tsg_lua_cache_exec failed...\n", thread_id);
        return (void *)"cache exec failed";
    }
    debuginfo(thread_id, out, out_len, out_type);
    printf("[C][thread_id:%d]:tsg_lua_cache_exec success...\n", thread_id);
        
    printf("[C][thread_id:%d]:tsg_lua_cache_script_file start...\n", thread_id);
    script_info[2].script_id = tsg_lua_cache_script_file(L, script_info[2].script_data);
    if (script_info[2].script_id < 0)
    {
        printf("[C][thread_id:%d]:tsg_lua_cache_script_file failed...\n", thread_id);
        return (void *)"cache script failed";
    }
    printf("[C][thread_id:%d]:script_id:%d ...\n", thread_id, script_info[2].script_id);
    printf("[C][thread_id:%d]:tsg_lua_cache_script_file success...\n", thread_id);

    printf("[C][thread_id:%d]:tsg_lua_cache_exec start...\n", thread_id);
    ret = tsg_lua_cache_exec(L, script_info[2].script_id, data->data, data->data_len, out, &out_len, &out_type);
    if(ret < 0)
    {
        printf("[C][thread_id:%d]:tsg_lua_cache_exec failed...\n", thread_id);
        return (void *)"cache exec failed";
    }
    debuginfo(thread_id, out, out_len, out_type);
    printf("[C][thread_id:%d]:tsg_lua_cache_exec success...\n", thread_id);

    printf("[C][thread_id:%d]:tsg_lua_uncache_script start...\n", thread_id);
    ret = tsg_lua_uncache_script(L, script_info[1].script_id);
    if(ret < 0)
    {
        printf("[C][thread_id:%d]:tsg_lua_uncache_script failed...\n", thread_id);
        return (void *)"uncache script failed";
    }
    printf("[C][thread_id:%d]:tsg_uncache_script success...\n", thread_id);

    printf("[C][thread_id:%d]:tsg_lua_uncache_script start...\n", thread_id);
    ret = tsg_lua_uncache_script(L, script_info[2].script_id);
    if(ret < 0)
    {
        printf("[C][thread_id:%d]:tsg_lua_uncache_script failed...\n", thread_id);
        return (void *)"uncache script failed";
    }
    printf("[C][thread_id:%d]:tsg_uncache_script success...\n", thread_id);

    printf("[C][thread_id:%d]:tsg_destory_lua start...\n", thread_id);
    ret = tsg_destory_lua(L);
    if(ret < 0)
    {
        printf("[C][thread_id:%d]:tsg_destory_lua failed...\n", thread_id);
        return (void *)"uncache script failed";
    }
    printf("[C][thread_id:%d]:tsg_destory_lua success...\n", thread_id);
    pthread_exit((void *)"OK");
}


int main(int argc, char *argv[])
{
    int thread_num;
    pthread_t thread[MAX_THREAD_NUM] = {0};
    tsg_lua_handle lua[MAX_THREAD_NUM] = {0};
    int thread_sum;
    int ch, i;
    char buf[1024];
    int buf_len;
    userdata_t userdata[MAX_THREAD_NUM] = {0};
    lua_data_t *data = NULL;
    char *retval;

#ifndef MIN
#define MIN(a, b)   ((a) > (b) ?(b):(a))
#endif

    /* default */
    thread_sum = 3;
    strcpy(buf, "this is tcp");
    buf_len =  strlen("this is tcp");

    while((ch = getopt(argc, argv, "t:s:b:d:h")) != -1)
    {
        switch(ch)
        {
            case 't':
                thread_sum = atoi(optarg);
                if (thread_sum > MAX_THREAD_NUM)
                {
                    printf("MAX_THREAD_NUM is %d\n",MAX_THREAD_NUM);
                    thread_sum = MAX_THREAD_NUM;
                }
                break; 
            case 's':
                buf_len = MIN(1024,  strlen(optarg));
                strncpy(buf, optarg, buf_len);
                buf[buf_len] = '\0';
                break;          
            case 'b':
                buf_len = MIN(1024, strlen(optarg));
                for (i = 0; i < buf_len; i++)
                {
                    if ((optarg[i] != '0') && (optarg[i] != '1'))
                    {
                        printf("error: input wrong\n");
                        return -1;
                    }
                    buf[i] = optarg[i] - '0';
                }
                break;
            case 'd':
                buf_len = MIN(1024, strlen(optarg));
                for (i = 0; i < buf_len; i++)
                {
                    if ((optarg[i] < '0') && (optarg[i] > '9'))
                    {
                        printf("error: input wrong\n");
                        return -1;
                    }
                    buf[i] = optarg[i] - '0';
                }
                break;
            case 'h':
            default:
                printf("usage: multithread [-t set thread num] [-s input string] [-b input bin] [-d input integer] [-h show help info]\n");
                return -1;
                break;
        }
    }

    for (thread_num = 0; thread_num < thread_sum; thread_num++)
    {
        userdata[thread_num].thread_id = thread_num;
        data = &userdata[thread_num].data;
        memcpy(data->data, buf, buf_len);
        data->data_len = buf_len;
        data->data[buf_len] = thread_num;
        data->data_len++;
        lua[i] = tsg_lua_vm_create();
        data->lua = lua[i];
        pthread_create(&thread[thread_num], NULL, lua_function, (void *)&userdata[thread_num]);
    }

    for (thread_num = 0; thread_num < thread_sum; thread_num++)
    {
        pthread_join(thread[thread_num], (void **)&retval);
        printf("[C][thread_id:%d]: %s\n", thread_num, retval);
    }

    return 0;
} 
