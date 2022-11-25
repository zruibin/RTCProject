/*
 *
 * app.cc
 *
 * Created by Ruibin.Chow on 2022/10/20.
 * Copyright (c) 2022年 Ruibin.Chow All rights reserved.
 */

#include "app.h"
#include <iostream>
#include <httplib/httplib.h>
#include "core/rtc_call_interface.h"
#include "log/logging.h"
#include "util/timer.h"
#include "platform/platform.h"
#include "engine/signalings/signaling_manager.h"

#if VIEW_DISPLAY
#include <QApplication>
#include <QTextCodec>
#include <QDir>
#include "view/window.h"
#endif

namespace app {

using namespace::core;


class CallObserver : public RTCCallObserverInterface {
    
public:
    void OnCreateSdp(RTCStringMap& sdpDict,
                     const std::string& peerId) override {
        
    }
    
    void OnGenerateIceCandidate(RTCStringMap& sdpDict,
                                const RTCString& peerId) override {
        
    }
    
    void OnPeerStateChangge(core::RTCPeerState peerState,
                            const RTCString& peerId) override {
        
    }
};

void testRTC() {
    Log(INFO) << "App RTC Start.";
    RTCBaseConfig config;
    std::unique_ptr<RTCCallObserverInterface> observer = std::make_unique<CallObserver>();
    std::shared_ptr<RTCCallInterface> call = nullptr;
    auto ret = CreateRTCCallOrError(std::move(observer), config);
    if (!ret.ok()) {
        const char* message = ret.error().message();
        Log(ERROR) << "CreateRTCCall error: " << message;
        return;
    }
    call = ret.value();
    call->Init();
    Log(INFO) << "App RTC End.";
}

void testHttplib() {
    httplib::Client cli("http://www.baidu.com");
    if (auto res = cli.Get("/")) {
        Log(INFO) << res->status;
        Log(INFO)  << res->get_header_value("Content-Type");
        Log(INFO)  << res->body;
    } else {
        Log(INFO) << "error code: " << httplib::to_string(res.error());
    }
}

void testTimer() {
    Log(DEBUG) << "App timer Start.";
    util::AsynTimer::Detach([](void* pUser) {
        Log(DEBUG) << "Detach timer no repeate.";
    }, 1*TIME_NSEC_PER_SEC, false, nullptr);
    
    int runningTime = 0;
    util::AsynTimer::Detach([&runningTime](void* pUser) {
        runningTime += 4;
        Log(DEBUG) << "Detach timer has been running for "
                    << runningTime << " second(s).";
    }, 4*TIME_NSEC_PER_SEC, true, nullptr);
    
    util::Timer::Sleep(30*TIME_NSEC_PER_SEC); // must
    Log(DEBUG) << "App timer End.";
}

void testSocket() {
    Log(DEBUG) << "App Socket Start.";
    engine::testSocket();
    Log(DEBUG) << "App Socket End.";
}

void App::Init() {
    logger::SetMinWriteLogLevel(logger::VERBOSE);
    platform::thread_set_name("RTCProject.main-thread");
    Log(INFO) << "App Init Start.";
    Log(VERBOSE) << "Current Thread Name: " << platform::thread_get_current_name();
//    testHttplib();
//    testTimer();
//    testSocket();
    testRTC();
    Log(INFO) << "App Init End.";
}

void App::Run(int &argc, char **argv) {
#if VIEW_DISPLAY
    QApplication::setAttribute(Qt::AA_UseOpenGLES);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    
    QApplication app(argc, argv);
    app.setApplicationName("RTCProject");
    app.setApplicationVersion("V1.0.0");

    QString dirPath = QCoreApplication::applicationDirPath().append("/Data");
    QDir dir(dirPath);
    if(!dir.exists()) dir.mkdir(dirPath);
    logger::SetDefaultLoggerDir(dirPath.toStdString());
#else
    namespace fs = std::filesystem;
    fs::path dst = fs::current_path() / std::string("./Data");
    if (!fs::exists(dst)) fs::create_directory(dst);
    logger::SetDefaultLoggerDir(dst.string());
#endif
    
    Init();
    
#if VIEW_DISPLAY
    view::Window w;
    w.show();
    
    app.exec();
#endif
}

}


