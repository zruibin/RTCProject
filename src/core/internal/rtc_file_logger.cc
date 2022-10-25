/*
 *
 * rtc_file_logger.cc
 *
 * Created by Ruibin.Chow on 2022/10/25.
 * Copyright (c) 2022年 Ruibin.Chow All rights reserved.
 */

#include "rtc_file_logger.h"
#include <filesystem>

namespace core {

static const char kDefaultLogDirName[] = "webrtc_log";
static uint32_t const kDefaultMaxFileSize = 20 * 1024 * 1024; // 20MB.

RTCString GetDefaultRTCLoggerDir() {
    namespace fs = std::filesystem;
    fs::path dst = fs::current_path() / RTCString(kDefaultLogDirName);
    if (!fs::exists(dst)) {
        fs::create_directory(dst);
    }
    return dst.string();
}

RTCFileLogger::RTCFileLogger() : RTCFileLogger("", kDefaultMaxFileSize) {

}

RTCFileLogger::RTCFileLogger(const RTCString& dirPath,
                             uint32_t maxFileSize) {
    if (dirPath.length() > 0) {
        dirPath_ = std::make_unique<RTCString>(dirPath);
    } else {
        dirPath_ = std::make_unique<RTCString>(GetDefaultRTCLoggerDir());
    }
    maxFileSize_ = maxFileSize;
    SetSeverity(RTCFileLoggerSeverity::Info);
}

RTCFileLogger::~RTCFileLogger() {
    Stop();
}

void RTCFileLogger::SetSeverity(RTCFileLoggerSeverity severity) {
    severity_ = severity;
    rtc::LogMessage::LogToDebug(rtcSeverity());
}

void RTCFileLogger::SetShouldDisableBuffering(bool shouldDisableBuffering) {
    shouldDisableBuffering_ = shouldDisableBuffering;
}

void RTCFileLogger::Start() {
    if (hasStarted_) {
        return;
    }
    logSink_.reset(new rtc::CallSessionFileRotatingLogSink(dirPath_.get()->c_str(), maxFileSize_));
    if (!logSink_->Init()) {
        RTC_LOG(LS_ERROR) << "Failed to open log files at path: " << dirPath_;
        logSink_.reset();
        return;
    }
    if (shouldDisableBuffering_) {
        logSink_->DisableBuffering();
    }
    rtc::LogMessage::LogThreads(true);
    rtc::LogMessage::LogTimestamps(true);
    rtc::LogMessage::AddLogToStream(logSink_.get(), rtcSeverity());
    hasStarted_ = true;
}

void RTCFileLogger::Stop() {
    if (!hasStarted_) {
        return;
    }
    if (logSink_) {
        RTC_DCHECK(logSink_);
        rtc::LogMessage::RemoveLogToStream(logSink_.get());
        logSink_.reset();
    }
    hasStarted_ = false;
}

rtc::LoggingSeverity RTCFileLogger::rtcSeverity() {
    switch (severity_) {
        case RTCFileLoggerSeverity::Verbose:
            return rtc::LS_VERBOSE;
        case RTCFileLoggerSeverity::Info:
            return rtc::LS_INFO;
        case RTCFileLoggerSeverity::Warning:
            return rtc::LS_WARNING;
        case RTCFileLoggerSeverity::Error:
            return rtc::LS_ERROR;
    }
}

}


