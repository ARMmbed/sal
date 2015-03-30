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

#ifndef __MBED_NET_SOCKET_ABSTRACT_TEST_SAL_TEST_API_H__
#define __MBED_NET_SOCKET_ABSTRACT_TEST_SAL_TEST_API_H__
#include <mbed-net-socket-abstract/socket_types.h>
#ifdef __cplusplus
extern "C" {
#endif

int socket_api_test_create_destroy(socket_stack_t stack, socket_address_family_t disable_family);


#ifdef __cplusplus
}
#endif

#endif