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


namespace core {

struct RTCPeerStatusModel final {
    
public:
    std::string description(void) {
        char buffer[100];;
        snprintf(buffer, sizeof(buffer),
                 "{peerId:%s, peer:%p, lastIceState:%d, \
                 createdTimestamp:%f, lastUpdateTimestamp:%f, \
                 senders:%p, receivers:%p}",
                 peerId.c_str(), peer.get(), lastIceState,
                 createdTimestamp, lastUpdateTimestamp,
                 &senders, &receivers);
        std::string des(buffer);
        return des;
    }
    
public:
    /// 对等连接对象Id
    std::string peerId;
    /// 对等连接对象
    std::shared_ptr<webrtc::PeerConnectionInterface> peer;
    /// rtp发送器
    std::vector<webrtc::RtpSenderInterface> senders;
    /// rtp接收器
    std::vector<webrtc::RtpReceiverInterface> receivers;
    /// 创建的时间戳
    double createdTimestamp;
    /// 最新ice状态的更新时间戳
    double lastUpdateTimestamp;
    /// 最新的ice状态
    webrtc::PeerConnectionInterface::IceConnectionState lastIceState;
};


///**  */
//@property (nonatomic, assign) PAWebRTCIceConnectionState lastIceState;

}

#endif /* !RTC_PEER_STATUS_MODEL_H */
