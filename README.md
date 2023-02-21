
## RTCProject

* WebRTC Version : M106

## Doc

### mediasoup

* [mediasoup](./doc/mediasoup/mediasoup.md)
* [mediasoup传递信令与协议](./doc/mediasoup/mediasoup传递信令与协议.md)
* [mediasoup流程](./doc/mediasoup/mediasoup流程.md)
* [mediasoup服务器连接、接收、发送](./doc/mediasoup/mediasoup服务器连接、接收、发送.md)
* [mediasoup源码分析-初始化、建立连接及媒体数据的处理流程](./doc/mediasoup/mediasoup源码分析-初始化、建立连接及媒体数据的处理流程.md)

## webrtc源码调试

### 一、cmake源码集成方式

```
set(webrtc_source "path/WebRTC")
file(GLOB_RECURSE RTC_Source
    "${webrtc_source}/webrtc/src/**/*.c"
    "${webrtc_source}/webrtc/src/**/*.cc"
    "${webrtc_source}/webrtc/src/**/*.h"
    "${webrtc_source}/webrtc/src/**/*.hpp"
    "${webrtc_source}/webrtc/src/**/*.h++"
)
sourceGroup("${webrtc_source}" ${RTC_Source})
set_source_files_properties(${RTC_Source} PROPERTIES HEADER_FILE_ONLY TRUE)
list(APPEND ALL_SOURCES ${RTC_Source})
```

> 注意：此方式存在函数跳转问题

### 二、xcode工程加入方式

1. 生成webrtc xcode工程

```
gn gen out/mac_libs_xcode --args='target_os="mac" target_cpu="x64" proprietary_codecs=true rtc_include_tests=false is_debug=false enable_dsyms=true rtc_build_tools=false rtc_build_examples=false use_custom_libcxx=false use_rtti=true' --ide=xcode
```

2.   将xcode工程拉入RTCProject工程中，并在webrtc工程的**All->Build Phases**中新建脚本以下执行，**必须在ninja构建之后**

```
import shutil, os

#path替换为具体路径
webRTCLib = '/path/WebRTC/webrtc/src/out/mac_libs_xcode/obj/libwebrtc.a'
dstDir = '/path/RTCProject/webrtc/lib/libwebrtc.a'

if os.path.exists(dstDir):
    os.remove(dstDir)
shutil.copy(webRTCLib, dstDir)

print("Copy libwebrtc.a done.")
```

**或在ninja构建中使用管道重定向python处理**

```
ninja -C . | python3 -c '

import shutil, os, sys

tmpObj = sys.stdin.readline()
print(str(tmpObj), end="")

inObj = sys.stdin.readlines()

needCopy = True
if type(inObj) == list:
    for line in inObj:
        print(str(line), end="")
        if "no work to do" in line:
            needCopy = False

if needCopy:
    webRTCLib = "/path/WebRTC/webrtc/src/out/mac_libs_xcode/obj/libwebrtc.a"
    dstDir = "/path/RTCProject/webrtc/lib/libwebrtc.a"
    if os.path.exists(dstDir):
        os.remove(dstDir)
    shutil.copy(webRTCLib, dstDir)
    print("Copy WebRTC.framework done.")

'
```

3. RTCProject工程的Target Dependencies中加上 **2** 中工程的target(即 **All** )，**顺序位于Core构建之前(保证libwebrtc.a为更改的最新)**
