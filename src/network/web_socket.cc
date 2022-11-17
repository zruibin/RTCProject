/*
 *
 * web_socket.cc
 *
 * Created by Ruibin.Chow on 2022/11/16.
 * Copyright (c) 2022年 Ruibin.Chow All rights reserved.
 */

#include "web_socket.h"
#include "log/logging.h"
#include "util/util.h"

namespace network {

WebSocket::WebSocket(const std::string& url) : url_(url) {
    Log(INFO) << "WebSocket::WebSocket";
    clientRef_ = std::make_shared<WebsocketClient>();
    
    // 初始化 ASIO
    ErrorCode ec;
    clientRef_->init_asio(ec);
    if (ec) {
        Log(ERROR) << "init_asio failed: " << ec.message();
        return;
    }
    clientRef_->start_perpetual();
    
    std::shared_ptr<WebsocketClient> client = clientRef_;
    auto runLoop = [client] {
        Log(INFO) << "WebSocket RunLoop begin.";
        ErrorCode ec;
        client->get_io_service().run(ec);
        if (ec) {
            Log(ERROR) << "WebSocket RunLoop failed: " << ec.message();
        }
        Log(INFO) << "WebSocket RunLoop end.";
    };
    std::thread wsThread(runLoop);
    wsThread.detach();
}

WebSocket::~WebSocket() {
    Log(INFO) << "WebSocket::~WebSocket";
    clientRef_->stop_perpetual();
    clientRef_->get_io_service().stop();
}

SocketInterface::Error WebSocket::Init() {
    Log(INFO) << "WebSocket::Init";
    clientRef_->clear_access_channels(websocketpp::log::alevel::all);
    clientRef_->clear_error_channels(websocketpp::log::alevel::all);
    clientRef_->clear_access_channels(websocketpp::log::alevel::frame_payload);
    clientRef_->clear_access_channels(websocketpp::log::alevel::frame_header);
    
//    clientRef_->set_connect_timeout(connectTimeout_);
//    clientRef_->set_open_handshake_timeout(connectTimeout_);
//    clientRef_->set_pong_timeout(pongTimeout_);
    
    std::weak_ptr<WebSocket> weakSelf(WebSocket::shared_from_this());

    clientRef_->set_open_handler([this, weakSelf] (ConnectHandler hdl) {
        if (auto self = weakSelf.lock()) {
            asio::post(*ioContext_, [this, weakSelf, hdl] {
                if (auto self2 = weakSelf.lock()) {
                    OnOpen(hdl);
                }
            });
        }
    });
    
    clientRef_->set_close_handler([this, weakSelf] (ConnectHandler hdl) {
        if (auto self = weakSelf.lock()) {
            asio::post(*ioContext_, [this, weakSelf, hdl] {
                if (auto self2 = weakSelf.lock()) {
                    OnClose(hdl);
                }
            });
        }
    });
    
    clientRef_->set_fail_handler([this, weakSelf] (ConnectHandler hdl) {
        if (auto self = weakSelf.lock()) {
            asio::post(*ioContext_, [this, weakSelf, hdl] {
                if (auto self2 = weakSelf.lock()) {
                    OnFail(hdl);
                }
            });
        }
    });
    
    clientRef_->set_message_handler([this, weakSelf] (ConnectHandler hdl, MessagePtr msg) {
        if (auto self = weakSelf.lock()) {
            asio::post(*ioContext_, [this, weakSelf, hdl, msg] {
                if (auto self2 = weakSelf.lock()) {
                    OnMessage(hdl, msg);
                }
            });
        }
    });
    
    //*
    clientRef_->set_socket_init_handler([this, weakSelf] (ConnectHandler hdl, SocketType& socket) {
        if (auto self = weakSelf.lock()) {
            OnSocketInit(hdl, socket);
        }
    });

    clientRef_->set_tcp_pre_init_handler([this, weakSelf] (ConnectHandler hdl) {
        if (auto self = weakSelf.lock()) {
            OnTCPPreInit(hdl);
        }
    });
    
    clientRef_->set_tcp_post_init_handler([this, weakSelf] (ConnectHandler hdl) {
        if (auto self = weakSelf.lock()) {
            OnTCPPostInit(hdl);
        }
    });
    
    clientRef_->set_pong_handler([this, weakSelf] (ConnectHandler hdl, std::string str) {
        if (auto self = weakSelf.lock()) {
            OnPong(hdl, str);
        }
    });
    
    clientRef_->set_pong_timeout_handler([this, weakSelf] (ConnectHandler hdl, std::string str) {
        if (auto self = weakSelf.lock()) {
            OnPongTimeout(hdl, str);
        }
    });
    //*/
    
    SetState(State::kPrepared);
    error_ = Error::kNone;
    return error_;
}

Socket::Error WebSocket::DeInit() {
    Log(INFO) << "WebSocket::DeInit.";
    hdl_.reset();
    con_.reset();
    startConnectTime_ = 0;
    lastSentPingTime_ = 0;
    lastReceivedPongTime_ = 0;
    lastReceivedTime_ = 0;
    SetState(State::kClosed);
}

void WebSocket::SetConnectTimeout(int timeout) {
    connectTimeout_ = timeout;
}

void WebSocket::Open() {
    if (!IsCurrentContext()) {
        asio::post(*ioContext_, [this] {
            Open();
        });
        return;
    }
    Log(INFO) << "WebSocket::Open";
    Log(INFO) << "timeout: " << connectTimeout_;
    
    if (state_ >= State::kConnecting && state_ <= State::kClosing) {
        DeInit();
    }
    
    Error err = Init();
    if (err != Error::kNone) {
        DeInit();
        TriggerError(err, "init failed");
        return;
    }
    
    ErrorCode ec;
    con_ = clientRef_->get_connection(url_, ec);
    if (ec) {
        Log(ERROR) << " get connection failed: " << ec.message();
        TriggerError(Error::kUnknow, ec.message());
        return;
    }

    if (subProtocol_.size() > 0) {
        Log(INFO) << "SubProtocol: " << subProtocol_;
        con_->add_subprotocol(subProtocol_);
    }
    
    SetState(State::kConnecting);
    startConnectTime_ = util::get_current_time_milliseconds();
    hdl_ = con_->get_handle();
    clientRef_->connect(con_);
}

void WebSocket::Close() {
    if (!IsCurrentContext()) {
        asio::post(*ioContext_, [this] {
            Close();
        });
        return;
    }
    Log(INFO) << "WebSocket::Close";
    if (state_ == State::kIdle || state_ >=State::kClosing) {
        return;
    }
    Close(websocketpp::close::status::normal, "normal");
}

void WebSocket::Send(const char* buf, int len, FrameType frameType) {
    if (!IsCurrentContext()) {
        asio::post(*ioContext_, [this, buf, len, frameType] {
            Send(buf, len, frameType);
        });
        return;
    }
    
    if (!IsConnected()) {
        Log(WARNING) << "It must connected before Send.";
        return;
    }
    
    ErrorCode ec;
    clientRef_->send(hdl_, buf, (size_t)len, (websocketpp::frame::opcode::value)frameType, ec);
    if (ec) {
        Log(ERROR) << "send failed: " << ec.message();
    }
}

void WebSocket::TriggerError(Error error, const std::string& reason) {
    Close(websocketpp::close::status::normal, reason);
    Socket::TriggerError(error, reason);
}

void WebSocket::Close(CloseValue closeValue, const std::string& reason) {
    Log(INFO) << "close with code: " << closeValue << ", reason: " << reason;
    ErrorCode ec;
    clientRef_->close(hdl_, closeValue, reason, ec);
    if (!ec) {
        SetState(State::kClosing);
    } else {
        Log(ERROR) << "close failed: " << ec.message();
        DeInit();
    }
}

void WebSocket::OnSocketInit(ConnectHandler hdl, SocketType& socket) {
    Log(INFO) << "WebSocket::OnSocketInit.";
    auto& rawSocket = socket.lowest_layer();
    ErrorCode ec;
    rawSocket.open(true ? asio::ip::tcp::v4() : asio::ip::tcp::v6(), ec);
    if (ec) {
        Log(ERROR) << "socket open failed: " << ec.message();
        return;
    }
//    asio::ip::tcp::endpoint local(network_.Address(), 0);
//    rawSocket.bind(local, ec);
//    if (ec) {
//        Log(ERROR) << "socket bind failed: " << ec;
//    }
}

void WebSocket::OnTCPPreInit(ConnectHandler hdl) {
    Log(VERBOSE) << "WebSocket::OnTCPPreInit.";
    
}

void WebSocket::OnTCPPostInit(ConnectHandler hdl) {
    Log(VERBOSE) << "WebSocket::OnTCPPostInit.";
}

void WebSocket::OnOpen(ConnectHandler hdl) {
    Log(INFO) << "WebSocket::OnOpen.";
    ErrorCode ec;
    ConnectionPtr con = clientRef_->get_con_from_hdl(hdl, ec);
    if (!con_ || con_ != con) {
        Log(INFO) << "ignore.";
        return;
    }
    
//    auto& socket = con->get_socket();
//    Log(INFO) << "socket: " << socket.lowest_layer().local_endpoint(ec).data()
//                << " -> " << socket.lowest_layer().remote_endpoint(ec).data();
    uint32_t duration = util::get_current_time_milliseconds() - startConnectTime_;
    Log(INFO) << "connect duration: " << duration << "ms";

    SetState(State::kConnected);
}

void WebSocket::OnClose(ConnectHandler hdl) {
    Log(INFO) << "WebSocket::OnClose.";
    ErrorCode ec;
    ConnectionPtr con = clientRef_->get_con_from_hdl(hdl, ec);
    if (!con_ || con_ != con) {
        Log(INFO) << "ignore.";
        return;
    }
    
    LogConnectionInfo(con);
    const std::string& resason = con->get_ec().message();
    Log(INFO) << "On close: " << resason;
    if (state_ < State::kClosing) {
        TriggerError(Error::kNetwork, resason);
    } else {
        DeInit();
    }
}

void WebSocket::OnFail(ConnectHandler hdl) {
    Log(INFO) << "WebSocket::OnFail.";
    ErrorCode ec;
    ConnectionPtr con = clientRef_->get_con_from_hdl(hdl, ec);
    if (!con_ || con_ != con) {
        Log(INFO) << "ignore.";
        return;
    }
    LogConnectionInfo(con);
    const std::string& resason = con->get_ec().message();
    Log(INFO) << "on fail: " << resason;
    TriggerError(Error::kProtocol, resason);
}

void WebSocket::OnPong(ConnectHandler hdl, std::string str) {
    Log(INFO) << "WebSocket::OnPong.";
    ErrorCode ec;
    ConnectionPtr con = clientRef_->get_con_from_hdl(hdl, ec);
    if (!con_ || con_ != con) {
        Log(INFO) << "ignore.";
        return;
    }
    lastReceivedPongTime_ = util::get_current_time_milliseconds();
    uint32_t rtt = lastReceivedPongTime_ - lastSentPingTime_;
    if (rtt > kBadRTT) {
        Log(WARNING) << "received a pong frame, rtt bad: " << rtt;
    } else {
        Log(INFO) << "received a pong frame, rtt: " << rtt;
    }
}

void WebSocket::OnPongTimeout(ConnectHandler hdl, std::string str) {
    Log(INFO) << "WebSocket::OnPongTimeout.";
    ErrorCode ec;
    ConnectionPtr con = clientRef_->get_con_from_hdl(hdl, ec);
    if (!con_ || con_ != con) {
        Log(INFO) << "ignore.";
        return;
    }
    LogConnectionInfo(con);
    const std::string& resason = con->get_ec().message();
    Log(INFO) << "on pong timeout: " << resason;
    TriggerError(Error::kNetwork, "pong timeout");
}

void WebSocket::OnMessage(ConnectHandler hdl, MessagePtr msg) {
    Log(VERBOSE) << "WebSocket::OnMessage.";
    ErrorCode ec;
    ConnectionPtr con = clientRef_->get_con_from_hdl(hdl, ec);
    if (!con_ || con_ != con) {
        Log(INFO) << "ignore.";
        return;
    }
    if (receivedFrameHandler_) {
        receivedFrameHandler_((const char*)msg->get_payload().data(),
                              (int)msg->get_payload().size(),
                              (SocketInterface::FrameType)msg->get_opcode());
    }
}

void WebSocket::LogConnectionInfo(ConnectionPtr con) {
    if (!con) {
        Log(INFO) << "connection is nullptr";
        return;
    }
    
    ErrorCode ec;
    auto& socket = con->get_socket();
//    Log(INFO) << "socket: " << socket.lowest_layer().local_endpoint(ec).address().to_string()
//                << " -> " << socket.lowest_layer().remote_endpoint(ec).address().to_string();
    
    Log(INFO) << "con info: " << con.get() << "(" << con->get_ec().message() << "); "
                << "Local: " << con->get_local_close_code() << " ("
                << websocketpp::close::status::get_string(con->get_local_close_code())
                << "), close reason: " << con->get_local_close_reason()
                << "; Remote: " << con->get_remote_close_code() << " ("
                << websocketpp::close::status::get_string(con->get_remote_close_code())
                << "), close reason: " << con->get_remote_close_reason();
}

};

