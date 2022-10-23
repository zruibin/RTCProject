/*
 *
 * rtc_config.cc
 *
 * Created by Ruibin.Chow on 2022/10/23.
 * Copyright (c) 2022年 Ruibin.Chow All rights reserved.
 */

#include "rtc_config.h"

namespace core {

RTCVideoConfig RTCVideoConfig::DefaultConfig() {
    return ConfigWithPreset(RTCVideoConfigPreset::_360P);
}

RTCVideoConfig RTCVideoConfig::ConfigWithPreset(RTCVideoConfigPreset preset) {
    return RTCVideoConfig(preset);
}

RTCVideoConfig::RTCVideoConfig(RTCVideoConfigPreset preset) {
    switch (preset) {
        case RTCVideoConfigPreset::_180P: {
            this->captureResolution = RTCSizeMake(180, 320);
            this->encodeResolution = RTCSizeMake(180, 320);
            this->fps = 15;
            this->bitrate = 300;
            this->encodeResolutionLD = RTCSizeMake(90, 156);
            this->bitrateLD = 100;
            break;
        }
            
        case RTCVideoConfigPreset::_270P: {
            this->captureResolution = RTCSizeMake(270, 480);
            this->encodeResolution = RTCSizeMake(256, 480);
            this->fps = 15;
            this->bitrate = 400;
            this->encodeResolutionLD = RTCSizeMake(180, 320);
            this->bitrateLD = 200;
            break;
        }
            
        case RTCVideoConfigPreset::_540P: {
            this->captureResolution = RTCSizeMake(540, 960);
            this->encodeResolution = RTCSizeMake(540, 960);
            this->fps = 15;
            this->bitrate = 1200;
            this->encodeResolutionLD = RTCSizeMake(180, 320);
            this->bitrateLD = 200;
            break;
        }
            
        case RTCVideoConfigPreset::_720P: {
            this->captureResolution = RTCSizeMake(720, 1280);
            this->encodeResolution = RTCSizeMake(720, 1280);
            this->fps = 15;
            this->bitrate = 1500;
            this->encodeResolutionLD = RTCSizeMake(180, 320);
            this->bitrateLD = 200;
            break;
        }
            
        case RTCVideoConfigPreset::_1080P: {
            this->captureResolution = RTCSizeMake(1080, 1920);
            this->encodeResolution = RTCSizeMake(1080, 1920);
            this->fps = 15;
            this->bitrate = 3000;
            this->encodeResolutionLD = RTCSizeMake(180, 320);
            this->bitrateLD = 200;
            break;
        }
            
        default: {
            this->captureResolution = RTCSizeMake(360, 640);
            this->encodeResolution = RTCSizeMake(360, 640);
            this->fps = 15;
            this->bitrate = 600;
            this->encodeResolutionLD = RTCSizeMake(180, 320);
            this->bitrateLD = 200;
            break;
        }
    }
}

}

