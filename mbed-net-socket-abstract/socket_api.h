#ifndef LIBRARIES_MBED_HAL_SOCKET_API_H_
#define LIBRARIES_MBED_HAL_SOCKET_API_H_

#include "socket_types.h"
#ifdef __cplusplus
extern "C" {
#endif
socket_error_t socket_init();

socket_error_t socket_create(struct socket *socket, const socket_proto_family_t family, socket_api_handler_t const handler);
socket_error_t socket_destroy(struct socket *socket);

socket_error_t socket_close(struct socket *socket);
void socket_abort(struct socket *socket);

socket_api_handler_t socket_periodic_task(const struct socket * socket);
uint32_t socket_periodic_interval(const struct socket * socket);

socket_error_t socket_resolve(struct socket *socket, const char *address, struct socket_addr *addr);

socket_error_t socket_connect(struct socket *socket, const void *address, const uint16_t port);
socket_error_t socket_bind(struct socket *socket, const void *address, const uint16_t port);

socket_error_t socket_start_listen(struct socket *socket, const void *address, const uint16_t port, socket_api_handler_t const handler);
socket_error_t socket_stop_listen(struct socket *socket);

socket_error_t socket_start_send(struct socket *socket, struct socket_buffer *buf, void *arg);

socket_error_t socket_start_recv(struct socket *socket);

uint8_t socket_is_connected(const struct socket *socket);
uint8_t socket_is_bound(const struct socket *socket);

uint8_t socket_tx_is_busy(const struct socket *socket);
uint8_t socket_rx_is_busy(const struct socket *socket);

const char * socket_strerror(const socket_error_t err);

#ifdef __cplusplus
}
#endif

#endif /* LIBRARIES_MBED_HAL_SOCKET_API_H_ */
