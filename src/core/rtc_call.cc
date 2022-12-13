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
#include "core/internal/rtc_util.h"
#include "log/logging.h"

namespace core {

RTCCall::RTCCall(std::unique_ptr<RTCCallObserverInterface> observer,
                 RTCBaseConfig& baseConfig):
        observer_(std::move(observer)),
        baseConfig_(baseConfig) {
    Log(INFO) << "RTCCall Construction.";
            
    /// ice connect overtime, unit: second.
    iceConnectionTimeout_ = 10;
    /// ice state check rate: every 4 seconds
    iceStateCheckRate_ = 4;
    observerInternals_ = new RTCOberverInternalMap;
    peerStates_ = new RTCPeerStatusModelMap;
    toBeAddedICEs_ = new IceCandidatesMap;
    videoTrackSources_ = new RTCVideoTrackSourceMap;
    createSDPObserverMap_ = new RTCCreateSDPObserverMap;
    setSDPObserverMap_ = new RTCSetSDPObserverMap;
    fileLogger_ = std::make_unique<RTCFileLogger>(logger::GetDefaultLoggerDir(), 20);
}

RTCCall::~RTCCall() {
    Log(INFO) << "RTCCall Deconstruction.";
    
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
    Log(INFO) << "RTCCall Init.";
    
    fileLogger_->SetRTCLogThrowback(true);
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
    
    int64_t currentTime = RTCTimeIntervalSince1970();
    Log(INFO) << "currentTime: " << currentTime;
}

void RTCCall::TransferNetTypeToWebrtc(RTCNetType netType,
                                      bool isInitial) {
    Log(INFO) << "RTCCall TransferNetTypeToWebrtc.";
}

RTCVideoTrackSourceRef RTCCall::GetVideoTrackSource(const RTCString& peerId) {
    INVOKE_RETURN(RTCVideoTrackSourceRef, [&]{
        return this->GetVideoTrackSource(peerId);
    })
    Log(INFO) << "RTCCall GetVideoTrackSource, peerId:" << peerId;
    
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
    INVOKE([&]{ this->CreateOffer(sdpType, layers, peerId); });
    Log(INFO) << "RTCCall CreateOffer, peerId:" << peerId
                << ", sdpType:" << RTCStringBySdpType(sdpType)
                << ", layers:" << layers;
    
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
    if (layers > 1 && layers <= 3) {
        std::string str[] = {"low", /*"medium",*/ "high"};
        std::string msid[] = {"l", /*"m",*/ "h"};
        // double pri = 0.8;
        // initialization parameter sets the number of layers in the time domain
        // How many webrtc::RtpEncodingParameters is how many layers of simulcast.
        for (int i = 0; i < layers; ++i) {
            webrtc::RtpEncodingParameters videoEncoding;
            videoEncoding.rid = str[i-1];
            // videoEncoding.max_bitrate_bps = layers * i * 100 * 1000;
            // videoEncoding.bitrate_priority = pri;
            videoEncoding.num_temporal_layers = layers;
            transceiverInit.send_encodings.push_back(videoEncoding);
            // pri -= 0.2;
            // This is the name of the msid parameter in SDP
            transceiverInit.stream_ids.push_back(msid[i-1]);
        }
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
//    if (layers > 1) options.num_simulcast_layers = layers;
    peer->CreateOffer(observer, options);
}

void RTCCall::CreateAnswer(RTCSdpType sdpType,
                           RTCStringMap& offerMap,
                           const RTCString& peerId) {
    INVOKE([&]{ this->CreateAnswer(sdpType, offerMap, peerId); });
    Log(INFO) << "RTCCall CreateAnswer, peerId:" << peerId
                << ", sdpType:" << RTCStringBySdpType(sdpType)
                << ", offerMap:" << &offerMap;
    
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
    };
    auto observer = new RefCountedObject<SetSDPObserverAdapter>(peerId, handler);
    setSDPObserverMap_->emplace(peerId, observer);
    peer->SetRemoteDescription(observer, CreateSessionDescription(type, sdp, &error).get());
}

void RTCCall::AddAnswer(RTCStringMap& answerMap,
                        const RTCString& peerId) {
    INVOKE([&]{ this->AddAnswer(answerMap, peerId); });
    Log(INFO) << "RTCCall AddAnswer, peerId:" << peerId
                << ", answerMap:" << &answerMap;
    
    scoped_refptr<PeerConnectionInterface> peer = FindPeerById(peerId);
    if (peer == nullptr) {
        RTCString errorStr("peer[");
        errorStr.append(peerId);
        errorStr.append("] doesn't exist, failed to add answer.");
        Log(ERROR) << errorStr;
        RTCError error(RTCErrorType::INTERNAL_ERROR, errorStr);
        observer_->OnOccurredError(error, peerId);
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
    INVOKE([&]{ this->AddIceCandidate(candidateMap, peerId); });
    Log(INFO) << "RTCCall AddIceCandidate, peerId:" << peerId
                << ", candidateMap:" << &candidateMap;
    
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
            std::vector<IceCandidatesRef> candidates{candidateRef};
            toBeAddedICEs_->emplace(peerId, candidates);
        }
    }
}

