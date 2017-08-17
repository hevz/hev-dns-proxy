/*
 * Copyright (C) 2014 The Android Open Source Project
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
#ifndef _RESOLV_NETID_H
#define _RESOLV_NETID_H

/* This header contains declarations related to per-network DNS
 * server selection. They are used by system/netd/ and should not be
 * exposed by the C library's public NDK headers.
 */
#include <sys/cdefs.h>
#include <netinet/in.h>
#include <stdio.h>
#include <netdb.h>

/*
 * Passing NETID_UNSET as the netId causes system/netd/server/DnsProxyListener.cpp to
 * fill in the appropriate default netId for the query.
 */
#define NETID_UNSET 0u

/*
 * MARK_UNSET represents the default (i.e. unset) value for a socket mark.
 */
#define MARK_UNSET 0u

__BEGIN_DECLS

struct __res_params;
struct addrinfo;

int android_libc_resolver_init(void);

struct hostent * android_gethostbyaddrfornet(const void *addr, socklen_t len, int af,
        unsigned netid, unsigned mark);

struct hostent * android_gethostbynamefornet(const char *name, int af, unsigned netid, unsigned mark);

int android_getaddrinfofornet(const char *hostname, const char *servname,
        const struct addrinfo *hints, unsigned netid, unsigned mark,
        struct addrinfo **res);

/* set name servers for a network */
extern int _resolv_set_nameservers_for_net(unsigned netid, const char** servers,
        unsigned numservers, const char *domains, const struct __res_params* params);

/* flush the cache associated with a certain network */
extern void _resolv_flush_cache_for_net(unsigned netid);

/* delete the cache associated with a certain network */
extern void _resolv_delete_cache_for_net(unsigned netid);

/* delete the cache associated with a certain network */
extern void _resolv_delete_cache_for_net(unsigned netid);

__END_DECLS

#endif /* _RESOLV_NETID_H */
