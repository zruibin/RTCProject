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

namespace app {

using namespace::core;

class CallObserver : public RTCCallObserverInterface {
    
public:
    void OnCreateSdp(const StringHashMap &sdpDict,
                     const std::string &peerId) override {
        
    }
    
    void OnGenerateIceCandidate(const StringHashMap &sdpDict,
                                const std::string &peerId) override {
        
    }
    
    void OnPeerStateChangge(core::RTCPeerState peerState,
                            const std::string &peerId) override {
        
    }
};


void App::Init() {
    Log(INFO) << "App Init Start.";
    std::unique_ptr<RTCCallObserverInterface> observer = std::make_unique<CallObserver>();
    std::shared_ptr<RTCCallInterface> call = CreateRTCCall(observer.get());
    call->Init();
    Log(INFO) << "App Init End.";
}

}


