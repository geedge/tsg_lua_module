/*************************************************************************
    > File Name: main.cpp
    > Author: pxz
    > Created Time: Sun 02 Aug 2020 11:41:22 AM CST
 ************************************************************************/
extern "C"
{
#include<stdio.h>
#include<pcap.h>
#include<stdint.h>
#include<string.h>
#include<arpa/inet.h>
#include<time.h>
#include<unistd.h>
#include<pthread.h>
#include<stdlib.h>
#include<dlfcn.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<dirent.h>
}
#include<tcmalloc.h>
#include"tsg_lua_interface.h"


#define MAC_FRAME_HEADER_LEN    14
#define IP_HEADER_LEN   20
#define TCP_HEADER_LEN  20

#ifndef MAX_THREAD_NUM
#define MAX_THREAD_NUM 16
#endif

#ifndef MIN
#define MIN(a, b)   ((a) > (b) ?(b):(a))
#endif

#define TSG_LUA_LOOP_TIMES 10000

int c_pcap_flag[MAX_THREAD_NUM];
int c_text_flag[MAX_THREAD_NUM];
int lua_pcap_flag[MAX_THREAD_NUM];
int lua_text_flag[MAX_THREAD_NUM];

typedef int (*c_handle_function)(const u_char *packet, uint32_t packet_len, const char *feature, char *result);
enum {
    text_type = 0x01,
    pcap_type = 0x02
};
typedef struct c_thread_info_s{
    size_t      thread_id;
    const char  *lib;
    const char  *input;
    const char  *feature;
    uint8_t     file_type;
}c_thread_info_t;

typedef struct c_pcap_info_s{
    c_handle_function   c_handle;
    size_t              thread_id;
    uint32_t            packet_num;
    uint64_t            total_time;
    const char          *feature;
    const char          *debuginfo;
}c_pcap_info_t;

typedef struct c_text_info_s{
    c_handle_function   c_handle;
    size_t              thread_id;
    uint64_t            total_time;
    const char          *feature;
    const char          *file_name;
    const char          *debuginfo;
}c_text_info_t;

typedef struct lua_thread_info_s{
    size_t      thread_id;
    const char  *script;
    const char  *input;
    const char  *feature;
    uint8_t     file_type;
}lua_thread_info_t;

typedef struct lua_pacp_info_s
{
    tsg_lua_handle L;
    size_t      script_id;
    size_t      thread_id;
    uint32_t    packet_num;
    uint64_t    total_time;
    const char  *feature;
    char        *debuginfo;
}lua_pcap_info_t;

typedef struct lua_text_info_s
{
    tsg_lua_handle L;
    size_t      script_id;
    size_t      thread_id;
    uint64_t    total_time;
    const char  *feature;
    const char  *file_name;
    char        *debuginfo;
}lua_text_info_t;

typedef struct conf_t
{
    uint32_t thread_num;
    char     feature[1024];
    char     input[128];
    char     script[128];
    char     lib[128];
    uint8_t  file_type;
    uint8_t  complete;
#define THREAD_NUM_COMPLETE 0x01
#define FEATURE_COMPLETE    0x02
#define INPUT_COMPLETE      0x04
#define SCRIPT_COMPLETE     0x08
#define LIB_COMPLETE        0x10
#define FILE_TYPE_COMPLETE  0x20
#define ALL_COMPLETE    (THREAD_NUM_COMPLETE | FEATURE_COMPLETE | INPUT_COMPLETE | SCRIPT_COMPLETE | LIB_COMPLETE | FILE_TYPE_COMPLETE)
}conf_t;

