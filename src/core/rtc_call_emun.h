/*
 * rtc_call_emun.h
 *
 * Created by Ruibin.Chow on 2022/10/21.
 * Copyright (c) 2022年 Ruibin.Chow All rights reserved.
 */

#ifndef RTC_CALL_EMUN_H
#define RTC_CALL_EMUN_H

namespace core {

/// sdp type
enum class RTCSdpType {
    /// send only
    SendOnly,
    /// receive only
    ReceiveOnly,
    /// send and receive
    SendReceive
};

/// peer state
enum class RTCNetType {
    /// no network
    NoNet = 0,
    /// 2G
    _2G,
    /// 3G
    _3G,
    /// no wifi, mostly not happen
    NoWiFi,
    /// wifi
    WiFi,
    /// 4G
    _4G,
    /// 5G
    _5G
};

/// peer state
enum class RTCPeerState {
    /// peer connected
    Connected,
    /// peer disconnected
    Disconnected,
    /// peer failed
    Failed,
    /// peer closed
    Closed
};

/// error code of RTCCall
enum class RTCCallErrorCode {
    /// peer not eixt
    PeerNotExist,
    /// peer created sdp failed
    PeerCreateSDPFailed,
    /// peer set sdp failed
    PeerSetSDPFailed,
};

}

#endif /* !WEBRTC_CALL_EMUN_H */
