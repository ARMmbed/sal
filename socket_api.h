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
socket_error_t create_udp_socket(socket_t *socket, address_t *address, uint16_t port);
socket_error_t destroy_udp_socket(socket_t *socket);

socket_error_t socket_start_send(socket_t *socket, void *buf, size_t len);
socket_error_t socket_start_send_to(socket_t *socket, address_t *address, uint16_t port, void *buf, size_t len);
socket_error_t socket_start_recv(socket_t *socket, void *buf, size_t len);
socket_error_t socket_start_recv_from(socket_t *socket, address_t *address, uint16_t port, void *buf, size_t len);
listen
connect
close
set_options

#endif /* LIBRARIES_MBED_HAL_SOCKET_API_H_ */