int read_file_list(const char *path, char file_name[][256])
{
    DIR *dir;
    struct dirent *ptr;
    int path_len = strlen(path);
    int file_num = 0;

    if ((dir=opendir(path)) == NULL)
    {
        perror("Open dir error...");
        exit(-1);
    }
    
    while (((ptr=readdir(dir)) != NULL) && (file_num < 100))
    {
        if(strcmp(ptr->d_name,".")==0 || strcmp(ptr->d_name,"..")==0)    ///current dir OR parrent dir
            continue;
        else if((ptr->d_type == 8) || (ptr->d_type == 10))
        {
            memcpy(file_name[file_num], path, path_len);
            strcat(file_name[file_num], ptr->d_name);
        }
        else if(ptr->d_type == 4)    ///dir
        {
            continue;
        }
        file_num++;
    }
    closedir(dir);
    return 0;
}

static int init_conf(char *path, conf_t *conf)
{
    char buf[1024];
    size_t buf_len = sizeof(buf);
    FILE *fp = fopen(path, "r");
    if (fp == NULL)
    {
        perror("error opening file");
        return -1;
    }
    while(fgets(buf, buf_len, fp) != NULL)
    {
        switch(buf[0])
        {
            case '#':
            case '\n':
                break;
            case '/':
                if (buf[1] == '*')
                {
                    while(fgets(buf, buf_len, fp) != NULL)
                    {
                        if ((buf[0] == '*') && (buf[1] == '/') && ((buf[3] == '\0') || (buf[3] == '\n')))
                            break;
                    }
                }else
                {
                    printf("error:unknow conf\n");
                    return -1;
                }
                break;
            case 't':
                if (conf->complete & THREAD_NUM_COMPLETE)
                {   
                    break;
                }
                if ((sscanf(buf, "thread_num %d", &conf->thread_num) == EOF) || (conf->thread_num == 0))
                {
                    printf("error: conf_file: thread_num error\n");
                    return -1;
                }
                conf->complete |= THREAD_NUM_COMPLETE;
                break;
            case 'f':
                if (conf->complete & FEATURE_COMPLETE)
                {
                    if (conf->complete & FILE_TYPE_COMPLETE)
                    {
                        break;
                    }else
                    {
                        /* file type */
                        if ((sscanf(buf, "file_type %hhd", &conf->file_type) == EOF) )
                        {
                            printf("error: conf_file: feature error\n");
                            return -1;
                        }else
                        {
                            if ((conf->file_type != text_type) && (conf->file_type != pcap_type))
                            {
                                return -1;
                            }
                            conf->complete |= FILE_TYPE_COMPLETE;
                            break;
                        }
                    }
                }
                if ((sscanf(buf, "feature %s", conf->feature) == EOF) || (strlen(conf->feature) == 0))
                {
                    printf("error: conf_file: feature error\n");
                    return -1;
                }
                conf->complete |= FEATURE_COMPLETE;
                break;
            case 'i':
                if (conf->complete & INPUT_COMPLETE)
                {
                    break;
                }
                if ((sscanf(buf, "input %s", conf->input) == EOF) || strlen(conf->input) == 0)
                {
                    printf("error: conf_file: input error\n");
                    return -1;
                }
                conf->complete |= INPUT_COMPLETE;
                break;
            case 's':
                if (conf->complete & SCRIPT_COMPLETE)
                {
                    break;
                }
                if ((sscanf(buf, "script %s", conf->script) == EOF) || (strlen(conf->script) == 0))
                {
                    printf("error: conf_file: script error\n");
                    return -1;
                }
                conf->complete |= SCRIPT_COMPLETE;
                break;
            case 'l':
                if (conf->complete & LIB_COMPLETE)
                {
                    break;
                }
                if ((sscanf(buf, "lib %s", conf->lib) == EOF) || (strlen(conf->lib) == 0))
                {
                    printf("error: conf_file: lib error\n");
                    return -1;
                }
                conf->complete |= LIB_COMPLETE;
                break;
            default:
                printf("error:unknow conf\n");
                return -1;
        }
    }
    fclose(fp);
    if (conf->complete == ALL_COMPLETE )
    {
        return 0;
    }
    printf("configure not complete\n");
    return -1;
}

