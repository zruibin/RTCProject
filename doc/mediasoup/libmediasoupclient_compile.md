# libmediasoupclient compile


# 一、ubuntu下编译测试

## 1.1 基本要求
 - POSIX or Windows based operating system
 - cmake >= 3.5
 - gcc and g++ >= 4.9 or clang (with C++14 support)

## 1.2 编译webrtc

 - depot_tools
```
git clone https://chromium.googlesource.com/chromium/tools/depot_tools.git
export PATH=$PATH:/home/user/depot_tools
```
 - webrtc
```
mkdir webrtc
cd webrtc
fetch --nohooks webrtc
gclient sync
cd src
git checkout -b m94 refs/remotes/branch-heads/4606
gclient sync
```

 - deps
```
cd ./src
./build/install-build-deps.sh
```

 - 编译
```
gn gen out/m94 --args='is_debug=false is_component_build=false is_clang=false rtc_include_tests=false rtc_use_h264=true use_rtti=true use_custom_libcxx=false treat_warnings_as_errors=false use_ozone=true ffmpeg_branding="Chrome" proprietary_codecs=true use_glib=false rtc_use_dummy_audio_file_devices=true rtc_build_examples=false use_partition_alloc=false rtc_enable_protobuf=false rtc_build_json=true'

```

## 1.3 编译libmediasoupclient
 
 - 下载
```
$ git clone https://github.com/versatica/libmediasoupclient.git
$ cd libmediasoupclient/
$ git checkout 3.X.Y.
```
 - 编译
```
$ cmake . -Bbuild \
  -DLIBWEBRTC_INCLUDE_PATH:PATH=${PATH_TO_LIBWEBRTC_SOURCES} \
  -DLIBWEBRTC_BINARY_PATH:PATH=${PATH_TO_LIBWEBRTC_BINARY}

$ make -C build/
```

## 1.4 测试demo

 - main.cpp代码

```
#include <mediasoupclient.hpp>
#include <iostream>

int main()
{
	std::cout << mediasoupclient::Version() << std::endl;
	return 0;
}
```

 - 编译方法

```
g++  main.cpp -DWEBRTC_POSIX  -o demo\
        -I../libmediasoupclient/include\
        -I../libmediasoupclient/deps/libsdptransform/include\
        -I../webrtc-chekout/src\
        -I../webrtc-chekout/src/third_party/abseil-cpp\
        lib/libmediasoupclient.a\
        /usr/local/lib/libwebrtc.a\
        /usr/lib/x86_64-linux-gnu/libssl.so\
        /usr/lib/x86_64-linux-gnu/libcrypto.so\
        /usr/lib/x86_64-linux-gnu/libz.so\
        -ldl -lpthread
```
