/*************************************************************************
    > File Name: handle_weixinhao.cpp
    > Author: pxz
    > Created Time: Sun 09 Aug 2020 07:04:28 AM CST
 ************************************************************************/
extern "C"
{
#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<string.h>
#include<arpa/inet.h>
}

extern "C" int c_handle_function(const u_char *packet, uint32_t packet_len, const char *feature, char *result)
{
#define WEIXINNUM_NAME_LEN  4
#define WEIXINNUM_NAME      9
#define WEIXINNUM_LEN       4
#define WEIXINNUM           11
    uint32_t necessary_len;
    uint32_t i;
    const u_char *pweixinnum = NULL;
    const u_char *pweixinnum_name = NULL;
    uint32_t weixinnum_len;

    necessary_len = WEIXINNUM_NAME_LEN + WEIXINNUM_NAME + WEIXINNUM_LEN + WEIXINNUM;
    if (packet_len < necessary_len)
    {
        return -1;
    }
    pweixinnum_name = (const u_char *)memmem(packet, packet_len, feature, strlen(feature));
    if (pweixinnum_name == NULL)
    {
       return -1;
    }

    weixinnum_len = *(uint32_t*)(pweixinnum_name + WEIXINNUM_NAME);
    weixinnum_len = ntohl(weixinnum_len);
    if (weixinnum_len > 12)
    {
        return -1;
    }
    pweixinnum = pweixinnum_name + WEIXINNUM_NAME + WEIXINNUM_LEN;
    memcpy(result, pweixinnum, weixinnum_len);
    result[weixinnum_len] = '\0';
    for (i = 0; i < weixinnum_len; i++)
    {
        if (!(result[i] <= '9' && result[i] >= '0'))
        {
            return -1;
        }
    }
    
    return 0;
}
