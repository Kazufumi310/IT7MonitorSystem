#!/bin/bash
if [ $# -ne 1 ];then
    exit 1
fi
bin=$1
echo "converting $bin to CSV..."
program=/Users/sato/Desktop/SK_ICRR/work/implosionTest/IT7-monitorUpdate/program/bin/src_files/Binary_converter.py

python $program $bin
