#!/bin/bash
if [ $# -ne 1 ];then
    exit 1
fi
IPlastDigit=$1
reqFile=test.request
touch $reqFile
lftp -u teensy4x,ftp_test -p 21 192.168.11.${IPlastDigit} -e "put $reqFile;exit"
rm $reqFile