static uint64_t  calc_time(struct timespec start, struct timespec end)
{
    return (end.tv_sec * 1000 * 1000 * 1000 + end.tv_nsec - 
        (start.tv_sec * 1000 * 1000 * 1000 + start.tv_nsec));
}

int file_to_buffer(const char *file, char *buffer, size_t *len)
{
    FILE *fp = fopen(file, "r");
    int num = 0;
    *len = 0;
    if (fp == NULL)
    {
        printf("fopen file failed\n");
        return -1;
    }
    do{
        num = fread(&buffer[*len], 1, 4096, fp);
        if (num < 0)
        {
            return -1;
        }
        *len += num;
    }while(num == 4096);
    fclose(fp);
    return 0;
}

void c_pcap_handle(u_char *userarg, const struct pcap_pkthdr *pkthdr, const u_char *packet)
{
    struct timespec time_start, time_end;
    uint32_t i = 0;
    c_pcap_info_t *pcap_info = (c_pcap_info_t *)userarg;
    uint64_t time = 0;
    char result[512];
    int debuginfo_len;
    const u_char *tcp_payload;
    int tcp_payload_len;

    pcap_info->packet_num++;
    if (pkthdr->caplen <= MAC_FRAME_HEADER_LEN + IP_HEADER_LEN + TCP_HEADER_LEN)
    {
        return ;
    }
    tcp_payload = packet + MAC_FRAME_HEADER_LEN + IP_HEADER_LEN + TCP_HEADER_LEN;
    tcp_payload_len = pkthdr->caplen - MAC_FRAME_HEADER_LEN - IP_HEADER_LEN - TCP_HEADER_LEN;  
    
    /* 首次运行重复三次再取时间 */
    while(c_pcap_flag[pcap_info->thread_id] != 3)
    {
        memset(result, 0, sizeof(result));
        for(i = 0; i < TSG_LUA_LOOP_TIMES; i++)
        {
            pcap_info->c_handle(tcp_payload, tcp_payload_len, pcap_info->feature, result);
        }
        c_pcap_flag[pcap_info->thread_id]++;
    }

    memset(result, 0, sizeof(result));
    clock_gettime(CLOCK_MONOTONIC, &time_start);
    for(i = 0; i < TSG_LUA_LOOP_TIMES; i++)
    {
        pcap_info->c_handle(tcp_payload, tcp_payload_len, pcap_info->feature, result);
    }
    clock_gettime(CLOCK_MONOTONIC, &time_end);
    time += calc_time(time_start, time_end);
    time /= TSG_LUA_LOOP_TIMES;
    pcap_info->total_time += time;

    debuginfo_len = strlen(pcap_info->debuginfo);
    if (debuginfo_len < 9500)
    {
        sprintf((char *)&pcap_info->debuginfo[debuginfo_len], "%-10s%-10ld%-11d%-20s%-20s%-20ld%-20ld\n", 
                "C", pcap_info->thread_id, pcap_info->packet_num, pcap_info->feature, result, time, pcap_info->total_time);
    }
    return;
}

