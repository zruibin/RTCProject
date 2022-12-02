/*
 *
 * signaling_test.cc
 *
 * Created by Ruibin.Chow on 2022/12/02.
 * Copyright (c) 2022年 Ruibin.Chow All rights reserved.
 */

#include <string>
#include <iostream>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "engine/signalings/signaling.h"
#include "util/timer.h"
#include "engine/signalings/signaling_manager.h"

static const std::string testSignaling = R"(
{
    "data": {
        "codecs": [
            {
                "channels": 2,
                "clockRate": 48000,
                "kind": "audio",
                "mimeType": "audio/opus",
                "parameters": null,
                "payloadType": null,
                "preferredPayloadType": 100,
                "rtcpFeedback": [
                    {
                        "parameter": "",
                        "type": "transport-cc"
                    }
                ]
            },
            {
                "channels": null,
                "clockRate": 90000,
                "kind": "video",
                "mimeType": "video/H264",
                "parameters": {
                    "level-asymmetry-allowed": 1,
                    "minptime": null,
                    "packetization-mode": 1,
                    "profileId": null,
                    "profile-level-id": "42e01f",
                    "spropstereo": null,
                    "usedtx": null,
                    "useinbandfec": null,
                    "x-google-start-bitrate": 1000
                },
                "payloadType": null,
                "preferredPayloadType": 101,
                "rtcpFeedback": [
                    {
                        "parameter": "",
                        "type": "nack"
                    },
                    {
                        "parameter": "pli",
                        "type": "nack"
                    },
                    {
                        "parameter": "fir",
                        "type": "ccm"
                    },
                    {
                        "parameter": "",
                        "type": "goog-remb"
                    },
                    {
                        "parameter": "",
                        "type": "transport-cc"
                    }
                ]
            },
            {
                "channels": null,
                "clockRate": 90000,
                "kind": "video",
                "mimeType": "video/rtx",
                "parameters": {
                    "level-asymmetry-allowed": null,
                    "minptime": null,
                    "packetization-mode": null,
                    "profileId": null,
                    "profile-level-id": null,
                    "spropstereo": null,
                    "usedtx": null,
                    "useinbandfec": null,
                    "x-google-start-bitrate": null
                },
                "payloadType": null,
                "preferredPayloadType": 102,
                "rtcpFeedback": null
            },
            {
                "channels": null,
                "clockRate": 90000,
                "kind": "video",
                "mimeType": "video/H264",
                "parameters": {
                    "level-asymmetry-allowed": 1,
                    "minptime": null,
                    "packetization-mode": 1,
                    "profileId": null,
                    "profile-level-id": "4d0032",
                    "spropstereo": null,
                    "usedtx": null,
                    "useinbandfec": null,
                    "x-google-start-bitrate": 1000
                },
                "payloadType": null,
                "preferredPayloadType": 103,
                "rtcpFeedback": [
                    {
                        "parameter": "",
                        "type": "nack"
                    },
                    {
                        "parameter": "pli",
                        "type": "nack"
                    },
                    {
                        "parameter": "fir",
                        "type": "ccm"
                    },
                    {
                        "parameter": "",
                        "type": "goog-remb"
                    },
                    {
                        "parameter": "",
                        "type": "transport-cc"
                    }
                ]
            },
            {
                "channels": null,
                "clockRate": 90000,
                "kind": "video",
                "mimeType": "video/rtx",
                "parameters": {
                    "level-asymmetry-allowed": null,
                    "minptime": null,
                    "packetization-mode": null,
                    "profileId": null,
                    "profile-level-id": null,
                    "spropstereo": null,
                    "usedtx": null,
                    "useinbandfec": null,
                    "x-google-start-bitrate": null
                },
                "payloadType": null,
                "preferredPayloadType": 104,
                "rtcpFeedback": null
            },
            {
                "channels": null,
                "clockRate": 90000,
                "kind": "video",
                "mimeType": "video/H264",
                "parameters": {
                    "level-asymmetry-allowed": 1,
                    "minptime": null,
                    "packetization-mode": 1,
                    "profileId": null,
                    "profile-level-id": "42e034",
                    "spropstereo": null,
                    "usedtx": null,
                    "useinbandfec": null,
                    "x-google-start-bitrate": 1000
                },
                "payloadType": null,
                "preferredPayloadType": 105,
                "rtcpFeedback": [
                    {
                        "parameter": "",
                        "type": "nack"
                    },
                    {
                        "parameter": "pli",
                        "type": "nack"
                    },
                    {
                        "parameter": "fir",
                        "type": "ccm"
                    },
                    {
                        "parameter": "",
                        "type": "goog-remb"
                    },
                    {
                        "parameter": "",
                        "type": "transport-cc"
                    }
                ]
            },
            {
                "channels": null,
                "clockRate": 90000,
                "kind": "video",
                "mimeType": "video/rtx",
                "parameters": {
                    "level-asymmetry-allowed": null,
                    "minptime": null,
                    "packetization-mode": null,
                    "profileId": null,
                    "profile-level-id": null,
                    "spropstereo": null,
                    "usedtx": null,
                    "useinbandfec": null,
                    "x-google-start-bitrate": null
                },
                "payloadType": null,
                "preferredPayloadType": 106,
                "rtcpFeedback": null
            },
            {
                "channels": 1,
                "clockRate": 90000,
                "kind": "video",
                "mimeType": "video/VP8",
                "parameters": {
                    "level-asymmetry-allowed": null,
                    "minptime": null,
                    "packetization-mode": null,
                    "profileId": null,
                    "profile-level-id": null,
                    "spropstereo": null,
                    "usedtx": null,
                    "useinbandfec": null,
                    "x-google-start-bitrate": 1000
                },
                "payloadType": null,
                "preferredPayloadType": 107,
                "rtcpFeedback": [
                    {
                        "parameter": "",
                        "type": "nack"
                    },
                    {
                        "parameter": "pli",
                        "type": "nack"
                    },
                    {
                        "parameter": "fir",
                        "type": "ccm"
                    },
                    {
                        "parameter": "",
                        "type": "goog-remb"
                    },
                    {
                        "parameter": "",
                        "type": "transport-cc"
                    }
                ]
            },
            {
                "channels": null,
                "clockRate": 90000,
                "kind": "video",
                "mimeType": "video/rtx",
                "parameters": {
                    "level-asymmetry-allowed": null,
                    "minptime": null,
                    "packetization-mode": null,
                    "profileId": null,
                    "profile-level-id": null,
                    "spropstereo": null,
                    "usedtx": null,
                    "useinbandfec": null,
                    "x-google-start-bitrate": null
                },
                "payloadType": null,
                "preferredPayloadType": 108,
                "rtcpFeedback": null
            },
            {
                "channels": 1,
                "clockRate": 90000,
                "kind": "video",
                "mimeType": "video/VP9",
                "parameters": {
                    "level-asymmetry-allowed": null,
                    "minptime": null,
                    "packetization-mode": null,
                    "profileId": null,
                    "profile-level-id": null,
                    "spropstereo": null,
                    "usedtx": null,
                    "useinbandfec": null,
                    "x-google-start-bitrate": 1000
                },
                "payloadType": null,
                "preferredPayloadType": 109,
                "rtcpFeedback": [
                    {
                        "parameter": "",
                        "type": "nack"
                    },
                    {
                        "parameter": "pli",
                        "type": "nack"
                    },
                    {
                        "parameter": "fir",
                        "type": "ccm"
                    },
                    {
                        "parameter": "",
                        "type": "goog-remb"
                    },
                    {
                        "parameter": "",
                        "type": "transport-cc"
                    }
                ]
            },
            {
                "channels": null,
                "clockRate": 90000,
                "kind": "video",
                "mimeType": "video/rtx",
                "parameters": {
                    "level-asymmetry-allowed": null,
                    "minptime": null,
                    "packetization-mode": null,
                    "profileId": null,
                    "profile-level-id": null,
                    "spropstereo": null,
                    "usedtx": null,
                    "useinbandfec": null,
                    "x-google-start-bitrate": null
                },
                "payloadType": null,
                "preferredPayloadType": 110,
                "rtcpFeedback": null
            }
        ],
        "headerExtensions": [
            {
                "direction": "sendrecv",
                "encrypt": null,
                "id": null,
                "kind": "audio",
                "preferredEncrypt": false,
                "preferredId": 1,
                "uri": "urn:ietf:params:rtp-hdrext:sdes:mid"
            },
            {
                "direction": "sendrecv",
                "encrypt": null,
                "id": null,
                "kind": "video",
                "preferredEncrypt": false,
                "preferredId": 1,
                "uri": "urn:ietf:params:rtp-hdrext:sdes:mid"
            },
            {
                "direction": "recvonly",
                "encrypt": null,
                "id": null,
                "kind": "video",
                "preferredEncrypt": false,
                "preferredId": 2,
                "uri": "urn:ietf:params:rtp-hdrext:sdes:rtp-stream-id"
            },
            {
                "direction": "recvonly",
                "encrypt": null,
                "id": null,
                "kind": "video",
                "preferredEncrypt": false,
                "preferredId": 3,
                "uri": "urn:ietf:params:rtp-hdrext:sdes:repaired-rtp-stream-id"
            },
            {
                "direction": "sendrecv",
                "encrypt": null,
                "id": null,
                "kind": "audio",
                "preferredEncrypt": false,
                "preferredId": 4,
                "uri": "http://www.webrtc.org/experiments/rtp-hdrext/abs-send-time"
            },
            {
                "direction": "sendrecv",
                "encrypt": null,
                "id": null,
                "kind": "video",
                "preferredEncrypt": false,
                "preferredId": 4,
                "uri": "http://www.webrtc.org/experiments/rtp-hdrext/abs-send-time"
            },
            {
                "direction": "recvonly",
                "encrypt": null,
                "id": null,
                "kind": "audio",
                "preferredEncrypt": false,
                "preferredId": 5,
                "uri": "http://www.ietf.org/id/draft-holmer-rmcat-transport-wide-cc-extensions-01"
            },
            {
                "direction": "sendrecv",
                "encrypt": null,
                "id": null,
                "kind": "video",
                "preferredEncrypt": false,
                "preferredId": 5,
                "uri": "http://www.ietf.org/id/draft-holmer-rmcat-transport-wide-cc-extensions-01"
            },
            {
                "direction": "sendrecv",
                "encrypt": null,
                "id": null,
                "kind": "video",
                "preferredEncrypt": false,
                "preferredId": 6,
                "uri": "http://tools.ietf.org/html/draft-ietf-avtext-framemarking-07"
            },
            {
                "direction": "sendrecv",
                "encrypt": null,
                "id": null,
                "kind": "video",
                "preferredEncrypt": false,
                "preferredId": 7,
                "uri": "urn:ietf:params:rtp-hdrext:framemarking"
            },
            {
                "direction": "sendrecv",
                "encrypt": null,
                "id": null,
                "kind": "audio",
                "preferredEncrypt": false,
                "preferredId": 10,
                "uri": "urn:ietf:params:rtp-hdrext:ssrc-audio-level"
            },
            {
                "direction": "sendrecv",
                "encrypt": null,
                "id": null,
                "kind": "video",
                "preferredEncrypt": false,
                "preferredId": 11,
                "uri": "urn:3gpp:video-orientation"
            },
            {
                "direction": "sendrecv",
                "encrypt": null,
                "id": null,
                "kind": "video",
                "preferredEncrypt": false,
                "preferredId": 12,
                "uri": "urn:ietf:params:rtp-hdrext:toffset"
            }
        ]
    },
    "id": 3244342,
    "ok": true,
    "response": true
}
)";

