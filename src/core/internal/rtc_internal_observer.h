/*
 * rtc_internal_observer.h
 *
 * Created by Ruibin.Chow on 2022/10/24.
 * Copyright (c) 2022年 Ruibin.Chow All rights reserved.
 */

#ifndef RTC_INTERNAL_OBSERVER_H
#define RTC_INTERNAL_OBSERVER_H

#include <api/peer_connection_interface.h>
#include "../rtc_call_observer_interface.h"

namespace core {

using namespace::webrtc;
using namespace::rtc;
using RTCIceConnectionState = PeerConnectionInterface::IceConnectionState;
using RTCPeerConnectionState = PeerConnectionInterface::PeerConnectionState;

class RTCCall;

class RTCObserverInternal : public PeerConnectionObserver {

public:
    RTCObserverInternal() = default;
    virtual ~RTCObserverInternal() = default;
    void SetObserver(RTCCallObserverInterface* observer) {
        observer_ = observer;
    }
    void SetCall(RTCCall* call) {
        call_ = call;
    };
    void SetPeerId(const RTCString& peerId) {
        peerId_ = peerId;
    }
    
public:
    void OnSignalingChange(PeerConnectionInterface::SignalingState new_state) override;
    void OnDataChannel(rtc::scoped_refptr<DataChannelInterface> data_channel) override;
    void OnIceConnectionChange(RTCIceConnectionState new_state) override;
    void OnStandardizedIceConnectionChange(RTCIceConnectionState new_state) override;
    void OnConnectionChange(RTCPeerConnectionState new_state) override;
    void OnIceGatheringChange(PeerConnectionInterface::IceGatheringState new_state) override;
    void OnIceCandidate(const IceCandidateInterface* candidate) override;
    void OnIceCandidateError(const RTCString& address,
                             int port,
                             const RTCString& url,
                             int error_code,
                             const RTCString& error_text) override;
    void OnIceCandidatesRemoved(const std::vector<cricket::Candidate>& candidates) override;
    void OnAddTrack(rtc::scoped_refptr<RtpReceiverInterface> receiver,
                    const std::vector<rtc::scoped_refptr<MediaStreamInterface>>& streams) override;
    void OnTrack(rtc::scoped_refptr<RtpTransceiverInterface> transceiver) override;
    void OnRemoveTrack(rtc::scoped_refptr<RtpReceiverInterface> receiver) override;

private:
    RTCCallObserverInterface* observer_;
    RTCCall* call_;
    RTCString peerId_;
};

/*----------------------------------------------------------------------------*/

class CreateSDPObserverAdapter : public CreateSessionDescriptionObserver {
public:
    using SDPHandler = std::function<void(const RTCString& peerId,
                                          SessionDescriptionInterface* desc,
                                          RTCError error)>;
    CreateSDPObserverAdapter() = default;
    CreateSDPObserverAdapter(const RTCString& peerId, SDPHandler handler) {
        peerId_ = peerId;
        handler_ = handler;
    }
    
    ~CreateSDPObserverAdapter() override {
        handler_ = nullptr;
    }
    
    void OnSuccess(SessionDescriptionInterface* desc) override {
        handler_(peerId_, desc, RTCError(RTCErrorType::NONE));
        handler_ = nullptr;
    }
    
    void OnFailure(RTCError error) override {
        handler_(peerId_, nullptr, error);
        handler_ = nullptr;
    }
    
private:
    SDPHandler handler_;
    RTCString peerId_;
};

/*----------------------------------------------------------------------------*/

class SetSDPObserverAdapter : public SetSessionDescriptionObserver {
public:
    using SDPHandler = std::function<void(const RTCString& peerId, RTCError error)>;
    SetSDPObserverAdapter() = default;
    SetSDPObserverAdapter(const RTCString& peerId, SDPHandler handler) {
        peerId_ = peerId;
        handler_ = handler;
    }
    
    ~SetSDPObserverAdapter() override { handler_ = nullptr; }
    
    void OnSuccess() override {
        handler_(peerId_, RTCError(RTCErrorType::NONE));
        handler_ = nullptr;
    }
    
    void OnFailure(RTCError error) override {
        handler_(peerId_, error);
        handler_ = nullptr;
    }
    
private:
    SDPHandler handler_;
    RTCString peerId_;
};

}

#endif /* !RTC_INTERNAL_OBSERVER_H */