static void c_text_handle(c_text_info_t *text_info, char *debuginfo)
{
    char buffer[1024 * 1024];
    size_t buffer_len;
    char result[512];
    struct timespec time_start, time_end;
    uint64_t time;
    int i;
    const char *file_name;

    if ((text_info == NULL) || (debuginfo == NULL))
    {
        return ;
    }

    /* 普通文件处理 */
    if (file_to_buffer(text_info->file_name, buffer, &buffer_len))
    {
        return;
    }
    /* 首次运行重复三次再取时间 */
    while(c_text_flag[text_info->thread_id] != 3)
    {
        memset(result, 0, sizeof(result));
        for(i = 0; i < TSG_LUA_LOOP_TIMES; i++)
        {
            text_info->c_handle((const u_char *)buffer, buffer_len, text_info->feature, result);
        }
        c_text_flag[text_info->thread_id]++;
    }

    clock_gettime(CLOCK_MONOTONIC, &time_start);
    for (i = 0; i < TSG_LUA_LOOP_TIMES; i ++)
    {
        if (text_info->c_handle((const u_char *)buffer, buffer_len, text_info->feature, result))
        {
            return;
        }
    }
    clock_gettime(CLOCK_MONOTONIC, &time_end);
    time = calc_time(time_start, time_end);
    time /= TSG_LUA_LOOP_TIMES;
    text_info->total_time += time;

    file_name = (const char *)memrchr(text_info->file_name, '/', strlen(text_info->file_name));
    if (file_name == NULL)
    {
        file_name = text_info->file_name;
    }else
    {
        file_name++;
    }

    sprintf(&debuginfo[strlen(debuginfo)], "%-10s%-10ld%-20s%-20s%-20s%-20ld%-20ld\n", 
            "C", text_info->thread_id, file_name, text_info->feature, result, time, text_info->total_time);
    return;
}

static void c_pcap_prefix_handle(c_thread_info_t *thread_info, char *debuginfo, c_handle_function c_handle )
{
    char ebuf[PCAP_ERRBUF_SIZE];
    c_pcap_info_t pcap_info;
    pcap_t *pcap_handle;

    if ((thread_info == NULL) || (debuginfo == NULL) || (c_handle == NULL))
    {
        return ;
    }
    /* pcap 文件处理 */
    if (thread_info->file_type & pcap_type)
    {
        pcap_handle = pcap_open_offline(thread_info->input, ebuf);
        if (pcap_handle == NULL)
        {
            printf("input is error\n");
            return ;
        }

        pcap_info.feature = thread_info->feature;
        pcap_info.c_handle = c_handle;
        pcap_info.debuginfo = debuginfo;
        pcap_info.packet_num = 0;
        pcap_info.total_time = 0;
        pcap_info.thread_id = thread_info->thread_id;

        pcap_loop(pcap_handle, -1, c_pcap_handle, (u_char *)&pcap_info);
        pcap_close(pcap_handle);

    }
    return ;
}

static void *c_file_handle(void *ptr)
{
    c_thread_info_t *thread_info = (c_thread_info_t *)ptr;
    char *debuginfo;
    void *lib_handle = NULL;
    c_handle_function c_handle; 
    uint32_t i;
    struct stat s_buf;
    char file_name[100][256];
    c_text_info_t text_info;

    debuginfo = (char *)tc_malloc(10240);
    if (debuginfo == NULL)
    {
        perror(" ");
        pthread_exit(NULL);
    }
    memset(debuginfo, 0, 10240);

    lib_handle = dlopen(thread_info->lib, RTLD_LAZY);
    if (lib_handle == NULL)
    {
        fprintf (stderr, "%s\n", dlerror());
        pthread_exit(debuginfo);
    }

    c_handle = (c_handle_function)dlsym(lib_handle, "c_handle_function");
    if (c_handle == NULL)
    {
        fprintf (stderr, "%s\n", dlerror());
        dlclose(lib_handle);
        pthread_exit(debuginfo);
    }

    text_info.thread_id = thread_info->thread_id;
    text_info.total_time = 0;
    text_info.feature = thread_info->feature;
    text_info.debuginfo = debuginfo;
    text_info.c_handle = c_handle;

    stat(thread_info->input, &s_buf);
    /* 目录处理 */
    if (S_ISDIR(s_buf.st_mode))
    {
        memset(file_name, 0, 100 * 256);
        read_file_list(thread_info->input, file_name);

        if (thread_info->file_type & pcap_type)
        {
            for ( i = 0; i < 100; i++)
            {
                if(file_name[i][0] == '\0')
                    break;
                thread_info->input = file_name[i];
                c_pcap_prefix_handle(thread_info, debuginfo, c_handle); 
            }
        }else
        {
            for ( i = 0; i < 100; i++)
            {
                if(file_name[i][0] == '\0')
                    break;
                text_info.file_name = file_name[i];
                c_text_handle(&text_info, debuginfo);
            }
        }
    }else
    {
        /* 文件处理 */
        if (thread_info->file_type & pcap_type)
        {
            c_pcap_prefix_handle(thread_info, debuginfo, c_handle); 
        }else
        {
            text_info.file_name = thread_info->input;
            c_text_handle(&text_info, debuginfo);
        }
    }

    dlclose(lib_handle);
    pthread_exit(debuginfo);
}

