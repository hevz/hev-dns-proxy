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

#ifndef _FRAMEWORK_CLIENT_H
#define _FRAMEWORK_CLIENT_H

#include "List.h"

#include <pthread.h>

class FrameworkClient
{
    int mSocket;
    pthread_mutex_t mWriteMutex;

public:
    FrameworkClient (int sock);
    virtual ~FrameworkClient ()
    {
    }

    int sendMsg (const char *msg);
    int sendMsg (const char *msg, const char *data);
};

typedef android::sysutils::List<FrameworkClient *> FrameworkClientCollection;
#endif
