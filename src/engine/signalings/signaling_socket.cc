/*
 *
 * signaling_socket.cc
 *
 * Created by Ruibin.Chow on 2022/11/26.
 * Copyright (c) 2022年 Ruibin.Chow All rights reserved.
 */

#include "signaling_socket.h"
#include "engine/signalings/signaling.h"
#include "log/logging.h"
#include "foundation/notification_center.h"

namespace engine {

SignalingSocket::~SignalingSocket() {
    ClearRequests();
}

void SignalingSocket::Init(const std::string& url) {
    ClearRequests();
    socket_ = CreateSocket(url, SocketInterface::Protocol::kWS);
    socket_->SetConnectStateChangedHandler([this](bool connected,
                                                  SocketInterface::Protocol protocol,
                                                  const std::string& networkName,
                                                  int networkType) {
        OnConnectStateChanged(connected);
    });
    socket_->SetFailedHandler([this](SocketInterface::Error code,
                                     const std::string& reason) {
        OnFailed(code, reason);
    });
    socket_->SetReceivedFrameHandler([this](const char* buf,
                                            int len,
                                            SocketInterface::FrameType frameType) {
        std::string message(buf, len);
        OnMessage(message);
    });
    socket_->SetSubProtocol("protoo");
}

void SignalingSocket::Open() {
    socket_->Open();
}

void SignalingSocket::Close() {
    socket_->Close();
}

void SignalingSocket::Send(const std::string& text,
                           int64_t transcation,
                           SuccessCallback scb,
                           FailureCallback fcb) {
    if (!text.empty()) {
        auto request = std::make_shared<SignalingRequest>();
        request->text = text;
        request->id = transcation;
        request->resolve = scb;
        request->reject = fcb;
        {
            std::lock_guard<std::mutex> locker(requestMutex_);
            requestMap_[request->id] = request;
        }
        // sending message.
        if (socket_ != nullptr) {
            socket_->Send(text.c_str(), text.length(), SocketInterface::FrameType::kText);
        }
    }
    
    // test
    std::shared_ptr<SignalingRequest> request;
    {
        std::lock_guard<std::mutex> locker(requestMutex_);
        
        if (requestMap_.find(transcation) != requestMap_.end()) {
            request = requestMap_[transcation];
            requestMap_.erase(transcation);
        }
    }
    if (request) {
        request->resolve(testSignaling);
        using namespace foundation;
        NotificationRef notification = std::make_shared<Notification>();
        notification->name = "testSignaling";
        notification->message = testSignaling;
        notification->object = nullptr;
        NotificationCenter::DefaultCenter()->PostNotification("testSignaling", notification);
    }
}

void SignalingSocket::OnConnectStateChanged(bool connected) {
    Log(DEBUG) << "OnConnectStateChanged: " << connected;
}

void SignalingSocket::OnFailed(SocketInterface::Error code,
                               const std::string& reason) {
    Log(DEBUG) << "OnFailed: " << SocketInterface::ErrorToString(code)
                << ", reason: " << reason;
}

void SignalingSocket::OnMessage(const std::string& message) {
    Log(DEBUG) << "OnMessage: " << message;
    
    MessageType messageType;
    messageType.FromJsonString(message);
    if (messageType.request.has_value()) {
        HandleRequest(message);
    } else if (messageType.response.has_value()) {
        HandleResponse(message);
    } else if (messageType.notification.has_value()) {
        HandleNotification(message);
    } else {
        Log(ERROR) << "message error.";
    }
}

void SignalingSocket::HandleRequest(const std::string& message) {
    if (!message.empty()) {
        return;
    }
    RequestHeader header;
    header.FromJsonString(message);
    int64_t id = header.id.value();
    if (!header.request.value()) {
        Log(ERROR) << "RequestHeader Was Error, id:" << id;
        return;
    }
    Log(VERBOSE) << "message was request: " << header.method.value();
}

void SignalingSocket::HandleResponse(const std::string& message) {
    if (!message.empty()) {
        return;
    }
    ResponseHeader header;
    header.FromJsonString(message);
    int64_t id = header.id.value();
    if (!header.ok.value()) {
        Log(ERROR) << "ResponseHeader Was Error, id:" << id;
        return;
    }
    
    std::shared_ptr<SignalingRequest> request;
    {
        std::lock_guard<std::mutex> locker(requestMutex_);
        
        if (requestMap_.find(id) != requestMap_.end()) {
            request = requestMap_[id];
            requestMap_.erase(id);
        }
    }
    if (request) {
        request->resolve(testSignaling);
    }
}

void SignalingSocket::HandleNotification(const std::string& message) {
    if (!message.empty()) {
        return;
    }
    NotificationHeader header;
    header.FromJsonString(message);
    if (!header.notification.value()) {
        Log(ERROR) << "ResponseHeader Was Error, method:" << header.method.value();
        return;
    }
    NotificationMethod method = EnumMethodFromString<NotificationMethod>(header.method.value());
    
    using namespace foundation;
    NotificationRef notification = std::make_shared<Notification>();
    notification->name = header.method.value();
    notification->message = message;
    notification->object = nullptr;
    NotificationCenter::DefaultCenter()->PostNotification(header.method.value(), notification);
}

void SignalingSocket::ClearRequests() {
    std::lock_guard<std::mutex> locker(requestMutex_);
    requestMap_.clear();
}

}