void RTCCall::SetConfigForVideoEncoder(const RTCVideoConfig& config,
                                       const RTCString& peerId) {
    INVOKE([&]{ this->SetConfigForVideoEncoder(config, peerId); });
    Log(INFO) << "RTCCall SetConfigForVideoEncoder, peerId:" << peerId
                << ", config:" << &config;
    
    scoped_refptr<PeerConnectionInterface> peer = FindPeerById(peerId);
    if (peer == nullptr) {
        Log(ERROR) << "peer[" << peerId
                    << "] doesn't exist, failed to set video encoder with config.";
        return;
    }
    
    RTCRtpSenderRef videoSender = VideoSenderFromPeer(peerId);
    if (videoSender == nullptr) {
        Log(ERROR) << "peer[" << peerId
                    << "] doesn't have video sender, failed to set video encoder with config.";
        return;
    }
    
    RtpParameters modifiedParams = videoSender->GetParameters();
    int encodingsCount = modifiedParams.encodings.size();
    std::vector<RtpEncodingParameters> encodings;
    for (int i = 0; i < encodingsCount; ++i) {
        RtpEncodingParameters encoding = modifiedParams.encodings[i];
        if (encodingsCount == 1) {
            // only one encoding means no simulcast, just set bitrate
            encoding.max_bitrate_bps = config.bitrate * 1000;
            if (encoding.min_bitrate_bps > 0) {
                encoding.min_bitrate_bps = config.minBitrate * 1000;
            }
        } else {
            // more than one encodings means simulcast is active
            if (i == 0) {
                // low definition simulcast layer, set bitrate and resolution
                encoding.max_bitrate_bps = config.bitrateLD * 1000;
                if (encoding.min_bitrate_bps > 0) {
                    encoding.min_bitrate_bps = config.minBitrateLD * 1000;
                }
            } else {
                // high definition simulcast layer, set bitrate and resolution
                encoding.max_bitrate_bps = config.bitrate * 1000;
                if (encoding.min_bitrate_bps > 0) {
                    encoding.min_bitrate_bps = config.minBitrate * 1000;
                }
            }
        }
        encoding.max_framerate = config.fps;
        encodings.push_back(encoding);
    }
    modifiedParams.encodings = encodings;
    videoSender->SetParameters(modifiedParams);
}

