/*
 * PackageLicenseDeclared: Apache-2.0
 * Copyright (c) 2015 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <mbed-net-socket-abstract/test/sal_test_api.h>
#include <mbed-net-socket-abstract/socket_api.h>
#include <mbed-net-socket-abstract/test/ctest_env.h>
#include <mbed/Timeout.h>
#include <mbed/mbed.h>

#ifndef SOCKET_CONNECT_TIMEOUT
#define SOCKET_CONNECT_TIMEOUT 5.0f
#endif

struct IPv4Entry{
    const char * test;
    uint32_t expect;
};

const struct IPv4Entry IPv4_TestAddresses[] = {
        {"0.0.0.0", 0},
        {"127.0.0.1", 0x0100007f},
        {"8.8.8.8", 0x08080808},
        {"192.168.0.1",0x0100a8c0},
        {"165.90.165.90",0x5aa55aa5},
        {"90.165.90.165",0xa55aa55a},
        {"1.2.3.4", 0x04030201},
        {"4.3.2.1", 0x01020304}
};

const unsigned nIPv4Entries = sizeof(IPv4_TestAddresses)/sizeof(struct IPv4Entry);

static void create_test_handler(void)
{
}
int socket_api_test_create_destroy(socket_stack_t stack, socket_address_family_t disable_family)
{
    struct socket s;
    int afi, pfi;
    socket_error_t err;
    const struct socket_api * api = socket_get_api(stack);
    TEST_CLEAR();
    if (!TEST_NEQ(api, NULL)) {
        // Test cannot continue without API.
        return 0;
    }
    err = api->init();
    if (!TEST_EQ(err, SOCKET_ERROR_NONE)) {
        return 0;
    }

    // Create a socket for each address family
    for (afi = SOCKET_AF_UNINIT; afi <= SOCKET_AF_MAX; afi++) {
        socket_address_family_t af = static_cast<socket_address_family_t>(afi);
        // Create a socket for each protocol family
        for (pfi = SOCKET_PROTO_UNINIT; pfi <= SOCKET_PROTO_MAX; pfi++) {
            socket_proto_family_t pf = static_cast<socket_proto_family_t>(pfi);
            // Zero the implementation
            s.impl = NULL;
            err = api->create(&s, af, pf, &create_test_handler);
            // catch expected failing cases
            if (af == SOCKET_AF_UNINIT || af == SOCKET_AF_MAX ||
                    pf == SOCKET_PROTO_UNINIT || pf == SOCKET_PROTO_MAX ||
                    af == disable_family) {
                TEST_NEQ(err, SOCKET_ERROR_NONE);
                continue;
            }
            TEST_EQ(err, SOCKET_ERROR_NONE);
            if (!TEST_NEQ(s.impl, NULL)) {
                continue;
            }
            // Destroy the socket;
            err = api->destroy(&s);
            TEST_EQ(err, SOCKET_ERROR_NONE);
            // Zero the implementation
            s.impl = NULL;
            // Create a socket with a NULL handler
            err = api->create(&s, af, pf, NULL);
            TEST_NEQ(err, SOCKET_ERROR_NONE);
            TEST_EQ(s.impl, NULL);
            // Destroy the socket (since creation will have failed, a null pointer is expected);
            err = api->destroy(&s);
            TEST_EQ(err, SOCKET_ERROR_NULL_PTR);
            // Try destroying a socket that hasn't been created
            s.impl = NULL;
            err = api->destroy(&s);
            TEST_NEQ(err, SOCKET_ERROR_NONE);

            /*
             * Because the allocator is stack-dependent, there is no way to test for a
             * memory leak in a portable way
             */
        }
    }
    TEST_RETURN();
}

