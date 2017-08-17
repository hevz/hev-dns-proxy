/*
 * Copyright (C) 2008 The Android Open Source Project
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

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/eventfd.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define LOG_TAG "DnsProxy"
#include "log.h"

#define SOCK_PATH    "/dev/socket/hev-dns-proxy"
#define DNS_SERVER1  "8.8.8.8"
#define DNS_SERVER2  "8.8.4.4"

#include "main.h"
#include "resolv_netid.h"
#include "DnsProxyListener.h"
#include "DnsProxyPolicy.h"

static int event_fd = -1;
static DnsProxyPolicy *dns_proxy_policy;

static void blockSigpipe()
{
    sigset_t mask;

    sigemptyset(&mask);
    sigaddset(&mask, SIGPIPE);
    if (sigprocmask(SIG_BLOCK, &mask, NULL) != 0)
        ALOGW("WARNING: SIGPIPE not blocked\n");
}

static int setup_socket(const char *path)
{
    int fd;
    struct sockaddr_un addr;
    char buf[16];

    fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd < 0) {
        ALOGE("Create dns proxy socket failed!");
        return -1;
    }

    unlink(path);

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, path);
    if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        ALOGE("Bind socket failed!");
        return -1;
    }

    if (chmod(path, 0666) < 0) {
        ALOGE("Change mode for socket failed!");
        return -1;
    }

    snprintf(buf, 16, "%d", fd);
    setenv("ANDROID_SOCKET_dnsproxyd", buf, 1);

    return 0;
}

static void lookup_hostname(const char *hostname)
{
    struct hostent *hp = gethostbyname2(hostname, AF_INET);

    if (hp && hp->h_length) {
        struct in_addr **addr_list = (struct in_addr **)hp->h_addr_list;
        printf("%s\n", inet_ntoa(*addr_list[0]));
    }
}

static void signal_handler(int signum)
{
    quit();
}

void quit(void)
{
    eventfd_write(event_fd, 1);
}

void setup_proxy_uids(unsigned *uids, unsigned count, unsigned last_uid)
{
    if (!dns_proxy_policy) {
        dns_proxy_policy = new DnsProxyPolicy(last_uid + 1);
    }

    dns_proxy_policy->clear();
    for (unsigned i = 0; i < count; i++) {
        dns_proxy_policy->set(uids[i], DNS_PROXY_POLICY_PROXY);
    }
}

void setup_dns(const char *dns1, const char *dns2)
{
    setup_dns_for_net(NETID_UNSET, dns1, dns2);
}

void setup_dns_proxy(const char *dns1, const char *dns2)
{
    // Set local DNS mode, to prevent bionic from proxying
    // back to this service, recursively.
    setenv("ANDROID_DNS_MODE", "local", 1);

    // DNS_PROXY_POLICY_PROXY = 1
    setup_dns_for_net(1, dns1, dns2);
}

void setup_dns_for_net(unsigned netid, const char *dns1, const char *dns2)
{
    const char *servers[] = { dns1, dns2 };

    // clear dns servers
    _resolv_set_nameservers_for_net(netid, NULL, 0, "", NULL);

    // set dns servers
    _resolv_set_nameservers_for_net(netid, servers, 2, "", NULL);

    // clean dns cache
    _resolv_flush_cache_for_net(netid);
}

int main(int argc, char *argv[])
{
    int opt;
    char *sock_path = NULL;
    char *dns1 = NULL;
    char *dns2 = NULL;
    char *hostname = NULL;
    eventfd_t v;

    while ((opt = getopt(argc, argv, "p:a:b:l:")) != -1) {
        switch (opt) {
        case 'p':
            sock_path = optarg;
            break;
        case 'a':
            dns1 = optarg;
            break;
        case 'b':
            dns2 = optarg;
            break;
        case 'l':
            hostname = optarg;
            break;
        default:
            fprintf(stderr, "Usage: %s [-p path] [-a dns1] [-b dns2] "
                "[-l hostname]\n", argv[0]);
            exit(1);
        }
    }

    if (android_libc_resolver_init() < 0) {
        ALOGE("Unable to do libc resolver init(%s)", strerror(errno));
        exit(1);
    }

    // Set local DNS mode, to prevent bionic from proxying
    // back to this service, recursively.
    setenv("ANDROID_DNS_MODE", "local", 1);

    setup_dns(dns1 ? dns1 : DNS_SERVER1, dns2 ? dns2 : DNS_SERVER2);

    // Test mode: Lookup hostname
    if (hostname) {
        setenv("RES_OPTIONS", "debug", 1);
        lookup_hostname(hostname);
        exit(0);
    }

    ALOGI("DnsProxy 1.0 starting");

    event_fd = eventfd(0, 0);
    if (event_fd < 0) {
        ALOGE("Unable to create eventfd(%s)", strerror(errno));
        exit(1);
    }

    blockSigpipe();
    signal(SIGINT, signal_handler);
    signal(SIGQUIT, signal_handler);
    signal(SIGTERM, signal_handler);

    if (setup_socket(sock_path ? sock_path : SOCK_PATH) < 0) {
        ALOGE("Unable to setup socket(%s)", strerror(errno));
        exit(1);
    }

    DnsProxyListener dpl(dns_proxy_policy);
    if (dpl.startListener()) {
        ALOGE("Unable to start DnsProxyListener (%s)", strerror(errno));
        exit(1);
    }

    // Wait quit event
    eventfd_read(event_fd, &v);

    dpl.stopListener();

    ALOGI("DnsProxy exiting");

    exit(0);
}

