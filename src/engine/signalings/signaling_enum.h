/*
 * signaling_enum.h
 *
 * Created by Ruibin.Chow on 2022/11/23.
 * Copyright (c) 2022年 Ruibin.Chow All rights reserved.
 */

#ifndef SIGNALING_ENUM_H
#define SIGNALING_ENUM_H

#include <string>
#include <magic_enum/magic_enum.hpp>

namespace engine {

enum class RequestMethod {
    None,
    GetRouterRtpCapabilities,
    CreateWebRtcTransport,
    Join,
    ConnectWebRtcTransport,
    NewDataConsumer,
    NewConsumer,
    PauseConsumer,
    ResumeConsumer,
    RequestConsumerKeyFrame,
    Produce,
    ProduceData,
    PauseProducer,
    ResumeProducer,
    CloseProducer,
    ChangeDisplayName,
    RestartIce,
    SetConsumerPriority,
    SetConsumerPreferredLayers,
    GetTransportStats,
    GetProducerStats,
    GetDataProducerStats,
    GetConsumerStats,
    GetDataConsumerStats,
    ApplyNetworkThrottle,
    ResetNetworkThrottle,
};


enum class NotificationMethod {
    None,
    ProducerScore,
    NewPeer,
    PeerClosed,
    PeerDisplayNameChanged,
    DownlinkBwe,
    ConsumerClosed,
    ConsumerPaused,
    ConsumerResumed,
    ConsumerLayersChanged,
    ConsumerScore,
    DataConsumerClosed,
    ActiveSpeaker,
};


template<typename T>
std::string EnumMethodToOriginString(T method) {
    auto methodName = magic_enum::enum_name(method);
    std::string enumString{methodName};
    return enumString;
}

template<typename T>
std::string EnumMethodToString(T method) {
    std::string enumString = EnumMethodToOriginString<T>(method);
    if (enumString.size() > 0) {
        enumString[0] = tolower(enumString[0]);
    }
    return enumString;
}

template<typename T>
T EnumMethodFromString(const std::string& methodString) {
    if (methodString.length() == 0) {
        return T::None;
    }

    std::string methodStr{methodString};
    methodStr[0] = toupper(methodStr[0]);
    auto method = magic_enum::enum_cast<T>(methodStr);
    if (!method.has_value()) {
        return T::None;
    }

    return method.value();
}

}

#endif /* !SIGNALING_ENUM_H */
