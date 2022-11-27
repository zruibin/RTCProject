/*
 * signaling_manager.h
 *
 * Created by Ruibin.Chow on 2022/11/21.
 * Copyright (c) 2022年 Ruibin.Chow All rights reserved.
 */

#ifndef SIGNALING_MANAGER_H
#define SIGNALING_MANAGER_H

#include <memory>
#include <mutex>
#include <functional>
#include "engine/signalings/signaling.h"
#include "engine/signalings/signaling_socket.h"
#include "log/logging.h"

namespace engine {

void testSocket();

class SignalingManager final {

public:
    ~SignalingManager() = default;
    static std::shared_ptr<SignalingManager>& SharedInstance() {
        static std::shared_ptr<SignalingManager> instance_;
        static std::once_flag flag;
        std::call_once(flag, [](){
            instance_.reset(new SignalingManager);
        });
        return instance_;
    }
    
    void Init();
    void GetRouterRtpCapabilities(std::function<void(int32_t code,
                                                     const std::string& msg,
                                                     std::shared_ptr<GetRouterRtpCapabilitiesResponse> response)>);
  
protected:
    explicit SignalingManager();
    
private:
    template<typename Request, typename Response>
    void Send(Request request,
              std::function<void(int32_t,
                                 const std::string&,
                                 std::shared_ptr<Response>)> callback) {
        RequestMethod method = EnumMethodFromString<RequestMethod>(request.method.value());
        if (method == RequestMethod::None) {
            Log(ERROR) << "Send RequestMethod Was None.";
            return;
        }
        
        if (signalingSocketRef_ == nullptr) {
            Log(ERROR) << "signalingSocketRef Was nullptr.";
            return;
        }
        
        signalingSocketRef_->Send(request.ToJsonString(), request.id.value(), [callback](const std::string& json){
            if (json.empty()) {
                return;
            }
            std::shared_ptr<Response> response = std::make_shared<Response>();
            response->FromJsonString(json);
            if (callback) {
                callback(0, "", response);
            }
        }, [callback](int32_t errorCode, const std::string& errorInfo){
            if (callback) {
                callback(errorCode, errorInfo, nullptr);
            }
        });
    }
private:
    std::shared_ptr<SignalingSocket> signalingSocketRef_;
};

}



#endif /* !SIGNALING_MANAGER_H */
