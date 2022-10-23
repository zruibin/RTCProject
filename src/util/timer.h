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
#include <chrono>
#include <functional>
#include <cstdint>
#include <chrono>
#include <memory>
#include <mutex>
#include <thread>

namespace util {
    
using TimerEvent = std::function<void(void*)>;
using TimerId = std::pair<int64_t, uint32_t>;
 
class Timer {

public:
    Timer() = default;
    virtual  ~Timer() = default;
    
    /// 定时器
    /// @param event 定时器执行内容
    /// @param ms 毫秒
    /// @param repeat 是否重复
    /// @param pUser 用户数据
    Timer(const TimerEvent& event, uint32_t ms, bool repeat, void* pUser);
    
    /// 睡眠
    /// @param ms 毫秒
    void Sleep(unsigned ms);
 
    /// 是否重复
    bool IsRepeat() const;
    
    /// 设置执行事件内容
    /// @param event 事件内容
    void SetEventCallback(const TimerEvent& event);
    
    /// 设置事件执行数据
    /// @param event 事件内容
    void SetEventData(void* pUser);
    
    /// 开始
    /// @param ms 毫秒
    void Start(int64_t ms, bool repeat = false);
    
    /// 停止
    void Stop();
 
private:
    friend class TimerQueue;
 
    void SetNextTimeout(int64_t currentTimePoint);
    int64_t GetNextTimeout() const;
 
    //初始化为C++11—lambda函数
    TimerEvent eventCallback_ = [](void*){};
    bool isRepeat_ = false;
    uint32_t interval_ = 0;
    int64_t nextTimeout_ = 0;
    void* pUser_;
};

/*------------------------------------------------------------------------------*/
 
class TimerQueue
{
public:
    /// 添加定时器
    /// @param event 定时器执行内容
    /// @param ms 毫秒
    /// @param repeat 是否重复
    /// @param pUser 用户数据
    TimerId AddTimer(const TimerEvent& event, uint32_t ms, bool repeat, void* pUser);
    
    /// 删除定时器
    /// @param timerId 定时器id
    void RemoveTimer(TimerId timerId);
 
    // 返回最近一次超时的时间, 没有定时器任务返回-1
    int64_t GetTimeRemaining();
    void HandleTimerEvent();
 
private:
    int64_t GetTimeNow();
 
    std::mutex mutex_;
    //添加定时器 MAP会自动排序 依据TimerId->first
    std::map<TimerId, std::shared_ptr<Timer>> timers_;
    //重复执行定时器列表 unordered_map 采用HASH处理 查找非常快
    std::unordered_map<uint32_t, std::shared_ptr<Timer>> repeatTimers_;
    uint32_t lastTimerId_ = 0;
};

/*------------------------------------------------------------------------------*/

class AsynTimer final {

public:
    AsynTimer() = default;
    
    /// 自动异步执行定时器
    /// @param event 定时器执行内容
    /// @param ms 毫秒
    /// @param repeat 是否重复
    /// @param pUser 用户数据
    static AsynTimer Detach(const TimerEvent& event, uint32_t ms, bool repeat, void* pUser);
    
    /// 开始异步执行定时器
    /// @param event 定时器执行内容
    /// @param ms 毫秒
    /// @param repeat 是否重复
    /// @param pUser 用户数据
    void Start(const TimerEvent& event, uint32_t ms, bool repeat, void* pUser);
    
    /// 停止
    void Stop();

private:
    Timer timer_;
};

}
 
#endif /* !TIMER_H */
