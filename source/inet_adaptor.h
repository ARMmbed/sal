/*
 * SPDX-License-Identifier: Apache-2.0
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
#ifndef SAL_SOURCE_INET_ADAPTOR_H
#define SAL_SOURCE_INET_ADAPTOR_H

#include "sal/socket_types.h"
#include "sal/socket_api.h"

#undef __STRICT_ANSI__

#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>
#include <ctype.h>

#define AF_INET  SOCKET_AF_INET4
#define AF_INET6 SOCKET_AF_INET6

#ifndef u_char
typedef uint8_t u_char;
#endif

#ifndef u_int
typedef unsigned int u_int;
#endif

#ifndef u_long
typedef unsigned long u_long;
#endif

#ifndef in_addr_t
typedef	uint32_t	in_addr_t;
#endif

#endif // SAL_SOURCE_INET_ADAPTOR_H
