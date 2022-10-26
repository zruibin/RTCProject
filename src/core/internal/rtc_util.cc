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
RTCStringByIceConnectionState(RTCIceConnectionState new_state)
{
    switch (new_state) {
        case RTCIceConnectionState::kIceConnectionNew:
            return "New";
        case RTCIceConnectionState::kIceConnectionChecking:
            return "Checking";
        case RTCIceConnectionState::kIceConnectionConnected:
            return "Connected";
        case RTCIceConnectionState::kIceConnectionCompleted:
            return "Completed";
        case RTCIceConnectionState::kIceConnectionFailed:
            return "Failed";
        case RTCIceConnectionState::kIceConnectionDisconnected:
            return "Disconnected";
        case RTCIceConnectionState::kIceConnectionClosed:
            return "Closed";
        case RTCIceConnectionState::kIceConnectionMax:
            return "Count";
        default:
            return "Unknown";
    }
}

};

