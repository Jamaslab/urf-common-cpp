#include <chrono>
#include <thread>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <urf/common/containers/SharedObject.hpp>

using urf::common::containers::SharedObject;

TEST(SharedObjectShould, correctlyShare) {
    SharedObject obj1("test_object", 1024);
    SharedObject obj2("test_object", 1024);

    ASSERT_TRUE(obj1.lock());
    ASSERT_TRUE(obj1.write("testtest"));
    ASSERT_TRUE(obj1.unlock());

    ASSERT_TRUE(obj2.lock());
    ASSERT_EQ(obj2.read(), "testtest");
    ASSERT_TRUE(obj2.unlock());
}
