/*
 * signaling.h
 *
 * Created by Ruibin.Chow on 2022/11/21.
 * Copyright (c) 2022年 Ruibin.Chow All rights reserved.
 */

#ifndef SIGNALING_H
#define SIGNALING_H

#include <string>
#include <optional>
#include <random>
#include <limits>
#include <nlohmann/json.hpp>
#include <nlohmann_json_reflect/json_reflect.hpp>

namespace {

using json = nlohmann::json;
using jString = std::string;

#define JSONABLE_STRUCT(STRUCT_NAME) \
            struct STRUCT_NAME : Jsonable<STRUCT_NAME>

#define FIELDS_REFLECT(...)  REFLECT_INTRUSIVE(__VA_ARGS__)
#define FIELDS_MAP(...)  std::unordered_map<jString, jString> GetReplaceMap() override { \
                                return {__VA_ARGS__};\
                        };

int32_t generateID() {
    std::random_device sd;
    std::minstd_rand linear(sd());
    std::uniform_int_distribution<int32_t>dist(0, std::numeric_limits<int32_t>::max());
    return dist(linear);
}

jString replace(jString strSrc,
                    const jString &oldStr,
                    const jString &newStr,
                    int count=-1) {
    jString strRet = strSrc;
    size_t pos = 0;
    int l_count = 0;
    if(-1 == count) // replace all
        count = strRet.size();
    while ((pos = strRet.find(oldStr, pos)) != std::string::npos) {
        strRet.replace(pos, oldStr.size(), newStr);
        if(++l_count >= count) break;
        pos += newStr.size();
    }
    return strRet;
}

}

namespace engine {

extern const std::string testSignaling;

template<typename T>
struct Jsonable {
    virtual ~Jsonable() = default;
    virtual std::unordered_map<jString, jString> GetReplaceMap() { return {}; };
    