int socket_api_test_socket_str2addr(socket_stack_t stack, socket_address_family_t disable_family)
{
    struct socket s;
    socket_error_t err;
    int afi;
    struct socket_addr addr;
    const struct socket_api * api = socket_get_api(stack);
    s.api = api;
    s.stack = stack;
    // Create a socket for each address family
    for (afi = SOCKET_AF_UNINIT+1; afi < SOCKET_AF_MAX; afi++) {
        socket_address_family_t af = static_cast<socket_address_family_t>(afi);
        if (af == disable_family) {
            continue;
        }
        switch(af) {
            case SOCKET_AF_INET4:
                for (unsigned i = 0; i < nIPv4Entries; i++) {
                    err = api->str2addr(&s, &addr, IPv4_TestAddresses[i].test);
                    TEST_EQ(err, SOCKET_ERROR_NONE);
                    if (!TEST_EQ(addr.storage[0], IPv4_TestAddresses[i].expect)) {
                        printf ("Expected: %08lx, got: %08lx\r\n", IPv4_TestAddresses[i].expect,addr.storage[0]);
                    }
                }
                break;
            default:
                TEST_EQ(1,0);
                break;
        }
    }
    TEST_RETURN();
}

volatile int timedout;
static void onTimeout() {
    timedout = 1;
}

static struct socket *ConnectCloseSock;

volatile int eventid;
volatile int closed;
volatile int connected;
static void connect_close_handler(void)
{
    switch(ConnectCloseSock->event->event) {
        case SOCKET_EVENT_DISCONNECT:
            closed = 1;
            break;
        case SOCKET_EVENT_CONNECT:
            connected = 1;
            break;
        default:
            break;
    }
}

int socket_api_test_connect_close(socket_stack_t stack, socket_address_family_t disable_family, const char* server, uint16_t port)
{
    struct socket s;
    int afi, pfi;
    socket_error_t err;
    const struct socket_api * api = socket_get_api(stack);
    struct socket_addr addr;

    ConnectCloseSock = &s;
    TEST_CLEAR();
    if (!TEST_NEQ(api, NULL)) {
        // Test cannot continue without API.
        return 0;
    }
    err = api->init();
    if (!TEST_EQ(err, SOCKET_ERROR_NONE)) {
        return 0;
    }

    // Create a socket for each address family
    for (afi = SOCKET_AF_UNINIT+1; afi < SOCKET_AF_MAX; afi++) {
        socket_address_family_t af = static_cast<socket_address_family_t>(afi);
        if (af == disable_family) {
            continue;
        }
        // Create a socket for each protocol family
        for (pfi = SOCKET_PROTO_UNINIT+1; pfi < SOCKET_PROTO_MAX; pfi++) {
            socket_proto_family_t pf = static_cast<socket_proto_family_t>(pfi);
            // Zero the implementation
            s.impl = NULL;
            err = api->create(&s, af, pf, &connect_close_handler);
            // catch expected failing cases
            TEST_EQ(err, SOCKET_ERROR_NONE);
            if (!TEST_NEQ(s.impl, NULL)) {
                continue;
            }

            // connect to a remote host
            err = api->str2addr(&s, &addr, server);
            TEST_EQ(err, SOCKET_ERROR_NONE);

            timedout = 0;
            connected = 0;
            mbed::Timeout to;
            to.attach(onTimeout, SOCKET_CONNECT_TIMEOUT);
            err = api->connect(&s, &addr, port);
            TEST_EQ(err, SOCKET_ERROR_NONE);
            if (err!=SOCKET_ERROR_NONE) {
                printf("err = %d\r\n", err);
            }
            switch (pf) {
            case SOCKET_DGRAM:
                while ((!api->is_connected(&s)) && (!timedout)) {
                    __WFI();
                }
                break;
            case SOCKET_STREAM:
                while (!connected && !timedout) {
                    __WFI();
                }
                break;
            default: break;
            }
            to.detach();
            TEST_EQ(timedout, 0);

            // close the connection
            timedout = 0;
            closed = 0;
            to.attach(onTimeout, SOCKET_CONNECT_TIMEOUT);
            err = api->close(&s);
            TEST_EQ(err, SOCKET_ERROR_NONE);
            if (err!=SOCKET_ERROR_NONE) {
                printf("err = %d\r\n", err);
            }
            switch (pf) {
                case SOCKET_DGRAM:
                    while ((api->is_connected(&s)) && (!timedout)) {
                        __WFI();
                    }
                    break;
                case SOCKET_STREAM:
                    while (!closed && !timedout) {
                        __WFI();
                    }
                    break;
                default: break;
            }
            to.detach();
            TEST_EQ(timedout, 0);
            // Destroy the socket
            err = api->destroy(&s);
            TEST_EQ(err, SOCKET_ERROR_NONE);
        }
    }
    TEST_RETURN();
}
