#!/bin/bash
while [ 1 ];
do
    wget http://192.168.11.99 >& /dev/null
    data=`tail -n 1 index.html`
    data=`echo $data | sed 's:<br>: :g'`
    utime=`date "+%s.%3N"`
    echo $data" unixtime: "$utime
    rm -f index.html
    sleep 0.5
done
