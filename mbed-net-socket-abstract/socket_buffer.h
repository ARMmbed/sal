/*
 * socket_buffer.h
 *
 *  Created on: 2 Dec 2014
 *      Author: bremor01
 */

#ifndef LIBRARIES_MBED_HAL_SOCKET_BUFFER_H_
#define LIBRARIES_MBED_HAL_SOCKET_BUFFER_H_

#include "socket_types.h"

#ifdef __cplusplus
extern "C" {
#endif

socket_buffer_type_t socket_buf_stack_to_buf(const socket_stack_t stack);

void * socket_buf_get_ptr(const struct socket_buffer *b);
size_t socket_buf_get_size(const struct socket_buffer *b);

void socket_buf_alloc(const size_t len, const socket_alloc_pool_t p, struct socket_buffer *b);
socket_error_t socket_buf_try_free(struct socket_buffer *b);
void socket_buf_free(struct socket_buffer *b);

socket_error_t socket_copy_from_user(struct socket_buffer *b, const void *u, const size_t len);
uint16_t socket_copy_to_user(void *u, const struct socket_buffer *b, const size_t len);

#ifdef __cplusplus
}
#endif
#endif /* LIBRARIES_MBED_HAL_SOCKET_BUFFER_H_ */
