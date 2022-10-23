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
#include "../rtc_macro.h"

namespace core {

struct RTCPeerStatusModel final {
    
public:
    RTCString description(void) {
        char buffer[100];;
        snprintf(buffer, sizeof(buffer),
                 "{peerId:%s, peer:%p, lastIceState:%d, \
                 createdTimestamp:%f, lastUpdateTimestamp:%f, \
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
    std::vector<rtc::scoped_refptr<webrtc::RtpSenderInterface>> senders;
    /// rtp接收器
    std::vector<rtc::scoped_refptr<webrtc::RtpReceiverInterface>> receivers;
    /// 创建的时间戳
    double createdTimestamp;
    /// 最新ice状态的更新时间戳
    double lastUpdateTimestamp;
    /// 最新的ice状态
    webrtc::PeerConnectionInterface::IceConnectionState lastIceState;
};

}

#endif /* !RTC_PEER_STATUS_MODEL_H */
