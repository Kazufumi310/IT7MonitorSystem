#!/bin/bash

ndir=5
mode=( 1 1 2 3 4 )
#devtype=( Keller Keller strain tourmaline accelerometer )
odirname=( pressure1 pressure2 strain tourmaline accelerometer )

for i in `seq 0 4`
do
   
    odir=${odirname[${i}]}
    echo $odir
    if [ ! -e $odir ];then
       echo "no directory named ${odir}. skip. "
       continue
    fi
    cd $odir;

    if [ ${mode[${i}]} -eq 1 ];then
	for bin in *.bin
	do
	    bin2csv $bin
	done
    fi
    
    nT=`ls *.csv *.txt 2>/dev/null | grep timing | wc -l`
    nH=`ls *.csv *.txt 2>/dev/null | grep header | wc -l`
    if [ $nH -eq 0 ];then
	echo "error. no header file in $odir."
	exit 0
    fi
    if [ $nH -gt 1 -o $nT -gt 1 ];then
	echo "error. multiple header files or timing files in $odir."
	exit 0
    fi
    header=`ls *.csv *.txt 2>/dev/null | grep header`
    timing=`ls *.csv *.txt 2>/dev/null | grep timing`
    if [ x$timing == x ];then
	timing=timingDummy.txt
    fi
    
    for input in `ls *.csv *.txt 2>/dev/null | grep -v header | grep -v timing | grep -v convertlog`
    do
	log=convertlog_${input}
	echo convert_dev $input $header $timing ${mode[${i}]}
	echo convert_dev $input $header $timing ${mode[${i}]} > $log
	convert_dev $input $header $timing ${mode[${i}]} >> $log 2>&1 
    done
    cd - >& /dev/null
done
