/*
 *
 * timer.cc
 *
 * Created by Ruibin.Chow on 2022/10/23.
 * Copyright (c) 2022年 Ruibin.Chow All rights reserved.
 */

#include "util/timer.h"
#include <chrono>
#include <cstdint>
#include <chrono>
#include <memory>

/*
 参考：https://blog.csdn.net/hiwubihe/article/details/84206235
 */

namespace util {

Timer::Timer(const TimerEvent& event, int64_t ns, bool repeat, void* pUser)
        : eventCallback_(event)
        , isRepeat_(repeat)
        , interval_(ns)
        , pUser_(pUser) {
    if (interval_ == 0)
        interval_ = 1;
}

void Timer::Sleep(int64_t ns) {
    std::this_thread::sleep_for(std::chrono::nanoseconds(ns));
}

bool Timer::IsRepeat() const {
    return isRepeat_;
}

void Timer::SetEventCallback(const TimerEvent& event) {
    eventCallback_ = event;
}

void Timer::SetEventData(void* pUser) {
    pUser_ = pUser;
}

void Timer::Start(int64_t ns, bool repeat) {
    using namespace std::chrono;
    isRepeat_ = repeat;
    auto timeBegin = high_resolution_clock::now();
    int64_t elapsed = 0;
    
    do {
        std::this_thread::sleep_for(nanoseconds(ns - elapsed));
        timeBegin = high_resolution_clock::now();
        eventCallback_(pUser_);
        elapsed = duration_cast<nanoseconds>(high_resolution_clock::now() - timeBegin).count();
        if (elapsed < 0)
            elapsed = 0;
    } while (isRepeat_);
}

void Timer::Stop() {
    isRepeat_ = false;
}

void Timer::SetNextTimeout(int64_t currentTimePoint) {
    nextTimeout_ = currentTimePoint + interval_;
}

int64_t Timer::GetNextTimeout() const {
    return nextTimeout_;
}

/*------------------------------------------------------------------------------*/

TimerId TimerQueue::AddTimer(const TimerEvent& event, int64_t ns, bool repeat, void* pUser) {
    std::lock_guard<std::mutex> locker(mutex_);
    
    int64_t timeoutPoint = GetTimeNow();
    TimerId timerId = {timeoutPoint+ns, ++lastTimerId_};
    auto timer = std::make_shared<Timer>(event, ns, repeat, pUser);
    
    timer->SetNextTimeout(timeoutPoint);
    
    if(repeat) {
        //c++11 中添加emplace 和insert 区别时，插入不需要构造临时变量
        //参考下面 insert
        repeatTimers_.emplace(timerId.second, timer);
    }
    
    timers_.insert(std::pair<TimerId,std::shared_ptr<Timer>>(timerId, std::move(timer)));
    
    return timerId;
}

void TimerQueue::RemoveTimer(TimerId timerId) {
    std::lock_guard<std::mutex> locker(mutex_);
    
    auto iter = repeatTimers_.find(timerId.second);
    if (iter != repeatTimers_.end()) {
        TimerId t = {iter->second->GetNextTimeout(), timerId.second};
        repeatTimers_.erase(iter);
        timers_.erase(t);
    } else {
        timers_.erase(timerId);
    }
}

int64_t TimerQueue::GetTimeNow() {
    auto timePoint = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(timePoint.time_since_epoch()).count();
}

int64_t TimerQueue::GetTimeRemaining() {
    std::lock_guard<std::mutex> locker(mutex_);
    
    if(timers_.empty())
        return -1;
    
    //因为MAP会自动排序 所以每次所有定时任务 都是按照时间先后排列好的
    int64_t ns = timers_.begin()->first.first - GetTimeNow();
    if(ns <= 0)
        return 0;
    
    return ns;
}

void TimerQueue::HandleTimerEvent() {
    if(!timers_.empty() || !repeatTimers_.empty()) {
        std::lock_guard<std::mutex> locker(mutex_);
        
        int64_t timePoint = GetTimeNow();
        while(!timers_.empty() && timers_.begin()->first.first<=timePoint) {
            timers_.begin()->second->eventCallback_(timers_.begin()->second->pUser_);
            if(timers_.begin()->second->IsRepeat()) {
                timers_.begin()->second->SetNextTimeout(timePoint);
                TimerId t = {timers_.begin()->second->GetNextTimeout(), timers_.begin()->first.second};
                auto timerPtr = std::move(timers_.begin()->second);
                timers_.erase(timers_.begin());
                timers_.insert(std::pair<TimerId, std::shared_ptr<Timer>>(t, std::move(timerPtr)));
                //timers_.insert(std::pair<TimerId, std::shared_ptr<Timer>>(timerId, std::move(timer)));
            } else {
                timers_.erase(timers_.begin());
            }
        }
    }
}

/*------------------------------------------------------------------------------*/

AsynTimer::~AsynTimer() {
    if (t_) delete t_;
}

AsynTimer AsynTimer::Detach(const TimerEvent& event,
                            int64_t ns,
                            bool repeat,
                            void* pUser) {
    AsynTimer asynTimer;
    asynTimer.Start(event, ns, repeat, pUser);
    return asynTimer;
}

void AsynTimer::Start(const TimerEvent& event,
                      int64_t ns,
                      bool repeat,
                      void* pUser) {
    timer_.SetEventCallback(event);
    timer_.SetEventData(pUser);
    t_ = new std::thread(&Timer::Start, timer_, ns, repeat);
    t_->detach();
    isRuning_ = true;
}

void AsynTimer::Stop() {
    timer_.Stop();
    isRuning_ = false;
}

bool AsynTimer::IsRunable() const {
    return isRuning_;
}

}

