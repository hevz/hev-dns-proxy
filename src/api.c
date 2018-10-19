/*
 * Copyright (C) 2017 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdlib.h>
#include <limits.h>
#include <errno.h>
#include <sys/system_properties.h>
#include <android/api-level.h>

#include "api.h"

static int api_level;

int
android_getapilevel (void)
{
    if (!api_level) {
        char buf[16];
        long int val;

        __system_property_get ("ro.build.version.sdk", buf);
        val = strtol (buf, NULL, 10);
        if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN)) ||
            (errno != 0 && val == 0)) {
            api_level = __ANDROID_API__;
        } else {
            api_level = (int)val;
        }
    }

    return api_level;
}
