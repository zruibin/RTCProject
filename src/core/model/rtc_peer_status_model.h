/*
 * rtc_peer_status_model.h
 *
 * Created by Ruibin.Chow on 2022/10/22.
 * Copyright (c) 2022年 Ruibin.Chow All rights reserved.
 */

#ifndef RTC_PEER_STATUS_MODEL_H
#define RTC_PEER_STATUS_MODEL_H

#include <stdio.h>
#include <string>
#include <api/peer_connection_interface.h>
#include <api/rtp_sender_interface.h>
#include <api/rtp_receiver_interface.h>
#include "core/rtc_macro.h"

namespace core {

using RTCRtpSenderRef = rtc::scoped_refptr<webrtc::RtpSenderInterface>;
using RTCRtpReceiverRef = rtc::scoped_refptr<webrtc::RtpReceiverInterface>;
using RTCIceConnectionState = webrtc::PeerConnectionInterface::IceConnectionState;

struct RTCPeerStatusModel {
    
public:
    RTCString description(void) {
        char buffer[100];;
        snprintf(buffer, sizeof(buffer),
                 "{peerId:%s, peer:%p, lastIceState:%d, \
                 createdTimestamp:%lld, lastUpdateTimestamp:%lld, \
                 senders:%p, receivers:%p}",
                 peerId.c_str(), peer.get(), lastIceState,
                 createdTimestamp, lastUpdateTimestamp,
                 &senders, &receivers);
        RTCString des(buffer);
        return des;
    }
    
public:
    /// 对等连接对象Id
    RTCString peerId;
    /// 对等连接对象
    rtc::scoped_refptr<webrtc::PeerConnectionInterface> peer;
    /// rtp发送器
    std::vector<RTCRtpSenderRef> senders;
    /// rtp接收器
    std::vector<RTCRtpReceiverRef> receivers;
    /// 创建的时间戳
    int64_t createdTimestamp;
    /// 最新ice状态的更新时间戳
    int64_t lastUpdateTimestamp;
    /// 最新的ice状态
    RTCIceConnectionState lastIceState;
};

}

#endif /* !RTC_PEER_STATUS_MODEL_H */