void lua_pcap_handle(u_char *userarg, const struct pcap_pkthdr *pkthdr, const u_char *packet)
{
    char out[256] = {0};
    size_t out_len = 0;
    size_t out_type = 0;
    int ret = 0;
    int i;
    lua_pcap_info_t *pcap_info = (lua_pcap_info_t *)userarg;
    char result[512];
    int debuginfo_len;
    struct timespec time_start, time_end;
    uint64_t time = 0;

    if ((pcap_info == NULL) || (pkthdr == NULL) || (packet == NULL))
    {
        return ;
    }

    memset(result, 0, sizeof(result));
    pcap_info->packet_num++; 
    if (pkthdr->caplen <= MAC_FRAME_HEADER_LEN + IP_HEADER_LEN + TCP_HEADER_LEN)
    {
        return ;
    }
    const u_char *tcp_payload = packet + MAC_FRAME_HEADER_LEN + IP_HEADER_LEN + TCP_HEADER_LEN;
    int tcp_payload_len = pkthdr->caplen - MAC_FRAME_HEADER_LEN - IP_HEADER_LEN - TCP_HEADER_LEN;  

    while (lua_pcap_flag[pcap_info->thread_id] != 3)
    {
        for (i = 0; i < TSG_LUA_LOOP_TIMES; i++)
        {
            ret = tsg_lua_cache_exec(pcap_info->L, pcap_info->script_id, (const char *)tcp_payload, tcp_payload_len, out, &out_len, &out_type);
            if (ret < 0)
            {
                sprintf(result, "failed");
                pthread_exit(pcap_info->debuginfo);
            }
        }
        lua_pcap_flag[pcap_info->thread_id]++;
    }

    clock_gettime(CLOCK_MONOTONIC, &time_start);
    for (i = 0; i < TSG_LUA_LOOP_TIMES; i++)
    {
        ret = tsg_lua_cache_exec(pcap_info->L, pcap_info->script_id, (const char *)tcp_payload, tcp_payload_len, out, &out_len, &out_type);
        if (ret < 0)
        {
            sprintf(result, "failed");
            pthread_exit(pcap_info->debuginfo);
        }
    }
    clock_gettime(CLOCK_MONOTONIC, &time_end);
    time += calc_time(time_start, time_end);
    time /= TSG_LUA_LOOP_TIMES;
    pcap_info->total_time += time;

    switch(out_type)
    {
        case STRING:
            snprintf(result, out_len + 1, "%s", out);
            break;
        case BOOLEAN:
            sprintf(result, "%s", out[0] ? "true":"false");
            break;
        case INTEGER:
            sprintf(result, "%ld", *(uint64_t *)out);
            break;
        default:
            break;
    }
    debuginfo_len = strlen(pcap_info->debuginfo);
    if (debuginfo_len < 9500)
    {
        sprintf(&pcap_info->debuginfo[debuginfo_len], "%-10s%-10ld%-11d%-20s%-20s%-20ld%-20ld\n", 
                "LUA", pcap_info->thread_id, pcap_info->packet_num, pcap_info->feature, result, time, pcap_info->total_time);
    }
    return ;
}
static void lua_pcap_prefix_handle(lua_thread_info_t *thread_info, char *debuginfo, tsg_lua_handle L, size_t script_id)
{
    char ebuf[PCAP_ERRBUF_SIZE];
    lua_pcap_info_t pcap_info;
    pcap_t *pcap_handle;

    if ((thread_info == NULL) || (debuginfo == NULL) || (script_id < 1))
    {
        return ;
    }
    /* pcap 文件处理 */
    if (thread_info->file_type & pcap_type)
    {
        pcap_handle = pcap_open_offline(thread_info->input, ebuf);
        if (pcap_handle == NULL)
        {
            printf("input is error\n");
            return ;
        }

        pcap_info.L = L;
        pcap_info.feature = thread_info->feature;
        pcap_info.script_id = script_id;
        pcap_info.debuginfo = debuginfo;
        pcap_info.packet_num = 0;
        pcap_info.total_time = 0;
        pcap_info.thread_id = thread_info->thread_id;

        pcap_loop(pcap_handle, -1, lua_pcap_handle, (u_char *)&pcap_info);
        pcap_close(pcap_handle);

    }
    return ;
}

