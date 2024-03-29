<!-- 
    mediasoup.md
    Created by Ruibin.Chow on 2022/11/18.
    Copyright (c) 2022年 Ruibin.Chow All rights reserved.
    Suggest: using Typora for editing and export
 -->


### Site

* [MediaoSoup Demo协议分析](https://www.jianshu.com/p/36078f46a001)
* [mediasoup房间信令交换管理流程分析](https://www.ngui.cc/el/1435100.html)
* [cmediasoupclient](https://github.com/chensongpoixs/cmediasoupclient)
* [cmediasoup](https://github.com/chensongpoixs/cmediasoup)
* [wzw88486969文章系列](https://blog.csdn.net/wzw88486969?type=blog)

### Mediasoup相关

```js
信令执行的顺序

getRouterRtpCapabilities

createWebRtcTransport   （收 传输通道）

createWebRtcTransport    （发 传输通道）

join

connectWebRtcTransport

connectWebRtcTransport

produce

produceData

produceData
```

如：

```
creating Express app... //启动Express
running an HTTPS server... //HTTPS启动

running protoo WebSocketServer... //protoo websocke服务启动
protoo connection request [roomId: 6j8ioln7, peerId:yh1g6wbk, address: 172.16.11.124, origin:https: //172.16.11.124:3000]  //建立了protoo 连接

//创建了一个Room
creating a new Room [roomId: 6j8ioln7] 
Room create() [roomId: 6j8ioln7]

//获取C++能力接口
Room zzh getRouterRtpCapabilities  {}  
Room zzh  back {
        "codecs": [
            {
                "kind": "audio",
                "mimeType": "audio/opus",
                "clockRate": 48000,
                "channels": 2,
                "rtcpFeedback": [
                    {
                        "type": "transport-cc",
                        "parameter": ""
                    }
                ],
                "parameters": {},
                "preferredPayloadType": 100
            },
            {
                "kind": "video",
                "mimeType": "video/VP8",
                "clockRate": 90000,
                "rtcpFeedback": [
                    {
                        "type": "nack",
                        "parameter": ""
                    },
                    {
                        "type": "nack",
                        "parameter": "pli"
                    },
                    {
                        "type": "ccm",
                        "parameter": "fir"
                    },
                    {
                        "type": "goog-remb",
                        "parameter": ""
                    },
                    {
                        "type": "transport-cc",
                        "parameter": ""
                    }
                ],
                "parameters": {
                    "x-google-start-bitrate": 1000
                },
                "preferredPayloadType": 101
            },
            {
                "kind": "video",
                "mimeType": "video/rtx",
                "preferredPayloadType": 102,
                "clockRate": 90000,
                "parameters": {
                    "apt": 101
                },
                "rtcpFeedback": []
            },
            {
                "kind": "video",
                "mimeType": "video/VP9",
                "clockRate": 90000,
                "rtcpFeedback": [
                    {
                        "type": "nack",
                        "parameter": ""
                    },
                    {
                        "type": "nack",
                        "parameter": "pli"
                    },
                    {
                        "type": "ccm",
                        "parameter": "fir"
                    },
                    {
                        "type": "goog-remb",
                        "parameter": ""
                    },
                    {
                        "type": "transport-cc",
                        "parameter": ""
                    }
                ],
                "parameters": {
                    "profile-id": 2,
                    "x-google-start-bitrate": 1000
                },
                "preferredPayloadType": 103
            },
            {
                "kind": "video",
                "mimeType": "video/rtx",
                "preferredPayloadType": 104,
                "clockRate": 90000,
                "parameters": {
                    "apt": 103
                },
                "rtcpFeedback": []
            },
            {
                "kind": "video",
                "mimeType": "video/H264",
                "clockRate": 90000,
                "parameters": {
                    "packetization-mode": 1,
                    "level-asymmetry-allowed": 1,
                    "profile-level-id": "4d0032",
                    "x-google-start-bitrate": 1000
                },
                "rtcpFeedback": [
                    {
                        "type": "nack",
                        "parameter": ""
                    },
                    {
                        "type": "nack",
                        "parameter": "pli"
                    },
                    {
                        "type": "ccm",
                        "parameter": "fir"
                    },
                    {
                        "type": "goog-remb",
                        "parameter": ""
                    },
                    {
                        "type": "transport-cc",
                        "parameter": ""
                    }
                ],
                "preferredPayloadType": 105
            },
            {
                "kind": "video",
                "mimeType": "video/rtx",
                "preferredPayloadType": 106,
                "clockRate": 90000,
                "parameters": {
                    "apt": 105
                },
                "rtcpFeedback": []
            },
            {
                "kind": "video",
                "mimeType": "video/H264",
                "clockRate": 90000,
                "parameters": {
                    "packetization-mode": 1,
                    "level-asymmetry-allowed": 1,
                    "profile-level-id": "42e01f",
                    "x-google-start-bitrate": 1000
                },
                "rtcpFeedback": [
                    {
                        "type": "nack",
                        "parameter": ""
                    },
                    {
                        "type": "nack",
                        "parameter": "pli"
                    },
                    {
                        "type": "ccm",
                        "parameter": "fir"
                    },
                    {
                        "type": "goog-remb",
                        "parameter": ""
                    },
                    {
                        "type": "transport-cc",
                        "parameter": ""
                    }
                ],
                "preferredPayloadType": 107
            },
            {
                "kind": "video",
                "mimeType": "video/rtx",
                "preferredPayloadType": 108,
                "clockRate": 90000,
                "parameters": {
                    "apt": 107
                },
                "rtcpFeedback": []
            }
        ],
        "headerExtensions": [
            {
                "kind": "audio",
                "uri": "urn:ietf:params:rtp-hdrext:sdes:mid",
                "preferredId": 1,
                "preferredEncrypt": false,
                "direction": "sendrecv"
            },
            {
                "kind": "video",
                "uri": "urn:ietf:params:rtp-hdrext:sdes:mid",
                "preferredId": 1,
                "preferredEncrypt": false,
                "direction": "sendrecv"
            },
            {
                "kind": "video",
                "uri": "urn:ietf:params:rtp-hdrext:sdes:rtp-stream-id",
                "preferredId": 2,
                "preferredEncrypt": false,
                "direction": "recvonly"
            },
            {
                "kind": "video",
                "uri": "urn:ietf:params:rtp-hdrext:sdes:repaired-rtp-stream-id",
                "preferredId": 3,
                "preferredEncrypt": false,
                "direction": "recvonly"
            },
            {
                "kind": "audio",
                "uri": "http://www.webrtc.org/experiments/rtp-hdrext/abs-send-time",
                "preferredId": 4,
                "preferredEncrypt": false,
                "direction": "sendrecv"
            },
            {
                "kind": "video",
                "uri": "http://www.webrtc.org/experiments/rtp-hdrext/abs-send-time",
                "preferredId": 4,
                "preferredEncrypt": false,
                "direction": "sendrecv"
            },
            {
                "kind": "audio",
                "uri": "http://www.ietf.org/id/draft-holmer-rmcat-transport-wide-cc-extensions-01",
                "preferredId": 5,
                "preferredEncrypt": false,
                "direction": "recvonly"
            },
            {
                "kind": "video",
                "uri": "http://www.ietf.org/id/draft-holmer-rmcat-transport-wide-cc-extensions-01",
                "preferredId": 5,
                "preferredEncrypt": false,
                "direction": "sendrecv"
            },
            {
                "kind": "video",
                "uri": "http://tools.ietf.org/html/draft-ietf-avtext-framemarking-07",
                "preferredId": 6,
                "preferredEncrypt": false,
                "direction": "sendrecv"
            },
            {
                "kind": "video",
                "uri": "urn:ietf:params:rtp-hdrext:framemarking",
                "preferredId": 7,
                "preferredEncrypt": false,
                "direction": "sendrecv"
            },
            {
                "kind": "audio",
                "uri": "urn:ietf:params:rtp-hdrext:ssrc-audio-level",
                "preferredId": 10,
                "preferredEncrypt": false,
                "direction": "sendrecv"
            },
            {
                "kind": "video",
                "uri": "urn:3gpp:video-orientation",
                "preferredId": 11,
                "preferredEncrypt": false,
                "direction": "sendrecv"
            },
            {
                "kind": "video",
                "uri": "urn:ietf:params:rtp-hdrext:toffset",
                "preferredId": 12,
                "preferredEncrypt": false,
                "direction": "sendrecv"
            }
        ]
    }

    //创建了一个发送WebRtcTranspor
Room zzh createWebRtcTransport  {
        "forceTcp": false,
        "producing": true,
        "consuming": false,
        "sctpCapabilities": {
            "numStreams": {
                "OS": 1024,
                "MIS": 1024
            }
        }
    }

Room zzh  back {
        "id": "7966e750-a929-413c-9566-77345f1a9ed5",
        "iceParameters": {
            "iceLite": true,
            "password": "y1vgqsmhu7wfnb2wfyk9f5nenkpsowvl",
            "usernameFragment": "hoaf1jlk5u326a5v"
        },
        "iceCandidates": [
            {
                "foundation": "udpcandidate",
                "ip": "172.16.11.124",
                "port": 47099,
                "priority": 1076302079,
                "protocol": "udp",
                "type": "host"
            }
        ],
        "dtlsParameters": {
            "fingerprints": [
                {
                    "algorithm": "sha-1",
                    "value": "38:9F:D8:66:9D:85:E3:B8:26:77:0F:67:A0:6F:19:DB:4A:71:6C:E0"
                },
                {
                    "algorithm": "sha-224",
                    "value": "C4:9C:E0:90:B9:19:3A:69:8D:B6:7A:0A:EF:E1:31:38:13:6B:67:4F:C1:7D:99:CB:6D:D2:9A:48"
                },
                {
                    "algorithm": "sha-256",
                    "value": "5B:AC:9D:48:48:AF:52:38:A2:6E:CB:AA:75:8A:60:A4:C9:79:19:A7:94:4B:2B:0D:BA:8E:C7:C1:D8:E8:89:20"
                },
                {
                    "algorithm": "sha-384",
                    "value": "A2:DE:72:6B:71:B0:D7:61:52:56:32:2D:95:08:95:6D:4A:D9:D9:10:43:B5:35:D4:75:2B:9F:FC:61:AF:4F:24:2A:65:CA:1E:FB:FB:46:4C:18:09:64:4D:5A:40:27:C8"
                },
                {
                    "algorithm": "sha-512",
                    "value": "FF:25:E4:7E:E8:39:08:4D:73:37:23:64:1B:BE:84:2B:F1:C6:17:18:71:4A:6F:58:4E:5F:78:7D:1E:46:FD:8F:E8:F5:5A:61:8B:D7:1D:E6:82:17:1E:30:B7:3B:B1:F4:50:2A:FD:9A:EE:13:E0:2D:EA:83:45:9F:0F:BE:6B:FE"
                }
            ],
            "role": "auto"
        },
        "sctpParameters": {
            "MIS": 1024,
            "OS": 1024,
            "isDataChannel": true,
            "maxMessageSize": 262144,
            "port": 5000,
            "sctpBufferedAmount": 0,
            "sendBufferSize": 262144
        }
    }


//创建接收的WebRtcTransport 
Room zzh createWebRtcTransport  {
        "forceTcp": false,
        "producing": false,
        "consuming": true,
        "sctpCapabilities": {
            "numStreams": {
                "OS": 1024,
                "MIS": 1024
            }
        }
    }

    //返回了传输的id, ice的参数
Room zzh  back {
        "id": "3c679682-b045-431a-9c63-67f9ce8b2cb4",
        "iceParameters": {
            "iceLite": true,
            "password": "cef9fd81yzeyex3a5b0qgmdmv8nr7yoz",
            "usernameFragment": "vpyoawj8lm199sqh"
        },
        "iceCandidates": [
            {
                "foundation": "udpcandidate",
                "ip": "172.16.11.124",
                "port": 43913,
                "priority": 1076302079,
                "protocol": "udp",
                "type": "host"
            }
        ],
        "dtlsParameters": {
            "fingerprints": [
                {
                    "algorithm": "sha-1",
                    "value": "38:9F:D8:66:9D:85:E3:B8:26:77:0F:67:A0:6F:19:DB:4A:71:6C:E0"
                },
                {
                    "algorithm": "sha-224",
                    "value": "C4:9C:E0:90:B9:19:3A:69:8D:B6:7A:0A:EF:E1:31:38:13:6B:67:4F:C1:7D:99:CB:6D:D2:9A:48"
                },
                {
                    "algorithm": "sha-256",
                    "value": "5B:AC:9D:48:48:AF:52:38:A2:6E:CB:AA:75:8A:60:A4:C9:79:19:A7:94:4B:2B:0D:BA:8E:C7:C1:D8:E8:89:20"
                },
                {
                    "algorithm": "sha-384",
                    "value": "A2:DE:72:6B:71:B0:D7:61:52:56:32:2D:95:08:95:6D:4A:D9:D9:10:43:B5:35:D4:75:2B:9F:FC:61:AF:4F:24:2A:65:CA:1E:FB:FB:46:4C:18:09:64:4D:5A:40:27:C8"
                },
                {
                    "algorithm": "sha-512",
                    "value": "FF:25:E4:7E:E8:39:08:4D:73:37:23:64:1B:BE:84:2B:F1:C6:17:18:71:4A:6F:58:4E:5F:78:7D:1E:46:FD:8F:E8:F5:5A:61:8B:D7:1D:E6:82:17:1E:30:B7:3B:B1:F4:50:2A:FD:9A:EE:13:E0:2D:EA:83:45:9F:0F:BE:6B:FE"
                }
            ],
            "role": "auto"
        },
        "sctpParameters": {
            "MIS": 1024,
            "OS": 1024,
            "isDataChannel": true,
            "maxMessageSize": 262144,
            "port": 5000,
            "sctpBufferedAmount": 0,
            "sendBufferSize": 262144
        }
    }

//加入到某个房间,带rtpCapabilities 
Room zzh join  {
        "displayName": "Totodile",
        "device": {
            "flag": "chrome",
            "name": "Chrome",
            "version": "85.0.4183.83"
        },
        "rtpCapabilities": {
            "codecs": [
                {
                    "mimeType": "audio/opus",
                    "kind": "audio",
                    "preferredPayloadType": 100,
                    "clockRate": 48000,
                    "channels": 2,
                    "parameters": {
                        "minptime": 10,
                        "useinbandfec": 1
                    },
                    "rtcpFeedback": [
                        {
                            "type": "transport-cc",
                            "parameter": ""
                        }
                    ]
                },
                {
                    "mimeType": "video/VP8",
                    "kind": "video",
                    "preferredPayloadType": 101,
                    "clockRate": 90000,
                    "parameters": {},
                    "rtcpFeedback": [
                        {
                            "type": "goog-remb",
                            "parameter": ""
                        },
                        {
                            "type": "transport-cc",
                            "parameter": ""
                        },
                        {
                            "type": "ccm",
                            "parameter": "fir"
                        },
                        {
                            "type": "nack",
                            "parameter": ""
                        },
                        {
                            "type": "nack",
                            "parameter": "pli"
                        }
                    ]
                },
                {
                    "mimeType": "video/rtx",
                    "kind": "video",
                    "preferredPayloadType": 102,
                    "clockRate": 90000,
                    "parameters": {
                        "apt": 101
                    },
                    "rtcpFeedback": []
                },
                {
                    "mimeType": "video/VP9",
                    "kind": "video",
                    "preferredPayloadType": 103,
                    "clockRate": 90000,
                    "parameters": {
                        "profile-id": 2
                    },
                    "rtcpFeedback": [
                        {
                            "type": "goog-remb",
                            "parameter": ""
                        },
                        {
                            "type": "transport-cc",
                            "parameter": ""
                        },
                        {
                            "type": "ccm",
                            "parameter": "fir"
                        },
                        {
                            "type": "nack",
                            "parameter": ""
                        },
                        {
                            "type": "nack",
                            "parameter": "pli"
                        }
                    ]
                },
                {
                    "mimeType": "video/rtx",
                    "kind": "video",
                    "preferredPayloadType": 104,
                    "clockRate": 90000,
                    "parameters": {
                        "apt": 103
                    },
                    "rtcpFeedback": []
                },
                {
                    "mimeType": "video/H264",
                    "kind": "video",
                    "preferredPayloadType": 105,
                    "clockRate": 90000,
                    "parameters": {
                        "level-asymmetry-allowed": 1,
                        "packetization-mode": 1,
                        "profile-level-id": "4d0032"
                    },
                    "rtcpFeedback": [
                        {
                            "type": "goog-remb",
                            "parameter": ""
                        },
                        {
                            "type": "transport-cc",
                            "parameter": ""
                        },
                        {
                            "type": "ccm",
                            "parameter": "fir"
                        },
                        {
                            "type": "nack",
                            "parameter": ""
                        },
                        {
                            "type": "nack",
                            "parameter": "pli"
                        }
                    ]
                },
                {
                    "mimeType": "video/rtx",
                    "kind": "video",
                    "preferredPayloadType": 106,
                    "clockRate": 90000,
                    "parameters": {
                        "apt": 105
                    },
                    "rtcpFeedback": []
                },
                {
                    "mimeType": "video/H264",
                    "kind": "video",
                    "preferredPayloadType": 107,
                    "clockRate": 90000,
                    "parameters": {
                        "level-asymmetry-allowed": 1,
                        "packetization-mode": 1,
                        "profile-level-id": "42e01f"
                    },
                    "rtcpFeedback": [
                        {
                            "type": "goog-remb",
                            "parameter": ""
                        },
                        {
                            "type": "transport-cc",
                            "parameter": ""
                        },
                        {
                            "type": "ccm",
                            "parameter": "fir"
                        },
                        {
                            "type": "nack",
                            "parameter": ""
                        },
                        {
                            "type": "nack",
                            "parameter": "pli"
                        }
                    ]
                },
                {
                    "mimeType": "video/rtx",
                    "kind": "video",
                    "preferredPayloadType": 108,
                    "clockRate": 90000,
                    "parameters": {
                        "apt": 107
                    },
                    "rtcpFeedback": []
                }
            ],
            "headerExtensions": [
                {
                    "kind": "audio",
                    "uri": "urn:ietf:params:rtp-hdrext:sdes:mid",
                    "preferredId": 1,
                    "preferredEncrypt": false,
                    "direction": "sendrecv"
                },
                {
                    "kind": "video",
                    "uri": "urn:ietf:params:rtp-hdrext:sdes:mid",
                    "preferredId": 1,
                    "preferredEncrypt": false,
                    "direction": "sendrecv"
                },
                {
                    "kind": "audio",
                    "uri": "http://www.webrtc.org/experiments/rtp-hdrext/abs-send-time",
                    "preferredId": 4,
                    "preferredEncrypt": false,
                    "direction": "sendrecv"
                },
                {
                    "kind": "video",
                    "uri": "http://www.webrtc.org/experiments/rtp-hdrext/abs-send-time",
                    "preferredId": 4,
                    "preferredEncrypt": false,
                    "direction": "sendrecv"
                },
                {
                    "kind": "video",
                    "uri": "http://www.ietf.org/id/draft-holmer-rmcat-transport-wide-cc-extensions-01",
                    "preferredId": 5,
                    "preferredEncrypt": false,
                    "direction": "sendrecv"
                },
                {
                    "kind": "audio",
                    "uri": "urn:ietf:params:rtp-hdrext:ssrc-audio-level",
                    "preferredId": 10,
                    "preferredEncrypt": false,
                    "direction": "sendrecv"
                },
                {
                    "kind": "video",
                    "uri": "urn:3gpp:video-orientation",
                    "preferredId": 11,
                    "preferredEncrypt": false,
                    "direction": "sendrecv"
                },
                {
                    "kind": "video",
                    "uri": "urn:ietf:params:rtp-hdrext:toffset",
                    "preferredId": 12,
                    "preferredEncrypt": false,
                    "direction": "sendrecv"
                }
            ]
        },
        "sctpCapabilities": {
            "numStreams": {
                "OS": 1024,
                "MIS": 1024
            }
        }
    }

    //返回了房间的成员
Room zzh  back {
        "peers": []
    }

//接收WebRtcTransport连接  ,底层是 WebRtcTransport::connect
Room zzh connectWebRtcTransport  {
        "transportId": "3c679682-b045-431a-9c63-67f9ce8b2cb4",
        "dtlsParameters": {
            "role": "client",
            "fingerprints": [
                {
                    "algorithm": "sha-256",
                    "value": "40:A0:BA:FD:E5:2C:96:1A:98:84:1D:A4:CF:4A:3A:50:07:93:F1:5F:85:40:52:5E:17:CD:9E:39:B6:05:3A:54"
                }
            ]
        }
    }
Room zzh  back undefined

//发送WebRtcTransport连接  
Room zzh connectWebRtcTransport  {
        "transportId": "7966e750-a929-413c-9566-77345f1a9ed5",
        "dtlsParameters": {
            "role": "server",
            "fingerprints": [
                {
                    "algorithm": "sha-256",
                    "value": "CA:E9:47:51:2E:30:18:41:A2:02:1A:66:4F:15:0F:96:E2:D0:45:09:98:C2:50:CC:76:97:57:3C:31:C5:9A:FA"
                }
            ]
        }
    }
Room zzh  back undefined

//生产数据
//WebRtcTransport.produce,  WebRtcTransport.consume, audio
Room zzh produce  {
        "transportId": "7966e750-a929-413c-9566-77345f1a9ed5",
        "kind": "audio",
        "rtpParameters": {
            "codecs": [
                {
                    "mimeType": "audio/opus",
                    "payloadType": 111,
                    "clockRate": 48000,
                    "channels": 2,
                    "parameters": {
                        "minptime": 10,
                        "useinbandfec": 1,
                        "sprop-stereo": 1,
                        "usedtx": 1
                    },
                    "rtcpFeedback": [
                        {
                            "type": "transport-cc",
                            "parameter": ""
                        }
                    ]
                }
            ],
            "headerExtensions": [
                {
                    "uri": "urn:ietf:params:rtp-hdrext:sdes:mid",
                    "id": 4,
                    "encrypt": false,
                    "parameters": {}
                },
                {
                    "uri": "http://www.webrtc.org/experiments/rtp-hdrext/abs-send-time",
                    "id": 2,
                    "encrypt": false,
                    "parameters": {}
                },
                {
                    "uri": "http://www.ietf.org/id/draft-holmer-rmcat-transport-wide-cc-extensions-01",
                    "id": 3,
                    "encrypt": false,
                    "parameters": {}
                },
                {
                    "uri": "urn:ietf:params:rtp-hdrext:ssrc-audio-level",
                    "id": 1,
                    "encrypt": false,
                    "parameters": {}
                }
            ],
            "encodings": [
                {
                    "ssrc": 4107067730,
                    "dtx": false
                }
            ],
            "rtcp": {
                "cname": "yY9+ppRy2FqUmE0f",
                "reducedSize": true
            },
            "mid": "0"
        },
        "appData": {}
    }
Room zzh  back {
        "id": "810a2d7e-3d4a-494b-8032-26c75d485281"
    }

//WebRtcTransport.produceData
Room zzh produceData  {
        "transportId": "7966e750-a929-413c-9566-77345f1a9ed5",
        "sctpStreamParameters": {
            "streamId": 0,
            "ordered": false,
            "maxRetransmits": 1
        },
        "label": "chat",
        "protocol": "",
        "appData": {
            "info": "my-chat-DataProducer"
        }
    }
Room zzh  back {
        "id": "dfa5ff38-471f-411e-acb3-96d391269710"
    }


//生产数据
//WebRtcTransport.produce,  WebRtcTransport.consume, video
Room zzh produce  {
        "transportId": "7966e750-a929-413c-9566-77345f1a9ed5",
        "kind": "video",
        "rtpParameters": {
            "codecs": [
                {
                    "mimeType": "video/VP8",
                    "payloadType": 96,
                    "clockRate": 90000,
                    "parameters": {},
                    "rtcpFeedback": [
                        {
                            "type": "goog-remb",
                            "parameter": ""
                        },
                        {
                            "type": "transport-cc",
                            "parameter": ""
                        },
                        {
                            "type": "ccm",
                            "parameter": "fir"
                        },
                        {
                            "type": "nack",
                            "parameter": ""
                        },
                        {
                            "type": "nack",
                            "parameter": "pli"
                        }
                    ]
                },
                {
                    "mimeType": "video/rtx",
                    "payloadType": 97,
                    "clockRate": 90000,
                    "parameters": {
                        "apt": 96
                    },
                    "rtcpFeedback": []
                }
            ],
            "headerExtensions": [
                {
                    "uri": "urn:ietf:params:rtp-hdrext:sdes:mid",
                    "id": 4,
                    "encrypt": false,
                    "parameters": {}
                },
                {
                    "uri": "urn:ietf:params:rtp-hdrext:sdes:rtp-stream-id",
                    "id": 5,
                    "encrypt": false,
                    "parameters": {}
                },
                {
                    "uri": "urn:ietf:params:rtp-hdrext:sdes:repaired-rtp-stream-id",
                    "id": 6,
                    "encrypt": false,
                    "parameters": {}
                },
                {
                    "uri": "http://www.webrtc.org/experiments/rtp-hdrext/abs-send-time",
                    "id": 2,
                    "encrypt": false,
                    "parameters": {}
                },
                {
                    "uri": "http://www.ietf.org/id/draft-holmer-rmcat-transport-wide-cc-extensions-01",
                    "id": 3,
                    "encrypt": false,
                    "parameters": {}
                },
                {
                    "uri": "urn:3gpp:video-orientation",
                    "id": 13,
                    "encrypt": false,
                    "parameters": {}
                },
                {
                    "uri": "urn:ietf:params:rtp-hdrext:toffset",
                    "id": 14,
                    "encrypt": false,
                    "parameters": {}
                }
            ],
            "encodings": [
                {
                    "active": true,
                    "scaleResolutionDownBy": 4,
                    "maxBitrate": 500000,
                    "rid": "r0",
                    "scalabilityMode": "S1T3",
                    "dtx": false
                },
                {
                    "active": true,
                    "scaleResolutionDownBy": 2,
                    "maxBitrate": 1000000,
                    "rid": "r1",
                    "scalabilityMode": "S1T3",
                    "dtx": false
                },
                {
                    "active": true,
                    "scaleResolutionDownBy": 1,
                    "maxBitrate": 5000000,
                    "rid": "r2",
                    "scalabilityMode": "S1T3",
                    "dtx": false
                }
            ],
            "rtcp": {
                "cname": "",
                "reducedSize": true
            },
            "mid": "2"
        },
        "appData": {}
    }
Room zzh  back {
        "id": "38f924b8-bd8e-4f80-92a0-afe11698e9e0"
    }

protoo connection request [roomId: 6j8ioln7, peerId:q2seuuir, address: 172.16.11.124, origin:https: //172.16.11.124:3000]

Room zzh getRouterRtpCapabilities  {}
Room zzh  back {
            "codecs": [
                {
                    "kind": "audio",
                    "mimeType": "audio/opus",
                    "clockRate": 48000,
                    "channels": 2,
                    "rtcpFeedback": [
                        {
                            "type": "transport-cc",
                            "parameter": ""
                        }
                    ],
                    "parameters": {},
                    "preferredPayloadType": 100
                },
                {
                    "kind": "video",
                    "mimeType": "video/VP8",
                    "clockRate": 90000,
                    "rtcpFeedback": [
                        {
                            "type": "nack",
                            "parameter": ""
                        },
                        {
                            "type": "nack",
                            "parameter": "pli"
                        },
                        {
                            "type": "ccm",
                            "parameter": "fir"
                        },
                        {
                            "type": "goog-remb",
                            "parameter": ""
                        },
                        {
                            "type": "transport-cc",
                            "parameter": ""
                        }
                    ],
                    "parameters": {
                        "x-google-start-bitrate": 1000
                    },
                    "preferredPayloadType": 101
                },
                {
                    "kind": "video",
                    "mimeType": "video/rtx",
                    "preferredPayloadType": 102,
                    "clockRate": 90000,
                    "parameters": {
                        "apt": 101
                    },
                    "rtcpFeedback": []
                },
                {
                    "kind": "video",
                    "mimeType": "video/VP9",
                    "clockRate": 90000,
                    "rtcpFeedback": [
                        {
                            "type": "nack",
                            "parameter": ""
                        },
                        {
                            "type": "nack",
                            "parameter": "pli"
                        },
                        {
                            "type": "ccm",
                            "parameter": "fir"
                        },
                        {
                            "type": "goog-remb",
                            "parameter": ""
                        },
                        {
                            "type": "transport-cc",
                            "parameter": ""
                        }
                    ],
                    "parameters": {
                        "profile-id": 2,
                        "x-google-start-bitrate": 1000
                    },
                    "preferredPayloadType": 103
                },
                {
                    "kind": "video",
                    "mimeType": "video/rtx",
                    "preferredPayloadType": 104,
                    "clockRate": 90000,
                    "parameters": {
                        "apt": 103
                    },
                    "rtcpFeedback": []
                },
                {
                    "kind": "video",
                    "mimeType": "video/H264",
                    "clockRate": 90000,
                    "parameters": {
                        "packetization-mode": 1,
                        "level-asymmetry-allowed": 1,
                        "profile-level-id": "4d0032",
                        "x-google-start-bitrate": 1000
                    },
                    "rtcpFeedback": [
                        {
                            "type": "nack",
                            "parameter": ""
                        },
                        {
                            "type": "nack",
                            "parameter": "pli"
                        },
                        {
                            "type": "ccm",
                            "parameter": "fir"
                        },
                        {
                            "type": "goog-remb",
                            "parameter": ""
                        },
                        {
                            "type": "transport-cc",
                            "parameter": ""
                        }
                    ],
                    "preferredPayloadType": 105
                },
                {
                    "kind": "video",
                    "mimeType": "video/rtx",
                    "preferredPayloadType": 106,
                    "clockRate": 90000,
                    "parameters": {
                        "apt": 105
                    },
                    "rtcpFeedback": []
                },
                {
                    "kind": "video",
                    "mimeType": "video/H264",
                    "clockRate": 90000,
                    "parameters": {
                        "packetization-mode": 1,
                        "level-asymmetry-allowed": 1,
                        "profile-level-id": "42e01f",
                        "x-google-start-bitrate": 1000
                    },
                    "rtcpFeedback": [
                        {
                            "type": "nack",
                            "parameter": ""
                        },
                        {
                            "type": "nack",
                            "parameter": "pli"
                        },
                        {
                            "type": "ccm",
                            "parameter": "fir"
                        },
                        {
                            "type": "goog-remb",
                            "parameter": ""
                        },
                        {
                            "type": "transport-cc",
                            "parameter": ""
                        }
                    ],
                    "preferredPayloadType": 107
                },
                {
                    "kind": "video",
                    "mimeType": "video/rtx",
                    "preferredPayloadType": 108,
                    "clockRate": 90000,
                    "parameters": {
                        "apt": 107
                    },
                    "rtcpFeedback": []
                }
            ],
            "headerExtensions": [
                {
                    "kind": "audio",
                    "uri": "urn:ietf:params:rtp-hdrext:sdes:mid",
                    "preferredId": 1,
                    "preferredEncrypt": false,
                    "direction": "sendrecv"
                },
                {
                    "kind": "video",
                    "uri": "urn:ietf:params:rtp-hdrext:sdes:mid",
                    "preferredId": 1,
                    "preferredEncrypt": false,
                    "direction": "sendrecv"
                },
                {
                    "kind": "video",
                    "uri": "urn:ietf:params:rtp-hdrext:sdes:rtp-stream-id",
                    "preferredId": 2,
                    "preferredEncrypt": false,
                    "direction": "recvonly"
                },
                {
                    "kind": "video",
                    "uri": "urn:ietf:params:rtp-hdrext:sdes:repaired-rtp-stream-id",
                    "preferredId": 3,
                    "preferredEncrypt": false,
                    "direction": "recvonly"
                },
                {
                    "kind": "audio",
                    "uri": "http://www.webrtc.org/experiments/rtp-hdrext/abs-send-time",
                    "preferredId": 4,
                    "preferredEncrypt": false,
                    "direction": "sendrecv"
                },
                {
                    "kind": "video",
                    "uri": "http://www.webrtc.org/experiments/rtp-hdrext/abs-send-time",
                    "preferredId": 4,
                    "preferredEncrypt": false,
                    "direction": "sendrecv"
                },
                {
                    "kind": "audio",
                    "uri": "http://www.ietf.org/id/draft-holmer-rmcat-transport-wide-cc-extensions-01",
                    "preferredId": 5,
                    "preferredEncrypt": false,
                    "direction": "recvonly"
                },
                {
                    "kind": "video",
                    "uri": "http://www.ietf.org/id/draft-holmer-rmcat-transport-wide-cc-extensions-01",
                    "preferredId": 5,
                    "preferredEncrypt": false,
                    "direction": "sendrecv"
                },
                {
                    "kind": "video",
                    "uri": "http://tools.ietf.org/html/draft-ietf-avtext-framemarking-07",
                    "preferredId": 6,
                    "preferredEncrypt": false,
                    "direction": "sendrecv"
                },
                {
                    "kind": "video",
                    "uri": "urn:ietf:params:rtp-hdrext:framemarking",
                    "preferredId": 7,
                    "preferredEncrypt": false,
                    "direction": "sendrecv"
                },
                {
                    "kind": "audio",
                    "uri": "urn:ietf:params:rtp-hdrext:ssrc-audio-level",
                    "preferredId": 10,
                    "preferredEncrypt": false,
                    "direction": "sendrecv"
                },
                {
                    "kind": "video",
                    "uri": "urn:3gpp:video-orientation",
                    "preferredId": 11,
                    "preferredEncrypt": false,
                    "direction": "sendrecv"
                },
                {
                    "kind": "video",
                    "uri": "urn:ietf:params:rtp-hdrext:toffset",
                    "preferredId": 12,
                    "preferredEncrypt": false,
                    "direction": "sendrecv"
                }
            ]
        }

Room zzh createWebRtcTransport  {
            "forceTcp": false,
            "producing": true,
            "consuming": false,
            "sctpCapabilities": {
                "numStreams": {
                    "OS": 1024,
                    "MIS": 1024
                }
            }
        }
Room zzh  back {
            "id": "ada55f6b-41ae-467d-9eee-7c4edf3747c7",
            "iceParameters": {
                "iceLite": true,
                "password": "yhku39rlk9i3yq65zq1jo2c2dezk37f6",
                "usernameFragment": "p4f9e0eozsp2dlpk"
            },
            "iceCandidates": [
                {
                    "foundation": "udpcandidate",
                    "ip": "172.16.11.124",
                    "port": 45416,
                    "priority": 1076302079,
                    "protocol": "udp",
                    "type": "host"
                }
            ],
            "dtlsParameters": {
                "fingerprints": [
                    {
                        "algorithm": "sha-1",
                        "value": "38:9F:D8:66:9D:85:E3:B8:26:77:0F:67:A0:6F:19:DB:4A:71:6C:E0"
                    },
                    {
                        "algorithm": "sha-224",
                        "value": "C4:9C:E0:90:B9:19:3A:69:8D:B6:7A:0A:EF:E1:31:38:13:6B:67:4F:C1:7D:99:CB:6D:D2:9A:48"
                    },
                    {
                        "algorithm": "sha-256",
                        "value": "5B:AC:9D:48:48:AF:52:38:A2:6E:CB:AA:75:8A:60:A4:C9:79:19:A7:94:4B:2B:0D:BA:8E:C7:C1:D8:E8:89:20"
                    },
                    {
                        "algorithm": "sha-384",
                        "value": "A2:DE:72:6B:71:B0:D7:61:52:56:32:2D:95:08:95:6D:4A:D9:D9:10:43:B5:35:D4:75:2B:9F:FC:61:AF:4F:24:2A:65:CA:1E:FB:FB:46:4C:18:09:64:4D:5A:40:27:C8"
                    },
                    {
                        "algorithm": "sha-512",
                        "value": "FF:25:E4:7E:E8:39:08:4D:73:37:23:64:1B:BE:84:2B:F1:C6:17:18:71:4A:6F:58:4E:5F:78:7D:1E:46:FD:8F:E8:F5:5A:61:8B:D7:1D:E6:82:17:1E:30:B7:3B:B1:F4:50:2A:FD:9A:EE:13:E0:2D:EA:83:45:9F:0F:BE:6B:FE"
                    }
                ],
                "role": "auto"
            },
            "sctpParameters": {
                "MIS": 1024,
                "OS": 1024,
                "isDataChannel": true,
                "maxMessageSize": 262144,
                "port": 5000,
                "sctpBufferedAmount": 0,
                "sendBufferSize": 262144
            }
        }

Room zzh createWebRtcTransport  {
            "forceTcp": false,
            "producing": false,
            "consuming": true,
            "sctpCapabilities": {
                "numStreams": {
                    "OS": 1024,
                    "MIS": 1024
                }
            }
        }
Room zzh  back {
            "id": "ad8988ef-41d4-4be1-aaf0-c134ecea68f7",
            "iceParameters": {
                "iceLite": true,
                "password": "jpacigbodip15citkm39z1c1evy6yh3m",
                "usernameFragment": "ig13xjm78l8scfx0"
            },
            "iceCandidates": [
                {
                    "foundation": "udpcandidate",
                    "ip": "172.16.11.124",
                    "port": 49336,
                    "priority": 1076302079,
                    "protocol": "udp",
                    "type": "host"
                }
            ],
            "dtlsParameters": {
                "fingerprints": [
                    {
                        "algorithm": "sha-1",
                        "value": "38:9F:D8:66:9D:85:E3:B8:26:77:0F:67:A0:6F:19:DB:4A:71:6C:E0"
                    },
                    {
                        "algorithm": "sha-224",
                        "value": "C4:9C:E0:90:B9:19:3A:69:8D:B6:7A:0A:EF:E1:31:38:13:6B:67:4F:C1:7D:99:CB:6D:D2:9A:48"
                    },
                    {
                        "algorithm": "sha-256",
                        "value": "5B:AC:9D:48:48:AF:52:38:A2:6E:CB:AA:75:8A:60:A4:C9:79:19:A7:94:4B:2B:0D:BA:8E:C7:C1:D8:E8:89:20"
                    },
                    {
                        "algorithm": "sha-384",
                        "value": "A2:DE:72:6B:71:B0:D7:61:52:56:32:2D:95:08:95:6D:4A:D9:D9:10:43:B5:35:D4:75:2B:9F:FC:61:AF:4F:24:2A:65:CA:1E:FB:FB:46:4C:18:09:64:4D:5A:40:27:C8"
                    },
                    {
                        "algorithm": "sha-512",
                        "value": "FF:25:E4:7E:E8:39:08:4D:73:37:23:64:1B:BE:84:2B:F1:C6:17:18:71:4A:6F:58:4E:5F:78:7D:1E:46:FD:8F:E8:F5:5A:61:8B:D7:1D:E6:82:17:1E:30:B7:3B:B1:F4:50:2A:FD:9A:EE:13:E0:2D:EA:83:45:9F:0F:BE:6B:FE"
                    }
                ],
                "role": "auto"
            },
            "sctpParameters": {
                "MIS": 1024,
                "OS": 1024,
                "isDataChannel": true,
                "maxMessageSize": 262144,
                "port": 5000,
                "sctpBufferedAmount": 0,
                "sendBufferSize": 262144
            }
        }

Room zzh join  {
            "displayName": "Mienfoo",
            "device": {
                "flag": "chrome",
                "name": "Chrome",
                "version": "85.0.4183.83"
            },
            "rtpCapabilities": {
                "codecs": [
                    {
                        "mimeType": "audio/opus",
                        "kind": "audio",
                        "preferredPayloadType": 100,
                        "clockRate": 48000,
                        "channels": 2,
                        "parameters": {
                            "minptime": 10,
                            "useinbandfec": 1
                        },
                        "rtcpFeedback": [
                            {
                                "type": "transport-cc",
                                "parameter": ""
                            }
                        ]
                    },
                    {
                        "mimeType": "video/VP8",
                        "kind": "video",
                        "preferredPayloadType": 101,
                        "clockRate": 90000,
                        "parameters": {},
                        "rtcpFeedback": [
                            {
                                "type": "goog-remb",
                                "parameter": ""
                            },
                            {
                                "type": "transport-cc",
                                "parameter": ""
                            },
                            {
                                "type": "ccm",
                                "parameter": "fir"
                            },
                            {
                                "type": "nack",
                                "parameter": ""
                            },
                            {
                                "type": "nack",
                                "parameter": "pli"
                            }
                        ]
                    },
                    {
                        "mimeType": "video/rtx",
                        "kind": "video",
                        "preferredPayloadType": 102,
                        "clockRate": 90000,
                        "parameters": {
                            "apt": 101
                        },
                        "rtcpFeedback": []
                    },
                    {
                        "mimeType": "video/VP9",
                        "kind": "video",
                        "preferredPayloadType": 103,
                        "clockRate": 90000,
                        "parameters": {
                            "profile-id": 2
                        },
                        "rtcpFeedback": [
                            {
                                "type": "goog-remb",
                                "parameter": ""
                            },
                            {
                                "type": "transport-cc",
                                "parameter": ""
                            },
                            {
                                "type": "ccm",
                                "parameter": "fir"
                            },
                            {
                                "type": "nack",
                                "parameter": ""
                            },
                            {
                                "type": "nack",
                                "parameter": "pli"
                            }
                        ]
                    },
                    {
                        "mimeType": "video/rtx",
                        "kind": "video",
                        "preferredPayloadType": 104,
                        "clockRate": 90000,
                        "parameters": {
                            "apt": 103
                        },
                        "rtcpFeedback": []
                    },
                    {
                        "mimeType": "video/H264",
                        "kind": "video",
                        "preferredPayloadType": 105,
                        "clockRate": 90000,
                        "parameters": {
                            "level-asymmetry-allowed": 1,
                            "packetization-mode": 1,
                            "profile-level-id": "4d0032"
                        },
                        "rtcpFeedback": [
                            {
                                "type": "goog-remb",
                                "parameter": ""
                            },
                            {
                                "type": "transport-cc",
                                "parameter": ""
                            },
                            {
                                "type": "ccm",
                                "parameter": "fir"
                            },
                            {
                                "type": "nack",
                                "parameter": ""
                            },
                            {
                                "type": "nack",
                                "parameter": "pli"
                            }
                        ]
                    },
                    {
                        "mimeType": "video/rtx",
                        "kind": "video",
                        "preferredPayloadType": 106,
                        "clockRate": 90000,
                        "parameters": {
                            "apt": 105
                        },
                        "rtcpFeedback": []
                    },
                    {
                        "mimeType": "video/H264",
                        "kind": "video",
                        "preferredPayloadType": 107,
                        "clockRate": 90000,
                        "parameters": {
                            "level-asymmetry-allowed": 1,
                            "packetization-mode": 1,
                            "profile-level-id": "42e01f"
                        },
                        "rtcpFeedback": [
                            {
                                "type": "goog-remb",
                                "parameter": ""
                            },
                            {
                                "type": "transport-cc",
                                "parameter": ""
                            },
                            {
                                "type": "ccm",
                                "parameter": "fir"
                            },
                            {
                                "type": "nack",
                                "parameter": ""
                            },
                            {
                                "type": "nack",
                                "parameter": "pli"
                            }
                        ]
                    },
                    {
                        "mimeType": "video/rtx",
                        "kind": "video",
                        "preferredPayloadType": 108,
                        "clockRate": 90000,
                        "parameters": {
                            "apt": 107
                        },
                        "rtcpFeedback": []
                    }
                ],
                "headerExtensions": [
                    {
                        "kind": "audio",
                        "uri": "urn:ietf:params:rtp-hdrext:sdes:mid",
                        "preferredId": 1,
                        "preferredEncrypt": false,
                        "direction": "sendrecv"
                    },
                    {
                        "kind": "video",
                        "uri": "urn:ietf:params:rtp-hdrext:sdes:mid",
                        "preferredId": 1,
                        "preferredEncrypt": false,
                        "direction": "sendrecv"
                    },
                    {
                        "kind": "audio",
                        "uri": "http://www.webrtc.org/experiments/rtp-hdrext/abs-send-time",
                        "preferredId": 4,
                        "preferredEncrypt": false,
                        "direction": "sendrecv"
                    },
                    {
                        "kind": "video",
                        "uri": "http://www.webrtc.org/experiments/rtp-hdrext/abs-send-time",
                        "preferredId": 4,
                        "preferredEncrypt": false,
                        "direction": "sendrecv"
                    },
                    {
                        "kind": "video",
                        "uri": "http://www.ietf.org/id/draft-holmer-rmcat-transport-wide-cc-extensions-01",
                        "preferredId": 5,
                        "preferredEncrypt": false,
                        "direction": "sendrecv"
                    },
                    {
                        "kind": "audio",
                        "uri": "urn:ietf:params:rtp-hdrext:ssrc-audio-level",
                        "preferredId": 10,
                        "preferredEncrypt": false,
                        "direction": "sendrecv"
                    },
                    {
                        "kind": "video",
                        "uri": "urn:3gpp:video-orientation",
                        "preferredId": 11,
                        "preferredEncrypt": false,
                        "direction": "sendrecv"
                    },
                    {
                        "kind": "video",
                        "uri": "urn:ietf:params:rtp-hdrext:toffset",
                        "preferredId": 12,
                        "preferredEncrypt": false,
                        "direction": "sendrecv"
                    }
                ]
            },
            "sctpCapabilities": {
                "numStreams": {
                    "OS": 1024,
                    "MIS": 1024
                }
            }
        }
Room zzh  back {
            "peers": [
                {
                    "id": "yh1g6wbk",
                    "displayName": "Totodile",
                    "device": {
                        "flag": "chrome",
                        "name": "Chrome",
                        "version": "85.0.4183.83"
                    }
                }
            ]
        }
: 58337] webrtc: :FieldTrialParser: :ParseFieldTrial() | Failed to read empty key field with value: 'Enabled' in trial: "Enabled,100"

Room zzh connectWebRtcTransport  {
        "transportId": "ada55f6b-41ae-467d-9eee-7c4edf3747c7",
        "dtlsParameters": {
            "role": "server",
            "fingerprints": [
                {
                    "algorithm": "sha-256",
                    "value": "36:21:4A:19:64:41:E7:83:17:1F:22:33:EF:2E:22:B7:8B:78:85:1A:CA:E8:F4:F4:4C:65:E4:ED:48:38:74:84"
                }
            ]
        }
    }

Room zzh  back undefined
Room zzh connectWebRtcTransport  {
        "transportId": "ad8988ef-41d4-4be1-aaf0-c134ecea68f7",
        "dtlsParameters": {
            "role": "client",
            "fingerprints": [
                {
                    "algorithm": "sha-256",
                    "value": "78:65:C3:10:7E:38:49:97:20:5B:EB:01:F6:E9:F6:EB:13:7F:EE:BF:80:C8:D8:CB:44:4F:58:DB:9D:21:B1:89"
                }
            ]
        }
    }
Room zzh  back undefined
Room zzh produce  {
        "transportId": "ada55f6b-41ae-467d-9eee-7c4edf3747c7",
        "kind": "audio",
        "rtpParameters": {
            "codecs": [
                {
                    "mimeType": "audio/opus",
                    "payloadType": 111,
                    "clockRate": 48000,
                    "channels": 2,
                    "parameters": {
                        "minptime": 10,
                        "useinbandfec": 1,
                        "sprop-stereo": 1,
                        "usedtx": 1
                    },
                    "rtcpFeedback": [
                        {
                            "type": "transport-cc",
                            "parameter": ""
                        }
                    ]
                }
            ],
            "headerExtensions": [
                {
                    "uri": "urn:ietf:params:rtp-hdrext:sdes:mid",
                    "id": 4,
                    "encrypt": false,
                    "parameters": {}
                },
                {
                    "uri": "http://www.webrtc.org/experiments/rtp-hdrext/abs-send-time",
                    "id": 2,
                    "encrypt": false,
                    "parameters": {}
                },
                {
                    "uri": "http://www.ietf.org/id/draft-holmer-rmcat-transport-wide-cc-extensions-01",
                    "id": 3,
                    "encrypt": false,
                    "parameters": {}
                },
                {
                    "uri": "urn:ietf:params:rtp-hdrext:ssrc-audio-level",
                    "id": 1,
                    "encrypt": false,
                    "parameters": {}
                }
            ],
            "encodings": [
                {
                    "ssrc": 4156053425,
                    "dtx": false
                }
            ],
            "rtcp": {
                "cname": "QuIrLQ6CblKBIxBw",
                "reducedSize": true
            },
            "mid": "0"
        },
        "appData": {}
    }
Room zzh  back {
        "id": "e88ebf3f-5ca9-4ee0-abc7-49be08f116fa"
    }
Room zzh produce  {
        "transportId": "ada55f6b-41ae-467d-9eee-7c4edf3747c7",
        "kind": "video",
        "rtpParameters": {
            "codecs": [
                {
                    "mimeType": "video/VP8",
                    "payloadType": 96,
                    "clockRate": 90000,
                    "parameters": {},
                    "rtcpFeedback": [
                        {
                            "type": "goog-remb",
                            "parameter": ""
                        },
                        {
                            "type": "transport-cc",
                            "parameter": ""
                        },
                        {
                            "type": "ccm",
                            "parameter": "fir"
                        },
                        {
                            "type": "nack",
                            "parameter": ""
                        },
                        {
                            "type": "nack",
                            "parameter": "pli"
                        }
                    ]
                },
                {
                    "mimeType": "video/rtx",
                    "payloadType": 97,
                    "clockRate": 90000,
                    "parameters": {
                        "apt": 96
                    },
                    "rtcpFeedback": []
                }
            ],
            "headerExtensions": [
                {
                    "uri": "urn:ietf:params:rtp-hdrext:sdes:mid",
                    "id": 4,
                    "encrypt": false,
                    "parameters": {}
                },
                {
                    "uri": "urn:ietf:params:rtp-hdrext:sdes:rtp-stream-id",
                    "id": 5,
                    "encrypt": false,
                    "parameters": {}
                },
                {
                    "uri": "urn:ietf:params:rtp-hdrext:sdes:repaired-rtp-stream-id",
                    "id": 6,
                    "encrypt": false,
                    "parameters": {}
                },
                {
                    "uri": "http://www.webrtc.org/experiments/rtp-hdrext/abs-send-time",
                    "id": 2,
                    "encrypt": false,
                    "parameters": {}
                },
                {
                    "uri": "http://www.ietf.org/id/draft-holmer-rmcat-transport-wide-cc-extensions-01",
                    "id": 3,
                    "encrypt": false,
                    "parameters": {}
                },
                {
                    "uri": "urn:3gpp:video-orientation",
                    "id": 13,
                    "encrypt": false,
                    "parameters": {}
                },
                {
                    "uri": "urn:ietf:params:rtp-hdrext:toffset",
                    "id": 14,
                    "encrypt": false,
                    "parameters": {}
                }
            ],
            "encodings": [
                {
                    "active": true,
                    "scaleResolutionDownBy": 4,
                    "maxBitrate": 500000,
                    "rid": "r0",
                    "scalabilityMode": "S1T3",
                    "dtx": false
                },
                {
                    "active": true,
                    "scaleResolutionDownBy": 2,
                    "maxBitrate": 1000000,
                    "rid": "r1",
                    "scalabilityMode": "S1T3",
                    "dtx": false
                },
                {
                    "active": true,
                    "scaleResolutionDownBy": 1,
                    "maxBitrate": 5000000,
                    "rid": "r2",
                    "scalabilityMode": "S1T3",
                    "dtx": false
                }
            ],
            "rtcp": {
                "cname": "",
                "reducedSize": true
            },
            "mid": "2"
        },
        "appData": {}
    }
Room zzh  back {
        "id": "2e0dfa52-4be5-4e1f-b6ff-49158aa3ecbe"
    }
```




