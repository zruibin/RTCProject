/*
 * socket.h
 *
 * Created by Ruibin.Chow on 2022/11/16.
 * Copyright (c) 2022年 Ruibin.Chow All rights reserved.
 */

#ifndef SOCKET_H
#define SOCKET_H

#include "socket_interface.h"
#include <asio/io_context.hpp>

namespace network {

extern const char kProtocolWS[];
extern const char kProtocolWSS[];
extern const char kProtocolKWS[];
extern const char kProtocolKWSS[];

extern const int kConnnectTotalTimeout;
extern const int kConnnectTimeout;
extern const int kConnnectShortTimeout;
extern const int kCloseTimeout;
extern const int kReconnectInterval;
extern const int kHeartbeatPingInterval;
extern const int kPongTimeout;
extern const int kBadRTT;

//using InitedHandler = std::function<void(const asio::ip::address&)>;

class Socket : public SocketInterface {
    
public:
    Socket() = default;
    virtual ~Socket() = default;
    
    void SetGetOpenParamHandler(GetOpenParamHandler getOpenParamHandler) override;
    void SetConnectStateChangedHandler(ConnectStateChangedHandler connectStateChangedHandler) override;
    void SetFailedHandler(FailedHandler failedHandler) override;
    void SetReceivedFrameHandler(ReceivedFrameHandler receivedFrameHandler) override;
    
    void SetProtocol(Protocol protocol) override { protocol_ = protocol;};
    Protocol GetProtocol() const override { return protocol_; };
    void SetSubProtocol(const std::string& subProtocol) override { subProtocol_ = subProtocol;};
    
    State GetState() override { return state_; };
    Error GetError() override { return error_; };
    bool IsConnected() override { return state_ == State::kConnected; };
    
    void SetState(State newState);
    virtual void TriggerError(Error error, const std::string& reason);

public:
    State state_ = State::kIdle;
    Error error_ = Error::kNone;
    Protocol protocol_;
    std::string subProtocol_;
    
    GetOpenParamHandler getOpenParamHandler_ = nullptr;
//    InitedHandler initedHandler_ = nullptr;
    ConnectStateChangedHandler connectStateChangedHandler_ = nullptr;
    FailedHandler failedHandler_ = nullptr;
    ReceivedFrameHandler receivedFrameHandler_ = nullptr;
    
    int connectTimeout_ = kConnnectTimeout;
    int pongTimeout_ = kPongTimeout;
};


}


#endif /* !SOCKET_H */