    virtual std::string ToJsonString() {
        T* value = dynamic_cast<T *>(this);
        jString jsonString = jreflect::to_json(*value);
        for (auto& [k, v] : GetReplaceMap()) {
            jsonString = replace(jsonString, k, v);
        }
        return jsonString;
    }
    virtual void FromJsonString(const jString& jsonString) {
        T* value = dynamic_cast<T *>(this);
        jString jsonStr = jsonString;
        for (auto& [k, v] : GetReplaceMap()) {
            jsonStr = replace(jsonStr, v, k);
        }
        *value = jreflect::from_json<T>(jsonStr);
    }
};

#pragma mark - Basic

struct MessageType : Jsonable<MessageType> {
    std::optional<bool> request;
    std::optional<bool> response;
    std::optional<bool> notification;
    FIELDS_REFLECT(MessageType, request, response, notification);
};

struct RequestHeader : Jsonable<RequestHeader> {
    std::optional<bool> request;
    std::optional<int64_t> id;
    std::optional<std::string> method;
    FIELDS_REFLECT(RequestHeader, request, id, method);
};

struct ResponseHeader : Jsonable<ResponseHeader> {
    std::optional<bool> response;
    std::optional<int64_t> id;
    std::optional<bool> ok;
    FIELDS_REFLECT(ResponseHeader, response, id, ok);
};

struct NotificationHeader : Jsonable<NotificationHeader> {
    std::optional<bool> notification;
    std::optional<std::string> method;
    FIELDS_REFLECT(NotificationHeader, notification, method);
};

struct BasicResponse : Jsonable<BasicResponse> {
    std::optional<bool> response;
    std::optional<int64_t> id;
    std::optional<bool> ok;
    FIELDS_REFLECT(BasicResponse, response, id, ok);
};

#pragma mark - Common

struct RTCPFeedback {
    std::optional<std::string> type;
    std::optional<std::string> parameter;
    FIELDS_REFLECT(RTCPFeedback, type, parameter);
};

struct Device {
    std::optional<std::string> flag;
    std::optional<std::string> name;
    std::optional<std::string> version;
    FIELDS_REFLECT(Device, flag, name, version);
};

struct Parameters {
    std::optional<int32_t> apt;
    std::optional<int32_t> levelAsymmetryAllowed;
    std::optional<int32_t> packetizationMode;
    std::optional<std::string> profileLevelId;
    std::optional<int32_t> xGoogleStartBitrate;
    std::optional<int32_t> minptime;
    std::optional<int32_t> useinbandfec;
    std::optional<int32_t> profileId;
    std::optional<int32_t> spropstereo;
    std::optional<int32_t> usedtx;
    FIELDS_REFLECT(Parameters,
                   levelAsymmetryAllowed, packetizationMode,
                   profileLevelId, xGoogleStartBitrate,
                   minptime, useinbandfec, profileId,
                   spropstereo, usedtx);
};

struct Codec {
    std::optional<std::string> kind;
    std::optional<std::string> mimeType;
    std::optional<int32_t> clockRate;
    std::optional<int32_t> channels;
    std::optional<std::vector<RTCPFeedback>> rtcpFeedback;
    std::optional<Parameters> parameters;
    std::optional<int32_t> preferredPayloadType;
    std::optional<int32_t> payloadType;
    FIELDS_REFLECT(Codec,
                   kind, mimeType, clockRate, channels,
                   rtcpFeedback, parameters, preferredPayloadType, payloadType);
};

struct HeaderExtension {
    std::optional<std::string> kind;
    std::optional<std::string> uri;
    std::optional<int32_t> id;
    std::optional<bool> encrypt;
    std::optional<int32_t> preferredId;
    std::optional<bool> preferredEncrypt;
    std::optional<std::string> direction;
    FIELDS_REFLECT(HeaderExtension,
                   id, encrypt,
                   kind, uri, preferredId, preferredEncrypt, direction);
};

struct NumStreams {
    std::optional<int32_t> MIS;
    std::optional<int32_t> OS;
    FIELDS_REFLECT(NumStreams, MIS, OS);
};

struct SCTPCapabilities {
    std::optional<NumStreams> numStreams;
    FIELDS_REFLECT(SCTPCapabilities, numStreams);
};

struct AppData {
    std::optional<std::string> peerId;
    std::optional<std::string> info;
    FIELDS_REFLECT(AppData, peerId, info);
};

struct Fingerprint {
    std::optional<std::string> algorithm;
    std::optional<std::string> value;
    FIELDS_REFLECT(Fingerprint, algorithm, value);
};

struct DTLSParameters {
    std::optional<std::vector<Fingerprint>> fingerprints;
    std::optional<std::string> role;
    FIELDS_REFLECT(DTLSParameters, fingerprints, role);
};

struct RTX {
    std::optional<int64_t> ssrc;
    FIELDS_REFLECT(RTX, ssrc);
};

struct Encoding {
    std::optional<int64_t> ssrc;
    std::optional<bool> active;
    std::optional<bool> dtx;
    std::optional<int32_t> maxBitrate;
    std::optional<std::string> rid;
    std::optional<std::string> scalabilityMode;
    std::optional<float> scaleResolutionDownBy;
    std::optional<RTX> rtx;
    FIELDS_REFLECT(Encoding,
                   ssrc, active, dtx, maxBitrate, rid, rtx,
                   scalabilityMode, scaleResolutionDownBy);
};

struct Rtcp {
    std::optional<std::string> cname;
    std::optional<bool> mux;
    std::optional<bool> reducedSize;
    FIELDS_REFLECT(Rtcp, cname, mux, reducedSize);
};

struct RTPParameters {
    std::optional<std::vector<Encoding>> encodings;
    std::optional<std::vector<Codec>> codecs;
    std::optional<std::vector<HeaderExtension>> headerExtensions;
    std::optional<std::string> mid;
    std::optional<Rtcp> rtcp;
    FIELDS_REFLECT(RTPParameters, encodings, codecs, headerExtensions, mid, rtcp);
};

struct SCTPStreamParameters {
    std::optional<int32_t> streamId;
    std::optional<bool> ordered;
    std::optional<int32_t> maxRetransmits;
    FIELDS_REFLECT(SCTPStreamParameters, streamId, ordered, maxRetransmits);
};

#pragma mark - GetRouterRtpCapabilities

struct GetRouterRtpCapabilitiesRequest : Jsonable<GetRouterRtpCapabilitiesRequest> {
    std::optional<bool> request = true;
    std::optional<int64_t> id = generateID();
    std::optional<jString> method = "getRouterRtpCapabilities";
    FIELDS_REFLECT(GetRouterRtpCapabilitiesRequest, request, id, method);
};

struct GetRouterRtpCapabilitiesResponse : Jsonable<GetRouterRtpCapabilitiesResponse> {
    struct Data {
        std::optional<std::vector<Codec>> codecs;
        std::optional<std::vector<HeaderExtension>> headerExtensions;
        FIELDS_REFLECT(Data, codecs, headerExtensions);
    };

    std::optional<bool> response;
    std::optional<int64_t> id;
    std::optional<bool> ok;
    std::optional<Data> data;
    FIELDS_REFLECT(GetRouterRtpCapabilitiesResponse, response, id, ok, data);
    FIELDS_MAP(
               {"levelAsymmetryAllowed","level-asymmetry-allowed"},
               {"packetizationMode","packetization-mode"},
               {"profileLevelId", "profile-level-id"},
               {"xGoogleStartBitrate", "x-google-start-bitrate"}
               );
};

#pragma mark - CreateWebRtcTransport

struct CreateWebRtcTransportRequest : Jsonable<CreateWebRtcTransportRequest> {
    struct Data {
        std::optional<bool> consuming;
        std::optional<bool> producing;
        std::optional<bool> forceTcp;
        std::optional<SCTPCapabilities> sctpCapabilities;
        FIELDS_REFLECT(Data, consuming, producing, forceTcp, sctpCapabilities);
    };

