/*
 * util.h
 *
 * Created by Ruibin.Chow on 2022/01/27.
 * Copyright (c) 2022年 Ruibin.Chow All rights reserved.
 */

#ifndef UTIL_H
#define UTIL_H

#include <string>


namespace util {

std::string get_current_timestamp_string(int time_stamp_type = 0);
std::string get_current_time_string(void);
int64_t get_current_time_seconds(void);
int64_t get_current_time_milliseconds(void);

std::string get_random_string(int32_t len);

}


#endif /* !UTIL_H */
