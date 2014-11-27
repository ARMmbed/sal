/*
 * socket_api.h
 *
 *  Created on: 26 Nov 2014
 *      Author: bremor01
 */

#ifndef LIBRARIES_MBED_HAL_SOCKET_API_H_
#define LIBRARIES_MBED_HAL_SOCKET_API_H_

#include "socket_types.h"

typedef void (*socket_api_handler)(void*);
typedef struct {
    void * (*alloc)(void *, size_t);
    void *context;
} socket_allocator_t;

typedef struct

socket_error_t socket_init();
socket_error_t create_udp_socket(socket_t *socket, socket_api_handler handler);
socket_error_t destroy_udp_socket(socket_t *socket);

socket_error_t create_tcp_socket(socket_t *socket, socket_api_handler handler);
socket_error_t destroy_tcp_socket(socket_t *socket);

socket_error_t socket_start_alloc_recv(socket_t *socket, socket_allocator_t *alloc);
socket_error_t socket_stop_alloc_recv(socket_t *socket);

socket_error_t socket_connect(socket_t *socket, address_t *address, uint16_t port);
socket_error_t socket_bind(socket_t *socket, address_t *address, uint16_t port);

socket_error_t socket_start_listen(address_t *address, uint16_t port, socket_api_handler handler);
socket_error_t socket_stop_listen(address_t *address, uint16_t port);

socket_error_t socket_start_send(socket_t *socket, void *buf, size_t len);
socket_error_t socket_start_recv(socket_t *socket, void *buf, size_t len);

#endif /* LIBRARIES_MBED_HAL_SOCKET_API_H_ */
