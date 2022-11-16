/*
 *
 * socket.cc
 *
 * Created by Ruibin.Chow on 2022/11/16.
 * Copyright (c) 2022年 Ruibin.Chow All rights reserved.
 */

#include "socket.h"
#include "log/logging.h"

namespace network {

const char kProtocolWS[] = "ws";
const char kProtocolWSS[] = "wss";
const char kProtocolKWS[] = "kws";
const char kProtocolKWSS[] = "kwss";

const int kConnnectTotalTimeout = 60 * 1000;
const int kConnnectTimeout = 10 * 1000;
const int kConnnectShortTimeout = 5 * 1000;
const int kCloseTimeout = 2 * 1000;
const int kReconnectInterval = 1 * 1000;
const int kHeartbeatPingInterval = 15 * 1000;
const int kPongTimeout = 20 * 1000;
const int kBadRTT = 500;


const char* SocketInterface::ProtocolToString(Protocol protocol) {
    switch (protocol) {
        case Protocol::kWS:
            return kProtocolWS;
        case Protocol::kWSS:
            return kProtocolWSS;
        case Protocol::kKWS:
            return kProtocolKWS;
        case Protocol::kKWSS:
            return kProtocolKWSS;
        default:
            return "";
    }
}

const char* SocketInterface::StateToString(State state) {
    switch (state) {
        case State::kIdle:
            return "Idle";
        case State::kPrepared:
            return "Prepared";
        case State::kConnecting:
            return "Connecting";
        case State::kConnected:
            return "Connected";
        case State::kClosing:
            return "Closing";
        case State::kClosed:
            return "Closed";
        default:
            return "";
    }
}

const char* SocketInterface::ErrorToString(Error error) {
    switch (error) {
        case Error::kNone:
            return "None";
        case Error::kUnknow:
            return "Unknow";
        case Error::kMemory:
            return "Memory";
        case Error::kNetwork:
            return "Network";
        case Error::kCrypto:
            return "Crypto";
        case Error::kProtocol:
            return "Protocol";
        default:
            return "";
    }
}

void Socket::SetGetOpenParamHandler(GetOpenParamHandler getOpenParamHandler) {
    getOpenParamHandler_ = getOpenParamHandler;
}

void Socket::SetConnectStateChangedHandler(ConnectStateChangedHandler connectStateChangedHandler) {
    connectStateChangedHandler_ = connectStateChangedHandler;
}

void Socket::SetFailedHandler(FailedHandler failedHandler) {
    failedHandler_ = failedHandler;
}

void Socket::SetReceivedFrameHandler(ReceivedFrameHandler receivedFrameHandler) {
    receivedFrameHandler_ = receivedFrameHandler;
}

void Socket::SetState(State newState) {
    if (state_ == newState) {
        return;
    }
    
    Log(INFO) << "set state: " << StateToString(state_) << " -> " << StateToString(newState);
    bool oldConnected = IsConnected();
    state_ = newState;
    
    if (oldConnected != IsConnected() && connectStateChangedHandler_) {
        connectStateChangedHandler_(IsConnected(), GetProtocol(), "", 0);
    }
}

void Socket::TriggerError(Error error, const std::string& reason) {
    error_ = error;
    if (failedHandler_) {
        failedHandler_(error);
    }
}

}

