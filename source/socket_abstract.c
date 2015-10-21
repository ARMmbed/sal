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
#include "sal/socket_api.h"
#include "sal/socket_types.h"

#define SOCKET_ABSTRACTION_LAYER_VERSION 1

const struct socket_api * socket_api_ptrs[SOCKET_MAX_STACKS] = { NULL };

const char * socket_strerror(socket_error_t err)
{
    switch (err) {
        case SOCKET_ERROR_NONE:
            return "No Error";
        case SOCKET_ERROR_UNKNOWN:
            return "Unknown stack error";
        case SOCKET_ERROR_BUSY:
            return "Socket is busy";
        case SOCKET_ERROR_NULL_PTR:
            return "Null pointer";
        case SOCKET_ERROR_BAD_FAMILY:
            return "Unrecognized protocol family";
        case SOCKET_ERROR_TIMEOUT:
            return "Transaction timed out";
        case SOCKET_ERROR_BAD_ALLOC:
            return "Memory allocation failed";
        case SOCKET_ERROR_NO_CONNECTION:
            return "Socket not connected";
        case SOCKET_ERROR_UNIMPLEMENTED:
            return "API Functions not implemented";
        case SOCKET_ERROR_SIZE:
            return "Zero length not permitted";
        case SOCKET_ERROR_STACK_EXISTS:
            return "Socket Stack already registered";
        case SOCKET_ERROR_STACKS:
            return "attempt to register too many socket stacks";
        case SOCKET_ERROR_BAD_STACK:
            return "socket stack has incomplete API";
        case SOCKET_ERROR_DNS_FAILED:
            return "DNS lookup failed";
        case SOCKET_ERROR_WOULD_BLOCK:
            return "No data available";
        case SOCKET_ERROR_CLOSED:
            return "Attempt to perform an operation on a closed socket";
        case SOCKET_ERROR_BAD_ADDRESS:
            return "Invalid address format";
        case SOCKET_ERROR_VALUE:
            return "Invalid value";
        case SOCKET_ERROR_ADDRESS_IN_USE:
            return "Address in use";
        case SOCKET_ERROR_ALREADY_CONNECTED:
            return "Socket already connected";
        case SOCKET_ERROR_ABORT:
            return "Connection aborted";
        case SOCKET_ERROR_RESET:
            return "Connection reset by peer";
        case SOCKET_ERROR_BAD_ARGUMENT:
            return "Invalid argument";
        case SOCKET_ERROR_INTERFACE_ERROR:
            return "Low-level interface error";
        default:
            return "Unknown error";
    }
}

const struct socket_api * socket_get_api(const socket_stack_t stack)
{
    unsigned i;
    for (i = 0; i < SOCKET_MAX_STACKS; i++) {
        if (socket_api_ptrs[i] && socket_api_ptrs[i]->stack == stack) {
            return socket_api_ptrs[i];
        }
    }
    return NULL;
}

socket_error_t socket_register_stack(const struct socket_api * api)
{
    unsigned i;
    uintptr_t fn;
    if (api == NULL) {
        return SOCKET_ERROR_NULL_PTR;
    }
    if (api->stack == SOCKET_STACK_UNINIT || api->stack >= SOCKET_STACK_MAX) {
        return SOCKET_ERROR_BAD_STACK;
    }
    if (api->version != SOCKET_ABSTRACTION_LAYER_VERSION) {
        return SOCKET_ERROR_API_VERSION;
    }

    // Check each API pointer
    for (fn = offsetof(struct socket_api, SOCKET_API_FIRST_PTR); fn <= offsetof(struct socket_api, SOCKET_API_LAST_PTR);
            fn += sizeof(void*)) {
        void** fnptr = (void**) ((uintptr_t) api + fn);
        if (*fnptr == NULL) {
            return SOCKET_ERROR_NULL_PTR;
        }
    }
    for (i = 0; i < SOCKET_MAX_STACKS; i++) {
        if (socket_api_ptrs[i] && socket_api_ptrs[i]->stack == api->stack) {
            return SOCKET_ERROR_STACK_EXISTS;
        }
        if (socket_api_ptrs[i] == NULL) {
            socket_api_ptrs[i] = api;
            return SOCKET_ERROR_NONE;
        }
    }
    return SOCKET_ERROR_STACKS;
}
