#!/bin/bash

pid=`ps aux | grep "fire_alarm" | grep -v "grep" | head -1 | awk '{print $2}'`

kill -9 ${pid}
pid=`ps aux | grep "fire_alarm" | grep -v "grep" | head -1 | awk '{print $2}'`
kill -9 ${pid}


exit 0
