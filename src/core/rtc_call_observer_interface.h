/*
 * rtc_call_observer_interface.h
 *
 * Created by Ruibin.Chow on 2022/10/21.
 * Copyright (c) 2022年 Ruibin.Chow All rights reserved.
 */

#ifndef RTC_CALL_OBSERVER_INTERFACE_H
#define RTC_CALL_OBSERVER_INTERFACE_H

#include <vector>
#include <api/media_stream_interface.h>
#include <api/rtc_error.h>
#include "rtc_macro.h"
#include "rtc_emun.h"


namespace core {

class RTCCallObserverInterface {
    
public:
    virtual ~RTCCallObserverInterface() {};

public:
    /// create a sdp
    /// @param sdpDict sdp of offer or answer
    /// @param peerId peer identification
    virtual void OnCreateSdp(RTCStringMap& sdpDict,
                             const RTCString& peerId) = 0;

    /// generate a local candidate
    /// @param candidate candidate
    /// @param peerId peer identification
    virtual void OnGenerateIceCandidate(RTCStringMap& sdpDict,
                                        const RTCString& peerId) = 0;

    /// remove a group of local candidates
    /// @param candidates a group of candidates
    /// @param peerId peer identification
    virtual void OnRemoveIceCandidates(const std::vector<RTCStringMap>& candidates,
                                       const RTCString& peerId) {}

    /// add a received video track
    /// @param videoTrack video track
    /// @param peerId peer identification
    virtual void OnAddReceivedVideoTrack(std::shared_ptr<webrtc::VideoTrackInterface> track,
                                         const RTCString& peerId) {}

    /// add a received audio track
    /// @param audioTrack audio track
    /// @param peerId peer identification
    virtual void OnAddReceivedAudioTrack(std::shared_ptr<webrtc::AudioTrackInterface> track,
                                         const RTCString& peerId) {}

    /// peer state
    /// @param peerState peer current state
    /// @param peerId peer identification
    virtual void OnPeerStateChangge(core::RTCPeerState peerState,
                                    const RTCString& peerId) = 0;

    /// csrc changed in video track
    /// @param csrc new csrc
    /// @param peerId peer identity
    /// @param trackId video track identification
    virtual void OnChangedCsrcForVideo(uint32_t csrc,
                                       const RTCString& peerId,
                                       const RTCString& trackId) {}

    /// csrc changed in audio track
    /// @param csrc new csrc
    /// @param peerId peer identification
    /// @param trackId audio track identification
    virtual void OnChangedCsrcForAudio(uint32_t csrc,
                                       const RTCString& peerId,
                                       const RTCString& trackId) {}

    /// receive SEI from peer
    /// @param bytes SEI bytes
    /// @param peerId peer identification
    /// @param trackId audio track identification
    virtual void OnReceiveSEI(const void* bytes,
                              const RTCString& peerId,
                              const RTCString& trackId) {}

    /// record audio data at a frequency of 10ms/p
    /// @param data audio data in PCM format
    /// @param length data length
    /// @param channel channel
    /// @param rate sample rate
    virtual void OnRecordAudioData(unsigned char* data,
                                   int length,
                                   int channel,
                                   int rate) {}

    /// play audio data at the frequency of 10ms/p
    /// @param data audio data in PCM format
    /// @param length data length
    /// @param channel channel
    /// @param rate sample rate
    virtual void OnPlayAudioData(unsigned char* data,
                                 int length,
                                 int channel,
                                 int rate) {}

    /// recording audio by first frame callback
    virtual void OnRecordedAudioFirstFrame() {}

    /// playing audio by first frame callback
    virtual void OnPlayedAudioFirstFrame() {}

    /// audio unit state change
    /// @param stateCode state code of audio unit
    virtual void OnAudioUnitStateChange(uint32_t stateCode) {}

    /// an error occurred
    /// @param error error info
    /// @param peerId peer identification
    virtual void OnOccurredError(const webrtc::RTCError& error,
                                 const RTCString& trackId) {}

    /// csrc array changed
    /// @param csrcAry new csrcAry
    /// @param peerId peer identification
    virtual void OnReceivedRtpCsrcArray(const std::vector<uint32_t>& csrcAry,
                                        const RTCString& peerId) {}

    /// receive audio howling state change
    /// @param hasHowling hasHowling
    virtual void OnReceiveAudioHowlingStateChange(bool hasHowling) {}

    /// receive audio echo state
    /// @param hasEcho has echo
    virtual void OnReceiveAudioEchoStateChange(bool hasEcho) {}

    /// capture exception by audio
    /// @param hasAudioCaptureException has audio capture exception
    virtual void OnReceiveAudioCaptureExceptionStateChange(bool hasAudioCaptureException) {}

    /// low level for audio
    /// @param hasAudioLowLevel has audio low level
    virtual void OnReceiveAudioLowLevel(bool hasAudioLowLevel) {}
};

}

#endif /* !RTC_CALL_OBSERVER_INTERFACE_H */
