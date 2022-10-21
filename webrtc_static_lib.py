#! /usr/bin/env python3
# -*- coding: utf-8 -*- 
#
# webrtc_static_lib.py
#
# Created by Ruibin.Chow on 2021/12/11.
# Copyright (c) 2021年 Ruibin.Chow All rights reserved.
# 

"""
http://www.manongjc.com/detail/11-qwvnzokvfrdpvat.html

https://blog.csdn.net/qq_21358401/article/details/78614211
http://www.enkichen.com/2017/05/12/webrtc-ios-build/
"""


import os, re, subprocess, sys, shutil


def getAllFileInDirBeyoundTheDir(originDir, directory, outDir):
    """返回指定目录下所有文件的集合"""
    array = []
    for root, dirs, files in os.walk(originDir+directory):
        for name in files:
            if not name.endswith('.h'):
                continue

            outPath = root.replace(originDir, outDir)
            if not os.path.exists(outPath):
                os.makedirs(outPath)
            path = os.path.join(root, name)
            # print outPath
            shutil.copy(path, outPath)
            # array.append(path)
    return array


def main():
    srcDir = "src/"
    outDir = "./Header/"
    if not os.path.exists(outDir):
        os.mkdir(outDir)

    hedarDirs = ["api", "rtc_base", "p2p", "media",
        "modules", "logging", "call",
        "common_audio", "common_video", "system_wrappers",
        # "audio", "base",
        # "pc", "video", "third_party"
    ]
    for directory in hedarDirs:
        getAllFileInDirBeyoundTheDir(srcDir, directory, outDir)

    """
    libs = [
        "libbase_native_additions_objc.a", 
        "libcallback_logger_objc.a",
        "libdefault_codec_factory_objc.a",
        "libfile_logger_objc.a",
        "libmediaconstraints_objc.a",
        "libmediasource_objc.a",
        "libmetal_objc.a",
        "libnative_api.a",
        "libnative_video.a",
        "libpeerconnectionfactory_base_objc.a",
        "libui_objc.a",
        "libvideo_objc.a",
        "libvideo_toolbox_cc.a",
        "libvideocapture_objc.a",
        "libvideocodec_objc.a",
        "libvideoframebuffer_objc.a",
        "libvideorendereradapter_objc.a",
        "libvideosource_objc.a",
        "libvideotoolbox_objc.a",
        "libvp8.a",
        "libvp9.a",
        "libav1.a",
        "libvpx_codec_constants.a",
        "libwrapped_native_codec_objc.a"
    ]
    compileLibsDir = srcDir + "out/release/" + "obj/sdk/"
    libsDir = "./Libs/"
    if not os.path.exists(libsDir):
        os.mkdir(libsDir)
    for lib in libs:
        libPath = compileLibsDir + lib
        if os.path.exists(libPath):
            shutil.copy(libPath, libsDir)
        pass
    """
    
    # webRTCLib = srcDir + "out/release/" + "obj/" + "libwebrtc.a"
    # if os.path.exists(webRTCLib):
    #         shutil.copy(webRTCLib, libsDir)


    # commomType = "common_types.h"
    # shutil.copy(srcDir+commomType, outDir)

    pass

if __name__ == '__main__':
    main()
    pass



"""
依赖库:
CoreServices.framework
Foundation.framework
libobjc.tbd
OpenGL.framework
Metal.framework
AVFoundation.framework
VideoToolbox.framework
AudioUnit.framework
AudioToolbox.framework
CoreAudio.framework
ApplicationServices.framework
"""


"""
https://www.codercto.com/a/38694.html
fetch = +refs/branch-heads/*:refs/remotes/branch-heads/*

https://blog.csdn.net/uianster/article/details/121595205
https://www.cnblogs.com/ngxianyu/p/11412612.html

gn gen out/arm64_libs_xcode --args='target_os="ios" ios_enable_code_signing=false is_component_build=false rtc_include_tests=false is_debug=false target_environment="device" target_cpu="arm64" ios_deployment_target="11.0" rtc_libvpx_build_vp9=false use_goma=false rtc_enable_objc_symbol_export=true enable_dsyms=true enable_stripping=true' --ide=xcode

# arm64
gn gen out/mac_libs --args='target_os="mac" target_cpu="x64" proprietary_codecs=true rtc_include_tests=false is_debug=false enable_dsyms=true rtc_build_tools=false rtc_build_examples=false use_custom_libcxx=false'
"""

"""
__x86_64__=1 WEBRTC_MAC=1 WEBRTC_POSIX=1
"""


"""
Action "Compile and copy framework_objc_signed_bundle via ninja"

import shutil, os
webRTCLib = 'WebRTC.framework'
dstDir = '/Users/ruibin.chow/Documents/code/other_code/pingan_source_code/pfmc-ios_new/PFMCProject/PFMCSDK/PFMCSDK/Frameworks/WebRTC.framework'
if os.path.exists(dstDir):
    shutil.rmtree(dstDir)
shutil.copytree(webRTCLib, dstDir)
print("Copy WebRTC.framework done.")

process.wait()
"""

"""
.gclient

solutions = [
  {
    "name": "src",
    "url": "https://webrtc.googlesource.com/src.git",
    "deps_file": "DEPS",
    "managed": False,
    "custom_deps": {},
  },
]
target_os = ["ios", "mac", "linux", "android", "win"]

不同平台target_os不同。
"""



