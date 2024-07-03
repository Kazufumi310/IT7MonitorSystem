#!/bin/bash

if [ $# -ne 1 ];then
    echo arg error
    exit 0
fi
input=$1
matfile=`basename $input`
csvfile=`basename $matfile .MAT`.csv

indir=`dirname $input`
cd $indir
indir=`pwd`
cd -

cd $IT7DIR/bin/src/

matlab -batch mat2csv\(\'$indir/$matfile\',\'$indir/$csvfile\'\)