static void lua_text_handle(lua_text_info_t *text_info, char *debuginfo, tsg_lua_handle lua)
{
    char buffer[1024 * 1024];
    size_t buffer_len;
    char result[512];
    struct timespec time_start, time_end;
    uint64_t time;
    int i;
    int ret;
    size_t out_len = 0;
    size_t out_type = 0;
    char out[256] = {0};
    const char *file_name;

    if ((text_info == NULL) || (debuginfo == NULL))
    {
        return ;
    }

    /* 普通文件处理 */
    if (file_to_buffer(text_info->file_name, buffer, &buffer_len))
    {
        return;
    }

    while (lua_text_flag[text_info->thread_id] != 3)
    {
        for (i = 0; i < TSG_LUA_LOOP_TIMES; i++)
        {
            ret = tsg_lua_cache_exec(lua, text_info->script_id, (const char *)buffer, buffer_len, out, &out_len, &out_type);
            if (ret < 0)
            {
                return;
            }
        }
        lua_text_flag[text_info->thread_id]++;
    }

    clock_gettime(CLOCK_MONOTONIC, &time_start);
    for (i = 0; i < TSG_LUA_LOOP_TIMES; i ++)
    {
        ret = tsg_lua_cache_exec(lua, text_info->script_id, (const char *)buffer, buffer_len, out, &out_len, &out_type);
        if (ret < 0)
        {
            return;
        }
    }
    clock_gettime(CLOCK_MONOTONIC, &time_end);
    time = calc_time(time_start, time_end);
    time /= TSG_LUA_LOOP_TIMES;
    text_info->total_time += time;

    switch(out_type)
    {
        case STRING:
            snprintf(result, out_len + 1, "%s", out);
            break;
        case BOOLEAN:
            sprintf(result, "%s", out[0] ? "true":"false");
            break;
        case INTEGER:
            sprintf(result, "%ld", *(uint64_t *)out);
            break;
        default:
            break;
    }
    file_name = (const char *)memrchr(text_info->file_name, '/', strlen(text_info->file_name));
    if (file_name == NULL)
    {
        file_name = text_info->file_name;
    }else
    {
        file_name++;
    }

    sprintf(&debuginfo[strlen(debuginfo)], "%-10s%-10ld%-20s%-20s%-20s%-20ld%-20ld\n", 
            "lua", text_info->thread_id, file_name, text_info->feature, result, time, text_info->total_time);

    return ;
}