TEST(SignalingTest, BasicSignalingTest) {
    using namespace engine;
    GetRouterRtpCapabilitiesResponse response;
    response.FromJsonString(testSignaling);
    std::string jsonStr = response.ToJsonString();
    std::cout << "jsonStr: " << jsonStr;
    ASSERT_TRUE(jsonStr.length() > 0);
}

TEST(SignalingTest, EnumTest) {
    using namespace engine;
    NotificationMethod method = NotificationMethod::ProducerScore;
    std::cout << "Notification Name: "
                << EnumMethodToString<NotificationMethod>(method)
                << std::endl;
    std::cout << "Notification method: "
                << EnumMethodToString<NotificationMethod>(
                        EnumMethodFromString<NotificationMethod>("activeSpeaker"))
                << std::endl;
    
    RequestMethod method1 = RequestMethod::GetTransportStats;
    std::cout << "Request Name: "
                << EnumMethodToString<RequestMethod>(method1)
                << std::endl;
    std::cout << "Request method: "
                << EnumMethodToString<RequestMethod>(
                        EnumMethodFromString<RequestMethod>("newConsumer"))
                << std::endl;
}

TEST(SignalingTest, SocktTest) {
    using namespace engine;
    /*
    SignalingManager::SharedInstance()->Init();
    util::Timer::Sleep(3*TIME_NSEC_PER_SEC);
    SignalingManager::SharedInstance()->GetRouterRtpCapabilities([](int32_t code,
                                                                    const std::string& msg,
                                                                    std::shared_ptr<GetRouterRtpCapabilitiesResponse> response) {
        std::cout << "GetRouterRtpCapabilities:" << response->ToJsonString();
    });
    util::Timer::Sleep(2*TIME_NSEC_PER_SEC);
    //*/
}
