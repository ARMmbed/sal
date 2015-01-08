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

socket_error_t socket_create(struct socket *socket, socket_proto_family_t family, socket_api_handler handler);
socket_error_t socket_destroy(struct socket *socket);

socket_error_t socket_connect(struct socket *socket, const struct socket_addr *address, const uint16_t port);
socket_error_t socket_bind(struct socket *socket, const struct socket_addr *address, const uint16_t port);

socket_error_t socket_start_listen(const struct socket_addr *address, const uint16_t port, socket_api_handler handler);
socket_error_t socket_stop_listen(const struct socket_addr *address, const uint16_t port);

socket_error_t socket_start_send(struct socket *socket, void *arg, struct socket_buffer *buf, uint8_t autofree);
socket_error_t socket_start_recv(struct socket *socket, void *arg);

uint8_t socket_is_connected(struct socket *sock);
uint8_t socket_is_bound(struct socket *sock);

uint8_t socket_tx_is_busy(struct socket *sock);
uint8_t socket_rx_is_busy(struct socket *sock);

#endif /* LIBRARIES_MBED_HAL_SOCKET_API_H_ */