static void *lua_file_handle(void *ptr)
{
    size_t script_id;
    char script[4096];
    size_t script_len;
    tsg_lua_handle lua;
    char *debuginfo = NULL;
    lua_thread_info_t * thread_info = (lua_thread_info_t *)ptr;
    uint32_t i;
    char result[512];
    lua_text_info_t text_info;
    struct stat s_buf;
    char file_name[100][256];

    debuginfo = (char *)tc_malloc(10240);
    if (debuginfo == NULL)
    {
        perror(" ");
        pthread_exit(NULL);
    }
    memset(debuginfo, 0, 10240);
    memset(result, 0, sizeof(result));

    lua = tsg_lua_vm_create();
    if (lua == NULL)
    {
        pthread_exit(debuginfo);
    }
    if (file_to_buffer(thread_info->script, script, &script_len))
    {
        pthread_exit(debuginfo);
    }
    script_id = tsg_lua_cache_script(lua, script, script_len);
    if (script_id < 1)
    {
        pthread_exit(debuginfo);
    }

    text_info.L = lua;
    text_info.thread_id = thread_info->thread_id;
    text_info.script_id = script_id;
    text_info.total_time = 0;
    text_info.feature = thread_info->feature;
    text_info.debuginfo = debuginfo;

    stat(thread_info->input, &s_buf);
    /* 目录处理 */
    if (S_ISDIR(s_buf.st_mode))
    {
        memset(file_name, 0, 100 * 256);
        read_file_list(thread_info->input, file_name);

        if (thread_info->file_type & pcap_type)
        {
            for ( i = 0; i < 100; i++)
            {
                if(file_name[i][0] == '\0')
                    break;
                thread_info->input = file_name[i];
                lua_pcap_prefix_handle(thread_info, debuginfo, lua, script_id); 
            }
        }else
        {
            for ( i = 0; i < 100; i++)
            {
                if(file_name[i][0] == '\0')
                    break;
                text_info.file_name = file_name[i];
                lua_text_handle(&text_info, debuginfo, lua);
            }
        }
    }else
    {
        /* 文件处理 */
        if (thread_info->file_type & pcap_type)
        {
            lua_pcap_prefix_handle(thread_info, debuginfo, lua, script_id); 
        }else
        {
            text_info.file_name = thread_info->input;
            lua_text_handle(&text_info, debuginfo, lua);
        }
    }
    pthread_exit(debuginfo);
}

