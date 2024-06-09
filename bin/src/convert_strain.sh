#!/bin/bash
if [ $# -ne 1 ];then
    exit 1
fi
csv=$1
root=`echo $csv | sed 's:.csv:.root:g'`

#echo "convert from $bin to $csv"
#python Binary_converter.py $bin

echo "convert from $csv to $root"
rootCode=$IT7DIR/bin/src/csv2root_strain.C

root -b <<EOF
.L $rootCode
csv2root("$csv")
.q
EOF
