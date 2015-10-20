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

#include "sal/test/sal_test_api.h"
#include "sal/socket_api.h"
#include "sal/test/ctest_env.h"
#include "mbed-drivers/Timeout.h"
#include "mbed-drivers/Ticker.h"
#include "mbed-drivers/mbed.h"

#ifndef SOCKET_TEST_TIMEOUT
#define SOCKET_TEST_TIMEOUT 1.0f
#endif

#ifndef SOCKET_TEST_SERVER_TIMEOUT
#define SOCKET_TEST_SERVER_TIMEOUT 10.0f
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
                    if (!TEST_EQ(socket_addr_get_ipv4_addr(&addr), IPv4_TestAddresses[i].expect)) {
                        printf ("Expected: %08lx, got: %08lx\r\n", IPv4_TestAddresses[i].expect,socket_addr_get_ipv4_addr(&addr));
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

int socket_api_test_connect_close(socket_stack_t stack, socket_address_family_t af, const char* server, uint16_t port)
{
    struct socket s;
    int pfi;
    socket_error_t err;
    const struct socket_api * api = socket_get_api(stack);
    struct socket_addr addr;

    ConnectCloseSock = &s;
    TEST_CLEAR();
    if (!TEST_NEQ(api, NULL)) {
        // Test cannot continue without API.
        TEST_RETURN();
    }
    err = api->init();
    if (!TEST_EQ(err, SOCKET_ERROR_NONE)) {
        TEST_RETURN();
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
		// Tell the host launch a server
		TEST_PRINT(">>> ES,%d\r\n", pf);

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
		// Tell the host to kill the server
		TEST_PRINT(">>> KILL ES\r\n");

		// Destroy the socket
		err = api->destroy(&s);
		TEST_EQ(err, SOCKET_ERROR_NONE);
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
        socket_addr_copy((struct socket_addr *)(void *)&blocking_resolve_addr, (struct socket_addr *)(void *)&e->i.d.addr);
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
        TEST_RETURN();
    }
    err = api->init();
    if (!TEST_EQ(err, SOCKET_ERROR_NONE)) {
        TEST_RETURN();
    }

    struct socket_addr addr;
    // Resolve the host address
    err = blocking_resolve(stack, af, server, &addr);
    if (!TEST_EQ(err, SOCKET_ERROR_NONE)) {
        TEST_RETURN();
    }
    // Tell the host launch a server
    TEST_PRINT(">>> ES,%d\r\n", pf);
    // Allocate a data buffer for tx/rx
    void *data = malloc(SOCKET_SENDBUF_MAXSIZE);

    // Zero the socket implementation
    s.impl = NULL;
    // Create a socket
    err = api->create(&s, af, pf, &client_cb);
    if (!TEST_EQ(err, SOCKET_ERROR_NONE)) {
        TEST_EXIT();
    }
    // Connect to a host
    if (connect) {
        client_event_done = false;
        timedout = 0;
        to.attach(onTimeout, SOCKET_TEST_TIMEOUT);
        err = api->connect(&s, &addr, port);
        if (!TEST_EQ(err, SOCKET_ERROR_NONE)) {
            TEST_EXIT();
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
            TEST_EXIT();
        }
        to.detach();
    }

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
                int rc = memcmp(&rxaddr.ipv6be, &addr.ipv6be, sizeof(rxaddr.ipv6be));
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

test_exit:
    TEST_PRINT(">>> KILL,ES\r\n");
    free(data);
    TEST_RETURN();
}


static volatile bool incoming;
static volatile bool server_event_done;
static volatile struct socket *server_socket;
static volatile struct socket_event server_event;
static void server_cb(void)
{
    struct socket_event *e = server_socket->event;
    event_flag_t event = e->event;
    switch (event) {
        case SOCKET_EVENT_ACCEPT:
            incoming = true;
            server_event.event = event;
            server_event.i.a.newimpl = e->i.a.newimpl;
            e->i.a.reject = 0;
            break;
        default:
            server_event_done = true;
            break;
    }
}

int socket_api_test_echo_server_stream(socket_stack_t stack, socket_address_family_t af, const char* local_addr, uint16_t port)
{
    struct socket s;
    struct socket cs;
    struct socket_addr addr;
    socket_error_t err;
    const struct socket_api *api = socket_get_api(stack);
    server_socket = &s;
    client_socket = &cs;
    mbed::Timeout to;
    mbed::Ticker ticker;
    // Create the socket
    TEST_CLEAR();

    if (!TEST_NEQ(api, NULL)) {
        // Test cannot continue without API.
        TEST_RETURN();
    }
    err = api->init();
    if (!TEST_EQ(err, SOCKET_ERROR_NONE)) {
        TEST_RETURN();
    }

    // Zero the socket implementation
    s.impl = NULL;
    // Create a socket
    err = api->create(&s, af, SOCKET_STREAM, &server_cb);
    if (!TEST_EQ(err, SOCKET_ERROR_NONE)) {
        TEST_RETURN();
    }

    err = api->str2addr(&s, &addr, local_addr);
    if (!TEST_EQ(err, SOCKET_ERROR_NONE)) {
        TEST_RETURN();
    }

    // start the TCP timer
    uint32_t interval_ms = api->periodic_interval(&s);
    TEST_NEQ(interval_ms, 0);
    uint32_t interval_us = interval_ms * 1000;
    socket_api_handler_t periodic_task = api->periodic_task(&s);
    if (TEST_NEQ(periodic_task, NULL)) {
        ticker.attach_us(periodic_task, interval_us);
    }

    err = api->bind(&s, &addr, port);
    if (!TEST_EQ(err, SOCKET_ERROR_NONE)) {
        TEST_RETURN();
    }
    void *data = malloc(SOCKET_SENDBUF_MAXSIZE);
    if(!TEST_NEQ(data, NULL)) {
        TEST_RETURN();
    }
    // Tell the host test to try and connect
    TEST_PRINT(">>> EC,%s,%d\r\n", local_addr, port);

    bool quit = false;
    // For several iterations
    while (!quit) {
        timedout = false;
        server_event_done = false;
        incoming = false;
        to.attach(onTimeout, SOCKET_TEST_SERVER_TIMEOUT);
        // Listen for incoming connections
        err = api->start_listen(&s, 0);
        if (!TEST_EQ(err, SOCKET_ERROR_NONE)) {
            TEST_EXIT();
        }
        // Reset the state of client_rx_done
        client_rx_done = false;
        // Wait for a connect event
        while (!timedout && !incoming) {
            __WFI();
        }
        if (TEST_EQ(timedout,0)) {
            to.detach();
        } else {
            TEST_EXIT();
        }
        if(!TEST_EQ(server_event.event, SOCKET_EVENT_ACCEPT)) {
            TEST_PRINT("Event: %d\r\n",(int)client_event.event);
            continue;
        }
        // Stop listening
        server_event_done = false;
        // err = api->stop_listen(&s);
        // TEST_EQ(err, SOCKET_ERROR_NONE);
        // Accept an incoming connection
        cs.impl = server_event.i.a.newimpl;
        cs.family = s.family;
        cs.stack  = s.stack;
        err = api->accept(&cs, client_cb);
        if(!TEST_EQ(err, SOCKET_ERROR_NONE)) {
            continue;
        }
        to.attach(onTimeout, SOCKET_TEST_SERVER_TIMEOUT);

                    // Client should test for successive connections being rejected
        // Until Client disconnects
        while (client_event.event != SOCKET_EVENT_ERROR && client_event.event != SOCKET_EVENT_DISCONNECT) {
            // Wait for a read event
            while (!client_event_done && !client_rx_done && !timedout) {
                __WFI();
            }
            if (!TEST_EQ(client_event_done, false)) {
                client_event_done = false;
                continue;
            }
            // Reset the state of client_rx_done
            client_rx_done = false;

            // Receive some data
            size_t len = SOCKET_SENDBUF_MAXSIZE;
            err = api->recv(&cs, data, &len);
            if (!TEST_NEQ(err, SOCKET_ERROR_WOULD_BLOCK)) {
            	TEST_PRINT("Rx Would Block\r\n");
            	continue;
            }
            if (!TEST_EQ(err, SOCKET_ERROR_NONE)) {
            	TEST_PRINT("err: (%d) %s\r\n", err, socket_strerror(err));
                break;
            }

            // Check if the server should halt
            if (strncmp((const char *)data, "quit", 4) == 0) {
                quit = true;
                break;
            }
            // Send some data
            err = api->send(&cs, data, len);
            if (!TEST_EQ(err, SOCKET_ERROR_NONE)) {
                break;
            }
        }
        to.detach();
        TEST_NEQ(timedout, true);

        // Close client socket
        err = api->close(&cs);
        TEST_EQ(err, SOCKET_ERROR_NONE);
    }
    err = api->stop_listen(&s);
    TEST_EQ(err, SOCKET_ERROR_NONE);
test_exit:
    ticker.detach();
    TEST_PRINT(">>> KILL,EC\r\n");
    free(data);
    // Destroy server socket
    TEST_RETURN();
}
