/*
 * PackageLicenseDeclared: Apache-2.0
 * Copyright 2015 ARM Holdings PLC
 */

#include <mbed-net-socket-abstract/socket_api.h>
#include <stdio.h>
#include "../source/test/ctest_env.h"
#include <string.h>

#define TEST_PRINT(S,...) printf((S),__VA_ARGS__)
#define TEST_EQ(A,B)\
    ((A!=B)?(test_pass_global = 0, TEST_PRINT("%s:%d " #A "!=" #B " in %s [FAIL]\r\n", __FILE__, __LINE__, __func__),0):1)
#define TEST_NEQ(A,B)\
    ((A==B)?(test_pass_global = 0, TEST_PRINT("%s:%d " #A "==" #B " in %s [FAIL]\r\n", __FILE__, __LINE__, __func__),0):1)


struct socket_api test_api[SOCKET_MAX_STACKS+1];
struct socket_api expect_fail_api;

int test_pass_global = 1;

int test_socket_stack_registry() {
    unsigned int i;
    socket_error_t err = SOCKET_ERROR_NONE;
    printf("Testing stack registry...\n");

    // Try to register a stack marked as Uninitialized
    expect_fail_api.stack = SOCKET_STACK_UNINIT;
    err = socket_register_stack((&expect_fail_api));
    TEST_NEQ(err,SOCKET_ERROR_NONE);

    // Register a NULL socket api
    memset(&expect_fail_api, 0, sizeof(struct socket_api));
    err = socket_register_stack(&expect_fail_api);
    TEST_NEQ(err,SOCKET_ERROR_NONE);

    // Register a socket api with one zeroed API.
    memset(&expect_fail_api, 1, sizeof(struct socket_api));
    expect_fail_api.create = NULL;
    expect_fail_api.stack = SOCKET_STACK_UNINIT + 1;
    err = socket_register_stack(&expect_fail_api);
    TEST_NEQ(err,SOCKET_ERROR_NONE);

    // Register two of the same socket api's
    memset(&test_api[0], 1, sizeof(struct socket_api));
    memset(&expect_fail_api, 1, sizeof(struct socket_api));
    test_api[0].stack = SOCKET_STACK_UNINIT + 1;
    expect_fail_api.stack = SOCKET_STACK_UNINIT + 1;
    err = socket_register_stack(&test_api[0]);
    TEST_EQ(err,SOCKET_ERROR_NONE);
    err = socket_register_stack(&expect_fail_api);
    TEST_NEQ(err,SOCKET_ERROR_NONE);

    // Register the same stack again, but with the stack ID changed
    test_api[0].stack++;
    err = socket_register_stack(&test_api[0]);
    TEST_NEQ(err,SOCKET_ERROR_NONE);
    test_api[0].stack--;

    // Try to register a stack outside the accepted range
    expect_fail_api.stack = SOCKET_STACK_MAX;
    err = socket_register_stack(&expect_fail_api);
    TEST_NEQ(err,SOCKET_ERROR_NONE);

    // One stack is already registered
    // Try to register the maximum number of stacks
    for (i = 1; i < SOCKET_MAX_STACKS; i++) {
        socket_stack_t stack = SOCKET_STACK_UNINIT + 1 + i;
        memset (&test_api[i],1,sizeof(struct socket_api));
        test_api[i].stack = stack;
        err = socket_register_stack(&test_api[i]);
        TEST_EQ(err,SOCKET_ERROR_NONE);
    }
    // Then register one more.
    expect_fail_api.stack = SOCKET_MAX_STACKS+1;
    err = socket_register_stack(&expect_fail_api);
    TEST_NEQ(err,SOCKET_ERROR_NONE);

    // Extract an uninit socket api
    const struct socket_api *papi;
    papi = socket_get_api(SOCKET_STACK_UNINIT);
    TEST_EQ(papi, NULL);

    if (SOCKET_MAX_STACKS < SOCKET_STACK_MAX - 1) {
        // Get a valid, but unregistered stack
        papi = socket_get_api(SOCKET_STACK_MAX - 1);
        TEST_EQ(papi, NULL);
    }
    // Verify all registered stacks
    for (i = 0; i < SOCKET_MAX_STACKS; i++) {
        socket_stack_t stack = SOCKET_STACK_UNINIT + 1 + i;
        papi = socket_get_api(stack);
        TEST_EQ(papi, &test_api[i]);
    }

    cnotify_completion(test_pass_global);
    return !test_pass_global;
}

int main() {
    test_socket_stack_registry();
}
