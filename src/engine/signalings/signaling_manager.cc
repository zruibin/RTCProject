/*
 *
 * signaling_manager.cc
 *
 * Created by Ruibin.Chow on 2022/11/21.
 * Copyright (c) 2022年 Ruibin.Chow All rights reserved.
 */

#include "engine/signalings/signaling_manager.h"
#include "util/timer.h"
#include "network/socket_factory.h"
#include <magic_enum/magic_enum.hpp>
#include "foundation/notification_center.h"

namespace engine {

void testSocket() {
    NotificationMethod method = NotificationMethod::ProducerScore;
    Log(INFO) << "Notification Name: " << EnumMethodToString<NotificationMethod>(method);
    Log(INFO) << "Notification method: "
                << EnumMethodToString<NotificationMethod>(
                        EnumMethodFromString<NotificationMethod>("activeSpeaker"));
    
    RequestMethod method1 = RequestMethod::GetTransportStats;
    Log(INFO) << "Request Name: " << EnumMethodToString<RequestMethod>(method1);
    Log(INFO) << "Request method: "
                << EnumMethodToString<RequestMethod>(
                        EnumMethodFromString<RequestMethod>("newConsumer"));
    
    NotificationCenter::DefaultCenter()->AddNotification("testSignaling", [](NotificationRef notiRef) {
        Log(DEBUG) << "Notification:" << notiRef->message;
    });
    
    /*
//    SignalingManager::SharedInstance()->Init();
    util::Timer::Sleep(3*TIME_NSEC_PER_SEC);
    SignalingManager::SharedInstance()->GetRouterRtpCapabilities([](int32_t code,
                                                                    const std::string& msg,
                                                                    std::shared_ptr<GetRouterRtpCapabilitiesResponse> response) {
        Log(DEBUG) << "GetRouterRtpCapabilities:" << response->ToJsonString();
    });
    util::Timer::Sleep(2*TIME_NSEC_PER_SEC);
    //*/
}

SignalingManager::SignalingManager() {
    socketRef_.reset(new SignalingSocket);
}

SignalingManager::~SignalingManager() {
    if (socketRef_->IsConnected()) {
        socketRef_->Close();
    }
    socketRef_ = nullptr;
}

void SignalingManager::Init() {
    socketRef_->Init("ws://localhost:8001/?roomId=10086&peerId=ABCDEF");
    socketRef_->Open();
    
    socketRef_->SetStateChangeHandler([](bool connected) {
        
    });
    socketRef_->SetFailedHandler([](SocketInterface::Error error,
                                    const std::string& reason) {
        
    });
}

void SignalingManager::GetRouterRtpCapabilities(
        SignalingHandler(GetRouterRtpCapabilitiesResponse) handler) {
    auto request = std::make_shared<GetRouterRtpCapabilitiesRequest>();
    Send<GetRouterRtpCapabilitiesRequest, GetRouterRtpCapabilitiesResponse>(request, handler);
}

void SignalingManager::CreateWebRtcTransport(SignalingRequest(CreateWebRtcTransportRequest) request,
                                             SignalingHandler(CreateWebRtcTransportResponse) handler) {
    Send<CreateWebRtcTransportRequest, CreateWebRtcTransportResponse>(request, handler);
}

void SignalingManager::Join(SignalingRequest(JoinRequest) request,
                            SignalingHandler(JoinResponse) handler) {
    Send<JoinRequest, JoinResponse>(request, handler);
}

void SignalingManager::ConnectWebRtcTransport(SignalingRequest(ConnectWebRtcTransportRequest) request,
                                              SignalingHandler(BasicResponse) handler) {
    Send<ConnectWebRtcTransportRequest, BasicResponse>(request, handler);
}

void SignalingManager::PauseConsumer(const std::string& consumerId,
                                     SignalingHandler(BasicResponse) handler) {
    auto request = std::make_shared<ConsumerRequest>();
    request->method = EnumMethodToString<RequestMethod>(RequestMethod::PauseConsumer);
    request->data = ConsumerRequest::Data();
    request->data->consumerId = consumerId;
    Send<ConsumerRequest, BasicResponse>(request, handler);
}

void SignalingManager::ResumeConsumer(const std::string& consumerId,
                                      SignalingHandler(BasicResponse) handler) {
    auto request = std::make_shared<ConsumerRequest>();
    request->method = EnumMethodToString<RequestMethod>(RequestMethod::ResumeConsumer);
    request->data = ConsumerRequest::Data();
    request->data->consumerId = consumerId;
    Send<ConsumerRequest, BasicResponse>(request, handler);
}

void SignalingManager::RequestConsumerKeyFrame(const std::string& consumerId,
                                               SignalingHandler(BasicResponse) handler) {
    auto request = std::make_shared<ConsumerRequest>();
    request->method = EnumMethodToString<RequestMethod>(RequestMethod::RequestConsumerKeyFrame);
    request->data = ConsumerRequest::Data();
    request->data->consumerId = consumerId;
    Send<ConsumerRequest, BasicResponse>(request, handler);
}

void SignalingManager::Produce(SignalingRequest(ProduceRequest) request,
                               SignalingHandler(ProduceResponse) handler) {
    Send<ProduceRequest, ProduceResponse>(request, handler);
}

void SignalingManager::ProduceData(SignalingRequest(ProduceDataRequest) request,
                                   SignalingHandler(ProduceDataResponse) handler) {
    Send<ProduceDataRequest, ProduceDataResponse>(request, handler);
}

void SignalingManager::PauseProducer(const std::string& producerId,
                                     SignalingHandler(BasicResponse) handler) {
    auto request = std::make_shared<ProducerRequest>();
    request->method = EnumMethodToString<RequestMethod>(RequestMethod::PauseProducer);
    request->data = ProducerRequest::Data();
    request->data->producerId = producerId;
    Send<ProducerRequest, BasicResponse>(request, handler);
}

void SignalingManager::ResumeProducer(const std::string& producerId,
                                      SignalingHandler(BasicResponse) handler) {
    auto request = std::make_shared<ProducerRequest>();
    request->method = EnumMethodToString<RequestMethod>(RequestMethod::ResumeProducer);
    request->data = ProducerRequest::Data();
    request->data->producerId = producerId;
    Send<ProducerRequest, BasicResponse>(request, handler);
}

void SignalingManager::CloseProducer(const std::string& producerId,
                                     SignalingHandler(BasicResponse) handler) {
    auto request = std::make_shared<ProducerRequest>();
    request->method = EnumMethodToString<RequestMethod>(RequestMethod::CloseProducer);
    request->data = ProducerRequest::Data();
    request->data->producerId = producerId;
    Send<ProducerRequest, BasicResponse>(request, handler);
}

void SignalingManager::ChangeDisplayName(const std::string& displayName,
                                         SignalingHandler(BasicResponse) handler) {
    auto request = std::make_shared<ChangeDisplayNameRequest>();
    request->method = EnumMethodToString<RequestMethod>(RequestMethod::ChangeDisplayName);
    request->data = ChangeDisplayNameRequest::Data();
    request->data->displayName = displayName;
    Send<ChangeDisplayNameRequest, BasicResponse>(request, handler);
}

void SignalingManager::RestartIce(const std::string& transportId,
                                  SignalingHandler(RestartICEResponse) handler) {
    auto request = std::make_shared<RestartICERequest>();
    request->method = EnumMethodToString<RequestMethod>(RequestMethod::RestartIce);
    request->data = RestartICERequest::Data();
    request->data->transportId = transportId;
    Send<RestartICERequest, RestartICEResponse>(request, handler);
}

void SignalingManager::SetConsumerPriority(const std::string& consumerId,
                                           int32_t priority,
                                           SignalingHandler(BasicResponse) handler) {
    auto request = std::make_shared<SetConsumerPriorityRequest>();
    request->method = EnumMethodToString<RequestMethod>(RequestMethod::SetConsumerPriority);
    request->data = SetConsumerPriorityRequest::Data();
    request->data->consumerId = consumerId;
    Send<SetConsumerPriorityRequest, BasicResponse>(request, handler);
}

void SignalingManager::SetConsumerPreferredLayers(const std::string& consumerId,
                                                  int32_t spatialLayer,
                                                  int32_t temporalLayer,
                                                  SignalingHandler(BasicResponse) handler) {
    auto request = std::make_shared<SetConsumerPreferredLayersRequest>();
    request->method = EnumMethodToString<RequestMethod>(RequestMethod::SetConsumerPreferredLayers);
    request->data = SetConsumerPreferredLayersRequest::Data();
    request->data->consumerId = consumerId;
    request->data->spatialLayer = spatialLayer;
    request->data->temporalLayer = temporalLayer;
    Send<SetConsumerPreferredLayersRequest, BasicResponse>(request, handler);
}

void SignalingManager::GetTransportStats(const std::string& transportId,
                                         SignalingHandler(GetTransportStatsResponse) handler) {
    auto request = std::make_shared<GetTransportStatsRequest>();
    request->method = EnumMethodToString<RequestMethod>(RequestMethod::GetTransportStats);
    request->data = GetTransportStatsRequest::Data();
    request->data->transportId = transportId;
    Send<GetTransportStatsRequest, GetTransportStatsResponse>(request, handler);
}

void SignalingManager::GetProducerStats(const std::string& producerId,
                                        SignalingHandler(GetProducerStatsResponse) handler) {
    auto request = std::make_shared<GetProducerStatsRequest>();
    request->method = EnumMethodToString<RequestMethod>(RequestMethod::GetProducerStats);
    request->data = GetProducerStatsRequest::Data();
    request->data->producerId = producerId;
    Send<GetProducerStatsRequest, GetProducerStatsResponse>(request, handler);
}

void SignalingManager::GetDataProducerStats(const std::string& dataProducerId,
                                            SignalingHandler(GetDataProducerStatsResponse) handler) {
    auto request = std::make_shared<GetDataProducerStatsRequest>();
    request->method = EnumMethodToString<RequestMethod>(RequestMethod::GetDataProducerStats);
    request->data = GetDataProducerStatsRequest::Data();
    request->data->dataProducerId = dataProducerId;
    Send<GetDataProducerStatsRequest, GetDataProducerStatsResponse>(request, handler);
}

void SignalingManager::GetConsumerStats(const std::string& consumerId,
                                        SignalingHandler(GetConsumerStatsResponse) handler) {
    auto request = std::make_shared<GetConsumerStatsRequest>();
    request->method = EnumMethodToString<RequestMethod>(RequestMethod::GetConsumerStats);
    request->data = GetConsumerStatsRequest::Data();
    request->data->consumerId = consumerId;
    Send<GetConsumerStatsRequest, GetConsumerStatsResponse>(request, handler);
}

void SignalingManager::GetDataConsumerStats(const std::string& dataConsumerId,
                                            SignalingHandler(GetDataConsumerStatsResponse) handler) {
    auto request = std::make_shared<GetDataConsumerStatsRequest>();
    request->method = EnumMethodToString<RequestMethod>(RequestMethod::GetDataConsumerStats);
    request->data = GetDataConsumerStatsRequest::Data();
    request->data->dataConsumerId = dataConsumerId;
    Send<GetDataConsumerStatsRequest, GetDataConsumerStatsResponse>(request, handler);
}

void SignalingManager::ApplyNetworkThrottle(SignalingRequest(ApplyNetworkThrottleRequest) request,
                                            SignalingHandler(BasicResponse) handler) {
    Send<ApplyNetworkThrottleRequest, BasicResponse>(request, handler);
}

void SignalingManager::ResetNetworkThrottle(const std::string& secret,
                                            SignalingHandler(BasicResponse) handler) {
    auto request = std::make_shared<ResetNetworkThrottleRequest>();
    request->method = EnumMethodToString<RequestMethod>(RequestMethod::ResetNetworkThrottle);
    request->data = ResetNetworkThrottleRequest::Data();
    request->data->secret = secret;
    Send<ResetNetworkThrottleRequest, BasicResponse>(request, handler);
}



}

