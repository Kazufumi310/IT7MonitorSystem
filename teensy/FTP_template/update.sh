#!/bin/bash

ArduinoDir=$ARDUINODIR
if [ x$ArduinoDir = x ];then
    echo set ARDUINODIR
    exit 1
fi

prefix=FTPcontrol

nSensor=5
IPDigitArr=(2 3 4 5 6)
sensorNameArr=(pressure1 pressure2 strain controller alternativeOscillo)

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
    cp ../PTP_template/ptpfunction.hpp $dirName/
    if [ $IPDigit -ne 5 ];then
	cp pressurestrainAction.hpp $dirName/
    fi
    
    i=`expr $i + 1`
done

if [ ! -e $ArduinoDir/MS5837_Read_Ethernet_Refresh_SD ];then
    mkdir $ArduinoDir/MS5837_Read_Ethernet_Refresh_SD
fi
cp  MS5837_Read_Ethernet_Refresh_SD.cc $ArduinoDir/MS5837_Read_Ethernet_Refresh_SD/MS5837_Read_Ethernet_Refresh_SD.ino
