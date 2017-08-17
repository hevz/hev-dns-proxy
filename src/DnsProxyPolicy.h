/*
 ============================================================================
 Name        : DnsProxyPolicy.h
 Author      : Heiher <r@hev.cc>
 Copyright   : Copyright (c) 2017 everyone.
 Description : Dns proxy policy
 ============================================================================
 */

#ifndef __DNS_PROXY_POLICY_H
#define __DNS_PROXY_POLICY_H

enum
{
    DNS_PROXY_POLICY_NONE = 0,
    DNS_PROXY_POLICY_PROXY = 1,
};

class DnsProxyPolicy {
private:
    unsigned mLength;
    unsigned char *mBuffer;

public:

    DnsProxyPolicy(unsigned length);
    ~DnsProxyPolicy();

    unsigned char get(unsigned id) const;
    void set(unsigned id, unsigned char policy);

    void clear();
};

#endif
