#!/usr/bin/python

import socket

TEST_PORT0=32767

def runConnectClose():
    Sv4 = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    Sv4.bind(('',TEST_PORT0))
    Sv4.listen(1)
    while True:
        (clientsocket, address) = Sv4.accept()
        print 'Connection received from', address
        while True:
            try:
                data = clientsocket.recv(4096)
            except:
                data = None
            if not data:
                break
            clientsocket.sendall(data)
        print 'connection closed'
        clientsocket.close()

if __name__ == '__main__':
    runConnectClose()
