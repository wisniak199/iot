#!/usr/bin/python

import json
import sys
import socket
import subprocess

data_raw = ''
for line in sys.stdin:
    data_raw += line

data = json.loads(data_raw, strict=False)

if (data['level'] == 'WARNING'):
    subprocess.call('/home/wisniak199/iot/serwer/scripts/play_fire_alarm.sh', shell=True)
    subprocess.call('/home/wisniak199/iot/serwer/scripts/send_fire_start_email.sh', shell=True)
elif (data['level'] == 'OK'):
    subprocess.call('/home/wisniak199/iot/serwer/scripts/kill_fire_alarm.sh', shell=True)
    subprocess.call('/home/wisniak199/iot/serwer/scripts/send_fire_end_email.sh', shell=True)
