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

#ifndef _SOCKET_CLIENT_H
#define _SOCKET_CLIENT_H

#include "List.h"

#include <pthread.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <stdatomic.h>

class SocketClient {
    int             mSocket;
    bool            mSocketOwned;
    pthread_mutex_t mWriteMutex;

    // Peer process ID
    pid_t mPid;

    // Peer user ID
    uid_t mUid;

    // Peer group ID
    gid_t mGid;

    // Reference count (starts at 1)
    pthread_mutex_t mRefCountMutex;
    int mRefCount;

    int mCmdNum;

    bool mUseCmdNum;

public:
    SocketClient(int sock, bool owned);
    SocketClient(int sock, bool owned, bool useCmdNum);
    virtual ~SocketClient();

    int getSocket() { return mSocket; }
    pid_t getPid() const { return mPid; }
    uid_t getUid() const { return mUid; }
    gid_t getGid() const { return mGid; }
    void setCmdNum(int cmdNum) {
        volatile atomic_int_least32_t* a = (volatile atomic_int_least32_t*)&mCmdNum;
        atomic_store_explicit(a, cmdNum, memory_order_release);
    }
    int getCmdNum() { return mCmdNum; }

    // Send null-terminated C strings:
    int sendMsg(int code, const char *msg, bool addErrno);
    int sendMsg(int code, const char *msg, bool addErrno, bool useCmdNum);
    int sendMsg(const char *msg);

    // Provides a mechanism to send a response code to the client.
    // Sends the code and a null character.
    int sendCode(int code);

    // Provides a mechanism to send binary data to client.
    // Sends the code and a null character, followed by 4 bytes of
    // big-endian length, and the data.
    int sendBinaryMsg(int code, const void *data, int len);

    // Sending binary data:
    int sendData(const void *data, int len);
    // iovec contents not preserved through call
    int sendDatav(struct iovec *iov, int iovcnt);

    // Optional reference counting.  Reference count starts at 1.  If
    // it's decremented to 0, it deletes itself.
    // SocketListener creates a SocketClient (at refcount 1) and calls
    // decRef() when it's done with the client.
    void incRef();
    bool decRef(); // returns true at 0 (but note: SocketClient already deleted)

    // return a new string in quotes with '\\' and '\"' escaped for "my arg"
    // transmissions
    static char *quoteArg(const char *arg);

private:
    void init(int socket, bool owned, bool useCmdNum);

    // Sending binary data. The caller should make sure this is protected
    // from multiple threads entering simultaneously.
    // returns 0 if successful, -1 if there is a 0 byte write or if any
    // other error occurred (use errno to get the error)
    int sendDataLockedv(struct iovec *iov, int iovcnt);
};

typedef android::sysutils::List<SocketClient *> SocketClientCollection;
#endif
