/*************************************************************************
    > File Name: main.c
    > Author: pxz
    > Created Time: Thu 09 Jul 2020 07:08:40 AM CST
 ************************************************************************/
extern "C"
{
    #include<stdio.h>
    #include<string.h>
}
#include"../include/tsg_lua_interface.h"
char *content0 = (char *)"HanMeimei said \"hello\" to LiLei";
char content1[255];

char *contents[] ={content0, content1};


int main()
{
    const char *script = "./script/find_hello.lua";
    const char *script1 = "./script/find_1.luajit";
    const char *script2 = "local str = tsg.data print(str) return string.len(str), str";
    const char *script3 = "\x1B\x4C\x4A\x02\x0A\x75\x02\x00"\
        "\x04\x00\x07\x00\x11\x36\x00\x00"\
        "\x00\x39\x00\x01\x00\x36\x01\x02"\
        "\x00\x27\x03\x03\x00\x42\x01\x02"\
        "\x01\x36\x01\x02\x00\x27\x03\x04"\
        "\x00\x42\x01\x02\x01\x36\x01\x02"\
        "\x00\x27\x03\x03\x00\x42\x01\x02"\
        "\x01\x36\x01\x05\x00\x39\x01\x06"\
        "\x01\x12\x03\x00\x00\x42\x01\x02"\
        "\x02\x12\x02\x00\x00\x4A\x01\x03"\
        "\x00\x08\x6C\x65\x6E\x0B\x73\x74"\
        "\x72\x69\x6E\x67\x08\x73\x74\x72"\
        "\x10\x2D\x2D\x2D\x2D\x2D\x2D\x2D"\
        "\x2D\x2D\x2D\x2D\x0A\x70\x72\x69"\
        "\x6E\x74\x09\x64\x61\x74\x61\x08"\
        "\x74\x73\x67\x00";
    tsg_lua_handle L = NULL;
    content1[128] = 1;
    //memset(content1, 1, sizeof(content1));
    /* 初始化1个虚拟机 */
    printf("\n*********************tsg_lua_vm_create************************\n");
    printf("[C]:call tsg_lua_vm_create...\n");
    L = tsg_lua_vm_create();
    if (L == NULL)
    {
        printf("[C]:call tsg_lua_vm_create failed.\n");
        return -1;
    }
    printf("[C]:call tsg_lua_vm_create success.\t lua:%p\n", L);
    
    char out[255];
    memset(out, 0, sizeof(out));
    size_t out_len;
    size_t out_type;
    printf("\n=============================file==================================\n");
    printf("\n*********************tsg_lua_exec_file************************\n");
    printf("[C]:call tsg_lua_exec_file, exec %s ...\n", script);
    int ret = tsg_lua_exec_file(L, script, content0, strlen(content0), out, &out_len, &out_type);
    if (ret < 0)
    {
        printf("[C]:call tsg_lua_exec_file failed.\n");
        return -1;
    }
    printf("[C]:call tsg_lua_exec_file success.\n");
    printf("[C]:out_len:%ld\n", out_len);
    printf("[C]:out_type:%ld\n", out_type);
    switch(out_type)
    {
        case STRING:
            printf("[C]:out_data:%s\n", out);
            break;
        case INTEGER:
            printf("[C]:out_data:%ld\n", *(long int *)out);
            break;
        case BOOLEAN:
            printf("[C]:out_data:%d\n", out[0]);
            break;
    }

    printf("\n*********************tsg_lua_exec_file************************\n");
    printf("[C]:call tsg_lua_exec_file, exec %s ...\n", script1);
    ret = tsg_lua_exec_file(L, script1, content0, strlen(content0), out, &out_len, &out_type);
    if (ret < 0)
    {
        printf("[C]:call tsg_lua_exec_file failed.\n");
        return -1;
    }
    printf("[C]:call tsg_lua_exec_file success.\n");
    printf("[C]:out_len:%ld\n", out_len);
    printf("[C]:out_type:%ld\n", out_type);
    switch(out_type)
    {
        case STRING:
            printf("[C]:out_data:%s\n", out);
            break;
        case INTEGER:
            printf("[C]:out_data:%ld\n", *(long int *)out);
            break;
        case BOOLEAN:
            printf("[C]:out_data:%d\n", out[0]);
            break;
    }

    /* lua 缓存脚本， 再运行 */
    memset(out, 0, sizeof(out));
    printf("\n*********************tsg_lua_cache_script_file************************\n");
    printf("[C]: call tsg_lua_cache_script_file, script:%s ...\n", script);
    int script_id = tsg_lua_cache_script_file(L, script);
    if (script_id < 0)
    {
        printf("[C]: call tsg_lua_cache_script_file failed.\n");
        return -1;
    }
    printf("[C]: call tsg_lua_cache_script_file success.\t script_id:%d\n", script_id);
    
    printf("\n*********************tsg_lua_cache_exec************************\n");
    printf("[C]: call tsg_lua_cache_exec ...\n");
    ret = tsg_lua_cache_exec(L, script_id, content1, 255, out, &out_len, &out_type);
    if (ret < 0)
    {
        printf("[C]: call tsg_lua_cache_exec failed.\n");
        return -1;
    }
    printf("[C]: call tsg_lua_cache_exec success\n");
    printf("[C]:out_len:%ld\n", out_len);
    printf("[C]:out_type:%ld\n", out_type);
    switch(out_type)
    {
        case STRING:
            printf("[C]:out_data:%s\n", out);
            break;
        case BOOLEAN:
            printf("[C]:out_data:%d\n", out[0]);
            break;
        case INTEGER:
            printf("[C]:out_data:%ld\n", *(long int *)out);
            break;
    }
    
    memset(out, 0, sizeof(out));
    printf("\n*********************tsg_lua_cache_script_file************************\n");
    printf("[C]: call tsg_lua_cache_script_file, script:%s ...\n", script1);
    int script_id1 = tsg_lua_cache_script_file(L, script1);
    if (script_id1 < 0)
    {
        printf("[C]: call tsg_lua_cache_script_file failed.\n");
        return -1;
    }
    printf("[C]: call tsg_lua_cache_script_file success.\t script_id:%d\n", script_id1);
    
    printf("\n*********************tsg_lua_cache_exec************************\n");
    printf("[C]: call tsg_lua_cache_exec ...\n");
    ret = tsg_lua_cache_exec(L, script_id1, content1, 255, out, &out_len, &out_type);
    if (ret < 0)
    {
        printf("[C]: call tsg_lua_cache_exec failed.\n");
        return -1;
    }
    printf("[C]: call tsg_lua_cache_exec success\n");
    printf("[C]:out_len:%ld\n", out_len);
    printf("[C]:out_type:%ld\n", out_type);
    switch(out_type)
    {
        case STRING:
            printf("[C]:out_data:%s\n", out);
            break;
        case BOOLEAN:
            printf("[C]:out_data:%d\n", out[0]);
            break;
        case INTEGER:
            printf("[C]:out_data:%ld\n", *(long int *)out);
            break;
    }

    /****************************************************************************************/    
    printf("\n=============================string==================================\n");
    memset(out, 0, sizeof(out));
    printf("\n*********************tsg_lua_exec************************\n");
    printf("[C]: call tsg_lua_exec, text-script ...\n");
    ret = tsg_lua_exec(L, script2, strlen(script2), "hello wrold", 11, out, &out_len, &out_type);
    if (ret < 0)
    {
        printf("[C]: call tsg_lua_exec failed ...\n");
        return -1;
    }
    printf("[C]: call tsg_lua_exec success ...\n");
    printf("[C]:out_len:%ld\n", out_len);
    printf("[C]:out_type:%ld\n", out_type);
    switch(out_type)
    {
        case STRING:
            printf("[C]:out_data:%s\n", out);
            break;
        case BOOLEAN:
            printf("[C]:out_data:%d\n", out[0]);
            break;
        case INTEGER:
            printf("[C]:out_data:%ld\n", *(long int *)out);
            break;
    }

    memset(out, 0, sizeof(out));
    printf("\n*********************tsg_lua_exec************************\n");
    printf("[C]: call tsg_lua_exec, bytecode-script ...\n");
    ret = tsg_lua_exec(L, script3, 124, "hello wrold", 11, out, &out_len, &out_type);
    if (ret < 0)
    {
        printf("[C]: call tsg_lua_exec failed ...\n");
        return -1;
    }
    printf("[C]: call tsg_lua_exec success ...\n");
    printf("[C]:out_len:%ld\n", out_len);
    printf("[C]:out_type:%ld\n", out_type);
    switch(out_type)
    {
        case STRING:
            printf("[C]:out_data:%s\n", out);
            break;
        case BOOLEAN:
            printf("[C]:out_data:%d\n", out[0]);
            break;
        case INTEGER:
            printf("[C]:out_data:%ld\n", *(long int *)out);
            break;
    }
    memset(out, 0, sizeof(out));
    printf("\n*********************tsg_lua_cache_script************************\n");
    printf("[C]: call tsg_lua_cache_script, text-script  ...\n");
    int script_id2 = tsg_lua_cache_script(L, script2, strlen(script2));
    if (script_id2 < 0)
    {
        printf("[C]: call tsg_lua_cache_script failed.\n");
        return -1;
    }
    printf("[C]: call tsg_lua_cache_script success.\t script_id:%d\n", script_id2);

    printf("\n*********************tsg_lua_cache_exec************************\n");
    printf("[C]: call tsg_lua_cache_exec ...\n");
    ret = tsg_lua_cache_exec(L, script_id2, "hello wrold", 11, out, &out_len, &out_type);
    if (ret < 0)
    {
        printf("[C]: call tsg_lua_cache_exec failed.\n");
        return -1;
    }
    printf("[C]: call tsg_lua_cache_exec success\n");
    printf("[C]:out_len:%ld\n", out_len);
    printf("[C]:out_type:%ld\n", out_type);
    switch(out_type)
    {
        case STRING:
            printf("[C]:out_data:%s\n", out);
            break;
        case BOOLEAN:
            printf("[C]:out_data:%d\n", out[0]);
            break;
        case INTEGER:
            printf("[C]:out_data:%ld\n", *(long int *)out);
            break;
    }
    memset(out, 0, sizeof(out));
    printf("\n*********************tsg_lua_cache_script************************\n");
    printf("[C]: call tsg_lua_cache_script, text-script  ...\n");
    int script_id3 = tsg_lua_cache_script(L, script3, 124);
    if (script_id3 < 0)
    {
        printf("[C]: call tsg_lua_cache_script failed.\n");
        return -1;
    }
    printf("[C]: call tsg_lua_cache_script success.\t script_id:%d\n", script_id3);

    printf("\n*********************tsg_lua_cache_exec************************\n");
    printf("[C]: call tsg_lua_cache_exec ...\n");
    ret = tsg_lua_cache_exec(L, script_id3, "hello world", 11, out, &out_len, &out_type);
    if (ret < 0)
    {
        printf("[C]: call tsg_lua_cache_exec failed.\n");
        return -1;
    }
    printf("[C]: call tsg_lua_cache_exec success\n");
    printf("[C]:out_len:%ld\n", out_len);
    printf("[C]:out_type:%ld\n", out_type);
    switch(out_type)
    {
        case STRING:
            printf("[C]:out_data:%s\n", out);
            break;
        case BOOLEAN:
            printf("[C]:out_data:%d\n", out[0]);
            break;
        case INTEGER:
            printf("[C]:out_data:%ld\n", *(long int *)out);
            break;
    }
    /* lua 注销脚本 */
    memset(out, 0, sizeof(out));
    printf("\n*********************tsg_lua_uncache_script************************\n");
    printf("[C]: call tsg_lua_uncache_script, script_id:%d ...\n", script_id);
    ret = tsg_lua_uncache_script(L, script_id);
    if (ret < 0)
    {
        printf("[C]: call tsg_lua_uncache_script failed.\n");
        return -1;
    }
    printf("[C]: call tsg_lua_uncache_script success\n");
    printf("\n*********************tsg_lua_uncache_script************************\n");
    printf("[C]: call tsg_lua_uncache_script, script_id:%d ...\n", script_id1);
    ret = tsg_lua_uncache_script(L, script_id1);
    if (ret < 0)
    {
        printf("[C]: call tsg_lua_uncache_script failed.\n");
        return -1;
    }
    printf("[C]: call tsg_lua_uncache_script success\n");
    printf("\n*********************tsg_lua_uncache_script************************\n");
    printf("[C]: call tsg_lua_uncache_script, script_id:%d ...\n", script_id2);
    ret = tsg_lua_uncache_script(L, script_id2);
    if (ret < 0)
    {
        printf("[C]: call tsg_lua_uncache_script failed.\n");
        return -1;
    }
    printf("[C]: call tsg_lua_uncache_script success\n");
    printf("\n*********************tsg_lua_uncache_script************************\n");
    printf("[C]: call tsg_lua_uncache_script, script_id:%d ...\n", script_id3);
    ret = tsg_lua_uncache_script(L, script_id3);
    if (ret < 0)
    {
        printf("[C]: call tsg_lua_uncache_script failed.\n");
        return -1;
    }
    printf("[C]: call tsg_lua_uncache_script success\n");
        
    tsg_lua_cache_exec(L, script_id, content1, 255, out, &out_len, &out_type);


    /* 关闭lua */
    tsg_destory_lua(L);
    printf("[C]:Every day is good day!\n");

    return 0;
} 
