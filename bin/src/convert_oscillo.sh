#!/bin/bash
if [ $# -ne 1 ];then
    exit 1
fi
mat=$1
csv=`echo $mat | sed 's:.MAT:.csv:g'`
root=`echo $mat | sed 's:.MAT:.root:g'`

echo "convert from $mat to $csv"

mat2csv $mat

echo "convert from $csv to $root"

rootCode=$IT7DIR/bin/src/csv2root_oscillo.C

root -b <<EOF
.L $rootCode
csv2root("$csv")
.q
EOF
