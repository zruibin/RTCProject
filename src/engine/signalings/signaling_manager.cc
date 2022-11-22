/*
 *
 * signaling_manager.cc
 *
 * Created by Ruibin.Chow on 2022/11/21.
 * Copyright (c) 2022年 Ruibin.Chow All rights reserved.
 */

#include "engine/signalings/signaling_manager.h"
#include "engine/signalings/signaling.h"
#include "log/logging.h"
#include "util/timer.h"
#include "network/socket_factory.h"

namespace engine {

void testSocket() {
    MessageType messageType;
    messageType.FromJsonString(testSignaling);
    if (messageType.request.has_value()) {
        RequestHeader reqestHaeder;
        reqestHaeder.FromJsonString(testSignaling);
        Log(VERBOSE) << "messageType was request: " << reqestHaeder.method.value();
    } else if (messageType.response.has_value()) {
        ResponseHeader responseHeader;
        responseHeader.FromJsonString(testSignaling);
        Log(VERBOSE) << "messageType was response: " << responseHeader.id.value();
        if (responseHeader.ok.value()) {
            GetRouterRtpCapabilitiesResponse response;
            response.FromJsonString(testSignaling);
            Log(DEBUG) << "response: " <<response.ToJsonString();
        }
    } else if (messageType.notification.has_value()) {
        NotificationHeader notificationHeader;
        notificationHeader.FromJsonString(testSignaling);
        Log(VERBOSE) << "messageType was notification: " << notificationHeader.method.value();
    } else {
        Log(VERBOSE) << "messageType error.";
    }
    
    /*
    using namespace network;
    std::string url = "ws://localhost:8001/?roomId=123221&peerId=ffdsds";//"ws://localhost:9806";
    std::shared_ptr<SocketInterface> socket = CreateSocket(url,
                                                           SocketInterface::Protocol::kWS);
    socket->SetConnectStateChangedHandler([](bool connected,
                                             SocketInterface::Protocol protocol,
                                             const std::string& networkName,
                                             int networkType) {
        Log(DEBUG) << "SetConnectStateChangedHandler: " << connected;
    });
    socket->SetFailedHandler([](SocketInterface::Error code,
                                const std::string& reason) {
        Log(DEBUG) << "SetFailedHandler: " << SocketInterface::ErrorToString(code)
                    << ", reason: " << reason;
    });
    socket->SetReceivedFrameHandler([](const char* buf,
                                       int len,
                                       SocketInterface::FrameType frameType) {
        std::string str(buf, len);
        GetRouterRtpCapabilitiesResponse response;
        response.FromJsonString(str);
        Log(DEBUG) << "ReceivedFrameHandler: " << str;
        Log(DEBUG) << "response: " <<response.ToJsonString();
    });
    socket->SetSubProtocol("protoo");
    socket->Open();
    util::Timer::Sleep(3*TIME_NSEC_PER_SEC);
    std::string data = "{\"method\":\"getRouterRtpCapabilities\", \"id\":3244342, \"request\": true}";
    socket->Send(data.c_str(), data.length(), SocketInterface::FrameType::kText);
    util::Timer::Sleep(10*TIME_NSEC_PER_SEC);
    socket->Close();
    util::Timer::Sleep(3*TIME_NSEC_PER_SEC);
    //*/
}


}

