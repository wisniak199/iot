#!/bin/bash

touch /home/wisniak199/iot/dupa.txt
pid=`ps aux | grep "movement_alarm" | grep -v "grep" | head -1 | awk '{print $2}'`

kill -9 ${pid}
pid=`ps aux | grep "movement_alarm" | grep -v "grep" | head -1 | awk '{print $2}'`
kill -9 ${pid}


exit 0
