/*
 ============================================================================
 Name        : jni.c
 Author      : Heiher <r@hev.cc>
 Copyright   : Copyright (c) 2017 everyone.
 Description : JNI
 ============================================================================
 */

#include <jni.h>
#include <pthread.h>

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/system_properties.h>

#include "main.h"

/* clang-format off */
#ifndef PKGNAME
#define PKGNAME hev/htproxy
#endif
/* clang-format on */

#define STR(s) STR_ARG (s)
#define STR_ARG(c) #c
#define N_ELEMENTS(arr) (sizeof (arr) / sizeof ((arr)[0]))

static JavaVM *java_vm;
static pthread_t work_thread;
static pthread_key_t current_jni_env;

static void native_start_service (JNIEnv *env, jobject thiz,
                                  jstring conig_path);
static void native_stop_service (JNIEnv *env, jobject thiz);
static void native_reset_resolver (JNIEnv *env, jobject thiz);
static void native_set_resolver_proxy (JNIEnv *env, jobject thiz, jstring dns1,
                                       jstring dns2);
static void native_set_proxy_uids (JNIEnv *env, jobject thiz, jintArray uids,
                                   jint last_uid);

static JNINativeMethod native_methods[] = {
    { "DnsProxyStartService", "(Ljava/lang/String;)V",
      (void *)native_start_service },
    { "DnsProxyStopService", "()V", (void *)native_stop_service },
    { "DnsProxyResetResolver", "()V", (void *)native_reset_resolver },
    { "DnsProxySetResolverProxy", "(Ljava/lang/String;Ljava/lang/String;)V",
      (void *)native_set_resolver_proxy },
    { "DnsProxySetProxyUids", "([II)V", (void *)native_set_proxy_uids },
};

static void
detach_current_thread (void *env)
{
    (*java_vm)->DetachCurrentThread (java_vm);
}

jint
JNI_OnLoad (JavaVM *vm, void *reserved)
{
    JNIEnv *env = NULL;
    jclass klass;

    java_vm = vm;
    if (JNI_OK != (*vm)->GetEnv (vm, (void **)&env, JNI_VERSION_1_4)) {
        return 0;
    }

    klass = (*env)->FindClass (env, STR (PKGNAME) "/TProxyService");
    (*env)->RegisterNatives (env, klass, native_methods,
                             N_ELEMENTS (native_methods));
    (*env)->DeleteLocalRef (env, klass);

    pthread_key_create (&current_jni_env, detach_current_thread);

    return JNI_VERSION_1_4;
}

static void *
thread_handler (void *data)
{
    char **argv = data;

    argv[0] = "hev-dns-proxy";

    main (7, argv);

    free (argv[1]);
    free (argv[2]);
    free (argv[3]);
    free (argv[4]);
    free (argv[5]);
    free (argv[6]);
    free (argv);

    return NULL;
}

static void
native_start_service (JNIEnv *env, jobject thiz, jstring socket_path)
{
    char **argv;
    const jbyte *bytes;
    char dns1[PROP_VALUE_MAX];
    char dns2[PROP_VALUE_MAX];

    argv = malloc (sizeof (char *) * 7);

    argv[1] = strdup ("-p");
    bytes = (const jbyte *)(*env)->GetStringUTFChars (env, socket_path, NULL);
    argv[2] = strdup ((const char *)bytes);
    (*env)->ReleaseStringUTFChars (env, socket_path, (const char *)bytes);

    __system_property_get ("net.dns1", dns1);
    __system_property_get ("net.dns2", dns2);

    argv[3] = strdup ("-a");
    argv[4] = strdup (dns1);
    argv[5] = strdup ("-b");
    argv[6] = strdup (dns2);

    pthread_create (&work_thread, NULL, thread_handler, argv);
}

static void
native_stop_service (JNIEnv *env, jobject thiz)
{
    if (!work_thread) {
        return;
    }

    quit ();
    pthread_join (work_thread, NULL);
    work_thread = 0;
}

static void
native_reset_resolver (JNIEnv *env, jobject thiz)
{
    char dns1[PROP_VALUE_MAX];
    char dns2[PROP_VALUE_MAX];

    __system_property_get ("net.dns1", dns1);
    __system_property_get ("net.dns2", dns2);

    setup_dns (dns1, dns2);
}

static void
native_set_resolver_proxy (JNIEnv *env, jobject thiz, jstring dns1,
                           jstring dns2)
{
    const jbyte *bytes1, *bytes2;

    bytes1 = (const jbyte *)(*env)->GetStringUTFChars (env, dns1, NULL);
    bytes2 = (const jbyte *)(*env)->GetStringUTFChars (env, dns2, NULL);

    setup_dns_proxy ((const char *)bytes1, (const char *)bytes2);

    (*env)->ReleaseStringUTFChars (env, dns1, (const char *)bytes1);
    (*env)->ReleaseStringUTFChars (env, dns2, (const char *)bytes2);
}

static void
native_set_proxy_uids (JNIEnv *env, jobject thiz, jintArray uids, jint last_uid)
{
    jint *_uids;
    jsize i, uids_length;

    _uids = (*env)->GetIntArrayElements (env, uids, NULL);
    uids_length = (*env)->GetArrayLength (env, uids);

    setup_proxy_uids ((unsigned *)_uids, uids_length, last_uid);

    (*env)->ReleaseIntArrayElements (env, uids, _uids, 0);
}
