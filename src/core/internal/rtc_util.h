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
#include "../rtc_emun.h"
#include "../../util/util.h"

namespace core {

using namespace::webrtc;
using namespace::rtc;
using RTCIceConnectionState = PeerConnectionInterface::IceConnectionState;

RTC_INLINE int64_t RTCTimeIntervalSince1970() {
    return util::get_current_time_milliseconds();
}

RTCString RTCStringBySdpType(RTCSdpType sdpType);

}

#endif /* !RTC_UTIL_H */
