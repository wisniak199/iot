import socket
import sys
import threading
import time
import json
import signal

influx_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
influx_address = ('localhost', 8086)
influx_sock.connect(influx_address)


control_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
control_server_address = ('localhost', 7001)
control_sock.bind(control_server_address)
control_sock.listen(1)

iot_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
iot_server_address = ('0.0.0.0', 7000)
iot_sock.bind(iot_server_address)
iot_sock.listen(1)


iot_connections = []
control_connections = []
iot_active = True
control_active = True

def signal_handler(signal, frame):
    global iot_active
    global control_active
    iot_active = False
    control_active = False
    influx_sock.close()
    control_sock.close()
    iot_sock.close()
    for con in iot_connections:
        con.close()
    for con in control_connections:
        con.close()
    sys.exit(0)

signal.signal(signal.SIGINT, signal_handler)


def iot_server():
    while iot_active:
        # Wait for a connection
        connection, client_address = iot_sock.accept()
        iot_connections.append(connection)
        try:
            while iot_active:
                data = connection.recv(4096)
                print data
                if data:
                    influx_sock.sendall(data)
                    response = influx_sock.recv(4096)
                    print response
                else:
                    break

        finally:
            iot_connections.remove(connection)
            connection.close()

def control_server():
    while control_active:
        # Wait for a connection
        connection, client_address = control_sock.accept()
        control_connections.append(connection)
        try:
            while control_active:
                data = connection.recv(4096)
                print data
                if data:
                    for con in iot_connections:
                        con.sendall(data)
                else:
                    break

        finally:
            control_connections.remove(connection)
            connection.close()

iot_thread = threading.Thread(target=iot_server)
iot_thread.daemon = True
control_thread = threading.Thread(target=control_server)
control_thread.daemon = True

iot_thread.start()
control_thread.start()


while True:
    time.sleep(1)
