/*
 *
 * log_file_manager.cc
 *
 * Created by Ruibin.Chow on 2022/01/27.
 * Copyright (c) 2022年 Ruibin.Chow All rights reserved.
 */

#include "log_file_manager.h"
#include <memory>
#include <mutex>
#include <thread>


namespace log {

LogFileManager& LogFileManager::GetInstance() {
    static LogFileManager *instance = nullptr;
    static std::once_flag flag;
    std::call_once(flag, []() {
        if (instance == nullptr) {
            instance = new LogFileManager("log.log");
        }
    });
    return *instance;
}

LogFileManager::LogFileManager(const char* fileName) {
    fileStream_ = new std::ofstream(fileName, std::ios::out|std::ios::app);
}

LogFileManager::~LogFileManager() {
    if (fileStream_ != nullptr) {
        fileStream_->close();
        delete fileStream_;
    }
}

void LogFileManager::Write(const char* string) {
    *fileStream_ << string;
    fileStream_->flush();
}

void LogFileManager::Flush(void) {
    
}

}

