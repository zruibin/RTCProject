/*
 * engine_interface.h
 *
 * Created by Ruibin.Chow on 2022/11/27.
 * Copyright (c) 2022年 Ruibin.Chow All rights reserved.
 */

#ifndef ENGINE_INTERFACE_H
#define ENGINE_INTERFACE_H

namespace engine {

class EngineInterface {
    
public:
    static std::shared_ptr<EngineInterface> SharedInstance();

protected:
    explicit EngineInterface() = default;
    
public:
    virtual ~EngineInterface() = default;
    virtual void Init() = 0;
};

}

#endif /* !ENGINE_INTERFACE_H */
