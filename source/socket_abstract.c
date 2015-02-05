#include "socket_api.h"
#include "socket_types.h"

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
    case SOCKET_STACK_RAW:
        return SOCKET_BUFFER_RAW;
    case SOCKET_STACK_LWIP_IPV4:
    case SOCKET_STACK_LWIP_IPV6:
        return SOCKET_BUFFER_LWIP_PBUF;
    case SOCKET_STACK_PLACEHOLDER:
        break;
    case SOCKET_STACK_NANOSTACK_IPV6:
        return SOCKET_BUFFER_NANOSTACK_PBUF;
        break;
    default:
        break;
    }
    return SOCKET_BUFFER_UNINITIALISED;
}
