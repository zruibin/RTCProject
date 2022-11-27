/*
 * signaling_socket.h
 *
 * Created by Ruibin.Chow on 2022/11/26.
 * Copyright (c) 2022年 Ruibin.Chow All rights reserved.
 */

#ifndef SIGNALING_SOCKET_H
#define SIGNALING_SOCKET_H

#include <functional>
#include <string>
#include <mutex>
#include <unordered_map>
#include "network/socket_factory.h"

namespace engine {

namespace {
    using namespace network;
}

using SuccessCallback = std::function<void(const std::string& json)>;
using FailureCallback = std::function<void(int32_t errorCode,
                                           const std::string& errorInfo)>;
using StateChangeHandler = std::function<void(bool connected)>;
using FailedHandler = std::function<void(SocketInterface::Error error,
                                         const std::string& reason)>;

struct SignalingRequest final {
    int64_t id = -1;
    std::string text;
    SuccessCallback resolve;
    FailureCallback reject;
};


class SignalingSocket final {

public:
    explicit SignalingSocket() = default;
    ~SignalingSocket();
    void Init(const std::string& url);
    void Open();
    void Close();
    bool IsConnected();
    
    void SetStateChangeHandler(StateChangeHandler stateChangeHandler) {
        stateChangeHandler_ = stateChangeHandler;
    }
    void SetFailedHandler(FailedHandler failedHandler) {
        failedHandler_ = failedHandler;
    }
    
    void Send(const std::string& text,
              int64_t transcation,
              SuccessCallback scb,
              FailureCallback fcb);
private:
    void OnConnectStateChanged(bool connected);
    void OnFailed(SocketInterface::Error code,
                  const std::string& reason);
    void OnMessage(const std::string& message);
    
    void HandleRequest(const std::string& message);
    void HandleResponse(const std::string& message);
    void HandleNotification(const std::string& message);
    void ClearRequests();
    
private:
    std::mutex requestMutex_;
    std::unordered_map<int64_t, std::shared_ptr<SignalingRequest>> requestMap_;
    std::shared_ptr<SocketInterface> socket_;
    
    StateChangeHandler stateChangeHandler_;
    FailedHandler failedHandler_;
};

}


#endif /* !SIGNALING_SOCKET_H */
