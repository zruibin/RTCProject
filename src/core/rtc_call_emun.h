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

/*------------------------------------------------------------------------------*/

/// 流的不同清晰度的视频层，越大越清晰
enum class RTCStreamVideoLayer {
    /// 视频层0
    Layer0,
    /// 视频层1
    Layer1
};

/// 视频配置枚举
enum class RTCVideoConfigPreset {
    /// 分辨率: 320x180, 帧率: 15 fps, 码率: 300 kbps
    _180P,
    /// 分辨率: 480x270, 帧率: 15 fps, 码率: 400 kbps
    _270P,
    /// 分辨率: 640x360, 帧率: 15 fps, 码率: 600 kbps
    _360P,
    /// 分辨率: 960x540, 帧率: 15 fps, 码率: 1200 kbps
    _540P,
    /// 分辨率: 1280x720, 帧率: 15 fps, 码率: 1500 kbps
    _720P,
    /// 分辨率: 1920x1080, 帧率: 15 fps, 码率: 3000 kbps
    _1080P
};


}

#endif /* !WEBRTC_CALL_EMUN_H */
