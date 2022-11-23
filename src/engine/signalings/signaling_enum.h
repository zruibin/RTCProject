/*
 * signaling_enum.h
 *
 * Created by Ruibin.Chow on 2022/11/23.
 * Copyright (c) 2022年 Ruibin.Chow All rights reserved.
 */

#ifndef SIGNALING_ENUM_H
#define SIGNALING_ENUM_H

#include <string>

namespace engine {

enum class NotificationMethod {
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

std::string NotificationMethodToOriginString(NotificationMethod method);
std::string NotificationMethodToString(NotificationMethod method);

}

#endif /* !SIGNALING_ENUM_H */
