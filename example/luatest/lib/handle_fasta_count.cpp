/*************************************************************************
    > File Name: handle_fasta.cpp
    > Author: pxz
    > Created Time: Tue 11 Aug 2020 04:50:52 AM CST
 ************************************************************************/
extern "C"
{
#include<stdio.h>
#include<string.h>
#include<stdint.h>
#include<arpa/inet.h>
}
extern "C" int c_handle_function(const u_char *packet, uint32_t packet_len, const char *feature, char *result )
{
    const u_char *locate = NULL;
    uint32_t cnt = 0;
    int rest_len = packet_len;
    int feature_len = strlen(feature);
    if ((packet == NULL) || (feature == NULL) || (result == NULL) || (packet_len == 0) || (feature_len == 0))
    {
        return -1;
    }
    locate = packet;
    while(feature_len <= rest_len)
    {
#if 0
        locate = (const u_char *)memchr(locate, feature[0], rest_len);
        if (!locate)
        {
            break;
        }
        if ((feature_len == 1) || (memcmp((const u_char *)locate, feature, feature_len) == 0))
        {
            cnt++;
        }
        locate++;
        rest_len = packet_len - (locate - packet);  
#else
        locate = (const u_char *)memmem((const char *)locate, rest_len, feature, feature_len);
        if (!locate)
        {
            break;
        }
        cnt++;
        locate++;
        rest_len = packet_len - (locate - packet);  
#endif

    }

    sprintf(result, "count:%d", cnt);

    return 0;
}
