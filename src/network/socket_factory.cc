/*
 *
 * socket_factory.cc
 *
 * Created by Ruibin.Chow on 2022/11/16.
 * Copyright (c) 2022年 Ruibin.Chow All rights reserved.
 */

#include "network/socket_factory.h"
#include "web_socket.h"

namespace network {

std::shared_ptr<SocketInterface> CreateSocket(const std::string& url,
                                              SocketInterface::Protocol protocol) {
    std::shared_ptr<SocketInterface> socket = std::make_shared<WebSocket>(url);
    socket->SetProtocol(protocol);
    return socket;
}


};