    std::optional<bool> request = true;
    std::optional<int64_t> id = generateID();
    std::optional<std::string> method = "createWebRtcTransport";
    std::optional<Data> data;
    FIELDS_REFLECT(CreateWebRtcTransportRequest, request, id, method, data);
};

struct CreateWebRtcTransportResponse : Jsonable<CreateWebRtcTransportResponse> {
    struct ICECandidate {
        std::optional<std::string> type;
        std::optional<std::string> protocol;
        std::optional<std::string> foundation;
        std::optional<std::string> ip;
        std::optional<int32_t> port;
        std::optional<int32_t> priority;
        FIELDS_REFLECT(ICECandidate,
                       type, protocol, foundation, ip, port, priority);
    };

    struct ICEParameters {
        std::optional<bool> iceLite;
        std::optional<std::string> password;
        std::optional<std::string> usernameFragment;
        FIELDS_REFLECT(ICEParameters, iceLite, password, usernameFragment);
    };

    struct SCTPParameters {
        std::optional<int32_t> MIS;
        std::optional<int32_t> OS;
        std::optional<bool> isDataChannel;
        std::optional<int32_t> port;
        std::optional<int32_t> maxMessageSize;
        std::optional<int32_t> sctpBufferedAmount;
        std::optional<int32_t> sendBufferSize;
        FIELDS_REFLECT(SCTPParameters,
                       MIS, OS, isDataChannel, port, maxMessageSize,
                       sctpBufferedAmount, sendBufferSize);
    };

    struct Data {
        std::optional<std::string> id;
        std::optional<DTLSParameters> dtlsParameters;
        std::optional<std::vector<ICECandidate>> iceCandidates;
        std::optional<ICEParameters> iceParameters;
        std::optional<SCTPParameters> sctpParameters;
        FIELDS_REFLECT(Data,
                       id, dtlsParameters, iceCandidates, iceParameters, sctpParameters);
    };

    std::optional<bool> request = true;
    std::optional<int64_t> id = generateID();
    std::optional<bool> ok;
    std::optional<Data> data;
    FIELDS_REFLECT(CreateWebRtcTransportResponse, request, id, ok, data);
};

#pragma mark - Join

struct JoinRequest : Jsonable<JoinRequest> {
    struct RTPCapabilities {
        std::optional<std::vector<Codec>> codecs;
        std::optional<std::vector<HeaderExtension>> headerExtensions;
        FIELDS_REFLECT(RTPCapabilities, codecs, headerExtensions);
    };
    struct Data {
        std::optional<std::string> displayName;
        std::optional<Device> device;
        std::optional<RTPCapabilities> rtpCapabilities;
        std::optional<SCTPCapabilities> sctpCapabilities;
        FIELDS_REFLECT(Data, displayName, device, rtpCapabilities, sctpCapabilities);
    };

    std::optional<bool> request = true;
    std::optional<int64_t> id = generateID();
    std::optional<std::string> method = "join";
    std::optional<Data> data;
    FIELDS_REFLECT(JoinRequest, request, id, method, data);
    FIELDS_MAP(
               {"levelAsymmetryAllowed", "level-asymmetry-allowed"},
               {"packetizationMode", "packetization-mode"},
               {"profileLevelId", "profile-level-id"},
               {"xGoogleStartBitrate", "x-google-start-bitrate"},
               {"profileId", "profile-id"}
               );
};

struct JoinResponse : Jsonable<JoinResponse> {
    struct Peer {
        std::optional<std::string> id;
        std::optional<std::string> displayName;
        std::optional<Device> device;
        FIELDS_REFLECT(Peer, id, displayName, device);
    };

    struct Data {
        std::optional<std::vector<Peer>> peers;
        FIELDS_REFLECT(Data, peers);
    };

    std::optional<bool> response;
    std::optional<int64_t> id;
    std::optional<bool> ok;
    std::optional<Data> data;
    FIELDS_REFLECT(JoinResponse, response, id, ok, data);
};

#pragma mark - Consumer

struct NewDataConsumerRequest : Jsonable<NewDataConsumerRequest> {
    struct Data {
        std::optional<std::string> id;
        std::optional<std::string> peerId;
        std::optional<std::string> dataProducerId;
        std::optional<std::string> label;
        std::optional<std::string> protocol;
        std::optional<SCTPStreamParameters> sctpStreamParameters;
        std::optional<AppData> appData;
        FIELDS_REFLECT(Data,
                       id, peerId, dataProducerId, label, protocol,
                       sctpStreamParameters, appData);
    };

    std::optional<bool> request;
    std::optional<int64_t> id;
    std::optional<std::string> method;
    std::optional<Data> data;
    FIELDS_REFLECT(NewDataConsumerRequest, request, id, method, data);
};

struct NewConsumerRequest : Jsonable<NewConsumerRequest> {
    struct Parameters {
        std::optional<int32_t> spropstereo;
        std::optional<int32_t> minptime;
        std::optional<int32_t> useinbandfec;
        std::optional<int32_t> usedtx;
        FIELDS_REFLECT(Parameters, spropstereo, usedtx, minptime, useinbandfec);
    };
    
