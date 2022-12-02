/*
 * signaling_manager.h
 *
 * Created by Ruibin.Chow on 2022/11/21.
 * Copyright (c) 2022年 Ruibin.Chow All rights reserved.
 */

#ifndef SIGNALING_MANAGER_H
#define SIGNALING_MANAGER_H

#include <memory>
#include <mutex>
#include <functional>
#include "engine/signalings/signaling.h"
#include "engine/signalings/signaling_socket.h"
#include "log/logging.h"
#include "foundation/singleton.h"

namespace engine {

namespace  {
    using namespace foundation;
};

#define SignalingRequest(T) std::shared_ptr<T>
#define SignalingHandler(T) std::function<void( \
                                    int32_t code, \
                                    const std::string& msg, \
                                    std::shared_ptr<T> response)>

class SignalingManager final : public Singleton<SignalingManager> {

public:
    explicit SignalingManager();
    ~SignalingManager();
    void Init();
    
    void GetRouterRtpCapabilities(SignalingHandler(GetRouterRtpCapabilitiesResponse) handler);
    void CreateWebRtcTransport(SignalingRequest(CreateWebRtcTransportRequest) request,
                               SignalingHandler(CreateWebRtcTransportResponse) handler);
    void Join(SignalingRequest(JoinRequest) request,
              SignalingHandler(JoinResponse) handler);
    void ConnectWebRtcTransport(SignalingRequest(ConnectWebRtcTransportRequest) request,
                                SignalingHandler(BasicResponse) handler);
//    void NewDataConsumer(SignalingHandler(BasicResponse));
//    void NewConsumer(SignalingHandler(BasicResponse)); PauseConsumer
    void PauseConsumer(const std::string& consumerId,
                       SignalingHandler(BasicResponse) handler);
    void ResumeConsumer(const std::string& consumerId,
                        SignalingHandler(BasicResponse) handler);
    void RequestConsumerKeyFrame(const std::string& consumerId,
                                 SignalingHandler(BasicResponse) handler);
    void Produce(SignalingRequest(ProduceRequest) request,
                 SignalingHandler(ProduceResponse) handler);
    void ProduceData(SignalingRequest(ProduceDataRequest) request,
                     SignalingHandler(ProduceDataResponse) handler);
    void PauseProducer(const std::string& producerId,
                       SignalingHandler(BasicResponse) handler);
    void ResumeProducer(const std::string& producerId,
                        SignalingHandler(BasicResponse) handler);
    void CloseProducer(const std::string& producerId,
                       SignalingHandler(BasicResponse) handler);
    void ChangeDisplayName(const std::string& displayName,
                           SignalingHandler(BasicResponse) handler);
    void RestartIce(const std::string& transportId,
                    SignalingHandler(RestartICEResponse) handler);
    void SetConsumerPriority(const std::string& consumerId,
                             int32_t priority,
                             SignalingHandler(BasicResponse) handler);
    void SetConsumerPreferredLayers(const std::string& consumerId,
                                    int32_t spatialLayer,
                                    int32_t temporalLayer,
                                    SignalingHandler(BasicResponse) handler);
    void GetTransportStats(const std::string& transportId,
                           SignalingHandler(GetTransportStatsResponse) handler);
    void GetProducerStats(const std::string& producerId,
                          SignalingHandler(GetProducerStatsResponse) handler);
    void GetDataProducerStats(const std::string& dataProducerId,
                              SignalingHandler(GetDataProducerStatsResponse) handler);
    void GetConsumerStats(const std::string& consumerId,
                          SignalingHandler(GetConsumerStatsResponse) handler);
    void GetDataConsumerStats(const std::string& dataConsumerId,
                              SignalingHandler(GetDataConsumerStatsResponse) handler);
    void ApplyNetworkThrottle(SignalingRequest(ApplyNetworkThrottleRequest) request,
                              SignalingHandler(BasicResponse) handler);
    void ResetNetworkThrottle(const std::string& secret,
                              SignalingHandler(BasicResponse) handler);
    
private:
    template<typename Request, typename Response>
    void Send(SignalingRequest(Request) request, SignalingHandler(Response) handler) {
        if (request == nullptr) {
            Log(ERROR) << "Send Request Was nullptr.";
            return;
        }
        
        if (socketRef_ == nullptr) {
            Log(ERROR) << "socketRef Was nullptr.";
            return;
        }
        
        socketRef_->Send(request->ToJsonString(),
                         request->id.value(),
                         [handler](const std::string& json){
            if (json.empty()) {
                return;
            }
            std::shared_ptr<Response> response = std::make_shared<Response>();
            response->FromJsonString(json);
            if (handler) {
                handler(0, "", response);
            }
        }, [handler](int32_t errorCode, const std::string& errorInfo){
            if (handler) {
                handler(errorCode, errorInfo, nullptr);
            }
        });
    }
private:
    std::shared_ptr<SignalingSocket> socketRef_;
};

}



#endif /* !SIGNALING_MANAGER_H */
