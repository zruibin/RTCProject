/*
 *
 * notification_center.cc
 *
 * Created by Ruibin.Chow on 2022/11/11.
 * Copyright (c) 2022年 Ruibin.Chow All rights reserved.
 */

#include "notification_center.h"
#include <sstream>
#include <mutex>

namespace foundation {

inline NotificationCenter::String
GetObserverHashCode(const Observer* observer) {
    std::ostringstream oss;
    oss << observer;
    return oss.str();
}


std::shared_ptr<NotificationCenter> NotificationCenter::DefaultCenter() {
    return _DefaultCenter();
}

std::shared_ptr<NotificationCenter> NotificationCenter::_DefaultCenter() {
    static std::shared_ptr<NotificationCenter> instance_;
    static std::once_flag flag;
    std::call_once(flag, [&](){
//            instance_ = std::make_shared<NotificationCenter>();
        instance_ = std::shared_ptr<NotificationCenter>(new NotificationCenter);
    });
    return instance_;
}


NotificationCenter::NotificationCenter() {
    notificationMap_ = std::make_shared<std::unordered_map<String, SelectorMapRef>>();
    vectorMap_ = std::make_shared<std::unordered_map<String, VectorRef>>();
}

void NotificationCenter::AddObserver(const Observer* observer,
                                     const String& aName,
                                     Selector selector) {
    if (observer == nullptr) return;
    if (aName.empty()) return;
    if (selector == nullptr) return;

    String observerHashCodeKey = GetObserverHashCode(observer);
    SelectorMapRef observerMap;

    auto it = notificationMap_->find(aName);
    if (it != notificationMap_->end()) {
        observerMap = it->second;
        auto selectorIt = observerMap->find(observerHashCodeKey);
        if (selectorIt == observerMap->end()) {
            observerMap->emplace(observerHashCodeKey, selector);
        }
    } else {
        observerMap = std::make_shared<std::unordered_map<String, Selector>>();
        observerMap->emplace(observerHashCodeKey, selector);
        notificationMap_->emplace(aName, observerMap);
    }
}

void NotificationCenter::PostNotification(const String& aName,
                                          NotificationRef object) {
    if (aName.empty()) return;

    auto it = notificationMap_->find(aName);
    if (it != notificationMap_->end()) {
        SelectorMapRef observerMap = it->second;
        for (auto& [k, v] : *observerMap) {
            String observerHashCodeKey = k;
            if (observerHashCodeKey.empty()) continue;
            Selector selectors = v;
            if (selectors != nullptr) {
                selectors(object);
            }
        };
    }
    
    auto vectorIt = vectorMap_->find(aName);
    if (vectorIt != vectorMap_->end()) {
        VectorRef vectors = vectorIt->second;
        for (auto& value : *vectors) {
            Selector selectors = value;
            if (selectors != nullptr) {
                selectors(object);
            }
        }
    }
}

void NotificationCenter::PostNotification(const String& aName) {
    PostNotification(aName, nullptr);
}

void NotificationCenter::RemoveObserver(const Observer* observer,
                                        const String& aName) {
    if (observer == nullptr) return;
    if (aName.empty()) return;

    auto it = notificationMap_->find(aName);
    if (it == notificationMap_->end()) {
        return;
    }
    SelectorMapRef observerMap = it->second;
    String observerHashCodeKey = GetObserverHashCode(observer);
    auto observerIt = observerMap->find(observerHashCodeKey);
    if (observerIt != observerMap->end()) {
        observerMap->erase(observerHashCodeKey);
    }
}

void NotificationCenter::RemoveObserver(const Observer* observer) {
    if (observer == nullptr) return;
    for (auto& kv : *notificationMap_) {
        SelectorMapRef observerMap = kv.second;
        String observerHashCodeKey = GetObserverHashCode(observer);
        auto observerIt = observerMap->find(observerHashCodeKey);
        if (observerIt != observerMap->end()) {
            observerMap->erase(observerHashCodeKey);
        }
    }
}

void NotificationCenter::AddNotification(const String& aName,
                                         Selector selector) {
    if (aName.empty()) return;
    if (selector == nullptr) return;
    
    auto it = vectorMap_->find(aName);
    VectorRef vectors = nullptr;
    if (it == vectorMap_->end()) {
        vectors = std::make_shared<std::vector<Selector>>();
        vectors->push_back(selector);
        vectorMap_->emplace(aName, vectors);
    } else {
        vectors = it->second;
        vectors->push_back(selector);
    }
}

void NotificationCenter::RemoveNotification(const String& aName) {
    if (aName.empty()) return;
    
    for (auto& [k, _] : *vectorMap_) {
        if (aName == k) {
            vectorMap_->erase(k);
            break;
        }
    }
}

}
