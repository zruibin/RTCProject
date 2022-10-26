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

using namespace::webrtc;
using namespace::rtc;
using RTCIceConnectionState = PeerConnectionInterface::IceConnectionState;

RTC_INLINE int64_t RTCTimeIntervalSince1970() {
    std::chrono::milliseconds ms = std::chrono::duration_cast< std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()
        );
    return ms.count();
}

RTCString
RTCStringByIceConnectionState(RTCIceConnectionState new_state);

}


#endif /* !RTC_UTIL_H */
