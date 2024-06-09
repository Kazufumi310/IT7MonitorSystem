#!/bin/bash

cd ../../bin/
for fil in ../teensy/communication_command/*.sh 
do
    base=`basename $fil .sh`
    if [ $base != copy_to_bin_directory ];then
	ln -s $fil $base
    fi
done
