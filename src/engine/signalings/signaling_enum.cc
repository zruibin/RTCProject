/*
 *
 * signaling_enum.cc
 *
 * Created by Ruibin.Chow on 2022/11/23.
 * Copyright (c) 2022年 Ruibin.Chow All rights reserved.
 */

#include "signaling_enum.h"
#include <magic_enum/magic_enum.hpp>

namespace engine {

std::string NotificationMethodToOriginString(NotificationMethod method) {
    auto methodName = magic_enum::enum_name(method);
    std::string enumString(methodName);
    return enumString;
}

std::string NotificationMethodToString(NotificationMethod method) {
    std::string enumString = NotificationMethodToOriginString(method);
    if (enumString.size() > 0) {
        enumString[0] = tolower(enumString[0]);
    }
    return enumString;
}

}


