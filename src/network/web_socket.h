/*
 * web_socket.h
 *
 * Created by Ruibin.Chow on 2022/11/16.
 * Copyright (c) 2022年 Ruibin.Chow All rights reserved.
 */

#ifndef WEB_SOCKET_H
#define WEB_SOCKET_H

#include "socket.h"
#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>

namespace network {

using WebsocketClient = websocketpp::client<websocketpp::config::asio_client>;
using ConnectHandler = websocketpp::connection_hdl;
using SocketType = WebsocketClient::client::connection_type::socket_type;
using ConnectionPtr = WebsocketClient::client::connection_ptr;
using MessagePtr = WebsocketClient::client::message_ptr;
using ErrorCode = websocketpp::lib::error_code;
using CloseValue = websocketpp::close::status::value;

class WebSocket : public Socket, public std::enable_shared_from_this<WebSocket> {

public:
    WebSocket() = delete;
    WebSocket(const std::string& url);
    virtual ~WebSocket();
    
    Error Init() override;
    Error DeInit() override;
    void SetConnectTimeout(int timeout) override;
    void Open() override;
    void Close() override;
    void Send(const char/*uint8_t*/* buf, int len, FrameType frameType) override;
    void TriggerError(Error error, const std::string& reason) override;
    
private:
    void Close(CloseValue closeValue, const std::string& reason);
    void OnSocketInit(ConnectHandler hdl, SocketType& socket);
    void OnTCPPreInit(ConnectHandler hdl);
    void OnTCPPostInit(ConnectHandler hdl);
    void OnOpen(ConnectHandler hdl);
    void OnClose(ConnectHandler hdl);
    void OnFail(ConnectHandler hdl);
    void OnPong(ConnectHandler hdl, std::string str);
    void OnPongTimeout(ConnectHandler hdl, std::string str);
    void OnMessage(ConnectHandler hdl, MessagePtr msg);
    void LogConnectionInfo(ConnectionPtr con);
    
private:
    std::shared_ptr<WebsocketClient> clientRef_;
    ConnectHandler hdl_;
    ConnectionPtr con_;
    std::string url_;
    
    int64_t startConnectTime_ = 0;
    int64_t lastSentPingTime_ = 0;
    int64_t lastReceivedPongTime_ = 0;
    int64_t lastReceivedTime_ = 0;
};

};


#endif /* !WEB_SOCKET_H */