    struct Data {
        std::optional<std::string> id;
        std::optional<std::string> peerId;
        std::optional<std::string> type;
        std::optional<std::string> kind;
        std::optional<std::string> producerId;
        std::optional<bool> producerPaused;
        std::optional<RTPParameters> rtpParameters;
        std::optional<AppData> appData;
        FIELDS_REFLECT(Data,
                       id, peerId, kind, type, producerId,
                       producerPaused,  rtpParameters,  appData);
    };

    std::optional<bool> request;
    std::optional<int64_t> id;
    std::optional<std::string> method;
    std::optional<Data> data;
    FIELDS_REFLECT(NewConsumerRequest, request, id,  method, data);
    FIELDS_MAP({"spropstereo", "sprop-stereo"});
};

#pragma mark - ConnectWebRtcTransport

struct ConnectWebRtcTransportRequest : Jsonable<ConnectWebRtcTransportRequest> {
    struct Data {
        std::optional<std::string> transportId;
        std::optional<DTLSParameters> dtlsParameters;
        FIELDS_REFLECT(Data, transportId, dtlsParameters);
    };

    std::optional<bool> request = true;
    std::optional<int64_t> id = generateID();
    std::optional<std::string> method = "connectWebRtcTransport";
    std::optional<Data> data;
    FIELDS_REFLECT(ConnectWebRtcTransportRequest, request, id, method, data);
};

#pragma mark - Produce

struct ProduceRequest : Jsonable<ProduceRequest> {
    struct Data {
        std::optional<std::string> transportId;
        std::optional<std::string> kind;
        std::optional<RTPParameters> rtpParameters;
        std::optional<AppData> appData;
        FIELDS_REFLECT(Data, transportId, kind, rtpParameters, appData);
    };

    std::optional<bool> request = true;
    std::optional<int64_t> id = generateID();
    std::optional<std::string> method = "produce";
    std::optional<Data> data;
    FIELDS_REFLECT(ProduceRequest, request, id, method, data);
    FIELDS_MAP({"spropstereo", "sprop-stereo"});
};

struct ProduceResponse : Jsonable<ProduceResponse> {
    struct Data {
        std::optional<std::string> id;
        FIELDS_REFLECT(Data, id);
    };

    std::optional<bool> response;
    std::optional<int64_t> id;
    std::optional<bool> ok;
    std::optional<Data> data;
    FIELDS_REFLECT(ProduceResponse, response, id, ok, data);
};

#pragma mark - ProduceData

struct ProduceDataRequest : Jsonable<ProduceDataRequest> {
    struct Data {
        std::optional<std::string> transportId;
        std::optional<std::string> label;
        std::optional<std::string> protocol;
        std::optional<SCTPStreamParameters> sctpStreamParameters;
        std::optional<AppData> appData;
        FIELDS_REFLECT(Data,
                       transportId, label, protocol, sctpStreamParameters, appData);
    };

    std::optional<bool> request;
    std::optional<int64_t> id = generateID();
    std::optional<std::string> method = "produceData";
    std::optional<Data> data;
    FIELDS_REFLECT(ProduceDataRequest, request, id, method, data);
};

struct ProduceDataResponse : Jsonable<ProduceDataResponse> {
    struct Data {
        std::optional<std::string> id;
        FIELDS_REFLECT(Data, id);
    };

    std::optional<bool> response;
    std::optional<int64_t> id;
    std::optional<bool> ok;
    std::optional<Data> data;
    FIELDS_REFLECT(ProduceDataResponse, response, id, ok, data);
};

struct ProducerRequest : Jsonable<ProducerRequest> {
    struct Data {
        std::optional<std::string> producerId;
        FIELDS_REFLECT(Data, producerId);
    };
    std::optional<bool> request = true;
    std::optional<int64_t> id = generateID();

    // method: 'pauseProducer', 'resumeProducer', 'closeProducer'
    std::optional<std::string> method;
    std::optional<Data> data;
    FIELDS_REFLECT(ProducerRequest, request, id, method, data);
};

#pragma mark - Consumer

struct ConsumerRequest : Jsonable<ConsumerRequest> {
    struct Data {
        std::optional<std::string> consumerId;
        FIELDS_REFLECT(Data, consumerId);
    };
    std::optional<bool> request = true;
    std::optional<int64_t> id = generateID();

    // method: 'pauseConsumer', 'resumeConsumer', 'requestConsumerKeyFrame'
    std::optional<std::string> method;
    std::optional<Data> data;
    FIELDS_REFLECT(ConsumerRequest, request, id, method, data);
};

#pragma mark -

struct ChangeDisplayNameRequest : Jsonable<ChangeDisplayNameRequest> {
    struct Data {
        std::optional<std::string> displayName;
        FIELDS_REFLECT(Data, displayName);
    };
    std::optional<bool> request = true;
    std::optional<int64_t> id = generateID();
    std::optional<std::string> method = "changeDisplayName";
    std::optional<Data> data;
    FIELDS_REFLECT(ChangeDisplayNameRequest, request,id, method, data);
};

struct RestartICERequest : Jsonable<RestartICERequest> {
    struct Data {
        std::optional<std::string> transportId;
        FIELDS_REFLECT(Data, transportId);
    };

