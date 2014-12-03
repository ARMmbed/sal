#ifndef LIBRARIES_MBED_HAL_SOCKET_TYPES_H_
#define LIBRARIES_MBED_HAL_SOCKET_TYPES_H_

#include "ip_addr.h"

// TODO: Abstract ip_addr_t
typedef ip_addr_t address_t;

typedef enum {
    SOCKET_ERROR_NONE = 0,
    SOCKET_ERROR_UNKNOWN,
    SOCKET_ERROR_BUSY,
    SOCKET_ERROR_NULL_PTR,
    SOCKET_ERROR_BAD_FAMILY,
    SOCKET_ERROR_TIMEOUT,
    SOCKET_ERROR_BAD_ALLOC,

} socket_error_t;

typedef enum {
    SOCKET_ALLOC_HEAP = 0,
    SOCKET_ALLOC_POOL_BEST,
} socket_alloc_pool_t;

typedef enum {
    // TBD
} socket_flags_t;

typedef enum {
    SOCKET_EVENT_NONE = 0,
    SOCKET_EVENT_RX_DONE,
    SOCKET_EVENT_TX_DONE,
    SOCKET_EVENT_RX_ERROR,
    SOCKET_EVENT_TX_ERROR,
} event_flag_t;

typedef enum {
    SOCKET_STATUS_IDLE = 0,
    SOCKET_STATUS_RX_BUSY = 1 << 0,
    SOCKET_STATUS_TX_BUSY = 1 << 1,
} socket_status_t;

typedef void (*socket_api_handler)(void*);
typedef struct {
    void * (*alloc)(void *, size_t);
    void *context;
} socket_allocator_t;

struct socket_recv_info {
    void *context;
    struct socket *sock;
    address_t *src;
    uint16_t port;
    struct socket_buffer *buf;
    uint8_t free_buf;
};

struct socket_tx_info {
    void *context;
    struct socket *sock;
    struct socket_buffer *buf;
    uint8_t free_buf;
};

typedef struct {
    event_flag_t event;
    union {
        struct socket_recv_info r;
        struct socket_tx_info t;
    } i;
} socket_event_t;
// TODO: The type of handler_t is TBD.
// NOTE: Since handlers are passed using C++ references, a global null handler will be provided so that empty handlers
// are provided for.  Overriding the null handler is likely to be useful for debugging.
/*
 * std::function, in combination with std::bind appear to be the ideal solution for handler_t, however
 * std::bind, is not supported by all our targets. We might be able to work around this by porting
 * boost::bind to mbed
 */
//typedef std::function<void(event_t*)> handler_t;
/*
 * We already have a feature in mbed for handling function pointers.
 */
//#include "FunctionPointer.h"
//typedef FunctionPointer handler_t
/*
 * Our fallback alternative for handler_t is a function pointer:
 */
typedef void (*handler_t)(void *);

struct socket;
struct socket_buffer;


#endif /* LIBRARIES_MBED_HAL_SOCKET_TYPES_H_ */
