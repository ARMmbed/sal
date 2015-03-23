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
    ((A!=B)?(test_pass_global = 0, TEST_PRINT("%s:%d " #A "!=" #B " in %s [FAIL]\r\n", __FILE__, __LINE__, __func__),0):1)
#define TEST_NEQ(A,B)\
    ((A==B)?(test_pass_global = 0, TEST_PRINT("%s:%d " #A "==" #B " in %s [FAIL]\r\n", __FILE__, __LINE__, __func__),0):1)

#define TEST_CLEAR()\
    (test_pass_global = 1, 0)

#define TEST_RESULT()\
    (test_pass_global)
#ifdef __cplusplus
extern "C"
#endif
void cnotify_completion(int fail);

#
#endif /* STACK_REGISTRY_CTEST_ENV_H_ */
