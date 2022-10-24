/*
 *
 * rtc_internal_observer.cc
 *
 * Created by Ruibin.Chow on 2022/10/24.
 * Copyright (c) 2022年 Ruibin.Chow All rights reserved.
 */

#include "rtc_internal_observer.h"
#include "../rtc_call.h"
#include "../../log/logging.h"
#include "rtc_util.h"

namespace core {


void RTCObserverInternal::OnSignalingChange(PeerConnectionInterface::SignalingState new_state)  {
    
}

// Triggered when a remote peer opens a data channel.
void RTCObserverInternal::OnDataChannel(rtc::scoped_refptr<DataChannelInterface> data_channel)  {
    
}

void RTCObserverInternal::OnIceConnectionChange(PeerConnectionInterface::IceConnectionState new_state) {
    Log(INFO) << "peer[" << peerId_ << "] OnIceConnectionChange: "
                << RTCStringByIceConnectionState(new_state);
    call_->UpdateIceState(peerId_, new_state);
    
    switch (new_state) {
        case PeerConnectionInterface::IceConnectionState::kIceConnectionConnected: {
            observer_->OnPeerStateChangge(RTCPeerState::Connected, peerId_);
            // TODO
            break;
        }
        case PeerConnectionInterface::IceConnectionState::kIceConnectionDisconnected: {
            observer_->OnPeerStateChangge(RTCPeerState::Disconnected, peerId_);
            break;
        }
        case PeerConnectionInterface::IceConnectionState::kIceConnectionFailed: {
            // TODO
            observer_->OnPeerStateChangge(RTCPeerState::Failed, peerId_);
            break;
        }
        case PeerConnectionInterface::IceConnectionState::kIceConnectionClosed: {
            observer_->OnPeerStateChangge(RTCPeerState::Closed, peerId_);
            call_->toBeAddedICEs_->erase(peerId_);
            break;
        }
        default:
            break;
    }
}

// Called any time the standards-compliant IceConnectionState changes.
void RTCObserverInternal::OnStandardizedIceConnectionChange(PeerConnectionInterface::IceConnectionState new_state)  {
    
}

// Called any time the PeerConnectionState changes.
void RTCObserverInternal::OnConnectionChange(PeerConnectionInterface::PeerConnectionState new_state)  {
    
}

// Called any time the IceGatheringState changes.
void RTCObserverInternal::OnIceGatheringChange(PeerConnectionInterface::IceGatheringState new_state)  {
    
}

// A new ICE candidate has been gathered.
void RTCObserverInternal::OnIceCandidate(const IceCandidateInterface* candidate)  {
    
}

void RTCObserverInternal::OnIceCandidateError(const RTCString& address,
                                              int port,
                                              const RTCString& url,
                                              int error_code,
                                              const RTCString& error_text)  {
    
}

void RTCObserverInternal::OnIceCandidatesRemoved(const std::vector<cricket::Candidate>& candidates)  {
    
}

void RTCObserverInternal::OnIceConnectionReceivingChange(bool receiving)  {
    
}

void RTCObserverInternal::OnIceSelectedCandidatePairChanged(const cricket::CandidatePairChangeEvent& event)  {
                                                   
}

void RTCObserverInternal::OnAddTrack(rtc::scoped_refptr<RtpReceiverInterface> receiver,
                                     const std::vector<rtc::scoped_refptr<MediaStreamInterface>>& streams)  {
    
}

void RTCObserverInternal::OnTrack(rtc::scoped_refptr<RtpTransceiverInterface> transceiver)  {
    
}

void RTCObserverInternal::OnRemoveTrack(rtc::scoped_refptr<RtpReceiverInterface> receiver)  {
    
}

}

