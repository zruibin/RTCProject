/*
 * notification_center.h
 *
 * Created by Ruibin.Chow on 2022/11/11.
 * Copyright (c) 2022年 Ruibin.Chow All rights reserved.
 */

#ifndef NOTIFICATION_CENTER_H
#define NOTIFICATION_CENTER_H

#include <string>
#include <unordered_map>
#include <vector>

namespace foundation {

using Observer = void;

struct Notification final {
    std::string name;
    Observer* object;
    std::string message;
};

using NotificationRef = std::shared_ptr<Notification>;

class NotificationCenter final {
    
public:
    ~NotificationCenter() = default;

public:
    using Selector = std::function<void(NotificationRef)>;
    using String = std::string;
    using SelectorMapRef = std::shared_ptr<std::unordered_map<String, Selector>>;
    using MapRef = std::shared_ptr<std::unordered_map<String, SelectorMapRef>>;
    using VectorRef = std::shared_ptr<std::vector<Selector>>;
    using VectorMapRef = std::shared_ptr<std::unordered_map<String, VectorRef>>;
    
public:
    static std::shared_ptr<NotificationCenter> DefaultCenter();
    
    void AddObserver(const Observer* observer,
                     const String& aName,
                     Selector selector);
    void PostNotification(const String& aName,
                          NotificationRef object);
    void PostNotification(const String& aName);
    void RemoveObserver(const Observer* observer,
                        const String& aName);
    void RemoveObserver(const Observer* observer);
    
    void AddNotification(const String& aName,
                         Selector selector);
    void RemoveNotification(const String& aName);

private:
    NotificationCenter();
    NotificationCenter(NotificationCenter&) = default;
    static std::shared_ptr<NotificationCenter> _DefaultCenter();

private:
    MapRef notificationMap_;
    VectorMapRef vectorMap_;
};

}

#endif /* !NOTIFICATION_CENTER_H */
