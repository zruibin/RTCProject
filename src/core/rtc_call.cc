/*
 *
 * rtc_call.cc
 *
 * Created by Ruibin.Chow on 2022/10/20.
 * Copyright (c) 2022年 Ruibin.Chow All rights reserved.
 */

#include "rtc_call.h"
#include <stdlib.h>
#include <api/create_peerconnection_factory.h>
#include <api/media_stream_interface.h>
#include <api/peer_connection_interface.h>
#include <api/rtc_event_log/rtc_event_log_factory.h>
#include <api/audio_codecs/builtin_audio_decoder_factory.h>
#include <api/audio_codecs/builtin_audio_encoder_factory.h>
#include <api/video_codecs/builtin_video_decoder_factory.h>
#include <api/video_codecs/builtin_video_encoder_factory.h>
#include <api/video_track_source_proxy_factory.h>
#include <modules/audio_device/include/audio_device.h>
#include <modules/audio_processing/include/audio_processing.h>
#include <modules/video_capture/video_capture_factory.h>
#include <rtc_base/checks.h>
#include <rtc_base/logging.h>
//#include <rtc_base/strings/json.h>
#include "internal/rtc_util.h"
#include "../log/logging.h"

namespace core {

/*----------------------------------------------------------------------------*/

RTCCall::RTCCall(std::unique_ptr<RTCCallObserverInterface> observer):
        observer_(std::move(observer)) {
    observerInternals_ = new RTCOberverInternalMap;
    peerStates_ = new RTCPeerStatusModelMap;
    toBeAddedICEs_ = new IceCandidatesMap;
    videoTrackSources_ = new RTCVideoTrackSourceMap;
    createSDPObserverMap_ = new RTCCreateSDPObserverMap;
    setSDPObserverMap_ = new RTCSetSDPObserverMap;
    fileLogger_ = std::make_unique<RTCFileLogger>(log::GetDefaultLoggerDir(), 20);
}

RTCCall::~RTCCall() {
    if (peerStates_) delete peerStates_;
    if (videoTrackSources_) delete videoTrackSources_;
    if (toBeAddedICEs_) delete toBeAddedICEs_;
    if (observerInternals_) delete observerInternals_;
    if (createSDPObserverMap_) delete createSDPObserverMap_;
    if (setSDPObserverMap_) delete setSDPObserverMap_;
    if (audioSource_) audioSource_ = nullptr;
    if (peerFactory_) peerFactory_ = nullptr;
}


void RTCCall::Init(void) {
    fileLogger_->SetSeverity(RTCFileLoggerSeverity::Verbose);
    fileLogger_->Start();
    
    network_thread_ = rtc::Thread::CreateWithSocketServer();
    network_thread_->SetName("network_thread", nullptr);
    RTC_CHECK(network_thread_->Start()) << "Failed to start thread";

    worker_thread_ = rtc::Thread::Create();
    worker_thread_->SetName("worker_thread", nullptr);
    RTC_CHECK(worker_thread_->Start()) << "Failed to start thread";

    signaling_thread_ = rtc::Thread::Create();
    signaling_thread_->SetName("signaling_thread", nullptr);
    RTC_CHECK(signaling_thread_->Start()) << "Failed to start thread";
    
    this->peerFactory_ = CreatePeerConnectionFactory(
            network_thread_.get() /* network_thread */,
            worker_thread_.get() /* worker_thread */,
            signaling_thread_.get() /* signaling_thread */,
            nullptr /* default_adm */,
            CreateBuiltinAudioEncoderFactory(),
            CreateBuiltinAudioDecoderFactory(),
            CreateBuiltinVideoEncoderFactory(),
            CreateBuiltinVideoDecoderFactory(),
            nullptr /* audio_mixer */,
            nullptr /* audio_processing */);
}

void RTCCall::TransferNetTypeToWebrtc(RTCNetType netType,
                                      bool isInitial) {
    
}

RTCVideoTrackSourceRef RTCCall::GetVideoTrackSource(const RTCString& peerId) {
    RTCVideoTrackSourceRef videoTrackSource;
    RTCVideoTrackSourceMap::iterator it = videoTrackSources_->find(peerId);
    if (it != videoTrackSources_->end()) {
        videoTrackSource = it->second;
    } else {
        videoTrackSource = CreateVideoTrackSource();
        videoTrackSources_->emplace(peerId, videoTrackSource);
    }
    
    return videoTrackSource;
}

void RTCCall::CreateOffer(RTCSdpType sdpType,
                          int layers,
                          const RTCString& peerId) {
    scoped_refptr<PeerConnectionInterface> peer;
    RtpTransceiverInit transceiverInit;
    bool needSender = false;
    if (sdpType == RTCSdpType::SendOnly) {
        transceiverInit.direction = RtpTransceiverDirection::kSendOnly;
        needSender = true;
    } else if (sdpType == RTCSdpType::ReceiveOnly) {
        transceiverInit.direction = RtpTransceiverDirection::kRecvOnly;
        needSender = false;
    } else {
        transceiverInit.direction = RtpTransceiverDirection::kSendRecv;
        needSender = true;
    }
    
    CreateSDPObserverAdapter::SDPHandler handler = [this](const RTCString& peerId,
                                                      SessionDescriptionInterface* desc,
                                                      RTCError error) {
        DidCreateSDP(peerId, desc, error);
    };
    
    auto observer = new RefCountedObject<CreateSDPObserverAdapter>(peerId, handler);
    createSDPObserverMap_->emplace(peerId, observer);
    peer = CreatePeer(peerId, needSender);
    peer->AddTransceiver(cricket::MEDIA_TYPE_AUDIO, transceiverInit);
    peer->AddTransceiver(cricket::MEDIA_TYPE_VIDEO, transceiverInit);
    PeerConnectionInterface::RTCOfferAnswerOptions options;
    peer->CreateOffer(observer, options);
}

void RTCCall::CreateAnswer(RTCSdpType sdpType,
                           RTCStringMap& offerMap,
                           const RTCString& peerId) {
    scoped_refptr<PeerConnectionInterface> peer;
    RtpTransceiverInit transceiverInit;
    bool needSender = false;
     if (sdpType == RTCSdpType::ReceiveOnly) {
        transceiverInit.direction = RtpTransceiverDirection::kRecvOnly;
        needSender = false;
    } else {
        transceiverInit.direction = RtpTransceiverDirection::kSendRecv;
        needSender = true;
    }
    peer = CreatePeer(peerId, needSender);
    
    SdpType type = webrtc::SdpTypeFromString(offerMap[kRTCSessionDescriptionTypeName]).value();
    RTCString sdp = offerMap[kRTCSessionDescriptionSdpName];
    SdpParseError error;
    
    bool hasRemoteDes = peer->remote_description() != nullptr ? true : false;
    SetSDPObserverAdapter::SDPHandler handler = [=](const RTCString& peerId,
                                                    RTCError error) {
        CommitStashedCandidateToPeer(peerId);
        DidSetSDP(peerId, error);
        if (hasRemoteDes && error.ok()) {
            // peer already has an remoteDescription, reset the remoteDescription
            // TODO
        }
    };
    auto observer = new RefCountedObject<SetSDPObserverAdapter>(peerId, handler);
    setSDPObserverMap_->emplace(peerId, observer);
    peer->SetRemoteDescription(observer, CreateSessionDescription(type, sdp, &error).get());
}

void RTCCall::AddAnswer(RTCStringMap& answerMap,
                        const RTCString& peerId) {
    scoped_refptr<PeerConnectionInterface> peer = FindPeerById(peerId);
    if (peer == nullptr) {
        Log(ERROR) << "peerConnection[" << peerId << "] doesn't exist, failed to add answer.";
        // TODO
        return;
    }
    
    SdpType type = webrtc::SdpTypeFromString(answerMap[kRTCSessionDescriptionTypeName]).value();
    RTCString sdp = answerMap[kRTCSessionDescriptionSdpName];
    SdpParseError error;
    SetSDPObserverAdapter::SDPHandler handler = [=](const RTCString& peerId,
                                                    RTCError error) {
        CommitStashedCandidateToPeer(peerId);
        DidSetSDP(peerId, error);
    };
    auto observer = new RefCountedObject<SetSDPObserverAdapter>(peerId, handler);
    setSDPObserverMap_->emplace(peerId, observer);
    peer->SetRemoteDescription(observer, CreateSessionDescription(type, sdp, &error).get());
}
 
void RTCCall::AddIceCandidate(RTCStringMap& candidateMap,
                              const RTCString& peerId) {
    // assemble the candidate from iceDic
    RTCString sdpMid = candidateMap[kRTCCandidateSdpMidName];
    RTCString sdp = candidateMap[kRTCCandidateSdpName];
    RTCString mlineIndex = candidateMap[kRTCCandidateSdpMlineIndexName];
    int sdpMLineIndex = atoi(mlineIndex.c_str());
    SdpParseError error;
    IceCandidateInterface* candidate = CreateIceCandidate(sdpMid, sdpMLineIndex, sdp, &error);
    auto candidateRef = std::shared_ptr<IceCandidateInterface>(candidate);
    
    // add up candidate to peer
    scoped_refptr<PeerConnectionInterface> peer = FindPeerById(peerId);
    if (peer != nullptr &&
        peer->local_description() != nullptr &&
        peer->remote_description() != nullptr) {
        // peer already has localDescription and remoteDescription, add candidate to peer immediately
        peer->AddIceCandidate(candidateRef.get());
    } else {
        // peer doesn't have localDescription or remoteDescription, store candidate temporarily
        IceCandidatesMap::iterator it = toBeAddedICEs_->find(peerId);
        if (it != toBeAddedICEs_->end()) {
            it->second.push_back(candidateRef);
        } else {
            std::vector<std::shared_ptr<IceCandidateInterface>> candidates{candidateRef};
            toBeAddedICEs_->emplace(peerId, candidates);
        }
    }
}

void RTCCall::SetConfigForVideoEncoder(const RTCVideoConfig& config,
                                       const RTCString& peerId) {
    
}

void RTCCall::EnableVideoEncoderLyaer(bool enable,
                                      RTCStreamVideoLayer layer,
                                      const RTCString& peerId) {
    
}

void RTCCall::SetVideoEncodeDegradationPreference(const webrtc::DegradationPreference preference,
                                                  const RTCString& peerId) {
    
}

void RTCCall::EnableAudioSender(bool isEnabled,
                                const RTCString& peerId) {
    
}

void RTCCall::EnableVideoSender(bool isEnabled,
                                const RTCString& peerId) {
    
}

void RTCCall::EnableSendAudioTrack(bool isEnabled,
                                   const RTCString& peerId) {
    
}

void RTCCall::EnableSendVideoTrack(bool isEnabled,
                                   const RTCString& peerId) {
    
}

void RTCCall::EnableReceiveAudioTrack(bool isEnabled,
                                      const RTCString& trackId,
                                      const RTCString& peerId) {
    
}

void RTCCall::EnableReceiveVideoTrack(bool isEnabled,
                                      const RTCString& trackId,
                                      const RTCString& peerId) {
    
}

void RTCCall::ResetCsrcByAudioTrackId(const RTCString& trackId,
                                      const RTCString& peerId) {
    
}

void RTCCall::ResetCsrcByVideoTrackId(const RTCString& trackId,
                                      const RTCString& peerId) {
    
}

void RTCCall::SetReceiveCsrc(uint32_t csrc,
                             const RTCString& trackId,
                             const RTCString& peerId) {
    
}

void RTCCall::ResetSimulcastIdByVideoTrackId(const RTCString& trackId,
                                             const RTCString& peerId) {
    
}

void RTCCall::SendSEI(unsigned char *seiData,
                      const RTCString& peerId) {
    
}

void RTCCall::GetStats(std::function<void(const std::unordered_map<RTCString, RTCPeerStatsModel>& stats,
                                          const webrtc::RTCError &error)>
                       predicate) {
    
}

std::vector<RTCPeerStatsModel>*
RTCCall::GetAudioLevelStats() {
    return nullptr;
}

void RTCCall::GetStatsFormEngine(std::function<
                                 void(std::vector<RTCPeerStatsModel> statsAry)>
                                 predicate) {
    
}

std::vector<RTCPeerStatsModel>*
RTCCall::GetPeerStats() {
    return nullptr;
}

bool RTCCall::InsertDTMF(const RTCString& tones,
                         double duration,
                         double interToneGap,
                         const RTCString& peerId) {
    return false;
}
 
void RTCCall::ReleasePeer(const RTCString& peerId) {
    
}

void RTCCall::ReleaseRTCCall() {
    
}

#pragma mark - Public

/*----------------------------------------------------------------------------*/

void RTCCall::DidCreateSDP(const RTCString& peerId,
                           SessionDescriptionInterface* desc,
                           RTCError error) {
    if (!error.ok()) {
        Log(ERROR) << "peer[" << peerId <<"] failed to create sdp with error: "
                    << error.message();
        // TODO
        return;
    }

    scoped_refptr<PeerConnectionInterface> peer = FindPeerById(peerId);
    SetSDPObserverAdapter::SDPHandler handler = [=](const RTCString& peerId,
                                                    RTCError error) {
        CommitStashedCandidateToPeer(peerId);
        DidSetSDP(peerId, error);
    };
    auto observer = new RefCountedObject<SetSDPObserverAdapter>(peerId, handler);
    setSDPObserverMap_->emplace(peerId, observer);
    peer->SetLocalDescription(observer, desc->Clone().get());
}

void RTCCall::DidSetSDP(const RTCString& peerId,
                        RTCError error) {
    if (!error.ok()) {
        Log(ERROR) << "peerConnection[" << peerId << "] failed to set sdp with error:"
                    << error.message();
        // TODO
        return;
    }
    scoped_refptr<PeerConnectionInterface> peer = FindPeerById(peerId);
    if (peer != nullptr && peer->local_description() != nullptr) {
        CreateSDPObserverAdapter::SDPHandler handler = [this](const RTCString& peerId,
                                                          SessionDescriptionInterface* desc,
                                                          RTCError error) {
            DidCreateSDP(peerId, desc, error);
        };
        auto observer = new RefCountedObject<CreateSDPObserverAdapter>(peerId, handler);
        createSDPObserverMap_->emplace(peerId, observer);
        PeerConnectionInterface::RTCOfferAnswerOptions options;
        peer->CreateAnswer(observer, options);
    }
}

void RTCCall::CommitStashedCandidateToPeer(const RTCString& peerId) {
    // attain the peer from peerConnections list
    scoped_refptr<PeerConnectionInterface> peer = FindPeerById(peerId);
    if (peer == nullptr) {
        Log(ERROR) << "peerConnection[" << peerId << "] doesn't exist, failed to commit stashed candidate";
        return;
    }
    
    // if peer has localDecription and remoteDescription, then add stashed candidate to peer
    if (peer->local_description() != nullptr &&
        peer->remote_description() != nullptr) {
        IceCandidatesMap::iterator it = toBeAddedICEs_->find(peerId);
        if (it != toBeAddedICEs_->end()) {
            auto candidates = it->second;
            for (auto candidate : candidates) {
                peer->AddIceCandidate(candidate.get());
            }
            candidates.clear();
        }
    }
}

scoped_refptr<PeerConnectionInterface>
RTCCall::CreatePeer(const RTCString& peerId) {
    if (observer_ == nullptr) {
        Log(ERROR) << "observer was null.";
        return nullptr;
    }
    
    // TODO
    PeerConnectionInterface::RTCConfiguration config;
    config.sdp_semantics = SdpSemantics::kUnifiedPlan;
    
//        PeerConnectionInterface::IceServer server;
//        server.uri = GetPeerConnectionString();
//        config.servers.push_back(server);
    
    std::shared_ptr<RTCObserverInternal> observerInternal = std::make_shared<RTCObserverInternal>();
    observerInternal->SetCall(this);
    observerInternal->SetPeerId(peerId);
    observerInternal->SetObserver(observer_.get());
    observerInternals_->emplace(peerId, observerInternal);
    
    PeerConnectionDependencies dependencies(observerInternal.get());
    auto ret = this->peerFactory_->CreatePeerConnectionOrError(config, std::move(dependencies));
    if (!ret.ok()) {
        const char* message = ret.error().message();
        Log(ERROR) << "peerConnection error: " << message;
        return nullptr;
    }
    scoped_refptr<PeerConnectionInterface> peerConnection = ret.value();
    return peerConnection;
}

scoped_refptr<PeerConnectionInterface>
RTCCall::CreatePeer(const RTCString& peerId, bool needSender) {
    scoped_refptr<PeerConnectionInterface> peer = FindPeerById(peerId);
    if (peer == nullptr) {
        // create new peer
        peer = CreatePeer(peerId);
        
        auto peerStatus = std::make_shared<RTCPeerStatusModel>();
        peerStatus->peerId = peerId;
        peerStatus->peer = peer;
        
        // create senders if need
        if (needSender) {
            // create media stream id
            RTCString mediaStreamId = AssembleMediaStreamId(false);
            // create a audioTrack and add it to peer
            peer->AddTrack(CreateAudioTrack(), {mediaStreamId});
            // create a videoTrack and add it to peer
            peer->AddTrack(CreateVideoTrack(peerId), {mediaStreamId});
            
            // update senders and recivers
            auto senders = peer->GetSenders();
            auto receivers = peer->GetReceivers();
            peerStatus->senders = senders;
            peerStatus->receivers = receivers;
        }
        peerStates_->emplace(peerId, peerStatus);
    }

    return peer;
}

RTCString RTCCall::AssembleMediaStreamId(bool isAssistant) {
    if (isAssistant) {
        return "RTC-Core-MS-Assistant";
    } else {
        return "RTC-Core-MS";
    }
}

RTCVideoTrackSourceRef RTCCall::CreateVideoTrackSource() {
    VideoTrackSourceInterface* source = nullptr; // TODO
    return CreateVideoTrackSourceProxy(signaling_thread_.get(),
                                       worker_thread_.get(),
                                       source);
}

scoped_refptr<VideoTrackInterface>
RTCCall::CreateVideoTrack(const RTCString& peerId) {
    RTCString videoTrackId = "RTC-Core-MS-V0";
    RTCVideoTrackSourceRef videoTrackSourceRef = GetVideoTrackSource(peerId);
    return peerFactory_->CreateVideoTrack(videoTrackId, videoTrackSourceRef.get());
}

scoped_refptr<AudioTrackInterface>
RTCCall::CreateAudioTrack() {
    RTCString audioTrackId = "RTC-Core-MS-A0";
    cricket::AudioOptions audioOption; // TODO
    audioSource_ = peerFactory_->CreateAudioSource(audioOption);
    return peerFactory_->CreateAudioTrack(audioTrackId, audioSource_.get());
}

void RTCCall::UpdateIceState(const RTCString& peerId,
                             PeerConnectionInterface::IceConnectionState state) {
    RTCPeerStatusModelMap::iterator it = peerStates_->find(peerId);
    if (it == peerStates_->end()) {
        Log(ERROR) << "peer[" << peerId << "] doesn't exist, failed to update ice state";
        return ;
    }
    auto statusModel = it->second;
    statusModel->lastIceState = state;
    statusModel->lastUpdateTimestamp = RTCTimeIntervalSince1970();
}

#pragma mark - Private

scoped_refptr<PeerConnectionInterface>
RTCCall::FindPeerById(const RTCString& peerId) {
    RTCPeerStatusModelMap::iterator it = peerStates_->find(peerId);
    if (it == peerStates_->end()) {
        return nullptr;
    }
    return it->second->peer;
}

#pragma mark -

/*----------------------------------------------------------------------------*/

std::optional<std::shared_ptr<RTCCallInterface>>
CreateRTCCallOrNull(std::unique_ptr<RTCCallObserverInterface> observer) {
    std::optional<std::shared_ptr<RTCCallInterface>> ret = std::nullopt;
    std::shared_ptr<RTCCallInterface> call = std::make_shared<RTCCall>(std::move(observer));
    if (call != nullptr) {
        ret = call;
    }
    return ret;
}

webrtc::RTCErrorOr<std::shared_ptr<RTCCallInterface>>
CreateRTCCallOrError(std::unique_ptr<RTCCallObserverInterface> observer) {
    auto ret = webrtc::RTCErrorOr<std::shared_ptr<RTCCallInterface>>();
    std::shared_ptr<RTCCallInterface> call = std::make_shared<RTCCall>(std::move(observer));
    if (call != nullptr) {
        ret = call;
    }
    return ret;
}

RTCString getWebRTCVersion(void) {
    return "M106";
}

}

