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
