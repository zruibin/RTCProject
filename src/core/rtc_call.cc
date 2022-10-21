/*
 *
 * rtc_call.cc
 *
 * Created by Ruibin.Chow on 2022/10/20.
 * Copyright (c) 2022年 Ruibin.Chow All rights reserved.
 */

#include "rtc_call.h"
#include <iostream>
#include <string>
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

namespace engine {

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
    
    void OnIceCandidateError(const std::string& address,
                             int port,
                             const std::string& url,
                             int error_code,
                             const std::string& error_text) override {
        
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

RTCCall::RTCCall(RTCCallObserverInterface *observer) {
    this->observer_ = observer;
}

RTCCall::~RTCCall() {
   
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

/*----------------------------------------------------------------------------*/

RTCErrorOr<rtc::scoped_refptr<PeerConnectionInterface>>
RTCCall::createPeer(const std::string_view& peerId) {
    if (observer_ == nullptr) {
        return RTCError(RTCErrorType::NONE, "observer was null.");
    }
    
    std::unique_ptr<RTCObserverInternal> observerInternal = std::make_unique<RTCObserverInternal>();
    observerInternal->SetObserver(this->observer_);
    
    PeerConnectionInterface::RTCConfiguration config;
    config.sdp_semantics = SdpSemantics::kUnifiedPlan;
    
//        PeerConnectionInterface::IceServer server;
//        server.uri = GetPeerConnectionString();
//        config.servers.push_back(server);

    PeerConnectionDependencies dependencies(observerInternal.get());
    auto peerConnection = this->peerFactory_->CreatePeerConnectionOrError(config, std::move(dependencies));
    if (!peerConnection.ok()) {
        const char* message = peerConnection.error().message();
        std::cout << "peerConnection error: " << message << std::endl;
    }
    return peerConnection;
}

/*----------------------------------------------------------------------------*/

std::shared_ptr<RTCCallInterface>
CreateRTCCall(RTCCallObserverInterface *observer) {
    std::shared_ptr<RTCCallInterface> call = std::make_shared<RTCCall>(observer);
    return call;
}

}

