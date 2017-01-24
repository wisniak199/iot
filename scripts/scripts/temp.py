#!/usr/bin/python

import json
import sys
import socket

data_raw = ''
for line in sys.stdin:
    data_raw += line

data = json.loads(data_raw, strict=False)

if (data['level'] == 'INFO'):
    sock.sendall('0')
elif (data['level'] == 'WARNING'):
    sock.sendall('1')

sock.close()
