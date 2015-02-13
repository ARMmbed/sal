/*
 * socket_buffer.h
 *
 *  Created on: 2 Dec 2014
 *      Author: bremor01
 */

#ifndef LIBRARIES_MBED_HAL_SOCKET_BUFFER_H_
#define LIBRARIES_MBED_HAL_SOCKET_BUFFER_H_

#include "socket_types.h"
#include "socket_api.h"

#ifdef __cplusplus
extern "C" {
#endif

socket_buffer_type_t socket_buf_stack_to_type(const socket_stack_t stack);
const struct socket_buf_api * socket_buf_type_to_api(const socket_buffer_type_t buf_type);


#ifdef __cplusplus
}
#endif
#endif /* LIBRARIES_MBED_HAL_SOCKET_BUFFER_H_ */
