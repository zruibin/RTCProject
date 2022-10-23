/*
 * rtc_config.h
 *
 * Created by Ruibin.Chow on 2022/10/23.
 * Copyright (c) 2022年 Ruibin.Chow All rights reserved.
 */

#ifndef RTC_CONFIG_H
#define RTC_CONFIG_H

#include "rtc_macro.h"
#include "rtc_call_emun.h"

namespace core {

struct RTCVideoConfig {
    
    /// 创建默认的视频配置(360p, 15fps, 600kbps)
    /// @return RTCVideoConfig 实例
    static RTCVideoConfig DefaultConfig();
    
    /// 根据预先视频配置枚举创建配置
    /// @return RTCVideoConfig 实例
    static RTCVideoConfig ConfigWithPreset(RTCVideoConfigPreset preset);
    
public:
    
#pragma mark - 属性
    
    /// 采集分辨率，控制摄像头图像采集的分辨率。仅摄像头启动前且没有使用自定义视频采集时，设置有效
    RTCSize captureResolution;
    /// 编码分辨率，控制编码器编码推流的图像分辨率。推流前后设置均生效
    RTCSize encodeResolution;
    /// 帧率，控制摄像头采集帧率以及编码器编码帧率的大小。仅摄像头启动前设置有效
    int fps;
    /// 码率，单位为 kbps。
    int bitrate;
    /// 最低码率，单位为 kbps。
    int minBitrate;

#pragma mark - 高级属性（仅启用了simulcast功能才生效）

    /// 低清晰度的编码分辨率，控制编码器编码推流的图像分辨率。推流前后设置均生效
    /// 注意：
    ///     1. encodeResolutionLD的宽高比需要与encodeResolution的保持一致，不然低清晰度视频会变形；
    ///     2. encodeResolutionLD的宽高都需要小于encodeResolution的宽高，大于的情况将使用encodeResolution
    RTCSize encodeResolutionLD;
    /// 低清晰度的最大码率，单位为 kbps。
    int bitrateLD;
    /// 低清晰度的最低码率，单位为 kbps。
    int minBitrateLD;

private:
    RTCVideoConfig() = default;
    RTCVideoConfig(RTCVideoConfigPreset preset);
};

}

#endif /* !RTC_CONFIG_H */
