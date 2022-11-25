/*
 *
 * log_manager.cc
 *
 * Created by Ruibin.Chow on 2022/01/23.
 * Copyright (c) 2022年 Ruibin.Chow All rights reserved.
 */

#include "logging.h"
#include <iostream>
#include <sstream>
#include "platform/platform.h"
#include "util/util.h"
#include "log/log_file_manager.h"


namespace logger {

static LoggingSeverity minWriteLevel = INFO;

void SetMinWriteLogLevel(LoggingSeverity level) {
    minWriteLevel = level;
}

static const char* loggingSeverityCover(LoggingSeverity severity) {
    static const char* severityList[] = {
        [VERBOSE] = "[V]",
        [INFO] = "[I]",
        [DEBUG] = "[D]",
        [WARNING] = "[W]",
        [ERROR] = "[E]",
        [NONE] = "[N]",
    };
    
    return severityList[severity];
}

LogMessage::LogMessage(const char* file, int line, LoggingSeverity severity, bool origin)
        : stringBuffer_(new std::string), origin_(origin) {
    severity_ = severity;
    std::string fileStr(file);
    stringBuffer_->append(util::get_current_time_string());
    stringBuffer_->append(" [");
    stringBuffer_->append(std::to_string(platform::thread_get_current_id()));
    stringBuffer_->append("]");
    if (!origin_) {
        stringBuffer_->append("[");
        stringBuffer_->append(fileStr.substr(fileStr.find_last_of("/")+1));
        stringBuffer_->append(":");
        stringBuffer_->append(std::to_string(line));
        stringBuffer_->append("]");
        stringBuffer_->append(loggingSeverityCover(severity));
        stringBuffer_->append(" ");
    }
}

LogMessage::LogMessage(const char* file, int line, LoggingSeverity severity)
        : LogMessage(file, line, severity, false) {}

LogMessage::~LogMessage() {
    if (!origin_) {
        stringBuffer_->append("\n");
    }
    std::cout << stringBuffer_->c_str();
    if (severity_ >= minWriteLevel) {
        LogFileManager::GetInstance().Write(stringBuffer_->c_str());
    }
    delete stringBuffer_;
}


}
