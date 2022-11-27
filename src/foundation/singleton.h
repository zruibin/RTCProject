/*
 * singleton.h
 *
 * Created by Ruibin.Chow on 2022/11/26.
 * Copyright (c) 2022年 Ruibin.Chow All rights reserved.
 */

#ifndef SINGLETON_H
#define SINGLETON_H

#include <memory>
#include <mutex>

namespace foundation {

template<typename T>
class Singleton {
public:
    virtual ~Singleton() {}
    static std::shared_ptr<T>& SharedInstance() {
        static std::shared_ptr<T> instance_;
        static std::once_flag flag;
        std::call_once(flag, [](){
            instance_.reset(new T);
        });
        return instance_;
    }
};

}

#endif /* !SINGLETON_H */
