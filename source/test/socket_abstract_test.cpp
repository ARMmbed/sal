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

#ifndef SOCKET_TEST_TIMEOUT
#define SOCKET_TEST_TIMEOUT 1.0f
#endif

#ifndef SOCKET_SENDBUF_BLOCKSIZE
#define SOCKET_SENDBUF_BLOCKSIZE 32
#endif

#ifndef SOCKET_SENDBUF_MAXSIZE
#define SOCKET_SENDBUF_MAXSIZE 4096
#endif

#ifndef SOCKET_SENDBUF_ITERATIONS
#define SOCKET_SENDBUF_ITERATIONS 8
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
            // A NULL impl is not explicitly an exception since it can be zeroed during disconnect
            // Destroy the socket
            err = api->destroy(&s);
            TEST_EQ(err, SOCKET_ERROR_NONE);
            // Try destroying a socket that hasn't been created
            s.impl = NULL;
            err = api->destroy(&s);
            TEST_EQ(err, SOCKET_ERROR_NONE);

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
            to.attach(onTimeout, SOCKET_TEST_TIMEOUT);
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
            to.attach(onTimeout, SOCKET_TEST_TIMEOUT);
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


static volatile bool blocking_resolve_done;
static volatile socket_error_t blocking_resolve_err;
static volatile struct socket *blocking_resolve_socket;
static volatile struct socket_addr blocking_resolve_addr;
static volatile const char * blocking_resolve_domain;

static void blocking_resolve_cb()
{
    struct socket_event *e = blocking_resolve_socket->event;
    if (e->event == SOCKET_EVENT_ERROR) {
        blocking_resolve_err = e->i.e;
        blocking_resolve_done = true;
        return;
    } else if (e->event == SOCKET_EVENT_DNS) {
        blocking_resolve_addr.type = e->i.d.addr.type;
        blocking_resolve_addr.storage[0] = e->i.d.addr.storage[0];
        blocking_resolve_addr.storage[1] = e->i.d.addr.storage[1];
        blocking_resolve_addr.storage[2] = e->i.d.addr.storage[2];
        blocking_resolve_addr.storage[3] = e->i.d.addr.storage[3];
        blocking_resolve_domain = e->i.d.domain;
        blocking_resolve_err = SOCKET_ERROR_NONE;
        blocking_resolve_done = true;
    } else {
        blocking_resolve_err = SOCKET_ERROR_UNKNOWN;
        blocking_resolve_done = true;
        return;
    }
}

socket_error_t blocking_resolve(const socket_stack_t stack, const socket_address_family_t af, const char* server, struct socket_addr * addr) {
    struct socket s;
    const struct socket_api *api = socket_get_api(stack);
    (void) af;
    blocking_resolve_socket = &s;
    s.stack = stack;
    s.handler = blocking_resolve_cb;
    s.api = api;
    blocking_resolve_done = false;
    socket_error_t err = api->resolve(&s, server);
    if(!TEST_EQ(err, SOCKET_ERROR_NONE)) {
        return err;
    }
    while (!blocking_resolve_done) {
        __WFI();
    }
    if(!TEST_EQ(blocking_resolve_err, SOCKET_ERROR_NONE)) {
        return blocking_resolve_err;
    }
    int rc = strcmp(server, (char *)blocking_resolve_domain);
    TEST_EQ(rc,0);
    memcpy(addr, (const void*)&blocking_resolve_addr, sizeof(struct socket_addr));
    return SOCKET_ERROR_NONE;
}

/**
 * Tests the following APIs:
 * create
 * connect
 * send
 * recv
 * close
 * destroy
 *
 * @param stack
 * @param disable_family
 * @param server
 * @param port
 * @return
 */
static struct socket *client_socket;
static volatile bool client_event_done;
static volatile bool client_rx_done;
static volatile bool client_tx_done;
static volatile struct socket_tx_info client_tx_info;
static volatile struct socket_event client_event;
static void client_cb() {
    struct socket_event *e = client_socket->event;
    event_flag_t event = e->event;
    switch (event) {
        case SOCKET_EVENT_RX_DONE:
            client_rx_done = true;
            break;
        case SOCKET_EVENT_TX_DONE:
            client_tx_done = true;
            client_tx_info.sentbytes = e->i.t.sentbytes;
            break;
        default:
            memcpy((void *) &client_event, (const void*)e, sizeof(e));
            client_event_done = true;
            break;
    }
}

