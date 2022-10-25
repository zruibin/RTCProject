/*
 *
 * rtc_util.cc
 *
 * Created by Ruibin.Chow on 2022/10/25.
 * Copyright (c) 2022年 Ruibin.Chow All rights reserved.
 */

#include "rtc_util.h"

namespace core {

RTCString
RTCStringByIceConnectionState(PeerConnectionInterface::IceConnectionState new_state)
{
    switch (new_state) {
        case PeerConnectionInterface::IceConnectionState::kIceConnectionNew:
            return "New";
        case PeerConnectionInterface::IceConnectionState::kIceConnectionChecking:
            return "Checking";
        case PeerConnectionInterface::IceConnectionState::kIceConnectionConnected:
            return "Connected";
        case PeerConnectionInterface::IceConnectionState::kIceConnectionCompleted:
            return "Completed";
        case PeerConnectionInterface::IceConnectionState::kIceConnectionFailed:
            return "Failed";
        case PeerConnectionInterface::IceConnectionState::kIceConnectionDisconnected:
            return "Disconnected";
        case PeerConnectionInterface::IceConnectionState::kIceConnectionClosed:
            return "Closed";
        case PeerConnectionInterface::IceConnectionState::kIceConnectionMax:
            return "Count";
        default:
            return "Unknown";
    }
}

};

