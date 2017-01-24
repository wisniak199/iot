#!/usr/bin/python

import json
import sys
import socket

data_raw = ''
for line in sys.stdin:
    data_raw += line

data = json.loads(data_raw, strict=False)

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
address = ('localhost', 7001)
sock.connect(address)

if (data['level'] == 'INFO'):
    sock.sendall('0\n')
elif (data['level'] == 'WARNING'):
    sock.sendall('1\n')

sock.close()
