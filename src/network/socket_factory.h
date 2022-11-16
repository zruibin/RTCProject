/*
 * socket_factory.h
 *
 * Created by Ruibin.Chow on 2022/11/16.
 * Copyright (c) 2022年 Ruibin.Chow All rights reserved.
 */

#ifndef SOCKET_FACTORY_H
#define SOCKET_FACTORY_H

#include "socket_interface.h"

namespace network {

std::shared_ptr<SocketInterface> CreateSocket(const std::string& url,
                                              SocketInterface::Protocol protocol);

};

#endif /* !SOCKET_FACTORY_H */
