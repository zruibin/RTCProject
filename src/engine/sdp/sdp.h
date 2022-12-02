/*
 * sdp.h
 *
 * Created by Ruibin.Chow on 2022/11/29.
 * Copyright (c) 2022年 Ruibin.Chow All rights reserved.
 */

#ifndef SDP_H
#define SDP_H

#include <string>
#include <sdptransform/sdptransform.hpp>
#include "engine/signalings/signaling.h"


namespace engine {

template<typename T>
struct SDPSerializer : public Jsonable<T> {
    virtual ~SDPSerializer() = default;
    
    virtual std::string ToSDPString() {
        std::string jsonString = Jsonable<T>::ToJsonString();
        json sdpObj = json::parse(jsonString);
        std::string sdpStr = sdptransform::write(sdpObj);
        return sdpStr;
    }
    
    virtual void FromSDPString(const std::string& sdpString) {
        json sdpObj = sdptransform::parse(sdpString);
        std::string jsonString = sdpObj.dump(2);
        Jsonable<T>::FromJsonString(jsonString);
        originJsonString = jsonString; // must after FromJsonString.
    }
    
    std::string originJsonString = "";
};

struct SDPGroups {
    std::optional<std::string> mids;
    std::optional<std::string> type;
    FIELDS_REFLECT(SDPGroups, mids, type);
};

struct SDPCandidates {
    std::optional<int64_t> component;
    std::optional<std::string> foundation;
    std::optional<int64_t> generation;
    std::optional<std::string> ip;
    std::optional<int64_t> port;
    std::optional<int64_t> priority;
    std::optional<std::string> transport;
    std::optional<std::string> type;
    std::optional<std::string> raddr;
    std::optional<int64_t> rport;
    FIELDS_REFLECT(SDPCandidates, component, foundation, generation,
                   ip, port, priority, transport, type, raddr, rport);
};

struct SDPConnection {
    std::optional<int64_t> version;
    std::optional<std::string> ip;
    FIELDS_REFLECT(SDPConnection, version, ip);
};

struct SDPExt {
    std::optional<std::string> uri;
    std::optional<int64_t> value;
    FIELDS_REFLECT(SDPExt, uri, value);
};

struct SDPFingerprint {
    std::optional<std::string> hash;
    std::optional<std::string> type;
    FIELDS_REFLECT(SDPFingerprint, hash, type);
};

struct SDPFmtp {
    std::optional<std::string> config;
    std::optional<int64_t> payload;
    FIELDS_REFLECT(SDPFmtp, config, payload);
};

struct SDPRtcp {
    std::optional<std::string> address;
    std::optional<int64_t> ipVer;
    std::optional<std::string> netType;
    std::optional<int64_t> port;
    FIELDS_REFLECT(SDPRtcp, address, ipVer, netType, port);
};

struct SDPRtcpFb {
    std::optional<std::string> type;
    std::optional<std::string> subtype;
    std::optional<std::string> payload;
    FIELDS_REFLECT(SDPRtcpFb, type, subtype, payload);
};

struct SDPRtp {
    std::optional<std::string> codec;
    std::optional<std::string> encoding;
    std::optional<int64_t> payload;
    std::optional<int64_t> rate;
    FIELDS_REFLECT(SDPRtp, codec, encoding, payload, rate);
};

struct SDPSsrc {
    std::optional<std::string> attribute;
    std::optional<int64_t> id;
    std::optional<std::string> value;
    FIELDS_REFLECT(SDPSsrc, attribute, id, value);
};

struct SDPSsrcGroups {
    std::optional<std::string> semantics;
    std::optional<std::string> ssrcs;
    FIELDS_REFLECT(SDPSsrcGroups, semantics, ssrcs);
};

struct SDPMedia {
    std::optional<std::vector<SDPCandidates>> candidates;
    std::optional<SDPConnection> connection;
    std::optional<std::string> direction;
    std::optional<std::vector<SDPExt>> ext;
    std::optional<SDPFingerprint> fingerprint;
    std::optional<std::vector<SDPFmtp>> fmtp;
    std::optional<std::string> iceOptions;
    std::optional<std::string> icePwd;
    std::optional<std::string> iceUfrag;
    std::optional<std::string> mid;
    std::optional<std::string> payloads;
    std::optional<int64_t> port;
    std::optional<std::string> protocol;
    std::optional<std::string> rtcpMux;
    std::optional<SDPRtcp> rtcp;
    std::optional<std::vector<SDPRtcpFb>> rtcpFb;
    std::optional<std::vector<SDPRtp>> rtp;
    std::optional<std::string> setup;
    std::optional<std::vector<SDPSsrc>> ssrcs;
    std::optional<std::vector<SDPSsrcGroups>> ssrcGroups;
    std::optional<std::string> type;
    std::optional<std::string> rtcpRsize;
    FIELDS_REFLECT(SDPMedia, candidates, connection, direction, ext, fingerprint,
                   fmtp, iceOptions, icePwd, iceUfrag, mid, payloads, port, protocol,
                   rtcp, rtcpFb, rtcpMux, rtp, setup, ssrcs, type, ssrcGroups,
                   rtcpRsize);
};

struct SDPMsidSemantic {
    std::optional<std::string> semantic;
    std::optional<std::string> token;
    FIELDS_REFLECT(SDPMsidSemantic, semantic, token);
};

struct SDPTiming {
    std::optional<int64_t> start;
    std::optional<int64_t> stop;
    FIELDS_REFLECT(SDPTiming, start, stop);
};

struct SDPOrigin {
    std::optional<std::string> username;
    std::optional<int64_t> sessionId;
    std::optional<int64_t> sessionVersion;
    std::optional<std::string> netType;
    std::optional<int64_t> ipVer;
    std::optional<std::string> address;
    FIELDS_REFLECT(SDPOrigin, username, sessionId, sessionVersion, netType,
                   ipVer, address);
};

struct SessionDescription : SDPSerializer<SessionDescription> {
    std::optional<std::string> name;
    std::optional<SDPOrigin> origin;
    std::optional<std::string> description;
    std::optional<std::string> uri;
    std::optional<SDPTiming> timing;
    std::optional<int64_t> version;
    std::optional<std::vector<SDPGroups>> groups;
    std::optional<std::string> extmapAllowMixed;
    std::optional<SDPMsidSemantic> msidSemantic;
    std::optional<std::string> icelite;
    std::optional<std::vector<SDPMedia>> media;
    FIELDS_REFLECT(SessionDescription, name, origin, description, uri,
                   timing, version, groups, extmapAllowMixed,
                   msidSemantic, icelite, media);
};



}

#endif /* !SDP_H */