void RTCCall::EnableVideoEncoderLyaer(bool enable,
                                      RTCStreamVideoLayer layer,
                                      const RTCString& peerId) {
    INVOKE([&]{ this->EnableVideoEncoderLyaer(enable, layer, peerId); });
    Log(INFO) << "RTCCall EnableVideoEncoderLyaer, peerId:" << peerId
                << ", enable:" << enable;
    
    scoped_refptr<PeerConnectionInterface> peer = FindPeerById(peerId);
    if (peer == nullptr) {
        Log(ERROR) << "peer[" << peerId << "] doesn't exist, failed to "
                    << (enable?"enable":"disable") << " video encoder layer.";
        return;
    }
    
    RTCRtpSenderRef videoSender = VideoSenderFromPeer(peerId);
    if (videoSender == nullptr) {
        Log(ERROR) << "peer[" << peerId << "] doesn't have video sender, failed to "
                    << (enable?"enable":"disable") <<" video encoder layer.";
        return;
    }
    
    RtpParameters modifiedParams = videoSender->GetParameters();
    int encodingsCount = modifiedParams.encodings.size();
    std::vector<RtpEncodingParameters> encodings;
    for (int i = 0; i < encodingsCount; ++i) {
        RtpEncodingParameters encoding = modifiedParams.encodings[i];
        if (enable) {
            // enable all the encoding layers whose index(i) smaller than or equal to layer parameter
            if (i <= layer) {
                encoding.active = true;
            }
        } else {
            // disable all the encoding layers whose index(i) larger than or equal to layer parameter
            if (i >= layer) {
                encoding.active = false;
            }
        }
        encodings.push_back(encoding);
    }
    modifiedParams.encodings = encodings;
    videoSender->SetParameters(modifiedParams);
}

void RTCCall::SetVideoEncodeDegradationPreference(const webrtc::DegradationPreference preference,
                                                  const RTCString& peerId) {
    INVOKE([&]{ this->SetVideoEncodeDegradationPreference(preference, peerId); });
    Log(INFO) << "RTCCall SetVideoEncodeDegradationPreference, peerId:" << peerId
                << ", preference:" << &preference;
}

void RTCCall::EnableAudioSender(bool isEnabled,
                                const RTCString& peerId) {
    INVOKE([&]{ this->EnableAudioSender(isEnabled, peerId); });
    Log(INFO) << "RTCCall EnableAudioSender, peerId:" << peerId
                << ", isEnabled:" << isEnabled;
}

void RTCCall::EnableVideoSender(bool isEnabled,
                                const RTCString& peerId) {
    INVOKE([&]{ this->EnableVideoSender(isEnabled, peerId); });
    Log(INFO) << "RTCCall EnableVideoSender, peerId:" << peerId
                << ", isEnabled:" << isEnabled;
}

void RTCCall::EnableSendAudioTrack(bool isEnabled,
                                   const RTCString& peerId) {
    INVOKE([&]{ this->EnableSendAudioTrack(isEnabled, peerId); });
    Log(INFO) << "RTCCall EnableSendAudioTrack, peerId:" << peerId
                << ", isEnabled:" << isEnabled;
    
    scoped_refptr<PeerConnectionInterface> peer = FindPeerById(peerId);
    if (peer == nullptr) {
        Log(ERROR) << "peer[" << peerId << "] " << (isEnabled?"enable":"disable")
                    << "audioTrack in sending direction.";
        return;
    }
    
    RTCRtpSenderRef audioSender = AudioSenderFromPeer(peerId);
    if (audioSender == nullptr) {
        Log(ERROR) << "peer[" << peerId << "] doesn't exist, failed to "
                    << (isEnabled?"enable":"disable") <<" audioTrack in sending direction.";
        return;
    }
    // enable or disable the audioTrack
    audioSender->track()->set_enabled(isEnabled);
}

