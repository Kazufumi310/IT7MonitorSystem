#!/bin/bash
if [ $# -ne 4 ];then
    exit 1
fi
#bin=$1
csv=$1
header=$2
timing=$3
mode=$4

rootCode=$IT7DIR/bin/src/csv2root_dev.C
echo $rootCode

root -b <<EOF
.L $rootCode
csv2root("$csv","$header","$timing",$mode)
.q
EOF
