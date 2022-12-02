/*
 * sdp_builder.h
 *
 * Created by Ruibin.Chow on 2022/12/02.
 * Copyright (c) 2022年 Ruibin.Chow All rights reserved.
 */

#ifndef SDP_BUILDER_H
#define SDP_BUILDER_H

#include "sdp_media_builder.h"

namespace engine {

class SDPBuilder final {

public:
    SDPBuilder(const Optional(ICEParameters)& iceParameters,
               const Optional(std::vector<ICECandidate>)& iceCandidates,
               const Optional(DTLSParameters)& dtlsParameters,
               const Optional(SCTPParameters)& sctpParameters);
    ~SDPBuilder();
    SessionDescription GetSdp() const;
    
    void Send(const Optional(SDPMedia)& offerMediaObject,
              const std::string& reuseMid,
              const Optional(RTPParameters)& offerRtpParameters,
              const Optional(RTPParameters)& answerRtpParameters,
              const Optional(CodecOptions)& codecOptions);
    void Receive(const std::string& mid,
                 const std::string& kind,
                 const Optional(RTPParameters)& offerRtpParameters,
                 const std::string& streamId,
                 const std::string& trackId);
    
    void SendSctpAssociation(const Optional(SDPMedia)& offerMediaObject);
    void RecvSctpAssociation();
    
    void UpdateIceParameters(Optional(ICEParameters) iceParameters);
    void UpdateDtlsRole(const std::string& role);
    void DisableMediaBuilder(const std::string& mid);
    void CloseMediaBuilder(const std::string& mid);

protected:
    SessionDescription sdpObject = SessionDescription();
    // SDPMediaBuilder instances.
    std::vector<SDPMediaBuilder*> mediaBuilders;
    // SDPMediaBuilder indices indexed by MID.
    std::map<std::string, size_t> midToIndex;
    // First MID.
    std::string firstMid;
    
private:
    void AddMediaBuilder(SDPMediaBuilder* newMediaBuilder);
    void ReplaceMediaBuilder(SDPMediaBuilder* newMediaBuilder,
                             const std::string& reuseMid);
    void RegenerateBundleMids();
    
private:
    Optional(ICEParameters) iceParameters_;
    Optional(std::vector<ICECandidate>) iceCandidates_;
    Optional(DTLSParameters) dtlsParameters_;
    Optional(SCTPParameters) sctpParameters_;
};

}

#endif /* !SDP_BUILDER_H */
