#!/bin/bash

ndir=5
devtype=( Keller Keller strain accelerometer tourmaline )
odirname=( pressure1 pressure2 strain accelerometer tourmaline )

for i in `seq 0 4`
do
    odir=${odirname[${i}]}
    echo $odir
    if [ ! -e $odir ];then
       echo "no directory named ${odir}. skip. "
       continue
    fi
       
    output=$odir/header.txt
    if [ ! -e $output ];then
	cp $IT7DIR/bin/template/header/header_${devtype[${i}]}_template.txt $output
    else
	echo "$output already exists. ignore copy command"
    fi
done
