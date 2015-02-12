#include "socket_api.h"
#include "socket_types.h"

const struct socket_api * socket_api_ptrs[SOCKET_MAX_STACKS] = {NULL};

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
    default:
      return "Unknown error";
  }
}

socket_buffer_type_t socket_buf_stack_to_buf(const socket_stack_t stack)
{
    switch (stack) {
    case SOCKET_STACK_UNINIT:
        return SOCKET_BUFFER_UNINITIALISED;
    case SOCKET_STACK_LWIP_IPV4:
    case SOCKET_STACK_LWIP_IPV6:
        return SOCKET_BUFFER_LWIP_PBUF;
    case SOCKET_STACK_NANOSTACK_IPV6:
        return SOCKET_BUFFER_NANOSTACK_PBUF;
        break;
    default:
        break;
    }
    return SOCKET_BUFFER_UNINITIALISED;
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

socket_error_t socket_register_stack(const struct socket_api * api) {
    unsigned i;
    uintptr_t fn;
    if (api == NULL) {
        return SOCKET_ERROR_NULL_PTR;
    }
    if (api->stack == SOCKET_STACK_UNINIT || api->stack >= SOCKET_STACK_MAX) {
        return SOCKET_ERROR_BAD_STACK;
    }
    // Check each API pointer
    for (fn = offsetof(struct socket_api, SOCKET_API_FIRST_PTR);
            fn <= offsetof(struct socket_api, SOCKET_API_LAST_PTR);
            fn+=sizeof(void*) )
    {
        void** fnptr = (void**)((uintptr_t)api + fn);
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