    std::optional<bool> request = true;
    std::optional<int64_t> id = generateID();
    std::optional<std::string> method = "restartIce";
    std::optional<Data> data;
    FIELDS_REFLECT(RestartICERequest, request, id, method, data);
};

struct RestartICEResponse : Jsonable<RestartICEResponse> {
    struct Data {
        std::optional<bool> iceLite;
        std::optional<std::string> password;
        std::optional<std::string> usernameFragment;
        FIELDS_REFLECT(Data, iceLite, password, usernameFragment);
    };

    std::optional<bool> response;
    std::optional<int64_t> id;
    std::optional<bool> ok;
    std::optional<Data> data;
    FIELDS_REFLECT(RestartICEResponse, response, id, ok, data);
};

#pragma mark - SetConsumer

struct SetConsumerPriorityRequest : Jsonable<SetConsumerPriorityRequest> {
    struct Data {
        std::optional<std::string> consumerId;
        std::optional<int32_t> priority;
        FIELDS_REFLECT(Data, consumerId, priority);
    };
    std::optional<bool> request = true;
    std::optional<int64_t> id = generateID();

    std::optional<std::string> method = "setConsumerPriority";
    std::optional<Data> data;
    FIELDS_REFLECT(SetConsumerPriorityRequest, request, id, method, data);
};

struct SetConsumerPreferredLayersRequest : Jsonable<SetConsumerPreferredLayersRequest> {
    struct Data {
        std::optional<std::string> consumerId;
        std::optional<int32_t> spatialLayer;
        std::optional<int32_t> temporalLayer;
        FIELDS_REFLECT(Data, consumerId, spatialLayer, temporalLayer);
    };
    std::optional<bool> request = true;
    std::optional<int64_t> id = generateID();

    std::optional<std::string> method = "setConsumerPriority";
    std::optional<Data> data;
    FIELDS_REFLECT(SetConsumerPreferredLayersRequest,
                   request, id, method, data);
};

#pragma mark - GetTransportStats

struct GetTransportStatsRequest : Jsonable<GetTransportStatsRequest> {
    struct Data {
        std::optional<std::string> transportId;
        FIELDS_REFLECT(Data, transportId);
    };
    std::optional<bool> request = true;
    std::optional<int64_t> id = generateID();

    std::optional<std::string> method = "getTransportStats";
    std::optional<Data> data;
    FIELDS_REFLECT(GetTransportStatsRequest, request, id, method, data);
};

struct GetTransportStatsResponse : Jsonable<GetTransportStatsResponse> {
    struct ICESelectedTuple {
        std::optional<std::string> localIp;
        std::optional<int32_t> localPort;
        std::optional<std::string> remoteIp;
        std::optional<int32_t> remotePort;
        std::optional<std::string> protocol;
        FIELDS_REFLECT(ICESelectedTuple,
                       localIp, localPort, remoteIp, remotePort, protocol);
    };

    struct Data {
        std::optional<std::string> transportId;
        std::optional<std::string> type;
        std::optional<int64_t> availableOutgoingBitrate;
        std::optional<int64_t> bytesReceived;
        std::optional<int64_t> bytesSent;
        std::optional<std::string> dtlsState;
        std::optional<std::string> iceRole;
        std::optional<ICESelectedTuple> iceSelectedTuple;
        std::optional<std::string> iceState;
        std::optional<int64_t> maxIncomingBitrate;
        std::optional<int64_t> probationBytesSent;
        std::optional<int64_t> probationSendBitrate;
        std::optional<int64_t> recvBitrate;
        std::optional<int64_t> rtpBytesReceived;
        std::optional<int64_t> rtpBytesSent;
        std::optional<int64_t> rtpPacketLossReceived;
        std::optional<int64_t> rtpRecvBitrate;
        std::optional<int64_t> rtpSendBitrate;
        std::optional<int64_t> rtxBytesReceived;
        std::optional<int64_t> rtxBytesSent;
        std::optional<int64_t> rtxRecvBitrate;
        std::optional<int64_t> rtxSendBitrate;
        std::optional<std::string> sctpState;
        std::optional<int64_t> sendBitrate;
        std::optional<int64_t> timestamp;
        FIELDS_REFLECT(Data, transportId,
                   type, availableOutgoingBitrate, bytesReceived,
                   bytesSent, dtlsState, iceRole, iceSelectedTuple,
                   iceState, maxIncomingBitrate, probationBytesSent,
                   probationSendBitrate, recvBitrate,
                   rtpBytesReceived, rtpBytesSent,
                   rtpPacketLossReceived, rtpSendBitrate,
                   rtxBytesReceived, rtxBytesSent, rtxRecvBitrate,
                   rtxSendBitrate, sctpState, sendBitrate, timestamp);
    };

