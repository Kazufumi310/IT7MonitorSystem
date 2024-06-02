#!/bin/bash
mode=0
if [ $# -eq 2 ];then
    mode=$2
elif [ $# -ne 1 ];then
    exit 1
fi
IPlastDigit=$1

if [ $mode -eq 0 ];then
    reqFile=run.request
elif [ $mode -eq 1 ];then
    reqFile=test.request
fi
if [ ! -e $reqFile ];then
    touch $reqFile
fi
lftp -u teensy4x,ftp_test -p 21 192.168.11.${IPlastDigit} -e "cd data;put $reqFile;exit"

