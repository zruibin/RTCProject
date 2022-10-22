/*
 * rtc_call_interface.h
 *
 * Created by Ruibin.Chow on 2022/10/20.
 * Copyright (c) 2022年 Ruibin.Chow All rights reserved.
 */

#ifndef RTC_CALL_INTERFACE_H
#define RTC_CALL_INTERFACE_H

#include <memory>
#include <api/peer_connection_interface.h>
#include <media/base/video_source_base.h>
#include <api/rtp_parameters.h>
#include "rtc_call_observer_interface.h"
#include "rtc_object.h"

namespace core {

using RTCVideoSourceInterface = rtc::VideoSourceInterface<webrtc::VideoFrame>;
using RTCVideoSource = rtc::VideoSourceBase;

class RTCCallInterface {

public:
    RTCCallInterface() = default;
    virtual ~RTCCallInterface() = default;
    
public:
    virtual void Init(void) = 0;
    
    /// transfer netType to webrtc for chat optimization
    /// @param netType net type
    /// @param isInitial before or in chat
    virtual void TransferNetTypeToWebrtc(RTCNetType netType,
                                         bool isInitial) = 0;
    
    /// attain video source
    /// @param peerId peer id
    virtual std::shared_ptr<RTCVideoSource> GetVideoSource(const RTCString& peerId) = 0;
    
    /// create offer. if success, offer will be shipped by delegate
    /// @param sdpType sdp type
    /// @param layers number of simulcast layer
    /// @param peerId peer id
    virtual void CreateOffer(RTCSdpType sdpType,
                             int layers,
                             const RTCString& peerId) = 0;
    
    /// create answer. if success, offer will be shipped by delegate
    /// @param sdpType sdp type
    /// @param offer sdp of offer
    /// @param peerId peer id
    virtual void CreateAnswer(RTCSdpType sdpType,
                              const StringHashMap& offerMap,
                              const RTCString& peerId) = 0;
    
    /// add sdp of answer to peer
    /// @param peerId peer id
    /// @param answer sdp of answer
    virtual void AddAnswer(const StringHashMap& answerMap,
                           const RTCString& peerId) = 0;
    
    /// add candidate to peer
    /// @param iceCandidate ice candidate dictionary
    /// @param peerId peer id
    virtual void AddIceCandidate(const StringHashMap& candidateMap,
                                 const RTCString& peerId) = 0;

    /// set config for video encoder
    /// @param config config for video encoder
    /// @param peerId peer id
    virtual void SetConfigForVideoEncoder(const RTCObject/*PARTCVideoConfig*/& config,
                                          const RTCString& peerId) = 0;

    /// enable or disable specify video encoder layer
    /// @param enable enable or disable
    /// @param layer video encoder layer
    /// @param peerId peer id
    virtual void EnableVideoEncoderLyaer(bool enable,
                                         const RTCObject/*PARTCStreamVideoLayer*/& layer,
                                         const RTCString& peerId) = 0;

    /// set degradation preference for encoding video
    /// @param preference degradation preference
    /// @param peerId peer id
    virtual void SetVideoEncodeDegradationPreference(const webrtc::DegradationPreference preference,
                                                     const RTCString& peerId) = 0;

    /// enable or disable the audio transmition in send direction
    /// @param isEnabled enable or disable
    /// @param peerId peer id
    virtual void EnableAudioSender(bool isEnabled,
                                   const RTCString& peerId) = 0;

    /// enable or disable the video transmition in send direction
    /// @param isEnabled enable or disable
    /// @param peerId peer id
    virtual void EnableVideoSender(bool isEnabled,
                                   const RTCString& peerId) = 0;

    /// enable or disable the video track in sending direction.
    ///
    /// if enable, it will send original audio packages
    /// if disable, it will send mute packages
    ///
    /// @param isEnabled enable or disable
    /// @param peerId peer id
    virtual void EnableSendAudioTrack(bool isEnabled,
                                      const RTCString& peerId) = 0;

