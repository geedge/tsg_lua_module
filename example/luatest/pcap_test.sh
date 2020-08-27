#########################################################################
# File Name: pcap_test.sh
# Author: pxz
# Created Time: Thu 27 Aug 2020 07:14:17 PM CST
#########################################################################
#!/bin/bash
num=1

while ((${num} <= 16))
do
    ./luatest -i pcap/weixin_id_ios_20200423_004.pcap -t ${num}
    let "num++"
    sleep 1
done
