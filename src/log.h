/*
 * Copyright (C) 2005 The Android Open Source Project
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

//
// Templated list class.  Normally we'd use STL, but we don't have that.
// This class mimics STL's interfaces.
//
// Objects are copied into the list with the '=' operator or with copy-
// construction, so if the compiler's auto-generated versions won't work for
// you, define your own.
//
//

#ifndef _LOG_H
#define _LOG_H

#include <android/log.h>

#define SLOG_ENABLE 0

#define __android_slog_print(...)              \
    {                                          \
        if (SLOG_ENABLE) {                     \
            __android_log_print (__VA_ARGS__); \
        }                                      \
    }

#ifndef ALOGI
#define ALOGI(...) \
    (__android_log_print (ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__))
#endif

#ifndef ALOGW
#define ALOGW(...) \
    (__android_log_print (ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__))
#endif

#ifndef ALOGD
#define ALOGD(...) \
    (__android_log_print (ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__))
#endif

#ifndef ALOGE
#define ALOGE(...) \
    (__android_log_print (ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__))
#endif

#ifndef SLOGV
#define SLOGV(...) \
    (__android_slog_print (ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__))
#endif

#ifndef SLOGW
#define SLOGW(...) \
    (__android_slog_print (ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__))
#endif

#ifndef SLOGE
#define SLOGE(...) \
    (__android_slog_print (ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__))
#endif

#endif // _LOG_H
