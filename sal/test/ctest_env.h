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

#ifndef MBED_NET_SOCKET_ABSTRACT_TEST_CTEST_ENV_H_
#define MBED_NET_SOCKET_ABSTRACT_TEST_CTEST_ENV_H_

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
        TEST_PRINT("{{summary %s [%s] (%d/%d FAILED)}}\r\n", __func__, (TEST_RESULT()?"PASS":"FAIL"), tests_failed_global, tests_total_global);\
        return TEST_RESULT();\
    }while (0)

volatile static int test_pass_global = 1;
volatile static unsigned tests_failed_global = 0;
volatile static unsigned tests_total_global = 0;

#endif /* MBED_NET_SOCKET_ABSTRACT_TEST_CTEST_ENV_H_ */
