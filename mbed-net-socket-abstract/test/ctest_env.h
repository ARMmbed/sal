/*
 * ctest_env.h
 *
 *  Created on: 20 Mar 2015
 *      Author: bremor01
 */

#ifndef STACK_REGISTRY_CTEST_ENV_H_
#define STACK_REGISTRY_CTEST_ENV_H_

#include <stdio.h>
#include <string.h>

#define TEST_PRINT(S,...) printf((S),__VA_ARGS__)
#define TEST_EQ(A,B)\
    ((A!=B)?(test_pass_global = 0, TEST_PRINT("%s:%d " #A "!=" #B " [FAIL]\r\n", __func__, __LINE__),0):1)
#define TEST_NEQ(A,B)\
    ((A==B)?(test_pass_global = 0, TEST_PRINT("%s:%d " #A "==" #B " [FAIL]\r\n", __func__, __LINE__),0):1)

#define TEST_CLEAR()\
    (test_pass_global = 1, 0)

#define TEST_RESULT()\
    (test_pass_global)

#define TEST_RETURN()\
    do {\
        if(TEST_RESULT()) {\
            TEST_PRINT("%s [PASS]\r\n", __func__);\
        }\
        return TEST_RESULT();\
    }while (0)

#ifdef __cplusplus
extern "C"
#endif
void cnotify_completion(int fail);
static int test_pass_global = 1;

#
#endif /* STACK_REGISTRY_CTEST_ENV_H_ */