void RTCCall::EnableSendVideoTrack(bool isEnabled,
                                   const RTCString& peerId) {
    INVOKE([&]{ this->EnableSendVideoTrack(isEnabled, peerId); });
    Log(INFO) << "RTCCall EnableSendVideoTrack, peerId:" << peerId
                << ", isEnabled:" << isEnabled;
    
    scoped_refptr<PeerConnectionInterface> peer = FindPeerById(peerId);
    if (peer == nullptr) {
        Log(ERROR) << "peer[" << peerId << "] " << (isEnabled?"enable":"disable")
                    << "videoTrack in sending direction.";
        return;
    }
    
    RTCRtpSenderRef videoSender = VideoSenderFromPeer(peerId);
    if (videoSender == nullptr) {
        Log(ERROR) << "peer[" << peerId << "] doesn't exist, failed to "
                    << (isEnabled?"enable":"disable") <<" videoTrack in sending direction.";
        return;
    }
    // enable or disable the videoTrack
    videoSender->track()->set_enabled(isEnabled);
}

void RTCCall::EnableReceiveAudioTrack(bool isEnabled,
                                      const RTCString& trackId,
                                      const RTCString& peerId) {
    INVOKE([&]{ this->EnableReceiveAudioTrack(isEnabled, trackId, peerId); });
    Log(INFO) << "RTCCall EnableReceiveAudioTrack, peerId:" << peerId
                << ", trackId:" << trackId
                << ", isEnabled:" << isEnabled;
    
    scoped_refptr<PeerConnectionInterface> peer = FindPeerById(peerId);
    if (peer == nullptr) {
        Log(ERROR) << "peer[" << peerId << "] " << (isEnabled?"enable":"disable")
                    << "audioTrack in receiving direction.";
        return;
    }
    
    RTCRtpReceiverRef audioReceiver = AudioReceiverFromPeer(peerId);
    if (audioReceiver == nullptr) {
        Log(ERROR) << "peer[" << peerId << "] doesn't exist, failed to "
                    << (isEnabled?"enable":"disable") <<" audioTrack in receiving direction.";
        return;
    }
    // enable or disable the audioTrack
    audioReceiver->track()->set_enabled(isEnabled);
}

void RTCCall::EnableReceiveVideoTrack(bool isEnabled,
                                      const RTCString& trackId,
                                      const RTCString& peerId) {
    INVOKE([&]{ this->EnableReceiveVideoTrack(isEnabled, trackId, peerId); });
    Log(INFO) << "RTCCall EnableReceiveVideoTrack, peerId:" << peerId
                << ", trackId:" << trackId
                << ", isEnabled:" << isEnabled;
    
    scoped_refptr<PeerConnectionInterface> peer = FindPeerById(peerId);
    if (peer == nullptr) {
        Log(ERROR) << "peer[" << peerId << "] " << (isEnabled?"enable":"disable")
                    << "videoTrack in receiving direction.";
        return;
    }
    
    RTCRtpReceiverRef videoReceiver = VideoReceiverFromPeer(peerId);
    if (videoReceiver == nullptr) {
        Log(ERROR) << "peer[" << peerId << "] doesn't exist, failed to "
                    << (isEnabled?"enable":"disable") <<" receiving in sending direction.";
        return;
    }
    // enable or disable the videoTrack
    videoReceiver->track()->set_enabled(isEnabled);
}

void RTCCall::ResetCsrcByAudioTrackId(const RTCString& trackId,
                                      const RTCString& peerId) {
    INVOKE([&]{ this->ResetCsrcByAudioTrackId(trackId, peerId); });
    Log(INFO) << "RTCCall ResetCsrcByAudioTrackId, peerId:" << peerId
                << ", trackId:" << trackId;
}

void RTCCall::ResetCsrcByVideoTrackId(const RTCString& trackId,
                                      const RTCString& peerId) {
    INVOKE([&]{ this->ResetCsrcByVideoTrackId(trackId, peerId); });
    Log(INFO) << "RTCCall ResetCsrcByVideoTrackId, peerId:" << peerId
                << ", trackId:" << trackId;
}

void RTCCall::SetReceiveCsrc(uint32_t csrc,
                             const RTCString& trackId,
                             const RTCString& peerId) {
    INVOKE([&]{ this->SetReceiveCsrc(csrc, trackId, peerId); });
    Log(INFO) << "RTCCall SetReceiveCsrc, peerId:" << peerId
                << ", trackId:" << trackId
                << ", csrc:" << csrc;
}

