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
#include"../src/elua.h"
char *content0 = (char *)"HanMeimei said \"hello\" to LiLei";
char content1[255];

char *contents[] ={content0, content1};


int main()
{
    const char *script = "./script/find_hello.lua";
    const char *script1 = "./script/find_1.luajit";
    #if 0
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
        #endif
    elua_vm *L = NULL;
    content1[128] = 1;
    //memset(content1, 1, sizeof(content1));
    /* 初始化1个虚拟机 */
    printf("\n*********************elua_create_vm************************\n");
    printf("[C]:call elua_create_vm...\n");
    L = elua_create_vm(NULL);
    if (L == NULL)
    {
        printf("[C]:call elua_create_vm failed.\n");
        return -1;
    }
    printf("[C]:call elua_create_vm success.\t lua:%p\n", L);
    
    struct elua_data output;
    output.type = STRING;
    output.len = 1024;
    char buff[1024];
    memset(buff, 0, 1024);
    output.string = buff;

    /* lua 缓存脚本， 再运行 */
    printf("\n*********************elua_cache_script_file************************\n");
    printf("[C]: call elua_cache_script_file, script:%s ...\n", script);
    struct elua_script *escript = elua_cache_script_file(L, script, 0);
    if (escript == NULL)
    {
        printf("[C]: call elua_cache_script_file failed.\n");
        return -1;
    }
    printf("[C]: call elua_cache_script_file success.\n");
    
    printf("\n*********************elua_execute_script************************\n");
    printf("[C]: call elua_execute_script ...\n");
    int ret = elua_execute_script(escript, content1, 255, NULL, NULL, &output);
    if (ret < 0)
    {
        printf("[C]: call elua_execute_script failed.\n");
        return -1;
    }
    printf("[C]: call elua_execute_script success\n");
    printf("[C]:out_len:%zu\n", output.len);
    printf("[C]:out_type:%d\n", output.type);
    switch(output.type)
    {
        case STRING:
            printf("[C]:out_data:%s\n", output.string);
            break;
        case BOOLEAN:
            printf("[C]:out_data:%d\n", output.true_or_false);
            break;
        case INTEGER:
            printf("[C]:out_data:%ld\n", *(long int *)output.integer);
            break;
        default:
            break;
    }
    
    printf("\n*********************elua_cache_script_file************************\n");
    printf("[C]: call elua_cache_script_file, script:%s ...\n", script1);
    struct elua_script *escript1 = elua_cache_script_file(L, script1, 0);
    if (escript1 == NULL)
    {
        printf("[C]: call elua_cache_script_file failed.\n");
        return -1;
    }
    printf("[C]: call elua_cache_script_file success.\n");
    
    printf("\n*********************elua_execute_script************************\n");
    printf("[C]: call elua_execute_script ...\n");
    ret = elua_execute_script(escript1, content1, 255, NULL, NULL, &output);
    if (ret < 0)
    {
        printf("[C]: call elua_execute_script failed.\n");
        return -1;
    }
    printf("[C]: call elua_execute_script success\n");
    printf("[C]:out_len:%zu\n", output.len);
    printf("[C]:out_type:%d\n", output.type);
    switch(output.type)
    {
        case STRING:
            printf("[C]:out_data:%s\n", output.string);
            break;
        case BOOLEAN:
            printf("[C]:out_data:%d\n", output.true_or_false);
            break;
        case INTEGER:
            printf("[C]:out_data:%ld\n", output.integer);
            break;
        default:
            break;
    }

    /* lua 注销脚本 */
    printf("\n*********************elua_cleanup_script************************\n");
    printf("[C]: call elua_cleanup_script...\n");
    ret = elua_cleanup_script(escript);
    if (ret < 0)
    {
        printf("[C]: call elua_cleanup_script failed.\n");
        return -1;
    }
    printf("[C]: call elua_cleanup_script success\n");
    printf("\n*********************elua_cleanup_script************************\n");
    printf("[C]: call elua_cleanup_script...\n");
    ret = elua_cleanup_script(escript1);
    if (ret < 0)
    {
        printf("[C]: call elua_cleanup_script failed.\n");
        return -1;
    }
    printf("[C]: call elua_cleanup_script success\n");

    /* 关闭lua */
    elua_destroy_vm(L);
    printf("[C]:Every day is good day!\n");

    return 0;
} 
