/*
 * rtc_peer_stats_model.h
 *
 * Created by Ruibin.Chow on 2022/10/23.
 * Copyright (c) 2022年 Ruibin.Chow All rights reserved.
 */

#ifndef RTC_PEER_STATS_MODEL_H
#define RTC_PEER_STATS_MODEL_H

#include "core/rtc_macro.h"

namespace core {

/// medie track type
enum class RTCMediaTrackType {
    Audio, // audio track
    Video  // video track
};

/// media stream direction
enum class RTCMediaTrackDirection {
    Send, // sending stream
    Receive // receiving stream
};

struct RTCPeerStatsModel final {

public:
    RTCString description(void) {
        return "RTCPeerStatsModel";
    }
    
public:
    /// 对等连接对象Id
    RTCString peerId;
    ///  轨道Id
    RTCString trackId;
    /// 轨道类型
    RTCMediaTrackType trackType;
    /// 媒体流向
    RTCMediaTrackDirection trackDirection;
    /// 本端流，到janus发送音频的丢包率
    int txAudioLostRate;
    /// 本端流，到janus发送音频的抖动
    int txAudioJitter;
    /// 本端流，到janus发送音频的平均抖动
    int txAudioJitterAvg;
    /// 本端流，到janus发送音频的平均rtt
    int txAudioRttAvg;
    /// 本端流，平台类型
    RTCString txPlatform;
    /// 本端流，当前底层版本号
    RTCString txLibVersion;
    /// 本端流，音频设备错误码
    int txAudioUnitErrorCode;
    /// 本端流，音频采集回调次数
    long txAudioRecordCallback;
    /// 本端流，音频播放回调次数
    long txAudioPlayCallback;
    /// 本端流，音频采集次数
    long txRecordPcmCount;
    /// 本端流，音频播放次数
    long txPlayPcmCount;
    /// 本端流，平均发送码率
    long txSendBitAvg;
    /// 本端流，发送的rtp包数
    long txSendRTPPacket;
    /// 本端流，回声消除的ERLE的平均值
    long txAecERLEAve;
    /// 本端流，janus到对端发送音频的丢包率
    int remoteRxAudioLostRate;
    /// 本端流，发送音频的抖动
    int remoteRxAudioJitter;
    /// 本端流，发送音频的丢包率
    int sendAudioLostRate;
    /// 本端流，发送音频的能量值
    int inputAudioLevel;
    /// 对端流，到janus发送音频的丢包率
    int rxAudioLostRate;
    /// 对端流，到janus发送音频的抖动
    int rxAudioJitter;
    /// 对端流，到janus发送音频的平均抖动
    int rxAudioJitterAvg;
    /// 对端流，到janus发送音频的延时
    int rxAudioDelay;
    /// 对端流，janus到本端发送音频的丢包率
    int remoteTxAudioLostRate;
    /// 对端流，janus到本端发送音频的抖动
    int remoteTxAudioJitter;
    /// 对端流，接收音频的丢包率
    int receivedAudioLostRate;
    /// 对端流，接收音频的能量值
    int outputAudioLevel;
    /// 对端流，视频总的丢包率
    float rxVideoTotalPackageLostRate;
    /// 对端流，视频总的实际丢包数
    int rxVideoTotalRealPackageLost;
    /// 对端流，视频总的丢包恢复率
    float rxVideoTotalLostRecoveredRate;
    /// 卡顿率
    float disconRate;
    /// 丢包补偿率
    float effPlcRate;
    /// 大卡比率
    float badDisconRate;
    /// 网络丢包率
    float netLostRate;
    /// FEC后丢包率
    float aftFecLostRate;
    /// 丢包恢复率
    float effFecRate;
    /// 音频RTP接收包数
    long totalTimeReceivedRTP;
    /// 解码成功率
    int totalDecodeSucceedRate;
    /// 最大接收包间隔
    int packetRecvTimeInterMax;
    /// 最大接收包间隔发生的时间点
    int64_t packetRecvTimeInterMaxInTime;
    /// 接收包间隔 > 10s的次数
    int packetRecvTimeInterLevel5;
    /// 接收包间隔 (3s, 10s] 的次数
    int packetRecvTimeInterLevel4;
    /// 接收包间隔 (1s, 3s] 的次数
    int packetRecvTimeInterLevel3;
    /// 接收包间隔 (300ms, 1s] 的次数
    int packetRecvTimeInterLevel2;
    /// 接收包间隔 (50ms, 300ms] 的次数
    int packetRecvTimeInterLevel1;
    /// 接收包间隔 (0ms, 50ms] 的次数
    int packetRecvTimeInterLevel0;
    
    /// 有效解码率
    long effectiveDecodeRate;
    /// 接收的平均bit率
    long receviceBitAvg;
    /// 接收的总rtp包数
    long recRtpPacket;
    /// 总丢包数
    long loseRtpPacket;
    /// 接收的neck包数
    long recNackPacket;
    /// fec修复包数
    long fecRepairPacket;
    /// 总的抗弱网能力
    float resistWeaknetRate;
    /// 加速播放的数据量
    long audioAccelerateNumAcc;
    /// plc补偿的静音数据量
    long audioExpandedMuteSamplesAcc;
    /// plc补偿的总数据量
    long audioExpendTotalSampleAcc;
    /// 正常播放的数据量
    long audioNormalNumAcc;
    /// plc补偿的噪音的数据量
    long expandedNoiseSamplesAcc;
    /// plc补偿的总的语音数据量
    long expandedSpeechSamplesAcc;
    /// PLC补偿的有效数据量
    long expendEfficientSampleAcc;
    /// plc补偿的无效数据量
    long expendInefficientSampleAcc;
    /// 下行减速播放的数据量
    long audioPreexpandNumAcc;
    
    /// 视频卡顿次数
    long videoFreezeTimes;
    /// 视频卡顿总时长
    long videoTotalFreezeDuration;
    /// 视频解码总时长
    long videoTotalDuration;
};

}


#endif /* !RTC_PEER_STATS_MODEL_H */
