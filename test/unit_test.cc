/*
 *
 * unit_test.cc
 *
 * Created by Ruibin.Chow on 2022/11/30.
 * Copyright (c) 2022年 Ruibin.Chow All rights reserved.
 */

#include <string>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "engine/signalings/signaling.h"


// Demonstrate some basic assertions.
TEST(HelloTest, BasicAssertions) {
    // Expect two strings not to be equal.
    EXPECT_STRNE("hello", "world");
    // Expect equality.
    EXPECT_EQ(7 * 6, 42);
}

TEST(sample_test_case, sample_test) {
    EXPECT_EQ(1, 1);
}


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



