/*
 * rtc_call_observer_interface.h
 *
 * Created by Ruibin.Chow on 2022/10/21.
 * Copyright (c) 2022年 Ruibin.Chow All rights reserved.
 */

#ifndef RTC_CALL_OBSERVER_INTERFACE_H
#define RTC_CALL_OBSERVER_INTERFACE_H

#include <unordered_map>
#include <vector>
#include <api/media_stream_interface.h>
#include <api/rtc_error.h>
#include "rtc_call_emun.h"


class RTCCallObserverInterface {
    
public:
    /// create a sdp
    /// @param sdpDict sdp of offer or answer
    /// @param peerId peer identification
    void OnCreateSdp(const std::unordered_map<std::string, std::string>& sdpDict,
                     const std::string& peerId) {}
    
    /// generate a local candidate
    /// @param candidate candidate
    /// @param peerId peer identification
    void OnGenerateIceCandidate(const std::unordered_map<std::string, std::string>& sdpDict,
                                const std::string& peerId) {}
    
    /// remove a group of local candidates
    /// @param candidates a group of candidates
    /// @param peerId peer identification
    void OnRemoveIceCandidates(const std::vector<std::unordered_map<std::string, std::string>>& candidates,
                               const std::string& peerId) {}
    
    /// add a received video track
    /// @param videoTrack video track
    /// @param peerId peer identification
    void OnAddReceivedVideoTrack(rtc::scoped_refptr<webrtc::VideoTrackInterface> track,
                                 const std::string& peerId) {}
    
    /// add a received audio track
    /// @param audioTrack audio track
    /// @param peerId peer identification
    void OnAddReceivedAudioTrack(rtc::scoped_refptr<webrtc::AudioTrackInterface> track,
                                 const std::string& peerId) {}
    
    /// peer state
    /// @param peerState peer current state
    /// @param peerId peer identification
    void OnPeerStateChangge(engine::RTCPeerState peerState,
                            const std::string& peerId) {}
    
    /// csrc changed in video track
    /// @param csrc new csrc
    /// @param peerId peer identity
    /// @param trackId video track identification
    void OnChangedCsrcForVideo(uint32_t csrc,
                               const std::string& peerId,
                               const std::string& trackId) {}
    
    /// csrc changed in audio track
    /// @param csrc new csrc
    /// @param peerId peer identification
    /// @param trackId audio track identification
    void OnChangedCsrcForAudio(uint32_t csrc,
                               const std::string& peerId,
                               const std::string& trackId) {}
    
    /// receive SEI from peer
    /// @param bytes SEI bytes
    /// @param peerId peer identification
    /// @param trackId audio track identification
    void OnReceiveSEI(const void *bytes,
                      const std::string& peerId,
                      const std::string& trackId) {}
    
    /// record audio data at a frequency of 10ms/p
    /// @param data audio data in PCM format
    /// @param length data length
    /// @param channel channel
    /// @param rate sample rate
    void OnRecordAudioData(unsigned char* data,
                           int length,
                           int channel,
                           int rate) {}
    
    /// play audio data at the frequency of 10ms/p
    /// @param data audio data in PCM format
    /// @param length data length
    /// @param channel channel
    /// @param rate sample rate
    void OnPlayAudioData(unsigned char* data,
                         int length,
                         int channel,
                         int rate) {}
    
    /// recording audio by first frame callback
    void OnRecordedAudioFirstFrame() {}
    
    /// playing audio by first frame callback
    void OnPlayedAudioFirstFrame() {}
    
    /// audio unit state change
    /// @param stateCode state code of audio unit
    void OnAudioUnitStateChange(uint32_t stateCode) {}
    
    /// an error occurred
    /// @param error error info
    /// @param peerId peer identification
    void OnOccurredError(const webrtc::RTCError& error,
                         const std::string& trackId) {}
    
    /// csrc array changed
    /// @param csrcAry new csrcAry
    /// @param peerId peer identification
    void OnReceivedRtpCsrcArray(const std::vector<uint32_t>& csrcAry,
                                const std::string& peerId) {}
    
    /// receive audio howling state change
    /// @param hasHowling hasHowling
    void OnReceiveAudioHowlingStateChange(bool hasHowling) {}
    
    /// receive audio echo state
    /// @param hasEcho has echo
    void OnReceiveAudioEchoStateChange(bool hasEcho) {}
    
    /// capture exception by audio
    /// @param hasAudioCaptureException has audio capture exception
    void OnReceiveAudioCaptureExceptionStateChange(bool hasAudioCaptureException) {}
    
    /// low level for audio
    /// @param hasAudioLowLevel has audio low level
    void OnReceiveAudioLowLevel(bool hasAudioLowLevel) {}
};

#endif /* !RTC_CALL_OBSERVER_INTERFACE_H */
