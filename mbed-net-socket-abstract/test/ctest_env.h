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

#define TEST_PRINT(...) printf(__VA_ARGS__)
#define TEST_EQ(A,B)\
    (tests_total_global++,((A!=B)?(test_pass_global = 0, tests_failed_global++, TEST_PRINT("%s:%d " #A "!=" #B " [FAIL]\r\n", __func__, __LINE__),0):1))
#define TEST_NEQ(A,B)\
    (tests_total_global++,((A==B)?(test_pass_global = 0, tests_failed_global++, TEST_PRINT("%s:%d " #A "==" #B " [FAIL]\r\n", __func__, __LINE__),0):1))

#define TEST_CLEAR()\
    (test_pass_global = 1, tests_failed_global = 0, tests_total_global = 0, 0)

#define TEST_RESULT()\
    (test_pass_global)

#define TEST_EXIT()\
    do {\
        goto test_exit; \
    } while(0);
#define TEST_RETURN()\
    do {\
        TEST_PRINT("%s [%s] (%d/%d FAILED)\r\n", __func__, (TEST_RESULT()?"PASS":"FAIL"), tests_failed_global, tests_total_global);\
        return TEST_RESULT();\
    }while (0)

static int test_pass_global = 1;
static unsigned tests_failed_global = 0;
static unsigned tests_total_global = 0;

#endif /* STACK_REGISTRY_CTEST_ENV_H_ */
