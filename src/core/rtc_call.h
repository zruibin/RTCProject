/*
 * rtc_call.h
 *
 * Created by Ruibin.Chow on 2022/10/20.
 * Copyright (c) 2022年 Ruibin.Chow All rights reserved.
 */

#ifndef RTC_CALL_H
#define RTC_CALL_H

#include <api/jsep_ice_candidate.h>
#include "rtc_call_interface.h"
#include "model/rtc_peer_status_model.h"

namespace core {

using namespace::webrtc;
using namespace::rtc;

class RTCCall : public RTCCallInterface {

public:
    using RTCPeerStatusModelMap = std::unordered_map<RTCString, RTCPeerStatusModel>;
    using RTCVideoSourceMap = std::unordered_map<RTCString, RTCVideoSource>;
    using IceCandidatesMap = std::unordered_map<RTCString, std::vector<JsepIceCandidate>>;
    explicit RTCCall(std::unique_ptr<RTCCallObserverInterface>);
    virtual ~RTCCall();
    
public:
    void Init(void) override;
    void TransferNetTypeToWebrtc(RTCNetType netType,
                                 bool isInitial) override;
    std::shared_ptr<RTCVideoSource> GetVideoSource(const RTCString& peerId) override;
    void CreateOffer(RTCSdpType sdpType,
                     int layers,
                     const RTCString& peerId) override;
    void CreateAnswer(RTCSdpType sdpType,
                      const StringHashMap& offerMap,
                      const RTCString& peerId) override;
    void AddAnswer(const StringHashMap& answerMap,
                   const RTCString& peerId) override;
    void AddIceCandidate(const StringHashMap& candidateMap,
                         const RTCString& peerId) override;
    void SetConfigForVideoEncoder(const RTCObject/*PARTCVideoConfig*/& config,
                                  const RTCString& peerId) override;
    void EnableVideoEncoderLyaer(bool enable,
                                 const RTCObject/*PARTCStreamVideoLayer*/& layer,
                                 const RTCString& peerId) override;
    void SetVideoEncodeDegradationPreference(const webrtc::DegradationPreference preference,
                                             const RTCString& peerId) override;
    void EnableAudioSender(bool isEnabled,
                           const RTCString& peerId) override;
    void EnableVideoSender(bool isEnabled,
                           const RTCString& peerId) override;
    void EnableSendAudioTrack(bool isEnabled,
                              const RTCString& peerId) override;
    void EnableSendVideoTrack(bool isEnabled,
                              const RTCString& peerId) override;
    void EnableReceiveAudioTrack(bool isEnabled,
                                 const RTCString& trackId,
                                 const RTCString& peerId) override;
    void EnableReceiveVideoTrack(bool isEnabled,
                                 const RTCString& trackId,
                                 const RTCString& peerId) override;
    void ResetCsrcByAudioTrackId(const RTCString& trackId,
                                 const RTCString& peerId) override;
    void ResetCsrcByVideoTrackId(const RTCString& trackId,
                                 const RTCString& peerId) override;
    void SetReceiveCsrc(uint32_t csrc,
                        const RTCString& trackId,
                        const RTCString& peerId) override;
    void ResetSimulcastIdByVideoTrackId(const RTCString& trackId,
                                        const RTCString& peerId) override;
    void SendSEI(unsigned char *seiData,
                 const RTCString& peerId) override;
    void GetStats(std::function<void(const RTCObject/*NSDictionary */& stats,
                                     const webrtc::RTCError &error)>
                  predicate) override;
    std::vector<RTCObject/*PFMCPeerStatsModel */>* GetAudioLevelStats() override;
    void GetStatsFormEngine(std::function<
                            void(std::vector<RTCObject/*PFMCPeerStatsModel */> statsAry)>
                            predicate) override;
    std::vector<RTCObject/*PFMCPeerStatsModel */>* GetPeerStats() override;
    bool InsertDTMF(const RTCString& tones,
                    double duration,
                    double interToneGap,
                    const RTCString& peerId) override;
    void ReleasePeer(const RTCString& peerId) override;
    void ReleaseRTCCall() override;
    
private:
    RTCErrorOr<scoped_refptr<PeerConnectionInterface>>
    createPeer(const RTCString& peerId);
    
private:
    std::unique_ptr<RTCCallObserverInterface> observer_;
    scoped_refptr<PeerConnectionFactoryInterface> peerFactory_;
    /// audio source
    scoped_refptr<AudioSourceInterface> audioSource_;
    /// peer state list
    RTCPeerStatusModelMap* peerStates_;
    /// video source list
    RTCVideoSourceMap* videoSources_;
    /// candidates to be added to peer
    IceCandidatesMap* toBeAddedICEs_;
    
private:
    /// ice connect overtime, unit: second.
    int iceConnectionTimeout_;
    /// ice state check rate: every 4 seconds
    int iceStateCheckRate_;
    /// RTCCall error domain
    RTCString errorDomain;
};

}



#endif /* !WEBRTC_CALL_H */
