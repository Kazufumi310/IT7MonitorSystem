#!/bin/bash
allTeensyControl 2 $1
mkdir $1/accelerometer
mkdir $1/tourmaline
rsync -avt it7@192.168.11.41:data/ $IT7DataDir/accelerometer/
