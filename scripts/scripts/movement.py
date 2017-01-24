#!/usr/bin/python

import json
import sys
import socket
import subprocess

data_raw = ''
for line in sys.stdin:
    data_raw += line

data = json.loads(data_raw, strict=False)

if (data['level'] == 'INFO'):
    subprocess.call('/home/wisniak199/iot/serwer/scripts/play_movement_alarm.sh', shell=True)
elif (data['level'] == 'WARNING'):
    subprocess.call('/home/wisniak199/iot/serwer/scripts/kill_movement_alarm.sh', shell=True)
