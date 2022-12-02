/*
 *
 * unit_test.cc
 *
 * Created by Ruibin.Chow on 2022/11/30.
 * Copyright (c) 2022年 Ruibin.Chow All rights reserved.
 */

#include <string>
#include <iostream>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <httplib/httplib.h>
#include "util/timer.h"


class FooInterface {
public:
    virtual ~FooInterface() {}

public:
    virtual std::string getArbitraryString() = 0;
};

class MockFoo: public FooInterface {
public:
    MOCK_METHOD0(getArbitraryString, std::string());
};

TEST(MockFooTest, mockFooTest) {
    using ::testing::AtLeast;
    using ::testing::Return;
    std::string value = "Hello World!";
    MockFoo mockFoo;
    EXPECT_CALL(mockFoo, getArbitraryString()).Times(1).
    WillOnce(Return(value));
    std::string returnValue = mockFoo.getArbitraryString();
    std::cout << "Returned Value: " << returnValue << std::endl;
}



TEST(UnitTest, HttplibTest) {
    httplib::Client cli("http://www.baidu.com");
    if (auto res = cli.Get("/")) {
        std::cout << res->status << std::endl;
        std::cout << res->get_header_value("Content-Type") << std::endl;
//        std::cout << res->body << std::endl;
    } else {
        std::cout << "error code: " << httplib::to_string(res.error())
                    << std::endl;
    }
}

TEST(UnitTest, TimerTest) {
    util::AsynTimer::Detach([](void* pUser) {
        std::cout << "Detach timer no repeate." << std::endl;
    }, 1*TIME_NSEC_PER_SEC, false, nullptr);
    
    int runningTime = 0;
    util::AsynTimer::Detach([&runningTime](void* pUser) {
        runningTime += 4;
        std::cout << "Detach timer has been running for "
                    << runningTime << " second(s)." << std::endl;
    }, 4*TIME_NSEC_PER_SEC, true, nullptr);
    
    util::Timer::Sleep(30*TIME_NSEC_PER_SEC); // must
}
