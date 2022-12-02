/*
 *
 * sdp_builder.cc
 *
 * Created by Ruibin.Chow on 2022/12/02.
 * Copyright (c) 2022年 Ruibin.Chow All rights reserved.
 */

#include "sdp_builder.h"

namespace engine {

SDPBuilder::SDPBuilder(const Optional(ICEParameters)& iceParameters,
                       const Optional(std::vector<ICECandidate>)& iceCandidates,
                       const Optional(DTLSParameters)& dtlsParameters,
                       const Optional(SCTPParameters)& sctpParameters)
        : iceParameters_(iceParameters),
            iceCandidates_(iceCandidates),
            dtlsParameters_(dtlsParameters),
            sctpParameters_(sctpParameters)
{
    this->sdpObject.version = 0;
    this->sdpObject.name = "-";
    this->sdpObject.origin = {
        .address = "0.0.0.0",
        .ipVer = 4,
        .netType = "IN",
        .sessionId = 10000,
        .sessionVersion = 0,
        .username = "libRTCEngine"
    };
    this->sdpObject.timing = {.start = 0, .stop = 0};
    this->sdpObject.media = std::vector<SDPMedia>();
    
    // If ICE parameters are given, add ICE-Lite indicator.
    if (this->iceParameters_->iceLite.has_value())
        this->sdpObject.icelite = "ice-lite";

    this->sdpObject.msidSemantic = {
        .semantic = "WMS",
        .token = "*"
    };
    
    // NOTE: We take the latest fingerprint.
    auto numFingerprints = this->dtlsParameters_->fingerprints->size();
    Fingerprint lastFingerprint = this->dtlsParameters_->fingerprints.value()[numFingerprints - 1];
    this->sdpObject.fingerprint = {
        .type = lastFingerprint.algorithm,
        .hash = lastFingerprint.value
    };
    
    this->sdpObject.groups->push_back({
        .type = "BUNDLE",
        .mids = ""
    });
}

SDPBuilder::~SDPBuilder() {
    for (const auto* mediaBuilder : this->mediaBuilders) {
        delete mediaBuilder;
    }
}

SessionDescription SDPBuilder::GetSdp() const {
    return sdpObject;
}

void SDPBuilder::Send(const Optional(SDPMedia)& offerMediaObject,
                      const std::string& reuseMid,
                      const Optional(RTPParameters)& offerRtpParameters,
                      const Optional(RTPParameters)& answerRtpParameters,
                      const Optional(CodecOptions)& codecOptions) {
    auto* mediaBuilder = new AnswerSDPMediaBuilder(this->iceParameters_,
                                                   this->iceCandidates_,
                                                   this->dtlsParameters_,
                                                   this->sctpParameters_,
                                                   offerMediaObject,
                                                   offerRtpParameters,
                                                   answerRtpParameters,
                                                   codecOptions);
    // Closed media section replacement.
    if (!reuseMid.empty()) {
        this->ReplaceMediaBuilder(mediaBuilder, reuseMid);
    } else {
        this->AddMediaBuilder(mediaBuilder);
    }
}

void SDPBuilder::Receive(const std::string& mid,
                         const std::string& kind,
                         const Optional(RTPParameters)& offerRtpParameters,
                         const std::string& streamId,
                         const std::string& trackId) {
    auto* mediaBuilder = new OfferSDPMediaBuilder(this->iceParameters_,
                                                  this->iceCandidates_,
                                                  this->dtlsParameters_,
                                                  std::nullopt,
                                                  mid,
                                                  kind,
                                                  offerRtpParameters,
                                                  streamId,
                                                  trackId);
    // Let's try to recycle a closed media section (if any).
    // NOTE: We can recycle a closed m=audio section with a new m=video.
    auto mediaBuilderIt = find_if(this->mediaBuilders.begin(),
                                  this->mediaBuilders.end(),
                                  [](const SDPMediaBuilder* mediaBuilder) {
        return mediaBuilder->IsClosed();
    });
    
    if (mediaBuilderIt != this->mediaBuilders.end()) {
        this->ReplaceMediaBuilder(mediaBuilder, (*mediaBuilderIt)->GetMid());
    } else {
        this->AddMediaBuilder(mediaBuilder);
    }
}

void SDPBuilder::SendSctpAssociation(const Optional(SDPMedia)& offerMediaObject) {
    auto* mediaBuilder = new AnswerSDPMediaBuilder(this->iceParameters_,
                                                   this->iceCandidates_,
                                                   this->dtlsParameters_,
                                                   this->sctpParameters_,
                                                   offerMediaObject,
                                                   std::nullopt,
                                                   std::nullopt,
                                                   std::nullopt);
    this->AddMediaBuilder(mediaBuilder);
}

void SDPBuilder::RecvSctpAssociation() {
    auto* mediaBuilder = new OfferSDPMediaBuilder(this->iceParameters_,
                                                  this->iceCandidates_,
                                                  this->dtlsParameters_,
                                                  this->sctpParameters_,
                                                  "datachannel",
                                                  "application",
                                                  std::nullopt,
                                                  "",
                                                  "");
    this->AddMediaBuilder(mediaBuilder);
}


void SDPBuilder::AddMediaBuilder(SDPMediaBuilder* newMediaBuilder) {
    if (this->firstMid.empty())
        this->firstMid = newMediaBuilder->GetMid();
    
    this->mediaBuilders.push_back(newMediaBuilder);
    this->midToIndex[newMediaBuilder->GetMid()] = this->mediaBuilders.size() - 1;
    
    // Add to the SDP object.
    this->sdpObject.media->push_back(newMediaBuilder->GetObject());
    this->RegenerateBundleMids();
}

void SDPBuilder::ReplaceMediaBuilder(SDPMediaBuilder* newMediaBuilder,
                                     const std::string& reuseMid) {
    // Store it in the map.
    if (!reuseMid.empty()) {
        const auto idx              = this->midToIndex[reuseMid];
        auto* const oldMediaBuilder = this->mediaBuilders[idx];
        
        // Replace the index in the vector with the new media section.
        this->mediaBuilders[idx] = newMediaBuilder;
        
        // Update the map.
        this->midToIndex.erase(oldMediaBuilder->GetMid());
        this->midToIndex[newMediaBuilder->GetMid()] = idx;
        
        // Delete old MediaSection.
        delete oldMediaBuilder;
        
        // Update the SDP object.
        this->sdpObject.media.value()[idx] = newMediaBuilder->GetObject();
        
        // Regenerate BUNDLE mids.
        this->RegenerateBundleMids();
    } else {
        const auto idx              = this->midToIndex[newMediaBuilder->GetMid()];
        auto* const oldMediaBuilder = this->mediaBuilders[idx];
        
        // Replace the index in the vector with the new media section.
        this->mediaBuilders[idx] = newMediaBuilder;
        
        // Delete old MediaSection.
        delete oldMediaBuilder;
        
        // Update the SDP object.
        this->sdpObject.media.value()[this->mediaBuilders.size() - 1] = newMediaBuilder->GetObject();
    }
}

void SDPBuilder::RegenerateBundleMids() {
    std::string mids;
    for (const auto* mediaBuilder : this->mediaBuilders) {
        if (!mediaBuilder->IsClosed()) {
            if (mids.empty())
                mids = mediaBuilder->GetMid();
            else
                mids.append(" ").append(mediaBuilder->GetMid());
        }
    }
    this->sdpObject.groups.value()[0].mids = mids;
}

void SDPBuilder::UpdateIceParameters(Optional(ICEParameters) iceParameters) {
    this->iceParameters_ = iceParameters;
    
    if (iceParameters->iceLite.has_value())
        this->sdpObject.icelite = "ice-lite";
    
    for (auto idx{ 0u }; idx < this->mediaBuilders.size(); ++idx) {
        auto* mediaBuilder = this->mediaBuilders[idx];
        mediaBuilder->SetIceParameters(iceParameters);
        // Update SDP media section.
        this->sdpObject.media.value()[idx] = mediaBuilder->GetObject();
    }
}

void SDPBuilder::UpdateDtlsRole(const std::string& role) {
    this->dtlsParameters_->role = role;
    
    if (this->iceParameters_->iceLite.has_value())
        this->sdpObject.icelite = "ice-lite";
    
    for (auto idx{ 0u }; idx < this->mediaBuilders.size(); ++idx) {
        auto* mediaBuilder = this->mediaBuilders[idx];
        mediaBuilder->SetDtlsRole(role);
        // Update SDP media section.
        this->sdpObject.media.value()[idx] = mediaBuilder->GetObject();
    }
}
void SDPBuilder::DisableMediaBuilder(const std::string& mid) {
    const auto idx     = this->midToIndex[mid];
    auto* mediaBuilder = this->mediaBuilders[idx];
    mediaBuilder->Disable();
}

void SDPBuilder::CloseMediaBuilder(const std::string& mid) {
    const auto idx     = this->midToIndex[mid];
    auto* mediaBuilder = this->mediaBuilders[idx];
    
    // NOTE: Closing the first m section is a pain since it invalidates the
    // bundled transport, so let's avoid it.
    if (mid == this->firstMid)
        mediaBuilder->Disable();
    else
        mediaBuilder->Close();
    
    // Update SDP media section.
    this->sdpObject.media.value()[idx] = mediaBuilder->GetObject();
    
    // Regenerate BUNDLE mids.
    this->RegenerateBundleMids();
}


}

