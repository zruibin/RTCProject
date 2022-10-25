/*
 * rtc_file_logger.h
 *
 * Created by Ruibin.Chow on 2022/10/25.
 * Copyright (c) 2022年 Ruibin.Chow All rights reserved.
 */

#ifndef RTC_FILE_LOGGER_H
#define RTC_FILE_LOGGER_H

#include <memory>
#include <rtc_base/log_sinks.h>
#include <rtc_base/logging.h>
#include "../rtc_macro.h"

namespace core {

extern RTCString GetDefaultRTCLoggerDir();

enum class RTCFileLoggerSeverity {
    Verbose,
    Info,
    Warning,
    Error
};

class RTCFileLogger {
    
public:
    RTCFileLogger();
    RTCFileLogger(const RTCString& dirPath, uint32_t maxFileSize);
    ~RTCFileLogger();
    
    void SetSeverity(RTCFileLoggerSeverity severity);
    void SetShouldDisableBuffering(bool shouldDisableBuffering);
    void Start();
    void Stop();
    
private:
    rtc::LoggingSeverity rtcSeverity();
    
private:
    RTCFileLoggerSeverity severity_;
    bool shouldDisableBuffering_;
    bool hasStarted_;
    std::unique_ptr<RTCString> dirPath_;
    uint32_t maxFileSize_;
    std::unique_ptr<rtc::FileRotatingLogSink> logSink_;
};


};

#endif /* !RTC_FILE_LOGGER_H */
