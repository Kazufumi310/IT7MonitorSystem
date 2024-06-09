#!/bin/bash

if [ $# -lt 1 ];then
    echo "arg error."
    exit 1
fi
mode=$1 
nMode=3
if [ $mode -lt 0 -o $mode -gt $nMode ];then
    echo "strange mode : mode = $mode"
fi

progNameArr=("runStart" "testStart" "getData")
prog=${progNameArr[${mode}]}

if [ $mode -eq 0 -o $mode -eq 1 ];then # runStart or testStart
    if [ $# -ne 1 ];then 
	echo "arg error"
	exit 1;
    fi
    runMode=0
    if [ $mode -eq 1 ];then
	runMode=1
    fi
elif [ $mode -eq 2 ];then # getData
    if [ $# -ne 2 ];then 
	echo "usage : getData serial_number_of_IT"
	echo "ex) getData IT20240722_type2m"
	exit 1;
    fi
    serial=$2
    if [ -e $serial ];then
	echo "File named $serial already exsists."
	exit 1;
    fi
    mkdir $serial
fi




for i in 2 3 4
do
    if [ $mode -eq 2 ];then	 
        echo gnome-terminal -- bash -c "echo \"IPdigit = $i\";teensy_ls $i;teensy_get $i $serial/dev_$i; teensy_ls $i; bash"
    else
	echo gnome-terminal -- bash -c "echo \"IPdigit = $i\";teensy_ls $i;teensy_run $i $runMode;teensy_ls $i; bash"
    fi
done