    std::optional<bool> response;
    std::optional<int64_t> id;
    std::optional<bool> ok;
    std::optional<std::vector<Data>> data;
    FIELDS_REFLECT(GetTransportStatsResponse, response, id, ok, data);
};

#pragma mark - GetProducerStats

struct GetProducerStatsRequest : Jsonable<GetProducerStatsRequest> {
    struct Data {
        std::optional<std::string> producerId;
        FIELDS_REFLECT(Data, producerId);
    };
    std::optional<bool> request = true;
    std::optional<int64_t> id = generateID();

    std::optional<std::string> method = "getProducerStats";
    std::optional<Data> data;
    FIELDS_REFLECT(GetProducerStatsRequest, request, id, method, data);
};

struct GetProducerStatsResponse : Jsonable<GetProducerStatsResponse> {
    FIELDS_MAP({"L00", "0.0"}, {"L01", "0.1"}, {"L02", "0.2"});
    struct BitrateByLayer {
        std::optional<int32_t> L00;
        std::optional<int32_t> L01;
        std::optional<int32_t> L02;
        FIELDS_REFLECT(BitrateByLayer, L00, L01, L02);
    };

    struct Data {
        std::optional<std::string> type;
        std::optional<std::string> kind;
        std::optional<std::string> mimeType;
        std::optional<std::string> rid;
        std::optional<int64_t> bitrate;
        std::optional<int64_t> byteCount;
        std::optional<int64_t> firCount;
        std::optional<int64_t> fractionLost;
        std::optional<int64_t> jitter;
        std::optional<int64_t> nackCount;
        std::optional<int64_t> nackPacketCount;
        std::optional<int64_t> packetCount;
        std::optional<int64_t> packetsDiscarded;
        std::optional<int64_t> packetsLost;
        std::optional<int64_t> packetsRepaired;
        std::optional<int64_t> packetsRetransmitted;
        std::optional<int64_t> pliCount;
        std::optional<double> roundTripTime;
        std::optional<int64_t> rtxPacketsDiscarded;
        std::optional<int64_t> rtxSsrc;
        std::optional<int64_t> score;
        std::optional<int64_t> ssrc;
        std::optional<int64_t> timestamp;
        std::optional<BitrateByLayer> bitrateByLayer;
        FIELDS_REFLECT(Data,
                       type, kind, mimeType, rid, bitrate, byteCount, firCount,
                       fractionLost, jitter, nackCount, nackPacketCount,
                       packetCount, packetsDiscarded, packetsLost, packetsRepaired,
                       packetsRetransmitted, pliCount, roundTripTime,
                       rtxPacketsDiscarded, rtxSsrc, score, ssrc,
                       timestamp, bitrateByLayer);
    };

    std::optional<bool> response;
    std::optional<int64_t> id;
    std::optional<bool> ok;
    std::optional<std::vector<Data>> data;
    FIELDS_REFLECT(GetProducerStatsResponse, response, id, ok, data);
};

#pragma mark - GetDataProducerStats

struct GetDataProducerStatsRequest : Jsonable<GetDataProducerStatsRequest> {
    struct Data {
        std::optional<std::string> dataProducerId;
        FIELDS_REFLECT(Data, dataProducerId);
    };
    std::optional<bool> request = true;
    std::optional<int64_t> id = generateID();

    std::optional<std::string> method = "getDataProducerStats";
    std::optional<Data> data;
    FIELDS_REFLECT(GetDataProducerStatsRequest, request, id, method, data);
};

struct GetDataProducerStatsResponse : Jsonable<GetDataProducerStatsResponse> {
    struct Data {
        std::optional<std::string> type;
        std::optional<std::string> label;
        std::optional<std::string> protocol;
        std::optional<int64_t> bytesReceived;
        std::optional<int64_t> messagesReceived;
        std::optional<int64_t> timestamp;
        FIELDS_REFLECT(Data, type, label, protocol,
                       bytesReceived, messagesReceived, timestamp);
    };

    std::optional<bool> response;
    std::optional<int64_t> id;
    std::optional<bool> ok;
    std::optional<std::vector<Data>> data;
    FIELDS_REFLECT(GetDataProducerStatsResponse, response, id, ok, data);
};

#pragma mark - GetConsumerStats

struct GetConsumerStatsRequest : Jsonable<GetConsumerStatsRequest> {
    struct Data {
        std::optional<std::string> consumerId;
        FIELDS_REFLECT(Data, consumerId);
    };
    std::optional<bool> request = true;
    std::optional<int64_t> id = generateID();

