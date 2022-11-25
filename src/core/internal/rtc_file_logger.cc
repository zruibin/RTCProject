/*
 *
 * rtc_file_logger.cc
 *
 * Created by Ruibin.Chow on 2022/10/25.
 * Copyright (c) 2022年 Ruibin.Chow All rights reserved.
 */

#include "core/internal/rtc_file_logger.h"
#include <filesystem>
#include "log/logging.h"

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

RTCString RTCLoggingSeverityToString(rtc::LoggingSeverity severity) {
    RTCString severityString;
    switch (severity) {
        case rtc::LS_VERBOSE:
            severityString = "[V]";
            break;
        case rtc::LS_INFO:
            severityString = "[I]";
            break;
        case rtc::LS_WARNING:
            severityString = "[W]";
            break;
        case rtc::LS_ERROR:
            severityString = "[E]";
            break;
        default:
            severityString = "[N]";
            break;
    }
    return severityString;
}

void RTCLogSink::OnLogMessage(const std::string& message) {}

void RTCLogSink::OnLogMessage(const std::string& message,
                              rtc::LoggingSeverity severity) {
    LogOrigin(INFO) << "[RTC]" << RTCLoggingSeverityToString(severity)
                    << message;
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
    maxFileSize_ = maxFileSize * 1024 * 1024;
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
    if (throwback_) {
        rtcLogSink_.reset(new RTCLogSink());
        rtc::LogMessage::AddLogToStream(rtcLogSink_.get(), rtcSeverity());
        rtc::LogMessage::LogToDebug(rtc::LS_NONE); 
    } else {
        logSink_.reset(new rtc::CallSessionFileRotatingLogSink(dirPath_.get()->c_str(), maxFileSize_));
        if (!logSink_->Init()) {
            RTC_LOG(LS_ERROR) << "Failed to open log files at path: " << dirPath_;
            logSink_.reset();
            return;
        }
        if (shouldDisableBuffering_) {
            logSink_->DisableBuffering();
        }
        rtc::LogMessage::AddLogToStream(logSink_.get(), rtcSeverity());
    }
    rtc::LogMessage::LogThreads(true);
    rtc::LogMessage::LogTimestamps(true);
    rtc::LogMessage::SetLogToStderr(true);
    hasStarted_ = true;
}

void RTCFileLogger::Stop() {
    if (!hasStarted_) {
        return;
    }
    if (throwback_) {
        if (rtcLogSink_) {
            RTC_DCHECK(rtcLogSink_);
            rtc::LogMessage::RemoveLogToStream(rtcLogSink_.get());
            rtcLogSink_.reset();
        }
    } else {
        if (logSink_) {
            RTC_DCHECK(logSink_);
            rtc::LogMessage::RemoveLogToStream(logSink_.get());
            logSink_.reset();
        }
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
        default:
            return rtc::LS_NONE;
    }
}

}


