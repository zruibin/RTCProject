/*
 *
 * rtc_internal_observer.cc
 *
 * Created by Ruibin.Chow on 2022/10/24.
 * Copyright (c) 2022年 Ruibin.Chow All rights reserved.
 */

#include "core/internal/rtc_internal_observer.h"
#include <api/jsep_ice_candidate.h>
#include "core/rtc_call.h"
#include "log/logging.h"
#include "core/internal/rtc_util.h"

namespace core {


void RTCObserverInternal::OnSignalingChange(PeerConnectionInterface::SignalingState new_state)  {
    Log(INFO) << "peer[" << peerId_ << "] OnSignalingChange: "
                << PeerConnectionInterface::AsString(new_state).data();
}

// Triggered when a remote peer opens a data channel.
void RTCObserverInternal::OnDataChannel(rtc::scoped_refptr<DataChannelInterface> data_channel)  {
    Log(INFO) << "peer[" << peerId_ << "] OnDataChannel: "
                << DataChannelInterface::DataStateString(data_channel->state());
}

void RTCObserverInternal::OnIceConnectionChange(RTCIceConnectionState new_state) {
    Log(INFO) << "peer[" << peerId_ << "] OnIceConnectionChange: "
                << PeerConnectionInterface::AsString(new_state).data();
    call_->UpdateIceState(peerId_, new_state);
    switch (new_state) {
        case RTCIceConnectionState::kIceConnectionConnected: {
            observer_->OnPeerStateChangge(RTCPeerState::Connected, peerId_);
            break;
        }
        case RTCIceConnectionState::kIceConnectionDisconnected: {
            observer_->OnPeerStateChangge(RTCPeerState::Disconnected, peerId_);
            break;
        }
        case RTCIceConnectionState::kIceConnectionFailed: {
            // start traceRoute when ice failed ?
            
            observer_->OnPeerStateChangge(RTCPeerState::Failed, peerId_);
            break;
        }
        case RTCIceConnectionState::kIceConnectionClosed: {
            observer_->OnPeerStateChangge(RTCPeerState::Closed, peerId_);
            call_->toBeAddedICEs_->erase(peerId_);
            break;
        }
        default:
            break;
    }
}

// Called any time the standards-compliant IceConnectionState changes.
void RTCObserverInternal::OnStandardizedIceConnectionChange(RTCIceConnectionState new_state)  {
    Log(INFO) << "peer[" << peerId_ << "] OnStandardizedIceConnectionChange: "
                << PeerConnectionInterface::AsString(new_state).data();
}

// Called any time the PeerConnectionState changes.
void RTCObserverInternal::OnConnectionChange(RTCPeerConnectionState new_state)  {
    Log(INFO) << "peer[" << peerId_ << "] OnConnectionChange: "
                << PeerConnectionInterface::AsString(new_state).data();
}

// Called any time the IceGatheringState changes.
void RTCObserverInternal::OnIceGatheringChange(PeerConnectionInterface::IceGatheringState new_state)  {
    Log(INFO) << "peer[" << peerId_ << "] OnIceGatheringChange: "
                << PeerConnectionInterface::AsString(new_state).data();
}

// A new ICE candidate has been gathered.
void RTCObserverInternal::OnIceCandidate(const IceCandidateInterface* candidate)  {
    Log(INFO) << "peer[" << peerId_ << "] OnIceCandidate.";
    // trigger OnGenerateIceCandidate callback
    std::string sdp;
    candidate->ToString(&sdp);
    RTCStringMap iceDict {
        {kRTCCandidateSdpName, sdp},
        {kRTCCandidateSdpMidName, candidate->sdp_mid()},
        {kRTCCandidateSdpMlineIndexName, std::to_string(candidate->sdp_mline_index())}
    };
    observer_->OnGenerateIceCandidate(iceDict, peerId_);
}

void RTCObserverInternal::OnIceCandidateError(const RTCString& address,
                                              int port,
                                              const RTCString& url,
                                              int error_code,
                                              const RTCString& error_text)  {
    Log(INFO) << "peer[" << peerId_ << "] OnIceCandidateError, "
                << "address: " << address << ", port:" << std::to_string(port)
                << ", url:" << url << ", error_code:" << std::to_string(error_code)
                << ", error_text:" << error_text;
}

void RTCObserverInternal::OnIceCandidatesRemoved(const std::vector<cricket::Candidate>& candidates)  {
    Log(INFO) << "peer[" << peerId_ << "] OnIceCandidatesRemoved.";
    
    std::vector<RTCStringMap> candidateDictAry(candidates.size());
    for (auto candidate : candidates) {
        std::unique_ptr<JsepIceCandidate> candidate_wrapper(
            new JsepIceCandidate(candidate.transport_name(), -1, candidate));
        std::string sdp;
        candidate_wrapper->ToString(&sdp);
        RTCStringMap iceDict {
            {kRTCCandidateSdpName, sdp},
            {kRTCCandidateSdpMidName, candidate_wrapper->sdp_mid()},
            {kRTCCandidateSdpMlineIndexName, std::to_string(candidate_wrapper->sdp_mline_index())}
        };
        candidateDictAry.push_back(iceDict);
    }
    
    observer_->OnRemoveIceCandidates(candidateDictAry, peerId_);
}

void RTCObserverInternal::OnAddTrack(rtc::scoped_refptr<RtpReceiverInterface> receiver,
                                     const std::vector<rtc::scoped_refptr<MediaStreamInterface>>& streams)  {
    Log(INFO) << "peer[" << peerId_ << "] OnAddTrack, receiver:" << receiver->id()
                << ", streamsCount:" << streams.size();
    
    RTCPeerStatusModelRef statusModel = call_->FindPeerStatusModelById(peerId_);
    statusModel->receivers.push_back(receiver);
    
    if (receiver->track()->kind() == MediaStreamTrackInterface::kVideoKind) {
        // video track
        scoped_refptr<VideoTrackInterface> videoTrack = nullptr;
        for (scoped_refptr<MediaStreamInterface> strem: streams) {
            videoTrack = strem->FindVideoTrack(receiver->track()->id());
            break;
        }
        if (videoTrack == nullptr) {
            Log(ERROR) << "peer[" << peerId_ << "] OnAddTrack, receiver:" << receiver->id()
                        << ", trackId:" << receiver->track()->id()
                        << "video track was nullptr.";
        }
        // trigger OnAddReceivedVideoTrack callback
        observer_->OnAddReceivedVideoTrack(videoTrack, peerId_);
    } else if (receiver->track()->kind() == MediaStreamTrackInterface::kAudioKind) {
        // audio track
        // set track's isEnabled flag according to isAllAudioReceiverMute_
        if (call_->GetIsAllAudioReceiverMute()) {
            receiver->track()->set_enabled(false);
        }

        scoped_refptr<AudioTrackInterface> audioTrack = nullptr;
        for (scoped_refptr<MediaStreamInterface> strem: streams) {
            audioTrack = strem->FindAudioTrack(receiver->track()->id());
            break;
        }
        if (audioTrack == nullptr) {
            Log(ERROR) << "peer[" << peerId_ << "] OnAddTrack, receiver:" << receiver->id()
                        << ", trackId:" << receiver->track()->id()
                        << "audio track was nullptr.";
        }
        // trigger OnAddReceivedAudioTrack callback
        observer_->OnAddReceivedAudioTrack(audioTrack, peerId_);
    }
}

void RTCObserverInternal::OnTrack(rtc::scoped_refptr<RtpTransceiverInterface> transceiver)  {
    Log(INFO) << "peer[" << peerId_ << "] OnTrack";
}

void RTCObserverInternal::OnRemoveTrack(rtc::scoped_refptr<RtpReceiverInterface> receiver)  {
    Log(INFO) << "peer[" << peerId_ << "] OnRemoveTrack, receiver:" << receiver->id();
}

}