int socket_api_test_echo_client_connected(socket_stack_t stack, socket_address_family_t af, socket_proto_family_t pf, bool connect,
        const char* server, uint16_t port)
{
    struct socket s;
    socket_error_t err;
    const struct socket_api *api = socket_get_api(stack);
    client_socket = &s;
    mbed::Timeout to;
    // Create the socket
    TEST_CLEAR();
    TEST_PRINT("\r\n%s af: %d, pf: %d, connect: %d, server: %s:%d\r\n",__func__, (int) af, (int) pf, (int) connect, server, (int) port);
    if (!TEST_NEQ(api, NULL)) {
        // Test cannot continue without API.
        return 0;
    }
    err = api->init();
    if (!TEST_EQ(err, SOCKET_ERROR_NONE)) {
        return 0;
    }

    struct socket_addr addr;
    // Resolve the host address
    err = blocking_resolve(stack, af, server, &addr);
    if (!TEST_EQ(err, SOCKET_ERROR_NONE)) {
        return 0;
    }

    // Zero the socket implementation
    s.impl = NULL;
    // Create a socket
    err = api->create(&s, af, pf, &client_cb);
    if (!TEST_EQ(err, SOCKET_ERROR_NONE)) {
        return 0;
    }
    // Connect to a host
    if (connect) {
        client_event_done = false;
        timedout = 0;
        to.attach(onTimeout, SOCKET_TEST_TIMEOUT);
        err = api->connect(&s, &addr, port);
        if (!TEST_EQ(err, SOCKET_ERROR_NONE)) {
            return 0;
        }
        // Override event for dgrams.
        if (pf == SOCKET_DGRAM) {
            client_event.event = SOCKET_EVENT_CONNECT;
            client_event_done = true;
        }
        // Wait for onConnect
        while (!timedout && !client_event_done) {
            __WFI();
        }

        // Make sure that the correct event occurred
        if (!TEST_EQ(client_event.event, SOCKET_EVENT_CONNECT)) {
            return 0;
        }
        to.detach();
    }

    // Allocate a data buffer for tx/rx
    void *data = malloc(SOCKET_SENDBUF_MAXSIZE);

    // For several iterations
    for (size_t i = 0; i < SOCKET_SENDBUF_ITERATIONS; i++) {
        // Fill some data into a buffer
        const size_t nWords = SOCKET_SENDBUF_BLOCKSIZE * (1 << i) / sizeof(uint16_t);
        for (size_t j = 0; j < nWords; j++) {
            *((uint16_t*) data + j) = j;
        }
        // Send the data
        client_tx_done = false;
        client_rx_done = false;
        timedout = 0;
        to.attach(onTimeout, SOCKET_TEST_TIMEOUT);

        if(connect) {
            err = api->send(&s, data, nWords * sizeof(uint16_t));
        } else {
            err = api->send_to(&s, data, nWords * sizeof(uint16_t), &addr, port);
        }
        if (!TEST_EQ(err, SOCKET_ERROR_NONE)) {
            TEST_PRINT("Failed to send %u bytes\r\n", nWords * sizeof(uint16_t));
        } else {
            size_t tx_bytes = 0;
            do {
                // Wait for the onSent callback
                while (!timedout && !client_tx_done) {
                    __WFI();
                }
                if (!TEST_EQ(timedout,0)) {
                    break;
                }
                if (!TEST_NEQ(client_tx_info.sentbytes, 0)) {
                    break;
                }
                tx_bytes += client_tx_info.sentbytes;
                if (tx_bytes < nWords * sizeof(uint16_t)) {
                    client_tx_done = false;
                    continue;
                }
                to.detach();
                TEST_EQ(tx_bytes, nWords * sizeof(uint16_t));
                break;
            } while (1);
        }
        timedout = 0;
        to.attach(onTimeout, SOCKET_TEST_TIMEOUT);
        memset(data, 0, nWords * sizeof(uint16_t));
        // Wait for the onReadable callback
        size_t rx_bytes = 0;
        do {
            while (!timedout && !client_rx_done) {
                __WFI();
            }
            if (!TEST_EQ(timedout,0)) {
                break;
            }
            size_t len = SOCKET_SENDBUF_MAXSIZE - rx_bytes;
            // Receive data
            if (connect) {
                err = api->recv(&s, (void*) ((uintptr_t) data + rx_bytes), &len);
            } else {
                struct socket_addr rxaddr;
                uint16_t rxport = 0;
                // addr may contain unused data in the storage element.
                memcpy(&rxaddr, &addr, sizeof(rxaddr));
                // Receive from...
                err = api->recv_from(&s, (void*) ((uintptr_t) data + rx_bytes), &len, &rxaddr, &rxport);
                TEST_EQ(rxaddr.type, stack);
                int rc = memcmp(&rxaddr.storage, &addr.storage, sizeof(rxaddr.storage));
                if(!TEST_EQ(rc, 0)) {
                    TEST_PRINT("Spurious receive packet\r\n");
                }
                TEST_EQ(rxport, port);
            }
            if (!TEST_EQ(err, SOCKET_ERROR_NONE)) {
                break;
            }
            rx_bytes += len;
            if (rx_bytes < nWords * sizeof(uint16_t)) {
                client_rx_done = false;
                continue;
            }
            to.detach();
            break;
        } while (1);
        if(!TEST_EQ(rx_bytes, nWords * sizeof(uint16_t))) {
            TEST_PRINT("Expected %u, got %u\r\n", nWords * sizeof(uint16_t), rx_bytes);
        }

        // Validate that the two buffers are the same
        bool match = true;
        size_t j;
        for (j = 0; match && j < nWords; j++) {
            match = (*((uint16_t*) data + j) == j);
        }
        if(!TEST_EQ(match, true)) {
            TEST_PRINT("Mismatch in %u byte packet at offset %u\r\n", nWords * sizeof(uint16_t), j * sizeof(uint16_t));
        }

    }
    if (connect) {
        // close the socket
        client_event_done = false;
        timedout = 0;
        to.attach(onTimeout, SOCKET_TEST_TIMEOUT);
        err = api->close(&s);
        TEST_EQ(err, SOCKET_ERROR_NONE);

        // Override event for dgrams.
        if (pf == SOCKET_DGRAM) {
            client_event.event = SOCKET_EVENT_DISCONNECT;
            client_event_done = true;
        }

        // wait for onClose
        while (!timedout && !client_event_done) {
            __WFI();
        }
        if (TEST_EQ(timedout,0)) {
            to.detach();
        }
        // Make sure that the correct event occurred
        TEST_EQ(client_event.event, SOCKET_EVENT_DISCONNECT);
    }

    // destroy the socket
    err = api->destroy(&s);
    TEST_EQ(err, SOCKET_ERROR_NONE);

    free(data);
    TEST_RETURN();
}

