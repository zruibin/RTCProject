/*
 *
 * util.cc
 *
 * Created by Ruibin.Chow on 2022/01/27.
 * Copyright (c) 2022年 Ruibin.Chow All rights reserved.
 */

#include "util/util.h"
#include <chrono>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <random>


namespace util {

std::string get_current_timestamp_string(int time_stamp_type) {
    using namespace std::chrono;
    system_clock::time_point now = system_clock::now();
    std::time_t now_time_t = system_clock::to_time_t(now);
    std::tm* now_tm = std::localtime(&now_time_t);

    char buffer[128];
    strftime(buffer, sizeof(buffer), "%F %T", now_tm);

    std::ostringstream ss;
    ss.fill('0');

    milliseconds ms;
    microseconds cs;
    nanoseconds ns;
    
    switch (time_stamp_type) {
        case 0:
            ss << buffer;
            break;
        case 1:
            ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;
            ss << buffer << "." << std::setw(3) << std::setfill('0') << ms.count();
            break;
        case 2:
            ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;
            cs = duration_cast<microseconds>(now.time_since_epoch()) % 1000000;
            ss << buffer << "." << std::setw(3) << std::setfill('0') << ms.count()
                         << "." << std::setw(3) << std::setfill('0') << cs.count() % 1000;
            break;
        case 3:
            ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;
            cs = duration_cast<microseconds>(now.time_since_epoch()) % 1000000;
            ns = duration_cast<nanoseconds>(now.time_since_epoch()) % 1000000000;
            ss << buffer << "." << std::setw(3) << std::setfill('0') << ms.count()
                         << "." << std::setw(3) << std::setfill('0') << cs.count() % 1000
                         << "." << std::setw(3) << std::setfill('0') << ns.count() % 1000;
            break;
        default:
            ss << buffer;
            break;
    }

    return ss.str();
}

std::string get_current_time_string(void) {
    return get_current_timestamp_string(2);
}

int64_t get_current_time_seconds(void) {
    using namespace std::chrono;
    auto sec_since_epoch = duration_cast<seconds>(system_clock::now().time_since_epoch()).count();
    
    return sec_since_epoch;
}

int64_t get_current_time_milliseconds(void) {
    using namespace std::chrono;
    auto millisec_since_epoch = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    return millisec_since_epoch;
}


std::string get_random_string(int32_t len) {
    std::string charSet = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    std::string randomString;
    
    using namespace std::chrono;
    std::default_random_engine engine((int64_t)steady_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<int> dist(0, charSet.length() - 1);

    for (int32_t i = 0; i < len; ++i) {
        int32_t randomPoz = dist(engine);
        randomString += charSet.substr(randomPoz, 1);
    }
    return randomString;
}

}

