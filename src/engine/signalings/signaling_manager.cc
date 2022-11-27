/*
 *
 * signaling_manager.cc
 *
 * Created by Ruibin.Chow on 2022/11/21.
 * Copyright (c) 2022年 Ruibin.Chow All rights reserved.
 */

#include "engine/signalings/signaling_manager.h"
#include "util/timer.h"
#include "network/socket_factory.h"
#include <magic_enum/magic_enum.hpp>
#include "foundation/notification_center.h"

namespace engine {

namespace  {
    using namespace foundation;
};

void testSocket() {
    NotificationMethod method = NotificationMethod::ProducerScore;
    Log(INFO) << "Notification Name: " << EnumMethodToString<NotificationMethod>(method);
    Log(INFO) << "Notification method: "
                << EnumMethodToString<NotificationMethod>(
                        EnumMethodFromString<NotificationMethod>("activeSpeaker"));
    
    RequestMethod method1 = RequestMethod::GetTransportStats;
    Log(INFO) << "Request Name: " << EnumMethodToString<RequestMethod>(method1);
    Log(INFO) << "Request method: "
                << EnumMethodToString<RequestMethod>(
                        EnumMethodFromString<RequestMethod>("newConsumer"));
    
    NotificationCenter::DefaultCenter()->AddNotification("testSignaling", [](NotificationRef notiRef) {
        Log(DEBUG) << "Notification:" << notiRef->message;
    });
    SignalingManager::SharedInstance()->GetRouterRtpCapabilities([](int32_t code,
                                                                    const std::string& msg,
                                                                    std::shared_ptr<GetRouterRtpCapabilitiesResponse> response) {
        Log(DEBUG) << "GetRouterRtpCapabilities:" << response->ToJsonString();
    });
    
    /*

    using namespace network;
    std::string url = "ws://localhost:8001/?roomId=123221&peerId=ffdsds";//"ws://localhost:9806";
    std::shared_ptr<SocketInterface> socket = CreateSocket(url,
                                                           SocketInterface::Protocol::kWS);
    
    socket->Open();
    util::Timer::Sleep(3*TIME_NSEC_PER_SEC);
    std::string data = "{\"method\":\"getRouterRtpCapabilities\", \"id\":3244342, \"request\": true}";
    socket->Send(data.c_str(), data.length(), SocketInterface::FrameType::kText);
    util::Timer::Sleep(10*TIME_NSEC_PER_SEC);
    socket->Close();
    util::Timer::Sleep(3*TIME_NSEC_PER_SEC);
    //*/
}

SignalingManager::SignalingManager() {
    signalingSocketRef_.reset(new SignalingSocket);
}

void SignalingManager::Init() {
    
}

void SignalingManager::GetRouterRtpCapabilities(
    std::function<void(int32_t code,
                       const std::string& msg,
                       std::shared_ptr<GetRouterRtpCapabilitiesResponse> response)> handler) {
    GetRouterRtpCapabilitiesRequest request;
    Send<GetRouterRtpCapabilitiesRequest, GetRouterRtpCapabilitiesResponse>(request, handler);
}



}

