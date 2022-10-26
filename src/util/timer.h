/*
 * timer.h
 *
 * Created by Ruibin.Chow on 2022/10/23.
 * Copyright (c) 2022年 Ruibin.Chow All rights reserved.
 */

#ifndef TIMER_H
#define TIMER_H

#include <map>
#include <unordered_map>
#include <functional>
#include <mutex>
#include <thread>

/// SEC:秒,PER:每, NSEC:纳秒, MSEC:毫秒, USEC:微秒
#define TIME_NSEC_PER_SEC 1000000000ull // 多少纳秒 = 1秒, 1秒 = 10亿纳秒
#define TIME_NSEC_PER_MSEC 1000000ull   // 多少纳秒 = 1毫秒, 1毫秒 = 100万纳秒
#define TIME_USEC_PER_SEC 1000000ull    // 多少微秒 = 1秒, 1秒 = 100万微秒
#define TIME_NSEC_PER_USEC 1000ull      // 多少纳秒 = 1微秒, 1微秒 = 1000 纳秒

namespace util {
    
using TimerEvent = std::function<void(void*)>;
using TimerId = std::pair<int64_t, uint32_t>;
 
class Timer {

public:
    Timer() = default;
    virtual ~Timer() = default;
    
    /// 定时器
    /// @param event 定时器执行内容
    /// @param ns 纳秒
    /// @param repeat 是否重复
    /// @param pUser 用户数据
    Timer(const TimerEvent& event, int64_t ns, bool repeat, void* pUser);
    
    /// 睡眠
    /// @param ns 纳秒
    static void Sleep(int64_t ns);
 
    /// 是否重复
    bool IsRepeat() const;
    
    /// 设置执行事件内容
    /// @param event 事件内容
    void SetEventCallback(const TimerEvent& event);
    
    /// 设置事件执行数据
    /// @param event 事件内容
    void SetEventData(void* pUser);
    
    /// 开始
    /// @param ns 纳秒
    void Start(int64_t ns, bool repeat = false);
    
    /// 停止
    void Stop();
 
private:
    friend class TimerQueue;
 
    void SetNextTimeout(int64_t currentTimePoint);
    int64_t GetNextTimeout() const;
 
    //初始化为C++11—lambda函数
    TimerEvent eventCallback_ = [](void*){};
    bool isRepeat_ = false;
    int64_t interval_ = 0;
    int64_t nextTimeout_ = 0;
    void* pUser_;
};

/*------------------------------------------------------------------------------*/
 
class TimerQueue
{
public:
    /// 添加定时器
    /// @param event 定时器执行内容
    /// @param ms 纳秒
    /// @param repeat 是否重复
    /// @param pUser 用户数据
    TimerId AddTimer(const TimerEvent& event, int64_t ns, bool repeat, void* pUser);
    
    /// 删除定时器
    /// @param timerId 定时器id
    void RemoveTimer(TimerId timerId);
 
    /// 返回最近一次超时的时间, 没有定时器任务返回-1
    int64_t GetTimeRemaining();
    void HandleTimerEvent();
 
private:
    int64_t GetTimeNow();
 
    std::mutex mutex_;
    /// 添加定时器 MAP会自动排序 依据TimerId->first
    std::map<TimerId, std::shared_ptr<Timer>> timers_;
    /// 重复执行定时器列表 unordered_map 采用HASH处理 查找非常快
    std::unordered_map<uint32_t, std::shared_ptr<Timer>> repeatTimers_;
    uint32_t lastTimerId_ = 0;
};

/*------------------------------------------------------------------------------*/

class AsynTimer final {

public:
    AsynTimer() = default;
    ~AsynTimer();
    
    /// 自动异步执行定时器
    /// @param event 定时器执行内容
    /// @param ns 纳秒
    /// @param repeat 是否重复
    /// @param pUser 用户数据
    static AsynTimer Detach(const TimerEvent& event, int64_t ns, bool repeat, void* pUser);
    
    /// 开始异步执行定时器
    /// @param event 定时器执行内容
    /// @param ns 纳秒
    /// @param repeat 是否重复
    /// @param pUser 用户数据
    void Start(const TimerEvent& event, int64_t ns, bool repeat, void* pUser);
    
    /// 停止
    void Stop();
    
    /// 是否执行中
    bool IsRunable() const;

private:
    Timer timer_;
    std::thread* t_ = nullptr;
    bool isRuning_ = false;
};

}
 
#endif /* !TIMER_H */
