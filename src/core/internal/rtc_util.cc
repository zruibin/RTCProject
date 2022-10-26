/*
 *
 * rtc_util.cc
 *
 * Created by Ruibin.Chow on 2022/10/25.
 * Copyright (c) 2022年 Ruibin.Chow All rights reserved.
 */

#include "rtc_util.h"

namespace core {

RTCString RTCStringBySdpType(RTCSdpType sdpType) {
    switch (sdpType) {
        case RTCSdpType::SendOnly:
            return "SendOnly";
            break;
        case RTCSdpType::ReceiveOnly:
            return "ReceiveOnly";
            break;
        case RTCSdpType::SendReceive:
            return "SendReceive";
            break;
        default:
            return "";
            break;
    }
}

};

