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
#include "core/rtc_macro.h"

namespace core {

extern RTCString GetDefaultRTCLoggerDir();


class RTCLogSink : public rtc::LogSink {

public:
    RTCLogSink() = default;
    ~RTCLogSink() = default;
    void OnLogMessage(const std::string& message) override;
    void OnLogMessage(const std::string& message,
                      rtc::LoggingSeverity severity) override;
};


enum class RTCFileLoggerSeverity {
    Verbose,
    Info,
    Warning,
    Error
};

class RTCFileLogger {
    
public:
    RTCFileLogger();
    /// dirPath: 日志输出路径; maxFileSize: 单个文件最大值，单位MB.
    RTCFileLogger(const RTCString& dirPath, uint32_t maxFileSize);
    ~RTCFileLogger();
    
    void SetRTCLogThrowback(bool throwback) { throwback_ = throwback; }
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
    std::unique_ptr<RTCLogSink> rtcLogSink_;
    bool throwback_ = false;
};


};

#endif /* !RTC_FILE_LOGGER_H */
