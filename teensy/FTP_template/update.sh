#!/bin/bash

ArduinoDir=$ARDUINODIR
if [ x$ArduinoDir = x ];then
    echo set ARDUINODIR
    exit 1
fi

prefix=FTPcontrol

nSensor=3
IPDigitArr=(2 3 4)
sensorNameArr=(pressure1 pressure2 strain)

i=0
while [ $i -lt $nSensor ]
do
    IPDigit=${IPDigitArr[${i}]}
    sensorName=${sensorNameArr[${i}]}
    echo $IPDigit $sensorName
    
    dirName=$ArduinoDir/${prefix}_${sensorName}
    if [ ! -e $dirName ];then
	mkdir $dirName
    fi
    cp FTP_template.cc $dirName/${prefix}_${sensorName}.ino
    cp ${sensorName}Sensor.hpp $dirName/sensors.hpp
    cat defines.h | sed 's:IPLASTDIGIT:'${IPDigit}':' > $dirName/defines.h
    i=`expr $i + 1`
done