void RTCCall::SetVideoContentType(uint32_t contentType,
                                  const RTCString& trackId,
                                  const RTCString& peerId) {
    INVOKE([&]{ this->SetVideoContentType(contentType, trackId, peerId); });
    Log(INFO) << "RTCCall SetVideoContentType, peerId:" << peerId
                << ", trackId:" << trackId
                << ", contentType:" << contentType;
}

void RTCCall::ResetSimulcastIdByVideoTrackId(const RTCString& trackId,
                                             const RTCString& peerId) {
    INVOKE([&]{ this->ResetSimulcastIdByVideoTrackId(trackId, peerId); });
    Log(INFO) << "RTCCall ResetSimulcastIdByVideoTrackId, peerId:" << peerId
                << ", trackId:" << trackId;
}

void RTCCall::SendSEI(unsigned char *seiData,
                      const RTCString& peerId) {
    INVOKE([&]{ this->SendSEI(seiData, peerId); });
    Log(INFO) << "RTCCall SendSEI, peerId:" << peerId
                << ", seiData:" << seiData;
}

void RTCCall::GetStats(std::function<void(const std::unordered_map<RTCString, RTCPeerStatsModel>& stats,
                                          const webrtc::RTCError &error)>
                       predicate) {
    INVOKE([&]{ this->GetStats(predicate); });
    Log(INFO) << "RTCCall GetStats, predicate:" << &predicate;
}

std::vector<RTCPeerStatsModel>*
RTCCall::GetAudioLevelStats() {
    INVOKE_RETURN(std::vector<RTCPeerStatsModel>*, [&]{
        return this->GetAudioLevelStats();
    })
    Log(INFO) << "RTCCall GetAudioLevelStats.";
    return nullptr;
}

void RTCCall::GetStatsFormEngine(std::function<
                                 void(std::vector<RTCPeerStatsModel> statsAry)>
                                 predicate) {
    INVOKE([&]{ this->GetStatsFormEngine(predicate); });
    Log(INFO) << "RTCCall GetStatsFormEngine, predicate:" << &predicate;
}

std::vector<RTCPeerStatsModel>*
RTCCall::GetPeerStats() {
    INVOKE_RETURN(std::vector<RTCPeerStatsModel>*, [&]{
        return this->GetPeerStats();
    })
    Log(INFO) << "RTCCall GetPeerStats.";
    return nullptr;
}

bool RTCCall::InsertDTMF(const RTCString& tones,
                         double duration,
                         double interToneGap,
                         const RTCString& peerId) {
    INVOKE_RETURN(bool, [&]{
        return this->InsertDTMF(tones, duration, interToneGap, peerId);
    })
    Log(INFO) << "RTCCall SetReceiveCsrc, peerId:" << peerId
                << ", tones:" << tones
                << ", duration:" << duration
                << ", interToneGap" << interToneGap;
    
    scoped_refptr<PeerConnectionInterface> peer = FindPeerById(peerId);
    if (peer == nullptr) {
        Log(ERROR) << "peer[" << peerId << "] " << "doesn't exist, failed to insert dtmf.";
        return false;
    }
    
    RTCRtpSenderRef audioSender = AudioSenderFromPeer(peerId);
    if (audioSender == nullptr) {
        Log(ERROR) << "peer[" << peerId << "] doesn't exist audio sender.";
        return false;
    }
    
    if (!audioSender->GetDtmfSender()->CanInsertDtmf()) {
        Log(ERROR) << "peer[" << peerId << "] cann't insert dtmf.";
        return false;
    }
    
    return audioSender->GetDtmfSender()->InsertDtmf(tones, duration, interToneGap);
}
 
