#ifndef LIBRARIES_MBED_HAL_SOCKET_TYPES_H_
#define LIBRARIES_MBED_HAL_SOCKET_TYPES_H_


typedef struct {
    // TBD
} socket_t;

typedef struct {
    // TBD
} event_t;

typedef struct {
    // TBD
} address_t;

typedef enum {
    SOCKET_ERROR_NONE = 0,
    SOCKET_ERROR_BUSY,
} socket_error_t;

typedef enum {
    // TBD
} socket_flags_t;

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
typedef void (*handler_t)(socket_error_t, event_t *);

// TODO: The type of the buffers is TBD
// buffer_t is always passed by value to avoid losing temporary buffer_t's
typedef struct {
    void *p;
    size_t l;
} buffer_t;

#endif /* LIBRARIES_MBED_HAL_SOCKET_TYPES_H_ */
