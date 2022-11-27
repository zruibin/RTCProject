/*
 *
 * engine.cc
 *
 * Created by Ruibin.Chow on 2022/11/27.
 * Copyright (c) 2022年 Ruibin.Chow All rights reserved.
 */

#include "engine/engine.h"
#include <mutex>

namespace engine {

std::shared_ptr<Engine>& Engine::SharedInstance() {
    static std::shared_ptr<Engine> instance_;
    static std::once_flag flag;
    std::call_once(flag, [](){
        instance_.reset(new Engine);
    });
    return instance_;
}

Engine::Engine() {
    
}

void Engine::Init() {
    
}

}

