/*
 *
 * rtc_call.cc
 *
 * Created by Ruibin.Chow on 2022/10/20.
 * Copyright (c) 2022年 Ruibin.Chow All rights reserved.
 */

#include "rtc_call.h"
#include <api/create_peerconnection_factory.h>
#include <api/media_stream_interface.h>
#include <api/peer_connection_interface.h>
#include <api/rtc_event_log/rtc_event_log_factory.h>
#include <api/audio_codecs/builtin_audio_decoder_factory.h>
#include <api/audio_codecs/builtin_audio_encoder_factory.h>
#include <api/video_codecs/builtin_video_decoder_factory.h>
#include <api/video_codecs/builtin_video_encoder_factory.h>
//#include <media/engine/webrtcvideocapturerfactory.h>
#include <modules/audio_device/include/audio_device.h>
#include <modules/audio_processing/include/audio_processing.h>
#include <modules/video_capture/video_capture_factory.h>
#include <rtc_base/checks.h>
#include <rtc_base/logging.h>
//#include <rtc_base/strings/json.h>
#include "../log/logging.h"

namespace core {

class RTCObserverInternal : public PeerConnectionObserver {

public:
    virtual ~RTCObserverInternal() = default;
    void SetObserver(RTCCallObserverInterface* observer) {
        observer_ = observer;
    }
    
public:
    // Triggered when the SignalingState changed.
    void OnSignalingChange(PeerConnectionInterface::SignalingState new_state) override {
        
    };

    // Triggered when a remote peer opens a data channel.
    void OnDataChannel(rtc::scoped_refptr<DataChannelInterface> data_channel) override {
        
    };
    
    void OnIceConnectionChange(PeerConnectionInterface::IceConnectionState new_state) override {
        
    };
    
    // Called any time the standards-compliant IceConnectionState changes.
    void OnStandardizedIceConnectionChange(PeerConnectionInterface::IceConnectionState new_state) override {
        
    }
    
    // Called any time the PeerConnectionState changes.
    void OnConnectionChange(PeerConnectionInterface::PeerConnectionState new_state) override {
        
    }
    
    // Called any time the IceGatheringState changes.
    void OnIceGatheringChange(PeerConnectionInterface::IceGatheringState new_state) override {
        
    };
    
    // A new ICE candidate has been gathered.
    void OnIceCandidate(const IceCandidateInterface* candidate) override {
        
    }
    
    void OnIceCandidateError(const RTCString& address,
                             int port,
                             const RTCString& url,
                             int error_code,
                             const RTCString& error_text) override {
        
    }
    
    void OnIceCandidatesRemoved(const std::vector<cricket::Candidate>& candidates) override {
        
    }
    
    void OnIceConnectionReceivingChange(bool receiving) override {
        
    }
    
    void OnIceSelectedCandidatePairChanged(const cricket::CandidatePairChangeEvent& event) override {
                                                       
    }
    
    void OnAddTrack(rtc::scoped_refptr<RtpReceiverInterface> receiver,
                    const std::vector<rtc::scoped_refptr<MediaStreamInterface>>& streams) override {
        
    }
    
    void OnTrack(rtc::scoped_refptr<RtpTransceiverInterface> transceiver) override {
        
    }
    
    void OnRemoveTrack(rtc::scoped_refptr<RtpReceiverInterface> receiver) override {
        
    }

private:
    RTCCallObserverInterface* observer_;
};

/*----------------------------------------------------------------------------*/

RTCCall::RTCCall(std::unique_ptr<RTCCallObserverInterface> observer):
        observer_(std::move(observer)),
        peerStates_(new RTCPeerStatusModelMap),
        videoSources_(new RTCVideoSourceMap),
        toBeAddedICEs_(new IceCandidatesMap) {
}

RTCCall::~RTCCall() {
    if (this->peerStates_) {
        delete this->peerStates_;
    }
    if (this->videoSources_) {
        delete this->videoSources_;
    }
    if (this->toBeAddedICEs_) {
        delete this->toBeAddedICEs_;
    }
}


void RTCCall::Init(void) {
//    std::unique_ptr<rtc::Thread> network_thread_ = rtc::Thread::CreateWithSocketServer();
//    network_thread_->SetName("network_thread", nullptr);
//    RTC_CHECK(network_thread_->Start()) << "Failed to start thread";
//
//    std::unique_ptr<rtc::Thread> worker_thread_ = rtc::Thread::Create();
//    worker_thread_->SetName("worker_thread", nullptr);
//    RTC_CHECK(worker_thread_->Start()) << "Failed to start thread";
//
//    std::unique_ptr<rtc::Thread> signaling_thread_ = rtc::Thread::Create();
//    signaling_thread_->SetName("signaling_thread", nullptr);
//    RTC_CHECK(signaling_thread_->Start()) << "Failed to start thread";
    
    this->peerFactory_ = CreatePeerConnectionFactory(
            nullptr /* network_thread */,
            nullptr/* worker_thread */,
            nullptr /* signaling_thread */,
            nullptr /* default_adm */,
            CreateBuiltinAudioEncoderFactory(),
            CreateBuiltinAudioDecoderFactory(),
            CreateBuiltinVideoEncoderFactory(),
            CreateBuiltinVideoDecoderFactory(),
            nullptr /* audio_mixer */,
            nullptr /* audio_processing */);
    
    auto peerConnection = this->createPeer("");
}

void RTCCall::TransferNetTypeToWebrtc(RTCNetType netType,
                                      bool isInitial) {
    
}

std::shared_ptr<RTCVideoSource> RTCCall::GetVideoSource(const RTCString& peerId) {
    RTCVideoSource videoSource;
    RTCVideoSourceMap::iterator it = videoSources_->find(peerId);
    if (it != videoSources_->end()) {
        videoSource = it->second;
    } else {

    }
    
    return std::make_shared<RTCVideoSource>(videoSource);;
}

void RTCCall::CreateOffer(RTCSdpType sdpType,
                          int layers,
                          const RTCString& peerId) {
    
}

void RTCCall::CreateAnswer(RTCSdpType sdpType,
                           const StringHashMap& offerMap,
                           const RTCString& peerId) {
    
}

void RTCCall::AddAnswer(const StringHashMap& answerMap,
                        const RTCString& peerId) {
    
}
 
void RTCCall::AddIceCandidate(const StringHashMap& candidateMap,
                              const RTCString& peerId) {
    
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

#pragma mark - Private
/*----------------------------------------------------------------------------*/

scoped_refptr<PeerConnectionInterface>
RTCCall::createPeer(const RTCString& peerId) {
    if (observer_ == nullptr) {
        Log(ERROR) << "observer was null.";
        return nullptr;
    }
    
    std::unique_ptr<RTCObserverInternal> observerInternal = std::make_unique<RTCObserverInternal>();
    observerInternal->SetObserver(this->observer_.get());
    
    PeerConnectionInterface::RTCConfiguration config;
    config.sdp_semantics = SdpSemantics::kUnifiedPlan;
    
//        PeerConnectionInterface::IceServer server;
//        server.uri = GetPeerConnectionString();
//        config.servers.push_back(server);

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