    std::optional<std::string> method = "getConsumerStats";
    std::optional<Data> data;
    FIELDS_REFLECT(GetConsumerStatsRequest, request, id, method, data);
};

struct GetConsumerStatsResponse : Jsonable<GetConsumerStatsResponse> {
    FIELDS_MAP({"L00", "0.0"}, {"L01", "0.1"}, {"L02", "0.2"});
    struct BitrateByLayer {
        std::optional<int32_t> L00;
        std::optional<int32_t> L01;
        std::optional<int32_t> L02;
        FIELDS_REFLECT(BitrateByLayer, L00, L01, L02);
    };
    struct Data {
        std::optional<std::string> type;
        std::optional<std::string> kind;
        std::optional<std::string> mimeType;
        std::optional<std::string> rid;
        std::optional<int64_t> bitrate;
        std::optional<int64_t> byteCount;
        std::optional<int64_t> firCount;
        std::optional<int64_t> fractionLost;
        std::optional<int64_t> jitter;
        std::optional<int64_t> nackCount;
        std::optional<int64_t> nackPacketCount;
        std::optional<int64_t> packetCount;
        std::optional<int64_t> packetsDiscarded;
        std::optional<int64_t> packetsLost;
        std::optional<int64_t> packetsRepaired;
        std::optional<int64_t> packetsRetransmitted;
        std::optional<int64_t> pliCount;
        std::optional<int64_t> rtxPacketsDiscarded;
        std::optional<int64_t> rtxSsrc;
        std::optional<double> roundTripTime;
        std::optional<int64_t> score;
        std::optional<int64_t> ssrc;
        std::optional<int64_t> timestamp;
        std::optional<BitrateByLayer> bitrateByLayer;
        FIELDS_REFLECT(Data, type, kind, mimeType, rid, bitrate, byteCount,
                       firCount, fractionLost, jitter, nackCount, nackPacketCount,
                       packetCount, packetsDiscarded, packetsLost,
                       packetsRepaired, packetsRetransmitted, pliCount,
                       rtxPacketsDiscarded, rtxSsrc, roundTripTime, score,
                       ssrc, timestamp, bitrateByLayer);
    };

    std::optional<bool> response;
    std::optional<int64_t> id;
    std::optional<bool> ok;
    std::optional<std::vector<Data>> data;
    FIELDS_REFLECT(GetConsumerStatsResponse, response, id, ok, data);
};

#pragma mark - GetDataConsumerStats

struct GetDataConsumerStatsRequest : Jsonable<GetDataConsumerStatsRequest> {
    struct Data {
        std::optional<std::string> dataConsumerId;
        FIELDS_REFLECT(Data, dataConsumerId);
    };
    std::optional<bool> request = true;
    std::optional<int64_t> id = generateID();

    std::optional<std::string> method = "getDataConsumerStats";
    std::optional<Data> data;
    FIELDS_REFLECT(GetDataConsumerStatsRequest, request, id, method, data);
};

struct GetDataConsumerStatsResponse : Jsonable<GetDataConsumerStatsResponse> {
    struct Data {
        std::optional<std::string> type;
        std::optional<std::string> label;
        std::optional<std::string> protocol;
        std::optional<int64_t> bufferedAmount;
        std::optional<int64_t> bytesSent;
        std::optional<int64_t> messagesSent;
        std::optional<int64_t> timestamp;
        FIELDS_REFLECT(Data, type, label, protocol, bufferedAmount,
                       bytesSent, messagesSent, timestamp);
    };

    std::optional<bool> response;
    std::optional<int64_t> id;
    std::optional<bool> ok;
    std::optional<std::vector<Data>> data;
    FIELDS_REFLECT(GetDataConsumerStatsResponse, response, id, ok, data);
};

#pragma mark - NetworkThrottle

struct ApplyNetworkThrottleRequest : Jsonable<ApplyNetworkThrottleRequest> {
    struct Data {
        std::optional<std::string> secret;
        std::optional<int64_t> uplink;
        std::optional<int64_t> downlink;
        std::optional<int64_t> rtt;
        FIELDS_REFLECT(Data, secret, uplink, downlink, rtt);
    };
    std::optional<bool> request = true;
    std::optional<int64_t> id = generateID();

    std::optional<std::string> method = "applyNetworkThrottle";
    std::optional<Data> data;
    FIELDS_REFLECT(ApplyNetworkThrottleRequest, request, id, method, data);
};

struct ResetNetworkThrottleRequest : Jsonable<ResetNetworkThrottleRequest> {
    struct Data {
        std::optional<std::string> secret;
        FIELDS_REFLECT(Data, secret);
    };
    std::optional<bool> request = true;
    std::optional<int64_t> id = generateID();

    std::optional<std::string> method = "resetNetworkThrottle";
    std::optional<Data> data;
    FIELDS_REFLECT(ResetNetworkThrottleRequest, request, id, method, data);
};

#pragma mark - Notification

struct DownlinkBweNotification : Jsonable<DownlinkBweNotification> {
    struct Data {
        std::optional<int64_t> availableBitrate;
        std::optional<int64_t> desiredBitrate;
        std::optional<int64_t> effectiveDesiredBitrate;
        FIELDS_REFLECT(Data, availableBitrate,
                       desiredBitrate, effectiveDesiredBitrate);
    };

    std::optional<bool> notification;
    std::optional<std::string> method;
    std::optional<Data> data;
    FIELDS_REFLECT(DownlinkBweNotification, notification, method, data);
};

struct ConsumerScoreNotification : Jsonable<ConsumerScoreNotification> {
    struct Score {
        std::optional<int32_t> producerScore;
        std::optional<std::vector<int32_t>> producerScores;
        std::optional<int32_t> score;
        FIELDS_REFLECT(Score, producerScore, producerScores, score);
    };

