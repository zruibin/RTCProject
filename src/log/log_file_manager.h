/*
 * log_file_manager.h
 *
 * Created by Ruibin.Chow on 2022/01/27.
 * Copyright (c) 2022年 Ruibin.Chow All rights reserved.
 */

#ifndef LOG_FILE_MANAGER_H
#define LOG_FILE_MANAGER_H

#include <fstream>

namespace logger {

class LogFileManager
{
public:
    static LogFileManager& GetInstance();
public:
    LogFileManager() = default;
    explicit LogFileManager(const char* fileName);
    ~LogFileManager();
public:
    void Write(const char* string);
    void Flush(void);
    
private:
    std::ofstream *fileStream_;
};

}

#endif /* !LOG_FILE_MANAGER_H */