    /// enable or disable the video track in sending direction.
    ///
    /// if enable, it will send original video packages
    /// if disable, it will send black frames
    ///
    /// @param isEnabled enable or disable
    /// @param peerId peer id
    virtual void EnableSendVideoTrack(bool isEnabled,
                                      const RTCString& peerId) = 0;

    /// enable or disable the auido track in receiving direction. if disable, received auido data will be discarded.
    /// @param isEnabled enable or disable
    /// @param trackId track id
    /// @param peerId peer id
    virtual void EnableReceiveAudioTrack(bool isEnabled,
                                         const RTCString& trackId,
                                         const RTCString& peerId) = 0;

    /// enable or disable the video track in receiving direction. if disable, received video data will be discarded.
    /// @param isEnabled enable or disable
    /// @param trackId track id
    /// @param peerId peer id
    virtual void EnableReceiveVideoTrack(bool isEnabled,
                                         const RTCString& trackId,
                                         const RTCString& peerId) = 0;

    /// reset csrc of the audio track of the peer
    /// @param trackId audio track id
    /// @param peerId peer id
    virtual void ResetCsrcByAudioTrackId(const RTCString& trackId,
                                         const RTCString& peerId) = 0;

    /// reset csrc of the video track of the peer
    /// @param trackId video track id
    /// @param peerId peer id
    virtual void ResetCsrcByVideoTrackId(const RTCString& trackId,
                                         const RTCString& peerId) = 0;

    /// set csrc of the video track of the peer which the peer will only receive
    /// @param csrc new csrc
    /// @param trackId video track id
    /// @param peerId peer id
    virtual void SetReceiveCsrc(uint32_t csrc,
                                const RTCString& trackId,
                                const RTCString& peerId) = 0;

    /// reset simulcastId of the video track of the peer
    /// @param trackId video track id
    /// @param peerId peer id
    virtual void ResetSimulcastIdByVideoTrackId(const RTCString& trackId,
                                                const RTCString& peerId) = 0;

    /// send SEI through peer
    /// @param seiData supplementary enhancement information data
    /// @param peerId peer id
    virtual void SendSEI(unsigned char *seiData,
                         const RTCString& peerId) = 0;

    /// get webrtc stats of all peers
    virtual void GetStats(std::function<void(const RTCObject/*NSDictionary */& stats,
                                             const webrtc::RTCError &error)>
                          predicate) = 0;

    /// get peer statics of audio level
    virtual std::vector<RTCObject/*PFMCPeerStatsModel */>* GetAudioLevelStats() = 0;

    /// getting the engine stats data
    virtual void GetStatsFormEngine(std::function<
                                        void(std::vector<RTCObject/*PFMCPeerStatsModel */> statsAry)>
                                    predicate) = 0;

    /// getting the peer stats data
    virtual std::vector<RTCObject/*PFMCPeerStatsModel */>* GetPeerStats() = 0;

    /// insert dtmf data
    /// @param tones   content, can only contain 0-9,a-z,A-Z,#,*,','
    /// @param duration    duration, range in [0.07s, 6s]
    /// @param interToneGap    gap betwen two tones, range in [0.05s, ~), the shorter the better
    /// @param peerId    peer identification
    /// @return    YES or NO
    virtual bool InsertDTMF(const RTCString& tones,
                            double duration,
                            double interToneGap,
                            const RTCString& peerId) = 0;
    
    /// release peer resource
    /// @param peerId peer id
    virtual void ReleasePeer(const RTCString& peerId) = 0;

    /// release all webrtc resource
    virtual void ReleaseRTCCall() = 0;

private:
    /// manual audio enabled flag
    bool useManualAudio_;
    /// audio enable flag
    bool audioEnabled_;
    /// built in speaker enabled flag
    bool builtInSpeakerOn_;
    /// all audio sender mute flag
    bool allAudioSenderMute_;
    /// all audio receiver mute flag
    bool allAudioReceiverMute_;
};



/*----------------------------------------------------------------------------*/

std::shared_ptr<RTCCallInterface>
CreateRTCCall(RTCCallObserverInterface *observer);

/// get webrtc version
RTCString getWebRTCVersion(void);

}

#endif /* !RTC_CALL_INTERFACE_H */
