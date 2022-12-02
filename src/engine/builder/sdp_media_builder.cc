/*
 *
 * sdp_media_builder.cc
 *
 * Created by Ruibin.Chow on 2022/12/02.
 * Copyright (c) 2022年 Ruibin.Chow All rights reserved.
 */

#include "sdp_media_builder.h"
#include <algorithm> // ::transform
#include <cctype>    // ::tolower
#include <regex>
#include <sstream>
#include <utility>

namespace engine {

namespace {

std::string GetCodecName(const Codec& codec) {
    static const std::regex MimeTypeRegex("^(audio|video)/",
                                          std::regex_constants::ECMAScript |
                                          std::regex_constants::icase);
    auto mimeType = codec.mimeType.value();
    return std::regex_replace(mimeType, MimeTypeRegex, "");
}

};

SDPMediaBuilder::SDPMediaBuilder(const Optional(ICEParameters)& iceParameters,
                                 const Optional(std::vector<ICECandidate>)& iceCandidates) {
    SetIceParameters(iceParameters);
    
    this->mediaObject.candidates = std::vector<SDPCandidates>();
    for (auto& candidate : iceCandidates.value()) {
        auto candidateObj = SDPCandidates();

        candidateObj.component = 1;
        candidateObj.foundation = candidate.foundation;
        candidateObj.ip = candidate.ip;
        candidateObj.port = candidate.port;
        candidateObj.priority = candidate.priority;
        candidateObj.transport = candidate.protocol;
        candidateObj.type = candidate.type;

        if (candidate.tcptype.has_value()) {
            candidateObj.tcptype = candidate.tcptype;
        }
        
        this->mediaObject.candidates->push_back(candidateObj);
    }
    
    this->mediaObject.iceOptions = "renomination";
    this->mediaObject.endOfCandidates = "end-of-candidates";
}

SDPMedia SDPMediaBuilder::GetObject() const {
    return mediaObject;
}

std::string SDPMediaBuilder::GetMid() const {
    return this->mediaObject.mid.value();
}

bool SDPMediaBuilder::IsClosed() const {
    return this->mediaObject.port.value() == 0;
}

void SDPMediaBuilder::SetIceParameters(const Optional(ICEParameters)& iceParameters) {
    this->mediaObject.iceUfrag = iceParameters->usernameFragment;
    this->mediaObject.icePwd = iceParameters->password;
}

void SDPMediaBuilder::Disable() {
    this->mediaObject.direction = "inactive";
    
    this->mediaObject.ext = std::nullopt;
    this->mediaObject.ssrcs = std::nullopt;
    this->mediaObject.ssrcGroups = std::nullopt;
    this->mediaObject.simulcast = std::nullopt;
    this->mediaObject.rids = std::nullopt;
}

void SDPMediaBuilder::Close() {
    this->mediaObject.direction = "inactive";
    this->mediaObject.port = 0;
    
    this->mediaObject.ext = std::nullopt;
    this->mediaObject.ssrcs = std::nullopt;
    this->mediaObject.ssrcGroups = std::nullopt;
    this->mediaObject.simulcast = std::nullopt;
    this->mediaObject.rids = std::nullopt;
    this->mediaObject.extmapAllowMixed = std::nullopt;
}


AnswerSDPMediaBuilder::AnswerSDPMediaBuilder(const Optional(ICEParameters)& iceParameters,
                                             const Optional(std::vector<ICECandidate>)& iceCandidates,
                                             const Optional(DTLSParameters)& dtlsParameters,
                                             const Optional(SCTPParameters)& sctpParameters,
                                             const Optional(SDPMedia)& offerMediaObject,
                                             const Optional(RTPParameters)& offerRtpParameters,
                                             const Optional(RTPParameters)& answerRtpParameters,
                                             const Optional(CodecOptions)& codecOptions)
        : SDPMediaBuilder(iceParameters, iceCandidates) {
    auto type = offerMediaObject->type.value();
    
    this->mediaObject.mid = offerMediaObject->mid;
    this->mediaObject.type = type;
    this->mediaObject.protocol = offerMediaObject->protocol;
    this->mediaObject.connection = SDPConnection{.version = 4, .ip = "127.0.0.1"};
    this->mediaObject.port = 7;
    
    auto dtlsRole = dtlsParameters->role.value();
    if (dtlsRole == "client")
        this->mediaObject.setup = "active";
    else if (dtlsRole == "server")
        this->mediaObject.setup = "passive";
    else if (dtlsRole == "auto")
        this->mediaObject.setup = "actpass";
    
    if (type == "audio" || type == "video") {
        this->mediaObject.direction = "recvonly";
        this->mediaObject.rtp = std::vector<SDPRtp>();
        this->mediaObject.rtcpFb = std::vector<SDPRtcpFb>();
        this->mediaObject.fmtp = std::vector<SDPFmtp>();
        
        for (auto& codec : answerRtpParameters->codecs.value()) {
            SDPRtp rtp {
                .payload = codec.payloadType.value(),
                .codec = GetCodecName(codec),
                .rate = codec.clockRate
            };
            if (codec.channels.has_value() && codec.channels.value() > 1) {
                rtp.encoding = std::to_string(codec.channels.value());
            }
            this->mediaObject.rtp->push_back(rtp);
            
            auto codecParameters = codec.parameters;
            
            if (codecOptions.has_value()) {
                auto& offerCodecs = offerRtpParameters->codecs.value();
                auto codecIt = std::find_if(offerCodecs.begin(),
                                            offerCodecs.end(),
                                            [codec](const Codec& offerCodec) {
                    return offerCodec.payloadType.value() == codec.payloadType.value();
                });
                
                auto offerCodec = const_cast<const Codec&>(*codecIt);
                auto mimeType = codec.mimeType.value();
                std::transform(mimeType.begin(), mimeType.end(), mimeType.begin(), ::tolower);
                
                if (mimeType == "audio/opus") {
                    if (codecOptions->opusStereo.has_value()) {
                        bool opusStereo = codecOptions->opusStereo.value();
                        offerCodec.parameters->spropstereo = (opusStereo ? 1 : 0);
                        codecParameters->stereo = opusStereo ? 1 : 0;
                    }
                    if (codecOptions->opusFec.has_value()) {
                        bool opusFec = codecOptions->opusFec.value();
                        offerCodec.parameters->useinbandfec = opusFec ? 1 : 0;
                        codecParameters->useinbandfec = opusFec ? 1 : 0;
                    }
                    if (codecOptions->opusDtx.has_value()) {
                        bool opusDtx = codecOptions->opusDtx.value();
                        offerCodec.parameters->usedtx = opusDtx ? 1 : 0;
                        codecParameters->usedtx = opusDtx ? 1 : 0;
                    }
                    if (codecOptions->opusMaxPlaybackRate.has_value()) {
                        auto opusMaxPlaybackRate = codecOptions->opusMaxPlaybackRate.value();
                        codecParameters->maxplaybackrate = opusMaxPlaybackRate;
                    }
                    if (codecOptions->opusMaxAverageBitrate.has_value()) {
                        auto opusMaxAverageBitrate = codecOptions->opusMaxAverageBitrate.value();
                        codecParameters->maxaveragebitrate = opusMaxAverageBitrate;
                    }
                    if (codecOptions->opusPtime.has_value()) {
                        auto opusPtime = codecOptions->opusPtime.value();
                        codecParameters->ptime = opusPtime;
                    }
                } else if (mimeType == "video/vp8" || mimeType == "video/vp9"
                           || mimeType == "video/h264" || mimeType == "video/h265") {
                    if (codecOptions->videoGoogleStartBitrate.has_value()) {
                        auto videoGoogleStartBitrate = codecOptions->videoGoogleStartBitrate.value();
                        codecParameters->xGoogleStartBitrate = videoGoogleStartBitrate;
                    }
                    if (codecOptions->videoGoogleMaxBitrate.has_value()) {
                        auto videoGoogleMaxBitrate = codecOptions->videoGoogleMaxBitrate.value();
                        codecParameters->xGoogleMaxBitrate = videoGoogleMaxBitrate;
                    }
                    if (codecOptions->videoGoogleMinBitrate.has_value()) {
                        auto videoGoogleMinBitrate = codecOptions->videoGoogleMinBitrate.value();
                        codecParameters->xGoogleMinBitrate = videoGoogleMinBitrate;
                    }
                }
            }
            
            SDPFmtp fmtp {
                .payload = codec.payloadType.value()
            };
            
            std::ostringstream config;
            nlohmann::json codecParametersJsonObj = json::parse(codecParameters->ToJsonString());
            for (auto& item : codecParametersJsonObj.items()) {
                if (!config.str().empty())
                    config << ";";
                
                config << item.key();
                config << "=";
                if (item.value().is_string())
                    config << item.value().get<std::string>();
                else if (item.value().is_number_float())
                    config << item.value().get<float>();
                else if (item.value().is_number())
                    config << item.value().get<int64_t>();
            }
            if (!config.str().empty()) {
                fmtp.config = config.str();
                this->mediaObject.fmtp->push_back(fmtp);
            }
            
            std::string payloads;
            for (auto& codec : answerRtpParameters->codecs.value()) {
                auto payloadType = codec.payloadType.value();
                if (!payloads.empty()) payloads.append(" ");
                payloads.append(std::to_string(payloadType));
            }
            this->mediaObject.payloads = payloads;
            
            this->mediaObject.ext = std::vector<SDPExt>();
            // Don't add a header extension if not present in the offer.
            for (auto& ext : answerRtpParameters->headerExtensions.value()) {
                const auto& localExts = offerMediaObject->ext.value();
                auto localExtIt = find_if(localExts.begin(), localExts.end(), [&ext](const SDPExt& localExt) {
                    return localExt.uri.value() == ext.uri.value();
                });
                if (localExtIt == localExts.end()) continue;
                this->mediaObject.ext->push_back({
                    .uri = ext.uri,
                    .value = ext.id
                });
            }
            
            if (offerMediaObject->extmapAllowMixed.has_value()) {
                this->mediaObject.extmapAllowMixed = "extmap-allow-mixed";
            }
            
            // Simulcast.
            auto simulcast = offerMediaObject->simulcast;
            auto rids = offerMediaObject->rids;
            if (simulcast.has_value()) {
                this->mediaObject.simulcast = SDPSimulcast();
                this->mediaObject.simulcast.value().dir1 = "recv";
                this->mediaObject.simulcast.value().list1 = simulcast->list1;
                
                this->mediaObject.rids = std::vector<SDPRids>();
                for (const auto& rid : *rids) {
                    if (rid.direction.value() != "send") continue;
                    this->mediaObject.rids->push_back({
                        .id = rid.id,
                        .direction = "recv"
                    });
                }
            }
            
            this->mediaObject.rtcpMux = "rtcp-mux";
            this->mediaObject.rtcpRsize = "rtcp-rsize";
        }
        
    } else if (type == "application") {
        this->mediaObject.payloads = "webrtc-datachannel";
        this->mediaObject.sctpPort = sctpParameters->port;
        this->mediaObject.maxMessageSize = sctpParameters->maxMessageSize;
    }
}

void AnswerSDPMediaBuilder::SetDtlsRole(const std::string& role) {
    if (role == "client")
        this->mediaObject.setup = "active";
    else if (role == "server")
        this->mediaObject.setup = "passive";
    else if (role == "auto")
        this->mediaObject.setup = "actpass";
}


OfferSDPMediaBuilder::OfferSDPMediaBuilder(const Optional(ICEParameters)& iceParameters,
                                           const Optional(std::vector<ICECandidate>)& iceCandidates,
                                           const Optional(DTLSParameters)& dtlsParameters,
                                           const Optional(SCTPParameters)& sctpParameters,
                                           const std::string& mid,
                                           const std::string& kind,
                                           const Optional(RTPParameters)& offerRtpParameters,
                                           const std::string& streamId,
                                           const std::string& trackId)
        : SDPMediaBuilder(iceParameters, iceCandidates) {
    this->mediaObject.mid  = mid;
    this->mediaObject.type = kind;
    
    if (sctpParameters.has_value())
        this->mediaObject.protocol = "UDP/TLS/RTP/SAVPF";
    else
        this->mediaObject.protocol = "UDP/DTLS/SCTP";
    
    this->mediaObject.connection = SDPConnection{.version = 4, .ip = "127.0.0.1"};
    this->mediaObject.port = 7;
    
    // Set DTLS role.
    this->mediaObject.setup = "actpass";
    
    if (kind == "audio" || kind == "video") {
        this->mediaObject.direction = "sendonly";
        this->mediaObject.rtp = std::vector<SDPRtp>();
        this->mediaObject.rtcpFb = std::vector<SDPRtcpFb>();
        this->mediaObject.fmtp = std::vector<SDPFmtp>();
        for (auto& codec : offerRtpParameters->codecs.value()) {
            SDPRtp rtp {
                .payload = codec.payloadType.value(),
                .codec = GetCodecName(codec),
                .rate = codec.clockRate
            };
            if (codec.channels.has_value() && codec.channels.value() > 1) {
                rtp.encoding = std::to_string(codec.channels.value());
            }
            this->mediaObject.rtp->push_back(rtp);
            
            SDPFmtp fmtp {
                .payload = codec.payloadType.value()
            };
            
            auto codecParameters = codec.parameters;
            std::ostringstream config;
            nlohmann::json codecParametersJsonObj = json::parse(codecParameters->ToJsonString());
            for (auto& item : codecParametersJsonObj.items()) {
                if (!config.str().empty())
                    config << ";";
                
                config << item.key();
                config << "=";
                if (item.value().is_string())
                    config << item.value().get<std::string>();
                else if (item.value().is_number_float())
                    config << item.value().get<float>();
                else if (item.value().is_number())
                    config << item.value().get<int64_t>();
            }
            if (!config.str().empty()) {
                fmtp.config = config.str();
                this->mediaObject.fmtp->push_back(fmtp);
            }
            
            for (const auto& fb : codec.rtcpFeedback.value()) {
                this->mediaObject.rtcpFb->push_back({
                    .payload = std::to_string(codec.payloadType.value()),
                    .type = fb.type,
                    .subtype = fb.parameter
                });
            }
        }
        
        std::string payloads;
        for (const auto& codec : offerRtpParameters->codecs.value()) {
            auto payloadType = codec.payloadType.value();
            if (!payloads.empty()) payloads.append(" ");
            payloads.append(std::to_string(payloadType));
        }
        this->mediaObject.payloads = payloads;
        
        this->mediaObject.ext = std::vector<SDPExt>();
        for (const auto& ext : offerRtpParameters->headerExtensions.value()) {
            this->mediaObject.ext->push_back({
                .uri = ext.uri,
                .value = ext.id
            });
        }
        
        this->mediaObject.rtcpMux   = "rtcp-mux";
        this->mediaObject.rtcpRsize = "rtcp-rsize";
        
        const auto& encoding = offerRtpParameters->encodings.value()[0];
        auto ssrc = encoding.ssrc.value();
        uint32_t rtxSsrc;
        if (encoding.rtx.has_value() && encoding.rtx.value().ssrc.has_value())
            rtxSsrc = encoding.rtx.value().ssrc.value();
        else
            rtxSsrc = 0u;
        
        this->mediaObject.ssrcs = std::vector<SDPSsrc>();
        this->mediaObject.ssrcGroups = std::vector<SDPSsrcGroups>();
        
        if (offerRtpParameters->rtcp.value().cname.has_value()) {
            auto cname = offerRtpParameters->rtcp.value().cname.value();
            std::string msid(streamId);
            msid.append(" ").append(trackId);
            this->mediaObject.ssrcs->push_back({
                .id = ssrc,
                .attribute = "cname",
                .value = cname
            });
            this->mediaObject.ssrcs->push_back({
                .id = ssrc,
                .attribute = "msid",
                .value = msid
            });
            if (rtxSsrc != 0u) {
                std::string ssrcs = std::to_string(ssrc).append(" ").append(std::to_string(rtxSsrc));
                this->mediaObject.ssrcs->push_back({
                    .id = rtxSsrc,
                    .attribute = "cname",
                    .value = cname
                });
                this->mediaObject.ssrcs->push_back({
                    .id = rtxSsrc,
                    .attribute = "msid",
                    .value = msid
                });

                // Associate original and retransmission SSRCs.
                this->mediaObject.ssrcGroups->push_back({
                    .semantics = "FID",
                    .ssrcs = ssrcs
                });
            }
        }
    } else if (kind == "application") {
        this->mediaObject.payloads = "webrtc-datachannel";
        this->mediaObject.sctpPort = sctpParameters->port;
        this->mediaObject.maxMessageSize = sctpParameters->maxMessageSize;
    }
}

void OfferSDPMediaBuilder::SetDtlsRole(const std::string& role) {
    // The SDP offer must always have a=setup:actpass.
    this->mediaObject.setup = "actpass";
}


}

