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


#pragma mark -

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
    FIELDS_REFLECT(Parameters,
                   levelAsymmetryAllowed, packetizationMode,
                   profileLevelId, xGoogleStartBitrate,
                   minptime, useinbandfec, profileId);
};

struct Codec {
    std::optional<std::string> kind;
    std::optional<std::string> mimeType;
    std::optional<int32_t> clockRate;
    std::optional<int32_t> channels;
    std::optional<std::vector<RTCPFeedback>> rtcpFeedback;
    std::optional<Parameters> parameters;
    std::optional<int32_t> preferredPayloadType;
    FIELDS_REFLECT(Codec,
                   kind, mimeType, clockRate, channels,
                   rtcpFeedback, parameters, preferredPayloadType);
};

struct HeaderExtension {
    std::optional<std::string> kind;
    std::optional<std::string> uri;
    std::optional<int32_t> preferredId;
    std::optional<bool> preferredEncrypt;
    std::optional<std::string> direction;
    FIELDS_REFLECT(HeaderExtension,
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

}

#endif /* !SIGNALING_H */
