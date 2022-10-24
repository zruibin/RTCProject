/*
 * rtc_util.h
 *
 * Created by Ruibin.Chow on 2022/10/24.
 * Copyright (c) 2022年 Ruibin.Chow All rights reserved.
 */

#ifndef RTC_UTIL_H
#define RTC_UTIL_H

#include <chrono>
#include <api/peer_connection_interface.h>
#include "../rtc_macro.h"

namespace core {

RTC_INLINE double RTCTimeIntervalSince1970() {
    std::chrono::milliseconds ms = std::chrono::duration_cast< std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()
        );
    return ms.count();
}

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

}


#endif /* !RTC_UTIL_H */