void RTCCall::ReleasePeer(const RTCString& peerId) {
    INVOKE([&]{ this->ReleasePeer(peerId); });
    Log(INFO) << "RTCCall ReleasePeer, peerId:" << peerId;
    
    scoped_refptr<PeerConnectionInterface> peer = FindPeerById(peerId);
    if (peer != nullptr) {
        Log(INFO) << "release peer[" << peerId << "].";
        peer->Close();
        peer = nullptr;
        peerStates_->erase(peerId);
        toBeAddedICEs_->erase(peerId);
    }
}

void RTCCall::ReleaseRTCCall() {
    INVOKE([&]{ this->ReleaseRTCCall(); });
    Log(INFO) << "try to release RTCCall(" << this << ").";
    // stop timer of peer state checking
    StopPeerStateCheckTimer();
    
    if (peerStates_->size() > 0) {
        for (auto it = peerStates_->begin(); it != peerStates_->end(); ++it) {
            auto peer = it->second->peer;
            peer->Close();
        }
    }
    
    peerStates_->clear();
    toBeAddedICEs_->clear();
    videoTrackSources_->clear();
}

void RTCCall::SetIsAllAudioSenderMute(bool isMute) {
    INVOKE([&]{ this->SetIsAllAudioSenderMute(isMute); });
    if (isAllAudioSenderMute_ == isMute) {
        Log(INFO) << "SetIsAllAudioSenderMute is already "
                    << (isMute?"mute":"unmute") <<", ignore setting it.";
        return;
    }
    for (auto it = peerStates_->begin(); it != peerStates_->end(); ++it) {
        RTCPeerStatusModelRef obj = it->second;
        for (RTCRtpSenderRef sender : obj->senders) {
            if (sender->track()->kind() == MediaStreamTrackInterface::kAudioKind) {
                sender->track()->set_enabled(!isMute);
            }
        }
    }
}

bool RTCCall::GetIsAllAudioSenderMute() const {
    INVOKE_RETURN(bool, [&]{return this->isAllAudioSenderMute_;})
    return isAllAudioSenderMute_;
}

void RTCCall::SetIsAllAudioReceiverMute(bool isMute) {
    INVOKE([&]{ this->SetIsAllAudioReceiverMute(isMute); });
    if (isAllAudioReceiverMute_ == isMute) {
        Log(INFO) << "SetIsAllAudioReceiverMute is already "
                    << (isMute?"mute":"unmute") <<", ignore setting it.";
        return;
    }
    for (auto it = peerStates_->begin(); it != peerStates_->end(); ++it) {
        RTCPeerStatusModelRef obj = it->second;
        for (RTCRtpReceiverRef receiver : obj->receivers) {
            if (receiver->track()->kind() == MediaStreamTrackInterface::kAudioKind) {
                receiver->track()->set_enabled(!isMute);
            }
        }
    }
}

bool RTCCall::GetIsAllAudioReceiverMute() const {
    INVOKE_RETURN(bool, [&]{return this->isAllAudioReceiverMute_;})
    return isAllAudioReceiverMute_;
}

#pragma mark - Public

/*----------------------------------------------------------------------------*/

