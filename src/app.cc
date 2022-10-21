/*
 *
 * app.cc
 *
 * Created by Ruibin.Chow on 2022/10/20.
 * Copyright (c) 2022年 Ruibin.Chow All rights reserved.
 */

#include "app.h"
#include <iostream>
#include "core/rtc_call_interface.h"
#include "log/logging.h"

namespace engine {

using namespace::engine;

class CallObserver : public RTCCallObserverInterface {
    
};

void App::Init() {
    Log(INFO) << "App Init Start.";
    std::unique_ptr<RTCCallObserverInterface> observer = std::make_unique<RTCCallObserverInterface>();
    std::shared_ptr<RTCCallInterface> call = CreateRTCCall(observer.get());
    call->Init();
    Log(INFO) << "App Init End.";
}

}