int main(int argc, char *argv[])
{
    conf_t conf;
    int ret;
    char conf_path[128];
    pthread_t thread[MAX_THREAD_NUM] = {0};
    pthread_attr_t attr[MAX_THREAD_NUM] = {0};
    uint32_t thread_num; 
    int ch;
    int buf_len;
    lua_thread_info_t lua_thread_info[MAX_THREAD_NUM];
    c_thread_info_t c_thread_info[MAX_THREAD_NUM];
    char *retval;
    cpu_set_t mask;
    FILE *log = NULL;
    char log_name[256];
    const char *log_prefix = "./log/";
    time_t timep;

    memset(&conf, 0, sizeof(conf_t));
    strcpy(conf_path, "./conf/default.conf");
    while((ch = getopt(argc, argv, "t:f:i:s:l:c:h")) != -1)
    {
        switch(ch)
        {
            case 't':
                conf.thread_num = atoi(optarg);
                if (conf.thread_num >= MAX_THREAD_NUM)
                {
                    printf("MAX_THREAD_NUM is %d\n",MAX_THREAD_NUM - 1);
                    conf.thread_num = MAX_THREAD_NUM;
                }
                conf.complete |= THREAD_NUM_COMPLETE;
                break; 
            case 'f':
                buf_len = MIN(1024,  strlen(optarg));
                strncpy((char *)conf.feature, optarg, buf_len);
                conf.complete |= FEATURE_COMPLETE;
                break;          
            case 'i':
                buf_len = MIN(127, strlen(optarg));
                strncpy(conf.input, optarg, buf_len);
                conf.input[buf_len] = '\0';
                conf.complete |= INPUT_COMPLETE;
                break;
            case 's':
                buf_len = MIN(127, strlen(optarg));
                strncpy(conf.script, optarg, buf_len);
                conf.script[buf_len] = '\0';
                conf.complete |= SCRIPT_COMPLETE;
                break;
            case 'l':
                buf_len = MIN(127, strlen(optarg));
                strncpy(conf.lib, optarg, buf_len);
                conf.lib[buf_len] = '\0';
                conf.complete |= LIB_COMPLETE;
                break;
            case 'c':
                buf_len = MIN(127, strlen(optarg));
                strncpy(conf_path, optarg, buf_len);
                conf_path[buf_len] = '\0';
                break;
            case 'h':
            default:
                printf("usage: multithread \n[-t set thread num] \n[-f set feature] \n"
                        "[-i set input] \n[-s set script] \n[-l set library] \n"
                        "[-c set conf_path] \n[-h show help info]\n");
                return -1;
                break;
        }
    }

    ret = init_conf(conf_path, &conf);
    if (ret != 0)
    {
        return -1;
    }

    strcpy(log_name, log_prefix);
    time(&timep);
    strftime(&log_name[strlen(log_prefix)], sizeof(log_name) - strlen(log_prefix),"%Y%m%d%H%M%S.log", localtime(&timep));
    log = fopen(log_name, "a+");
    if (log == NULL)
    {
        perror(log_name);
        return -1;
    }

    /* 识别文件类型 */

    printf("\n==============================================================================================================================\n");
    if (conf.file_type & pcap_type)
    {
        printf("%-10s%-10s%-11s%-20s%-20s%-20s%-20s\n", "C or LUA", "thread_id", "packet_num", "feature", "result", "time", "total_time");
    }else
    {
        printf("%-10s%-10s%-20s%-20s%-20s%-20s\n", "C or LUA", "thread_id", "file_name", "feature", "result", "time");
    }
#if 1
    /* c function */
    for ( thread_num = 0; thread_num < conf.thread_num; thread_num++ )
    {
        c_thread_info[thread_num].lib = conf.lib;
        c_thread_info[thread_num].input = conf.input;
        c_thread_info[thread_num].feature = conf.feature;
        c_thread_info[thread_num].thread_id = thread_num;
        c_thread_info[thread_num].file_type = conf.file_type;
        pthread_attr_init(&attr[thread_num]);
        if(pthread_create(&thread[thread_num], &attr[thread_num], c_file_handle, (void *)&c_thread_info[thread_num]))
        {
            perror(" ");
            fclose(log);
            return -1;
        }

        CPU_ZERO(&mask);
        CPU_SET(thread_num, &mask);
        if (pthread_setaffinity_np(thread[thread_num], sizeof(mask), &mask) != 0)
        {
            printf("warning:could not set CPU affinity, continuing...\n");
        }
    }

    for (thread_num = 0; thread_num < conf.thread_num; thread_num++)
    {
        pthread_join(thread[thread_num], (void **)&retval);
        if (retval)
        {
            printf("%s", retval);
            fwrite(retval, strlen(retval), 1, log);
            tc_free(retval);
        }
    }
#endif
    /* lua function */
    for (thread_num = 0; thread_num < conf.thread_num; thread_num++)
    {
        lua_thread_info[thread_num].script = conf.script;
        lua_thread_info[thread_num].input = conf.input;
        lua_thread_info[thread_num].feature = conf.feature;
        lua_thread_info[thread_num].thread_id = thread_num;
        lua_thread_info[thread_num].file_type = conf.file_type;
        pthread_attr_init(&attr[thread_num]);
        if(pthread_create(&thread[thread_num], &attr[thread_num], lua_file_handle, (void *)&lua_thread_info[thread_num]) != 0)
        {
            perror(" ");
            fclose(log);
            return -1;
        }

        CPU_ZERO(&mask);
        CPU_SET(thread_num, &mask);
        if (pthread_setaffinity_np(thread[thread_num], sizeof(mask), &mask) != 0)
        {
            printf("warning:could not set CPU affinity, continuing...\n");
        }
    }

    for (thread_num = 0; thread_num < conf.thread_num; thread_num++)
    {
        pthread_join(thread[thread_num], (void **)&retval);
        if (retval)
        {
            printf("%s", retval);
            fwrite(retval, strlen(retval), 1, log);
            tc_free(retval);
        }
    }

    fclose(log);

    return 0;
}
