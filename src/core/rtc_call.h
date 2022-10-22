/*
 * rtc_call.h
 *
 * Created by Ruibin.Chow on 2022/10/20.
 * Copyright (c) 2022年 Ruibin.Chow All rights reserved.
 */

#ifndef RTC_CALL_H
#define RTC_CALL_H

#include "rtc_call_interface.h"
#include <api/peer_connection_interface.h>
#include <api/jsep.h>
#include "rtc_object.h"

namespace core {

using namespace::webrtc;
using namespace::rtc;

class RTCCall : public RTCCallInterface {

public:
    explicit RTCCall(RTCCallObserverInterface *);
    virtual ~RTCCall();
    void Init(void) override;
    
private:
    RTCErrorOr<scoped_refptr<PeerConnectionInterface>>
    createPeer(const std::string_view& peerId);
    
private:
    RTCCallObserverInterface* observer_;
    scoped_refptr<PeerConnectionFactoryInterface> peerFactory_;
    /// audio source
    scoped_refptr<AudioSourceInterface> audioSource_;
    /// peer state list
    std::shared_ptr<std::unordered_map<std::string, RTCObject/*PeerStatusModel*/>> peerStates_;
    /// video source list
    std::shared_ptr<std::unordered_map<std::string, VideoSourceInterface<VideoFrame>>> videoSources_;
    /// candidates to be added to peer
    std::shared_ptr<std::unordered_map<std::string, std::vector<IceCandidateInterface>>> toBeAddedICEs_;
    
private:
    /// ice connect overtime, unit: second.
    int iceConnectionTimeout_;
    /// ice state check rate: every 4 seconds
    int iceStateCheckRate_;
    /// RTCCall error domain
    std::string errorDomain;
};

}



#endif /* !WEBRTC_CALL_H */
