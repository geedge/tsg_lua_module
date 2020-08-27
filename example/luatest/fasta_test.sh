#########################################################################
# File Name: test.sh
# Author: pxz
# Created Time: Thu 27 Aug 2020 02:24:23 PM CST
#########################################################################
#!/bin/bash
feature=(" " "q" "qqqqqq" "qqqqqqqqqq" "qqq" "qqqqqq" "qqqqqqqqqq" "qqq" "qqq" "qqq" "q" "qGCTGA" "qGCTGA" "qGCTGA" "qGCTGA" "qCTCGG" "qGCTGA" "rAATCC" "sCGGGC")
num=1

while(($num <= 16))
do
    echo "num:${num}"
    if [ ${num} == 16 ]
    then
        ./luatest -c conf/fasta.conf -f ${feature[$num]} -i fasta/${num}_test/ -s script/${num}_1_handle_fasta_find.lua
        sleep 1
        ./luatest -c conf/fasta.conf -f ${feature[$num + 1]} -i fasta/${num}_test/ -s script/${num}_2_handle_fasta_find.lua
        sleep 1
        ./luatest -c conf/fasta.conf -f ${feature[$num + 2]} -i fasta/${num}_test/ -s script/${num}_3_handle_fasta_find.lua
    else
        ./luatest -c conf/fasta.conf -f ${feature[$num]} -i fasta/${num}_test/ -s script/${num}_handle_fasta_find.lua
        sleep 1
    fi
    let "num++"
done
