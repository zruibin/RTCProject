/*
 * rtc_call_interface.h
 *
 * Created by Ruibin.Chow on 2022/10/20.
 * Copyright (c) 2022年 Ruibin.Chow All rights reserved.
 */

#ifndef RTC_CALL_INTERFACE_H
#define RTC_CALL_INTERFACE_H

#include <memory>
#include "rtc_call_observer_interface.h"

namespace core {

class RTCCallInterface {

public:
    virtual ~RTCCallInterface() = default;
    
public:
    virtual void Init(void) = 0;
    
private:
    /// manual audio enabled flag
    bool useManualAudio_;
    /// audio enable flag
    bool audioEnabled_;
    /// built in speaker enabled flag
    bool builtInSpeakerOn_;
    /// all audio sender mute flag
    bool allAudioSenderMute_;
    /// all audio receiver mute flag
    bool allAudioReceiverMute_;
};



/*----------------------------------------------------------------------------*/

std::shared_ptr<RTCCallInterface>
CreateRTCCall(RTCCallObserverInterface *observer);

}

#endif /* !RTC_CALL_INTERFACE_H */