    struct Data {
        std::optional<std::string> consumerId;
        std::optional<Score> score;
        FIELDS_REFLECT(Data, consumerId, score);
    };

    std::optional<bool> notification;
    std::optional<std::string> method;
    std::optional<Data> data;
    FIELDS_REFLECT(ConsumerScoreNotification, notification, method, data);
};

struct ProducerScoreNotification : Jsonable<ProducerScoreNotification> {
    struct Score {
        std::optional<std::string> rid;
        std::optional<int32_t> encodingIdx;
        std::optional<int32_t> score;
        std::optional<int64_t> ssrc;
        FIELDS_REFLECT(Score, rid, encodingIdx, score, ssrc);
    };

    struct Data {
        std::optional<std::string> producerId;
        std::optional<std::vector<Score>> score;
        FIELDS_REFLECT(Data, producerId, score);
    };

    std::optional<bool> notification;
    std::optional<std::string> method;
    std::optional<Data> data;
    FIELDS_REFLECT(ProducerScoreNotification, notification, method, data);
};

struct ConsumerLayersChangedNotification : Jsonable<ConsumerLayersChangedNotification> {
    struct Data {
        std::optional<std::string> consumerId;
        std::optional<int32_t> spatialLayer;
        std::optional<int32_t> temporalLayer;
        FIELDS_REFLECT(Data, consumerId, spatialLayer, temporalLayer);
    };

    std::optional<bool> notification;
    std::optional<std::string> method;
    std::optional<Data> data;
    FIELDS_REFLECT(ConsumerLayersChangedNotification, notification, method, data);
};

struct ActiveSpeakerNotification : Jsonable<ActiveSpeakerNotification> {
    struct Data {
        std::optional<std::string> peerId;
        std::optional<int32_t> volume;
        FIELDS_REFLECT(Data, peerId, volume);
    };

    std::optional<bool> notification;
    std::optional<std::string> method;
    std::optional<Data> data;
    FIELDS_REFLECT(ActiveSpeakerNotification, notification, method, data);
};

struct ConsumerPausedNotification : Jsonable<ConsumerPausedNotification> {
    struct Data {
        std::optional<std::string> consumerId;
        FIELDS_REFLECT(Data, consumerId);
    };

    std::optional<bool> notification;
    std::optional<std::string> method;
    std::optional<Data> data;
    FIELDS_REFLECT(ConsumerPausedNotification, notification, method, data);
};

struct ConsumerResumedNotification : Jsonable<ConsumerResumedNotification> {
    struct Data {
        std::optional<std::string> consumerId;
        FIELDS_REFLECT(Data, consumerId);
    };

    std::optional<bool> notification;
    std::optional<std::string> method;
    std::optional<Data> data;
    FIELDS_REFLECT(ConsumerResumedNotification, notification, method, data);
};

struct ConsumerClosedNotification : Jsonable<ConsumerClosedNotification> {
    struct Data {
        std::optional<std::string> consumerId;
        FIELDS_REFLECT(Data, consumerId);
    };

    std::optional<bool> notification;
    std::optional<std::string> method;
    std::optional<Data> data;
    FIELDS_REFLECT(ConsumerClosedNotification, notification, method, data);
};

struct DataConsumerClosedNotification : Jsonable<DataConsumerClosedNotification> {
    struct Data {
        std::optional<std::string> dataConsumerId;
        FIELDS_REFLECT(Data, dataConsumerId);
    };

    std::optional<bool> notification;
    std::optional<std::string> method;
    std::optional<Data> data;
    FIELDS_REFLECT(DataConsumerClosedNotification, notification, method, data);
};

struct PeerDisplayNameChangedNotification : Jsonable<PeerDisplayNameChangedNotification> {
    struct Data {
        std::optional<std::string> peerId;
        std::optional<std::string> oldDisplayName;
        std::optional<std::string> displayName;
        FIELDS_REFLECT(Data, peerId, oldDisplayName, displayName);
    };

    std::optional<bool> notification;
    std::optional<std::string> method;
    std::optional<Data> data;
    FIELDS_REFLECT(PeerDisplayNameChangedNotification, notification, method, data);
};

struct NewPeerNotification : Jsonable<NewPeerNotification> {
    struct Data {
        std::optional<std::string> id;
        std::optional<std::string> displayName;
        std::optional<Device> device;
        FIELDS_REFLECT(Data, id, displayName, device);
    };

    std::optional<bool> notification;
    std::optional<std::string> method;
    std::optional<Data> data;
    FIELDS_REFLECT(NewPeerNotification, notification, method, data);
};

struct PeerClosedNotification : Jsonable<PeerClosedNotification> {
    struct Data {
        std::optional<std::string> peerId;
        FIELDS_REFLECT(Data, peerId);
    };

    std::optional<bool> notification;
    std::optional<std::string> method;
    std::optional<Data> data;
    FIELDS_REFLECT(PeerClosedNotification, notification, method, data);
};


}

#endif /* !SIGNALING_H */
