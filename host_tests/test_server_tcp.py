# PackageLicenseDeclared: Apache-2.0
# Copyright (c) 2015 ARM Limited
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

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
