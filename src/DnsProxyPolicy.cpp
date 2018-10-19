/*
 ============================================================================
 Name        : DnsProxyPolicy.c
 Author      : Heiher <r@hev.cc>
 Copyright   : Copyright (c) 2017 everyone.
 Description : Dns proxy policy
 ============================================================================
 */

#include <string.h>

#define LOG_TAG "DnsProxyPolicy"
#include "log.h"

#include "DnsProxyPolicy.h"

DnsProxyPolicy::DnsProxyPolicy (unsigned length)
{
    mLength = length;
    mBuffer = new unsigned char[length];
    clear ();
}

DnsProxyPolicy::~DnsProxyPolicy ()
{
    delete mBuffer;
}

unsigned char
DnsProxyPolicy::get (unsigned id) const
{
    return mBuffer[id];
}

void
DnsProxyPolicy::set (unsigned id, unsigned char policy)
{
    mBuffer[id] = policy;
}

void
DnsProxyPolicy::clear ()
{
    memset (mBuffer, DNS_PROXY_POLICY_NONE, mLength);
}