//int socket_api_test_echo_client_unconnected(socket_stack_t stack, socket_address_family_t disable_family, const char* server, uint16_t port)
//{
//
//    // Create the socket
//    // For several iterations
//        // Format some data into a buffer
//        // Send the data
//            // Check for failure if it's a TCP socket
//        // Receive data back
//            // Check for failure if it's a TCP socket
//        // Validate that the two buffers are the same
//    // close the socket
//    // destroy the socket
//    TEST_RETURN();
//
//}
//
//int socket_api_test_echo_server_stream(socket_stack_t stack, socket_address_family_t disable_family, const char* server, uint16_t port)
//{
//    // Create the socket
//    // For several iterations
//        // Listen for incoming connections
//        // Accept an incoming connection
//        // Stop listening
//            // Client should test for successive connections being rejected
//        // Until Client disconnects
//            // Receive some data
//            // Send some data
//        // Close client socket
//    // Destroy server socket
//    TEST_RETURN();
//
//}
//
//int socket_api_test_echo_server_dgram(socket_stack_t stack, socket_address_family_t disable_family, const char* server, uint16_t port)
//{
//    // Create the socket
//    // For several iterations
//        // Receive some data
//        // Send some data
//    // Destroy server socket
//    TEST_RETURN();
//
//}



//[OK] //socket_init                 init;
//[OK] //socket_create               create;
//[OK] //socket_destroy              destroy;
//[OK] //socket_close                close;
//[NT] //socket_periodic_task        periodic_task;
//[NT] //socket_periodic_interval    periodic_interval;
//[HL] //socket_resolve              resolve;
//[OK] //socket_connect              connect;
//[OK] //socket_str2addr             str2addr;

// Remaining tests could be wrapped in two classes of test application:
// Echo Server
// Echo Client

// Tested in parallel with listen?
//socket_bind                 bind;

// Connection attempts arrive?
//socket_start_listen         start_listen;

// Connections are rejected?
//socket_stop_listen          stop_listen;

// Connection attempts are accepted?
//socket_accept               accept;

// Data is sent on connected socket
//socket_send                 send;

// Data is sent on unconnected socket
//socket_send_to              send_to;

// Data is received on connected socket
//socket_recv                 recv;

// Data is received on unconnected socket
//socket_recv_from            recv_from;

// Valid state is returned for connected
//socket_is_connected         is_connected;

// Valid state is returned for bound
//socket_is_bound             is_bound;
