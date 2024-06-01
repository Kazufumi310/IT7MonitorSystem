#!/bin/bash
if [ $# -ne 1 ];then
    exit 1
fi
IPlastDigit=$1

mkdir temp_empty 
cd temp_empty
lftp -u teensy4x,ftp_test -p 21 192.168.11.${IPlastDigit} -e "cd data; mirror -R --delete; exit"
cd -

