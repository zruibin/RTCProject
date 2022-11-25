/*
 * logging.h
 *
 * Created by Ruibin.Chow on 2022/01/23.
 * Copyright (c) 2022年 Ruibin.Chow All rights reserved.
 */

#ifndef LOG_MANAGER_H
#define LOG_MANAGER_H

#include <cstdio>
#include <string>
#include <sstream>

#define Log(severity) logger::LogMessage(__FILE__, __LINE__, logger::severity)
#define LogOrigin(severity) logger::LogMessage(__FILE__, __LINE__, logger::severity, true)

namespace logger {

extern void SetDefaultLoggerDir(const std::string& defaultLoggerDir);
extern std::string GetDefaultLoggerDir();
extern std::string GetDefaultLoggerFile();

enum LoggingSeverity {
    NONE,
    VERBOSE,
    DEBUG,
    INFO,
    WARNING,
    ERROR,
};

void SetMinWriteLogLevel(LoggingSeverity level);

class LogMessage
{
public:
    typedef LogMessage self;
public:
    LogMessage(const char* file, int line, LoggingSeverity severity, bool origin);
    LogMessage(const char* file, int line, LoggingSeverity severity);
    ~LogMessage();
    
public:
    self& operator<<(bool v) {
        stringBuffer_->append(v ? "1" : "0");
        return *this;
    }
    
    self& operator<<(short i) {
        stringBuffer_->append(std::to_string(i));
        return *this;
    }
    
    self& operator<<(unsigned short si) {
        stringBuffer_->append(std::to_string(si));
        return *this;
    }

    self& operator<<(int i) {
        stringBuffer_->append(std::to_string(i));
        return *this;
    }
    
    self& operator<<(unsigned int i) {
        stringBuffer_->append(std::to_string(i));
        return *this;
    }
    
    self& operator<<(long i) {
        stringBuffer_->append(std::to_string(i));
        return *this;
    }
    
    self& operator<<(unsigned long i) {
        stringBuffer_->append(std::to_string(i));
        return *this;
    }
    
    self& operator<<(long long i) {
        stringBuffer_->append(std::to_string(i));
        return *this;
    }
    
    self& operator<<(unsigned long long i) {
        stringBuffer_->append(std::to_string(i));
        return *this;
    }

    self& operator<<(float v) {
        *this << static_cast<double>(v);
        return *this;
    }
    
    self& operator<<(double d) {
        stringBuffer_->append(std::to_string(d));
        return *this;
    }
    
    self& operator<<(char v) {
        *this << static_cast<short>(v);
        return *this;
    }
    
    self& operator<<(const char* charStr) {
        stringBuffer_->append(charStr);
        return *this;
    }
    
    self& operator<<(const unsigned char* str) {
        return operator<<(reinterpret_cast<const char*>(str));
    }
    
    self& operator<<(const std::string& str) {
        stringBuffer_->append(str);
        return *this;
    }
    
    self& operator<<(const std::string_view str) {
        stringBuffer_->append(str.data(), str.length());
        return *this;
    }

    self& operator<<(void* pVoid) {
        std::ostringstream oss;
        oss << pVoid;
        stringBuffer_->append(oss.str());
//        stringBuffer_->append(*static_cast<std::string*>(pVoid));
        return *this;
    }
    
    self& operator<<(const void* pConstVoid) {
//        void* pChar = const_cast<void*>(pConstVoid);
//        const char* p = static_cast<const char*>(pChar);
//        stringBuffer_->append(p);
        std::ostringstream oss;
        oss << pConstVoid;
        stringBuffer_->append(oss.str());
        return *this;
    }

private:
    std::string* stringBuffer_;
    LoggingSeverity severity_;
    bool origin_ = false;
};


}

#endif /* !LOG_MANAGER_H */
