/*
 *
 * platform.cc
 *
 * Created by Ruibin.Chow on 2022/01/25.
 * Copyright (c) 2022年 Ruibin.Chow All rights reserved.
 */

#include "platform/platform.h"
#include <pthread.h>
#include <sstream>
#include <thread>

namespace platform {

int thread_set_name(const char* name) {
#if defined(__APPLE__)
    return pthread_setname_np(name);
#else
    return pthread_setname_np(pthread_self(), name);
#endif
}

const char* thread_get_current_name() {
    constexpr size_t length = 256;
    char threadName[length];
    pthread_getname_np(pthread_self(), threadName, length);
    return threadName;
}

UINT16 thread_get_current_id(void) {
    std::ostringstream oss;
    oss << std::hash<std::thread::id>()(std::this_thread::get_id());
    unsigned short tid = std::stoull(oss.str());
    return tid;
}

}
