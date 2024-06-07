#!/bin/bash
if [ $# -ne 1 ];then
    exit 1
fi
bin=$1
csv=`echo $bin | sed 's:.bin:.csv:g'`
root=`echo $bin | sed 's:.bin:.root:g'`

echo "convert from $bin to $csv"

bin2csv $bin

echo "convert from $csv to $root"

rootCode=$IT7DIR/bin/src/csv2root_KELLERPressure.C

root -b <<EOF
.L $rootCode
csv2root("$csv")
.q
EOF
