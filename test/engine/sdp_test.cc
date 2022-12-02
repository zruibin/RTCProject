/*
 *
 * sdp_test.cc
 *
 * Created by Ruibin.Chow on 2022/12/02.
 * Copyright (c) 2022年 Ruibin.Chow All rights reserved.
 */

#include <string>
#include <iostream>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "engine/sdp/sdp.h"

static const std::string testSDPString = R"(v=0
o=- 2769202419187616652 2 IN IP4 127.0.0.1
s=-
t=0 0
a=group:BUNDLE audio video
a=extmap-allow-mixed
a=msid-semantic: WMS 6jWPL5zVxadwFUv3czcxk6OuMJhxwepXrnFt
a=ice-lite
m=audio 9 UDP/TLS/RTP/SAVPF 111 103 104 9 0 8 106 105 13 110 112 113 126
c=IN IP4 0.0.0.0
a=rtcp:9 IN IP4 0.0.0.0
c=IN IP4 193.84.77.194
a=rtcp:60017 IN IP4 193.84.77.194
a=candidate:1162875081 1 udp 2113937151 192.168.34.75 60017 typ host generation 0
a=candidate:1162875081 2 udp 2113937151 192.168.34.75 60017 typ host generation 0
a=candidate:3289912957 1 udp 1845501695 193.84.77.194 60017 typ srflx raddr 192.168.34.75 rport 60017 generation 0
a=candidate:3289912957 2 udp 1845501695 193.84.77.194 60017 typ srflx raddr 192.168.34.75 rport 60017 generation 0
a=candidate:198437945 1 tcp 1509957375 192.168.34.75 0 typ host generation 0
a=candidate:198437945 2 tcp 1509957375 192.168.34.75 0 typ host generation 0
a=ice-ufrag:V27U
a=ice-pwd:j8YGds5JpiHzbkd7Wwdxl5rq
a=ice-options:trickle
a=fingerprint:sha-256 97:23:44:6B:EC:52:D6:64:D6:02:42:69:EA:9C:99:F8:F3:F3:F4:1B:F3:74:D6:CF:4E:60:5D:89:97:9F:D6:3C
a=setup:actpass
a=mid:audio
a=extmap:1 urn:ietf:params:rtp-hdrext:ssrc-audio-level
a=extmap:2 http://www.webrtc.org/experiments/rtp-hdrext/abs-send-time
a=extmap:3 http://www.ietf.org/id/draft-holmer-rmcat-transport-wide-cc-extensions-01
a=sendrecv
a=rtcp-mux
a=rtpmap:111 opus/48000/2
a=rtcp-fb:111 transport-cc
a=fmtp:111 minptime=10;useinbandfec=1
a=rtpmap:103 ISAC/16000
a=rtpmap:104 ISAC/32000
a=rtpmap:9 G722/8000
a=rtpmap:0 PCMU/8000
a=rtpmap:8 PCMA/8000
a=rtpmap:106 CN/32000
a=rtpmap:105 CN/16000
a=rtpmap:13 CN/8000
a=rtpmap:110 telephone-event/48000
a=rtpmap:112 telephone-event/32000
a=rtpmap:113 telephone-event/16000
a=rtpmap:126 telephone-event/8000
a=ssrc:2096589939 cname:PK+nWUKVj/eH7KG7
a=ssrc:2096589939 msid:6jWPL5zVxadwFUv3czcxk6OuMJhxwepXrnFt ef3670e9-615c-4e0b-aa2f-a2b21d1b0b5d
a=ssrc:2096589939 mslabel:6jWPL5zVxadwFUv3czcxk6OuMJhxwepXrnFt
a=ssrc:2096589939 label:ef3670e9-615c-4e0b-aa2f-a2b21d1b0b5d
m=video 9 UDP/TLS/RTP/SAVPF 96 97 98 99 100 101 102 121 127 120 125 107 108 109 35 36 124 119 123 118 114 115 116
c=IN IP4 0.0.0.0
a=rtcp:9 IN IP4 0.0.0.0
a=ice-ufrag:V27U
a=ice-pwd:j8YGds5JpiHzbkd7Wwdxl5rq
a=ice-options:trickle
a=fingerprint:sha-256 97:23:44:6B:EC:52:D6:64:D6:02:42:69:EA:9C:99:F8:F3:F3:F4:1B:F3:74:D6:CF:4E:60:5D:89:97:9F:D6:3C
a=setup:actpass
a=mid:video
a=extmap:14 urn:ietf:params:rtp-hdrext:toffset
a=extmap:2 http://www.webrtc.org/experiments/rtp-hdrext/abs-send-time
a=extmap:13 urn:3gpp:video-orientation
a=extmap:3 http://www.ietf.org/id/draft-holmer-rmcat-transport-wide-cc-extensions-01
a=extmap:5 http://www.webrtc.org/experiments/rtp-hdrext/playout-delay
a=extmap:6 http://www.webrtc.org/experiments/rtp-hdrext/video-content-type
a=extmap:7 http://www.webrtc.org/experiments/rtp-hdrext/video-timing
a=extmap:8 http://www.webrtc.org/experiments/rtp-hdrext/color-space
a=sendrecv
a=rtcp-mux
a=rtcp-rsize
a=rtpmap:96 VP8/90000
a=rtcp-fb:96 goog-remb
a=rtcp-fb:96 transport-cc
a=rtcp-fb:96 ccm fir
a=rtcp-fb:96 nack
a=rtcp-fb:96 nack pli
a=rtpmap:97 rtx/90000
a=fmtp:97 apt=96
a=rtpmap:98 VP9/90000
a=rtcp-fb:98 goog-remb
a=rtcp-fb:98 transport-cc
a=rtcp-fb:98 ccm fir
a=rtcp-fb:98 nack
a=rtcp-fb:98 nack pli
a=fmtp:98 profile-id=0
a=rtpmap:99 rtx/90000
a=fmtp:99 apt=98
a=rtpmap:100 VP9/90000
a=rtcp-fb:100 goog-remb
a=rtcp-fb:100 transport-cc
a=rtcp-fb:100 ccm fir
a=rtcp-fb:100 nack
a=rtcp-fb:100 nack pli
a=fmtp:100 profile-id=2
a=rtpmap:101 rtx/90000
a=fmtp:101 apt=100
a=rtpmap:102 H264/90000
a=rtcp-fb:102 goog-remb
a=rtcp-fb:102 transport-cc
a=rtcp-fb:102 ccm fir
a=rtcp-fb:102 nack
a=rtcp-fb:102 nack pli
a=fmtp:102 level-asymmetry-allowed=1;packetization-mode=1;profile-level-id=42001f
a=rtpmap:121 rtx/90000
a=fmtp:121 apt=102
a=rtpmap:127 H264/90000
a=rtcp-fb:127 goog-remb
a=rtcp-fb:127 transport-cc
a=rtcp-fb:127 ccm fir
a=rtcp-fb:127 nack
a=rtcp-fb:127 nack pli
a=fmtp:127 level-asymmetry-allowed=1;packetization-mode=0;profile-level-id=42001f
a=rtpmap:120 rtx/90000
a=fmtp:120 apt=127
a=rtpmap:125 H264/90000
a=rtcp-fb:125 goog-remb
a=rtcp-fb:125 transport-cc
a=rtcp-fb:125 ccm fir
a=rtcp-fb:125 nack
a=rtcp-fb:125 nack pli
a=fmtp:125 level-asymmetry-allowed=1;packetization-mode=1;profile-level-id=42e01f
a=rtpmap:107 rtx/90000
a=fmtp:107 apt=125
a=rtpmap:108 H264/90000
a=rtcp-fb:108 goog-remb
a=rtcp-fb:108 transport-cc
a=rtcp-fb:108 ccm fir
a=rtcp-fb:108 nack
a=rtcp-fb:108 nack pli
a=fmtp:108 level-asymmetry-allowed=1;packetization-mode=0;profile-level-id=42e01f
a=rtpmap:109 rtx/90000
a=fmtp:109 apt=108
a=rtpmap:35 AV1X/90000
a=rtcp-fb:35 goog-remb
a=rtcp-fb:35 transport-cc
a=rtcp-fb:35 ccm fir
a=rtcp-fb:35 nack
a=rtcp-fb:35 nack pli
a=rtpmap:36 rtx/90000
a=fmtp:36 apt=35
a=rtpmap:124 H264/90000
a=rtcp-fb:124 goog-remb
a=rtcp-fb:124 transport-cc
a=rtcp-fb:124 ccm fir
a=rtcp-fb:124 nack
a=rtcp-fb:124 nack pli
a=fmtp:124 level-asymmetry-allowed=1;packetization-mode=1;profile-level-id=4d0032
a=rtpmap:119 rtx/90000
a=fmtp:119 apt=124
a=rtpmap:123 H264/90000
a=rtcp-fb:123 goog-remb
a=rtcp-fb:123 transport-cc
a=rtcp-fb:123 ccm fir
a=rtcp-fb:123 nack
a=rtcp-fb:123 nack pli
a=fmtp:123 level-asymmetry-allowed=1;packetization-mode=1;profile-level-id=640032
a=rtpmap:118 rtx/90000
a=fmtp:118 apt=123
a=rtpmap:114 red/90000
a=rtpmap:115 rtx/90000
a=fmtp:115 apt=114
a=rtpmap:116 ulpfec/90000
a=ssrc-group:FID 3606269878 2056516636
a=ssrc:3606269878 cname:PK+nWUKVj/eH7KG7
a=ssrc:3606269878 msid:6jWPL5zVxadwFUv3czcxk6OuMJhxwepXrnFt 9e5ca158-57c2-4900-9cd4-edc08d0cd3cd
a=ssrc:3606269878 mslabel:6jWPL5zVxadwFUv3czcxk6OuMJhxwepXrnFt
a=ssrc:3606269878 label:9e5ca158-57c2-4900-9cd4-edc08d0cd3cd
a=ssrc:2056516636 cname:PK+nWUKVj/eH7KG7
a=ssrc:2056516636 msid:6jWPL5zVxadwFUv3czcxk6OuMJhxwepXrnFt 9e5ca158-57c2-4900-9cd4-edc08d0cd3cd
a=ssrc:2056516636 mslabel:6jWPL5zVxadwFUv3czcxk6OuMJhxwepXrnFt
a=ssrc:2056516636 label:9e5ca158-57c2-4900-9cd4-edc08d0cd3cd
)";


TEST(SDPTest, BasicSDPTest) {
    using namespace engine;
    SessionDescription sdp;
    sdp.FromSDPString(testSDPString);
    std::cout << "SDP: " << sdp.ToSDPString();
    
    ASSERT_TRUE(sdp.originJsonString.length() > 0);
}

