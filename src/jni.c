/*
 ============================================================================
 Name        : hev-jni.c
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

#ifndef PKGNAME
#define PKGNAME    hev/htproxy
#endif

#define STR(s)    STR_ARG(s)
#define STR_ARG(c)    #c
#define N_ELEMENTS(arr)        (sizeof (arr) / sizeof ((arr)[0]))

static JavaVM *java_vm;
static pthread_t work_thread;
static pthread_key_t current_jni_env;

static void native_start_service (JNIEnv *env, jobject thiz, jstring conig_path);
static void native_stop_service (JNIEnv *env, jobject thiz);
static void native_reset_resolver (JNIEnv *env, jobject thiz);

static JNINativeMethod native_methods[] =
{
    { "DnsProxyStartService", "(Ljava/lang/String;)V", (void *) native_start_service },
    { "DnsProxyStopService", "()V", (void *) native_stop_service },
    { "DnsProxyResetResolver", "()V", (void *) native_reset_resolver },
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
    if (JNI_OK != (*vm)->GetEnv (vm, (void**) &env, JNI_VERSION_1_4)) {
        return 0;
    }

    klass = (*env)->FindClass (env, STR(PKGNAME)"/TProxyService");
    (*env)->RegisterNatives (env, klass, native_methods, N_ELEMENTS (native_methods));
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
    work_thread = 0;

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
    bytes = (const signed char *)(*env)->GetStringUTFChars (env, socket_path, NULL);
    argv[2] = strdup ((const char *)bytes);
    (*env)->ReleaseStringUTFChars (env, socket_path, (const char *)bytes);

    __system_property_get("net.dns1", dns1);
    __system_property_get("net.dns2", dns2);

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
}

static void
native_reset_resolver (JNIEnv *env, jobject thiz)
{
    char dns1[PROP_VALUE_MAX];
    char dns2[PROP_VALUE_MAX];

    __system_property_get("net.dns1", dns1);
    __system_property_get("net.dns2", dns2);

    setup_dns(dns1, dns2);
}

