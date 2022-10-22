/*
 *
 * util.cc
 *
 * Created by Ruibin.Chow on 2022/01/27.
 * Copyright (c) 2022年 Ruibin.Chow All rights reserved.
 */

#include "util.h"
#include <chrono>
#include <iomanip>
#include <sstream>
#include <ctime>


namespace util {

std::string get_current_time_string(void) {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    ss << '.' << std::setw(3) << std::setfill('0') << ms.count();
    return std::move(ss.str());
}

long get_current_time_seconds(void) {
    using std::chrono::duration_cast;
    using std::chrono::seconds;
    using std::chrono::system_clock;
    auto sec_since_epoch = duration_cast<seconds>(system_clock::now().time_since_epoch()).count();
    
    return sec_since_epoch;
}

long get_current_time_milliseconds(void) {
    using std::chrono::duration_cast;
    using std::chrono::milliseconds;
    using std::chrono::system_clock;
    auto millisec_since_epoch = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    return millisec_since_epoch;
}

}

