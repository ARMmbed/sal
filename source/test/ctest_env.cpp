/*
 * ctest_env.cpp
 *
 *  Created on: 20 Mar 2015
 *      Author: bremor01
 */
#include "ctest_env.h"
#include <mbed/test_env.h>
void cnotify_completion(int fail) {
    notify_completion(fail != 0);
}

