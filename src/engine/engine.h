/*
 * engine.h
 *
 * Created by Ruibin.Chow on 2022/11/27.
 * Copyright (c) 2022年 Ruibin.Chow All rights reserved.
 */

#ifndef ENGINE_H
#define ENGINE_H

#include <memory>

namespace engine {

class Engine final {
    
public:
    ~Engine();
    static std::shared_ptr<Engine>& SharedInstance();
    void Init();
    
protected:
    explicit Engine();
    
};

}

#endif /* !ENGINE_H */
