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
import sys
import array

def echo_test(addr, port):
    fail = False
    # TODO: determine address family from address
    print 'Connecting to %s:%d'%(addr, port)
    af = socket.AF_INET
    # Test the ability to accept a connection
    s = socket.socket(af, socket.SOCK_STREAM)
    try:
        s.connect((addr,port))
    except:
        print 'echo_test connect [FAIL]'
        return

    for x in range(8):
        nwords = 1<<(x+4)
        data = []
        for j in range(nwords):
            data.append(int(j))
        d = array.array('H',data).tostring()
        s.send(d)
        print 'receiving %d bytes'%len(d)
        e = s.recv(len(d))
        if len(d) != len(e):
            print 'Received %d/%d bytes [FAIL]'%(len(e),len(d))
            fail = True
        elif d != e:
            print 'echo does not match sent data [FAIL]'
            fail = True
    s.send('quit')

    try:
        s.close()
    except:
        print 'echo_test close [FAIL]'
        fail = True
    if fail:
        print '{{failure}}'
    else:
        print '{{success}}'

def main():
    echo_test(sys.argv[1], int(sys.argv[2]))

if __name__ == "__main__":
    main()
