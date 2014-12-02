/*
 * socket_api.h
 *
 *  Created on: 26 Nov 2014
 *      Author: bremor01
 */

#ifndef LIBRARIES_MBED_HAL_SOCKET_API_H_
#define LIBRARIES_MBED_HAL_SOCKET_API_H_

#include "socket_types.h"

socket_error_t socket_init();

socket_error_t socket_create(struct socket *socket, uint8_t family, socket_api_handler handler);
socket_error_t socket_destroy(struct socket *socket);

socket_error_t socket_start_alloc_recv(struct socket *socket, socket_allocator_t *alloc);
socket_error_t socket_stop_alloc_recv(struct socket *socket);


socket_error_t socket_connect(struct socket *socket, address_t *address, uint16_t port);
socket_error_t socket_bind(struct socket *socket, address_t *address, uint16_t port);

socket_error_t socket_start_listen(address_t *address, uint16_t port, socket_api_handler handler);
socket_error_t socket_stop_listen(address_t *address, uint16_t port);

socket_error_t socket_start_send(struct socket *socket, struct socket_buffer *buf);
socket_error_t socket_start_recv(struct socket *socket, struct socket_buffer *buf);

#endif /* LIBRARIES_MBED_HAL_SOCKET_API_H_ */
