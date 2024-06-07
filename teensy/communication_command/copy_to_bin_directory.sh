#!/bin/bash

cd ../../bin/
for fil in ../teensy/communication_command/teensy_*.sh
do
    base=`basename $fil .sh`
    ln -s $fil $base
done
