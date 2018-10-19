/*
 ============================================================================
 Name        : libc_resolver.c
 Author      : Heiher <r@hev.cc>
 Copyright   : Copyright (c) 2017 everyone.
 Description : Libc Resolver
 ============================================================================
 */

#include <dlfcn.h>

#define LOG_TAG "LibcResolver"
#include "log.h"

#include "resolv_netid.h"

typedef struct hostent *(*GetHostByAddrForNet) (const void *addr, socklen_t len,
                                                int af, unsigned netid,
                                                unsigned mark);
typedef struct hostent *(*GetHostByNameForNet) (const char *name, int af,
                                                unsigned netid, unsigned mark);
typedef int (*GetAddrInfoForNet) (const char *hostname, const char *servname,
                                  const struct addrinfo *hints, unsigned netid,
                                  unsigned mark, struct addrinfo **res);

static GetHostByAddrForNet _gethostbyaddrfornet;
static GetHostByNameForNet _gethostbynamefornet;
static GetAddrInfoForNet _getaddrinfofornet;

int
android_libc_resolver_init (void)
{
    _gethostbyaddrfornet = dlsym (RTLD_DEFAULT, "android_gethostbyaddrfornet");
    if (!_gethostbyaddrfornet) {
        ALOGE ("Resolve android_gethostbyaddrfornet failed!");
        return -1;
    }

    _gethostbynamefornet = dlsym (RTLD_DEFAULT, "android_gethostbynamefornet");
    if (!_gethostbynamefornet) {
        ALOGE ("Resolve android_gethostbynamefornet failed!");
        return -1;
    }

    _getaddrinfofornet = dlsym (RTLD_DEFAULT, "android_getaddrinfofornet");
    if (!_getaddrinfofornet) {
        ALOGE ("Resolve android_getaddrinfofornet failed!");
        return -1;
    }

    return 0;
}

struct hostent *
android_gethostbyaddrfornet (const void *addr, socklen_t len, int af,
                             unsigned netid, unsigned mark)
{
    if (!_gethostbyaddrfornet) {
        ALOGE ("Please run android_libc_resolver_init first!");
        return NULL;
    }

    return _gethostbyaddrfornet (addr, len, af, netid, mark);
}

struct hostent *
android_gethostbynamefornet (const char *name, int af, unsigned netid,
                             unsigned mark)
{
    if (!_gethostbynamefornet) {
        ALOGE ("Please run android_libc_resolver_init first!");
        return NULL;
    }

    return _gethostbynamefornet (name, af, netid, mark);
}

int
android_getaddrinfofornet (const char *hostname, const char *servname,
                           const struct addrinfo *hints, unsigned netid,
                           unsigned mark, struct addrinfo **res)
{
    if (!_getaddrinfofornet) {
        ALOGE ("Please run android_libc_resolver_init first!");
        return -1;
    }

    return _getaddrinfofornet (hostname, servname, hints, netid, mark, res);
}
