#!/usr/bin/python

import socket

TEST_PORT1=32766

def runConnectClose():
    Sv4 = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    Sv4.bind(('',TEST_PORT1))
    while True:
        (data,address) = Sv4.recvfrom(4096)
        if data and address:
            Sv4.sendto(data, address)

if __name__ == '__main__':
    runConnectClose()
