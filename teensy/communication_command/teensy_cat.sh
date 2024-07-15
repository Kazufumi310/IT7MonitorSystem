#!/bin/bash
dir=data
fil=timing.csv
if [ $# -eq 2 ];then
    fil=$2
elif [ $# -ne 1 ];then
    exit 1
fi
IPlastDigit=$1
lftp -u teensy4x,ftp_test -p 21 192.168.11.${IPlastDigit} -e "cd $dir;cat ${fil};exit"