void RTCCall::DidCreateSDP(const RTCString& peerId,
                           SessionDescriptionInterface* desc,
                           RTCError error) {
    if (!error.ok()) {
        Log(ERROR) << "peer[" << peerId <<"] failed to create sdp with error: "
                    << error.message();
        observer_->OnOccurredError(error, peerId);
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
    
    // trigger OnCreateSdp callback
    std::string sdp;
    desc->ToString(&sdp);
    RTCStringMap sdpDict {
        {kRTCSessionDescriptionSdpName, sdp},
        {kRTCSessionDescriptionTypeName, desc->type()}
    };
    this->observer_->OnCreateSdp(sdpDict, peerId);
}

void RTCCall::DidSetSDP(const RTCString& peerId,
                        RTCError error) {
    if (!error.ok()) {
        RTCString errorStr("peer[");
        errorStr.append(peerId);
        errorStr.append("] failed to set sdp with error:");
        errorStr.append(error.message());
        Log(ERROR) << errorStr;
        RTCError error(RTCErrorType::INTERNAL_ERROR, errorStr);
        observer_->OnOccurredError(error, peerId);
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
    
    using pc = PeerConnectionInterface;
    pc::IceServer turnServer;
    turnServer.tls_cert_policy = pc::kTlsCertPolicySecure;
    turnServer.urls = baseConfig_.interTurns;
    turnServer.username = baseConfig_.turnUserName;
    turnServer.password = baseConfig_.turnCredential;

    pc::RTCConfiguration config;
    config.servers = {turnServer};
    config.sdp_semantics = SdpSemantics::kUnifiedPlan;
    config.type = pc::IceTransportsType::kRelay;
    config.bundle_policy = pc::BundlePolicy::kBundlePolicyMaxBundle;
    config.candidate_network_policy = pc::CandidateNetworkPolicy::kCandidateNetworkPolicyLowCost;
    config.continual_gathering_policy = pc::ContinualGatheringPolicy::GATHER_CONTINUALLY;
    config.disable_ipv6 = true;
    config.tcp_candidate_policy = pc::TcpCandidatePolicy::kTcpCandidatePolicyDisabled;
    
    RTCOberverInternalRef observerInternal = std::make_shared<RTCObserverInternal>();
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
        peerStatus->createdTimestamp = RTCTimeIntervalSince1970();
        peerStatus->lastIceState = PeerConnectionInterface::IceConnectionState::kIceConnectionNew;
        peerStatus->lastUpdateTimestamp = peerStatus->createdTimestamp;
        
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
    
    // start timer for checking peer state
    StartPeerStateCheckTimer();

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
    RTCPeerStatusModelRef statusModel = FindPeerStatusModelById(peerId);
    if (statusModel == nullptr) {
        Log(ERROR) << "peer[" << peerId << "] doesn't exist, failed to update ice state";
        return ;
    }
    statusModel->lastIceState = state;
    statusModel->lastUpdateTimestamp = RTCTimeIntervalSince1970();
}

RTCRtpSenderRef RTCCall::VideoSenderFromPeer(const RTCString& peerId) {
    RTCPeerStatusModelRef statusModel = FindPeerStatusModelById(peerId);
    if (statusModel != nullptr) {
        for (auto sender : statusModel->senders) {
            if (sender->track()->kind() == MediaStreamTrackInterface::kVideoKind) {
                return sender;
            }
        }
    }
    return nullptr;
}

RTCRtpSenderRef RTCCall::AudioSenderFromPeer(const RTCString& peerId) {
    RTCPeerStatusModelRef statusModel = FindPeerStatusModelById(peerId);
    if (statusModel != nullptr) {
        for (auto sender : statusModel->senders) {
            if (sender->track()->kind() == MediaStreamTrackInterface::kAudioKind) {
                return sender;
            }
        }
    }
    return nullptr;
}

RTCRtpReceiverRef RTCCall::VideoReceiverFromPeer(const RTCString& peerId) {
    RTCPeerStatusModelRef statusModel = FindPeerStatusModelById(peerId);
    if (statusModel != nullptr) {
        for (auto reciver : statusModel->receivers) {
            if (reciver->track()->kind() == MediaStreamTrackInterface::kVideoKind) {
                return reciver;
            }
        }
    }
    return nullptr;
}

RTCRtpReceiverRef RTCCall::AudioReceiverFromPeer(const RTCString& peerId) {
    RTCPeerStatusModelRef statusModel = FindPeerStatusModelById(peerId);
    if (statusModel != nullptr) {
        for (auto reciver : statusModel->receivers) {
            if (reciver->track()->kind() == MediaStreamTrackInterface::kAudioKind) {
                return reciver;
            }
        }
    }
    return nullptr;;
}

void RTCCall::StartPeerStateCheckTimer() {
    if (peerStateCheckTimer_ != nullptr) {
        Log(WARNING) << "PeerStateCheckTimer is already running.";
        return;
    }
    int runningTime = 0;
    peerStateCheckTimer_ = std::make_shared<util::AsynTimer>();
    peerStateCheckTimer_->Start([this, &runningTime](void* pUser) {
        runningTime = runningTime + iceStateCheckRate_;
        Log(INFO) << "PeerStateCheckTimer has been running for "
                    << runningTime << " second(s).";
        CheckPeersState();
    }, iceStateCheckRate_*TIME_NSEC_PER_SEC, true, NULL);
}

void RTCCall::StopPeerStateCheckTimer() {
    if (peerStateCheckTimer_ != nullptr && !peerStateCheckTimer_->IsRunable()) {
        Log(WARNING) << "PeerStateCheckTimer is stopped.";
        peerStateCheckTimer_->Stop();
        peerStateCheckTimer_ = nullptr;
    }
}

/// check every peer's ice state, check if there is any connection timeout peers.
void RTCCall::CheckPeersState() {
    // get current timeStamp
    int64_t currentTime = RTCTimeIntervalSince1970();
    // hasUnconnectedPeer: if there any unconnected peer, default is NO
    bool hasUnconnectedPeer = false;
    for (auto it = peerStates_->begin(); it != peerStates_->end(); ++it) {
        RTCPeerStatusModelRef obj = it->second;
        if (obj->lastIceState < RTCIceConnectionState::kIceConnectionCompleted) {
            // has unconnected peer
            hasUnconnectedPeer = true;
            // calculate the connecting time so far
            int64_t connectingTime = currentTime - obj->lastUpdateTimestamp;
            // check if connectingTime is above kIceConnectingTime
            if (connectingTime > iceConnectionTimeout_) {
                Log(ERROR) << "peer[" << obj->peerId
                            << "] timed out, it has been connecting over "
                            << iceConnectionTimeout_ << " seconds.";
                observer_->OnPeerStateChangge(RTCPeerState::Failed, obj->peerId);
            }
        }
    }
    // if there is no unconnected peer then stop the timer.'
    if (!hasUnconnectedPeer) {
        StopPeerStateCheckTimer();
    }
}

#pragma mark - Private

scoped_refptr<PeerConnectionInterface>
RTCCall::FindPeerById(const RTCString& peerId) {
    RTCPeerStatusModelRef statusModel = FindPeerStatusModelById(peerId);
    if (statusModel == nullptr) {
        return nullptr;
    }
    return statusModel->peer;
}

RTCPeerStatusModelRef RTCCall::FindPeerStatusModelById(const RTCString& peerId) {
    RTCPeerStatusModelMap::iterator it = peerStates_->find(peerId);
    if (it == peerStates_->end()) {
        return nullptr;
    }
    return it->second;
}

#pragma mark -

/*----------------------------------------------------------------------------*/

std::optional<std::shared_ptr<RTCCallInterface>>
CreateRTCCallOrNull(std::unique_ptr<RTCCallObserverInterface> observer,
                    RTCBaseConfig& config) {
    std::optional<std::shared_ptr<RTCCallInterface>> ret = std::nullopt;
    std::shared_ptr<RTCCallInterface> call = std::make_shared<RTCCall>(std::move(observer),
                                                                       config);
    if (call != nullptr) {
        ret = call;
    }
    return ret;
}

webrtc::RTCErrorOr<std::shared_ptr<RTCCallInterface>>
CreateRTCCallOrError(std::unique_ptr<RTCCallObserverInterface> observer,
                     RTCBaseConfig& config) {
    auto ret = webrtc::RTCErrorOr<std::shared_ptr<RTCCallInterface>>();
    std::shared_ptr<RTCCallInterface> call = std::make_shared<RTCCall>(std::move(observer),
                                                                       config);
    if (call != nullptr) {
        ret = call;
    }
    return ret;
}

RTCString getWebRTCVersion(void) {
    return "M106";
}

}

