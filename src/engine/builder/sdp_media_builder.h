/*
 * sdp_media_builder.h
 *
 * Created by Ruibin.Chow on 2022/12/02.
 * Copyright (c) 2022年 Ruibin.Chow All rights reserved.
 */

#ifndef SDP_MEDIA_BUILDER_H
#define SDP_MEDIA_BUILDER_H

#include "engine/signalings/signaling.h"
#include "engine/sdp/sdp.h"

namespace engine {

#define Optional(T) std::optional<T>

class SDPMediaBuilder {
    
public:
    SDPMediaBuilder() = default;
    virtual ~SDPMediaBuilder() = default;
    SDPMediaBuilder(const Optional(ICEParameters)& iceParameters,
                    const Optional(std::vector<ICECandidate>)& iceCandidates);

public:
    virtual void SetDtlsRole(const std::string& role) = 0;

public:
    SDPMedia GetObject() const;
    std::string GetMid() const;
    bool IsClosed() const;
    void SetIceParameters(const Optional(ICEParameters)& iceParameters);
    void Disable();
    void Close();
    
protected:
    SDPMedia mediaObject = SDPMedia();
    
};

class AnswerSDPMediaBuilder : public SDPMediaBuilder {

public:
    AnswerSDPMediaBuilder(const Optional(ICEParameters)& iceParameters,
                          const Optional(std::vector<ICECandidate>)& iceCandidates,
                          const Optional(DTLSParameters)& dtlsParameters,
                          const Optional(SCTPParameters)& sctpParameters,
                          const Optional(SDPMedia)& offerMediaObject,
                          const Optional(RTPParameters)& offerRtpParameters,
                          const Optional(RTPParameters)& answerRtpParameters,
                          const Optional(CodecOptions)& codecOptions);

public:
    void SetDtlsRole(const std::string& role) override;
};

class OfferSDPMediaBuilder : public SDPMediaBuilder {

public:
    OfferSDPMediaBuilder(const Optional(ICEParameters)& iceParameters,
                         const Optional(std::vector<ICECandidate>)& iceCandidates,
                         const Optional(DTLSParameters)& dtlsParameters,
                         const Optional(SCTPParameters)& sctpParameters,
                         const std::string& mid,
                         const std::string& kind,
                         const Optional(RTPParameters)& offerRtpParameters,
                         const std::string& streamId,
                         const std::string& trackId);

public:
    void SetDtlsRole(const std::string& role) override;
};

}

#endif /* !SDP_MEDIA_BUILDER_H */
