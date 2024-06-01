#!/bin/bash
fromDir=data
if [ $# -eq 3 ];then
    toDir=$2
    fromDir=$3
elif [ $# -eq 2 ];then
    toDir=$2
elif [ $# -ne 1 ];then
    exit 1
fi
IPlastDigit=$1

if [ -e $toDir ];then
    echo "Directory $toDir is already exist."
    echo "Please assign a new directory name"
    exit 1;
fi

mkdir -p $toDir
cd $toDir
lftp -u teensy4x,ftp_test -p 21 192.168.11.${IPlastDigit} -e "cd $fromDir;mget *.*;exit"
cd -
