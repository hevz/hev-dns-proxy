# Copyright (C) 2017 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

TOP_PATH := $(call my-dir)

LOCAL_PATH = $(TOP_PATH)
include $(CLEAR_VARS)
LOCAL_MODULE    := hev-dns-proxy-library
LOCAL_SRC_FILES := \
	src/DnsProxyListener.cpp \
	src/DnsProxyPolicy.cpp \
	src/FrameworkClient.cpp \
	src/FrameworkCommand.cpp \
	src/FrameworkListener.cpp \
	src/NetdCommand.cpp \
	src/SocketClient.cpp \
	src/SocketListener.cpp \
	src/libc_resolver.c \
	src/main.cpp

ifeq ($(TARGET_ARCH_ABI),armeabi-v7a)
LOCAL_CPPFLAGS += -mfpu=neon
endif
include $(BUILD_STATIC_LIBRARY)

LOCAL_PATH = $(TOP_PATH)
include $(CLEAR_VARS)
LOCAL_MODULE    := hev-dns-proxy-jni
LOCAL_SRC_FILES := \
	src/jni.c

ifeq ($(TARGET_ARCH_ABI),armeabi-v7a)
LOCAL_CPPFLAGS += -mfpu=neon
endif
LOCAL_STATIC_LIBRARIES := hev-dns-proxy-library
LOCAL_LDLIBS := -llog
include $(BUILD_SHARED_LIBRARY)

LOCAL_PATH = $(TOP_PATH)
include $(CLEAR_VARS)
LOCAL_MODULE    := hev-dns-proxy

ifeq ($(TARGET_ARCH_ABI),armeabi-v7a)
LOCAL_CPPFLAGS += -mfpu=neon
endif
LOCAL_STATIC_LIBRARIES := hev-dns-proxy-library
LOCAL_LDLIBS := -llog
include $(BUILD_EXECUTABLE)

