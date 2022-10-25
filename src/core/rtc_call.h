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
#include "internal/rtc_internal_observer.h"
#include "internal/rtc_file_logger.h"

namespace core {

using namespace::webrtc;
using namespace::rtc;

using RTCOberverInternalRef = std::shared_ptr<RTCObserverInternal>;
using RTCOberverInternalMap = std::unordered_map<RTCString, RTCOberverInternalRef>;
using RTCCreateSDPObserverRef = scoped_refptr<CreateSDPObserverAdapter>;
using RTCCreateSDPObserverMap = std::unordered_map<RTCString, RTCCreateSDPObserverRef>;
using RTCSetSDPObserverRef = scoped_refptr<SetSDPObserverAdapter>;
using RTCSetSDPObserverMap = std::unordered_map<RTCString, RTCSetSDPObserverRef>;
using RTCPeerStatusModelRef = std::shared_ptr<RTCPeerStatusModel>;
using RTCPeerStatusModelMap = std::unordered_map<RTCString, RTCPeerStatusModelRef>;
using RTCVideoTrackSourceMap = std::unordered_map<RTCString, RTCVideoTrackSourceRef>;
using IceCandidatesRef = std::shared_ptr<IceCandidateInterface>;
using IceCandidatesMap = std::unordered_map<RTCString, std::vector<IceCandidatesRef>>;

class RTCCall : public RTCCallInterface {

public:
    explicit RTCCall(std::unique_ptr<RTCCallObserverInterface>);
    virtual ~RTCCall();
    
public:
    void Init(void) override;
    void TransferNetTypeToWebrtc(RTCNetType netType,
                                 bool isInitial) override;
    RTCVideoTrackSourceRef GetVideoTrackSource(const RTCString& peerId) override;
    void CreateOffer(RTCSdpType sdpType,
                     int layers,
                     const RTCString& peerId) override;
    void CreateAnswer(RTCSdpType sdpType,
                      RTCStringMap& offerMap,
                      const RTCString& peerId) override;
    void AddAnswer(RTCStringMap& answerMap,
                   const RTCString& peerId) override;
    void AddIceCandidate(RTCStringMap& candidateMap,
                         const RTCString& peerId) override;
    void SetConfigForVideoEncoder(const RTCVideoConfig& config,
                                  const RTCString& peerId) override;
    void EnableVideoEncoderLyaer(bool enable,
                                 RTCStreamVideoLayer layer,
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
    void SetVideoContentType(uint32_t contentType,
                             const RTCString& trackId,
                             const RTCString& peerId) override;
    void ResetSimulcastIdByVideoTrackId(const RTCString& trackId,
                                        const RTCString& peerId) override;
    void SendSEI(unsigned char *seiData,
                 const RTCString& peerId) override;
    void GetStats(std::function<void(const std::unordered_map<RTCString, RTCPeerStatsModel>& stats,
                                     const webrtc::RTCError &error)>
                  predicate) override;
    std::vector<RTCPeerStatsModel>* GetAudioLevelStats() override;
    void GetStatsFormEngine(std::function<
                            void(std::vector<RTCPeerStatsModel> statsAry)>
                            predicate) override;
    std::vector<RTCPeerStatsModel>* GetPeerStats() override;
    bool InsertDTMF(const RTCString& tones,
                    double duration,
                    double interToneGap,
                    const RTCString& peerId) override;
    void ReleasePeer(const RTCString& peerId) override;
    void ReleaseRTCCall() override;
    
public:
    void DidCreateSDP(const RTCString& peerId,
                      SessionDescriptionInterface* desc,
                      RTCError error);
    void DidSetSDP(const RTCString& peerId,
                   RTCError error);
    void CommitStashedCandidateToPeer(const RTCString& peerId);
    scoped_refptr<PeerConnectionInterface> CreatePeer(const RTCString& peerId);
    scoped_refptr<PeerConnectionInterface> CreatePeer(const RTCString& peerId,
                                                      bool needSender);
    RTCString AssembleMediaStreamId(bool isAssistant);
    RTCVideoTrackSourceRef CreateVideoTrackSource();
    scoped_refptr<VideoTrackInterface> CreateVideoTrack(const RTCString& peerId);
    scoped_refptr<AudioTrackInterface> CreateAudioTrack();
    void UpdateIceState(const RTCString& peerId,
                        PeerConnectionInterface::IceConnectionState state);
    RTCRtpSenderRef VideoSenderFromPeer(const RTCString& peerId);
    RTCRtpSenderRef AudioSenderFromPeer(const RTCString& peerId);
    RTCRtpReceiverRef VideoReceiverFromPeer(const RTCString& peerId);
    RTCRtpReceiverRef AudioReceiverFromPeer(const RTCString& peerId);

private:
    scoped_refptr<PeerConnectionInterface> FindPeerById(const RTCString& peerId);
    RTCPeerStatusModelRef FindPeerStatusModelById(const RTCString& peerId);
                    
public:
    RTCOberverInternalMap* observerInternals_;
    RTCCreateSDPObserverMap* createSDPObserverMap_;
    RTCSetSDPObserverMap* setSDPObserverMap_;
    /// peer state list
    RTCPeerStatusModelMap* peerStates_;
    /// candidates to be added to peer
    IceCandidatesMap* toBeAddedICEs_;
private:
    std::unique_ptr<RTCCallObserverInterface> observer_;
    scoped_refptr<PeerConnectionFactoryInterface> peerFactory_;
    /// audio source
    scoped_refptr<AudioSourceInterface> audioSource_;
    /// video source list
    RTCVideoTrackSourceMap* videoTrackSources_;
    
    std::unique_ptr<RTCFileLogger> fileLogger_;
    std::unique_ptr<rtc::Thread> network_thread_;
    std::unique_ptr<rtc::Thread> worker_thread_;
    std::unique_ptr<rtc::Thread> signaling_thread_;
    
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
