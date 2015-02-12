/*
 * PackageLicenseDeclared: Apache-2.0
 * Copyright 2015 ARM Holdings PLC
 */
#ifndef LIBRARIES_MBED_HAL_SOCKET_API_H_
#define LIBRARIES_MBED_HAL_SOCKET_API_H_

#include "socket_types.h"
#ifdef __cplusplus
extern "C" {
#endif

#ifndef SOCKET_MAX_STACKS
#define SOCKET_MAX_STACKS 2
#endif

typedef socket_error_t (*socket_init)();
typedef socket_error_t (*socket_create)(struct socket *socket, const socket_proto_family_t family, socket_api_handler_t const handler);
typedef socket_error_t (*socket_destroy)(struct socket *socket);
typedef socket_error_t (*socket_close)(struct socket *socket);
typedef void (*socket_abort)(struct socket *socket);
typedef socket_api_handler_t (*socket_periodic_task)(const struct socket * socket);
typedef uint32_t (*socket_periodic_interval)(const struct socket * socket);
typedef socket_error_t (*socket_resolve)(struct socket *socket, const char *address);
typedef socket_error_t (*socket_connect)(struct socket *sock, const struct socket_addr *address, const uint16_t port);
typedef socket_error_t (*socket_bind)(struct socket *socket, const struct socket_addr *address, const uint16_t port);
typedef socket_error_t (*socket_str2addr)(const struct socket *socket, struct socket_addr *addr, const char *address);
typedef socket_error_t (*socket_start_listen)(struct socket *socket, const struct socket_addr *address, const uint16_t port, socket_api_handler_t const handler);
typedef socket_error_t (*socket_stop_listen)(struct socket *socket);
typedef socket_error_t (*socket_start_send)(struct socket *socket, struct socket_buffer *buf, void *arg);
typedef socket_error_t (*socket_start_recv)(struct socket *socket);
typedef uint8_t (*socket_is_connected)(const struct socket *socket);
typedef uint8_t (*socket_is_bound)(const struct socket *socket);
typedef uint8_t (*socket_tx_is_busy)(const struct socket *socket);
typedef uint8_t (*socket_rx_is_busy)(const struct socket *socket);

struct socket_api {
    socket_stack_t              stack;
    socket_init                 init;
    socket_create               create;
    socket_destroy              destroy;
    socket_close                close;
    socket_abort                abort;
    socket_periodic_task        periodic_task;
    socket_periodic_interval    periodic_interval;
    socket_resolve              resolve;
    socket_connect              connect;
    socket_str2addr             str2addr;
    socket_bind                 bind;
    socket_start_listen         start_listen;
    socket_stop_listen          stop_listen;
    socket_start_send           start_send;
    socket_start_recv           start_recv;
    socket_is_connected         is_connected;
    socket_is_bound             is_bound;
    socket_tx_is_busy           tx_busy;
    socket_rx_is_busy           rx_busy;
    socket_buffer_type_t        pbuf_type;
};
#define SOCKET_API_FIRST_PTR init
#define SOCKET_API_LAST_PTR rx_busy

socket_error_t socket_register_stack(const struct socket_api * api);
const char * socket_strerror(const socket_error_t err);
const struct socket_api * socket_get_api(const socket_stack_t stack);

#ifdef __cplusplus
}
#endif

#endif /* LIBRARIES_MBED_HAL_SOCKET_API_H_ */
