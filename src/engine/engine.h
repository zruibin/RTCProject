/*
 * engine.h
 *
 * Created by Ruibin.Chow on 2022/11/27.
 * Copyright (c) 2022年 Ruibin.Chow All rights reserved.
 */

#ifndef ENGINE_H
#define ENGINE_H

#include <memory>
#include "engine/engine_interface.h"

namespace engine {

class Engine : public EngineInterface {
    
public:
    ~Engine();
    static std::shared_ptr<Engine>& SharedInstance();
    void Init() override;
    
protected:
    explicit Engine();
    Engine(const Engine&) = delete;
    Engine& operator=(const Engine&) = delete;
    Engine(Engine&&) = delete;
    Engine& operator=(Engine&&) = delete;
};

}

#endif /* !ENGINE_H */
