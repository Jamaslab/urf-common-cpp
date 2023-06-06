#include <gtest/gtest.h>
#include <urf/common/events/events.hpp>

using namespace urf::common::events;

TEST(EventTests, correctlyEmitAndReceiveSynchronousEvents) {
    event<int> e;
    int received = 0;
    e.subscribe([&received](int i) { received = i; }, event_policy::synchronous);
    e.emit(42);
    ASSERT_EQ(received, 42);
}

TEST(EventTests, correctlySubscribeMultipleTimesSynchronous) {
    event<int> e;
    int received = 0;
    e.subscribe([&received](int i) { received += i; }, event_policy::synchronous);
    e.subscribe([&received](int i) { received += i; }, event_policy::synchronous);
    e.emit(42);
    ASSERT_EQ(received, 84);
}

TEST(EventTests, correctlyReceiveAsynchronousEvents) {
    event<int> e;
    int received = 0;
    e.subscribe([&received](int i) { received = i; }, event_policy::asynchronous);
    e.emit(42);
    ASSERT_EQ(received, 0);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    ASSERT_EQ(received, 42);
}

TEST(EventTests, correctlySubscribeMultipleTimesAsynchronous) {
    event<int> e;
    int received = 0;
    e.subscribe([&received](int i) { received += i; }, event_policy::asynchronous);
    e.subscribe([&received](int i) { received += i; }, event_policy::asynchronous);
    e.emit(42);
    ASSERT_EQ(received, 0);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    ASSERT_EQ(received, 84);
}

TEST(EventTests, correctlySubscribeMultipleTimesMixed) {
    event<int> e;
    int received = 0;
    e.subscribe([&received](int i) { received += i; }, event_policy::asynchronous);
    e.subscribe([&received](int i) { received += i; }, event_policy::synchronous);
    e.emit(42);
    ASSERT_EQ(received, 42);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    ASSERT_EQ(received, 84);
}

TEST(EventTests, correcltyEmitMultipleTemplateArgs) {
    event<int, int> e;
    int received = 0;
    e.subscribe([&received](int i, int j) { received = i + j; }, event_policy::synchronous);
    e.emit(42, 42);
    ASSERT_EQ(received, 84);
}

TEST(EventTests, useOperatorToSubscribe) {
    event<int> e;
    int received = 0;
    e += [&received](int i) { received = i; };
    e.emit(42);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    ASSERT_EQ(received, 42);
}

TEST(EventTests, copyConstructor) {
    event<int> e;
    int received = 0;
    e += [&received](int i) { received = i; };
    event<int> e2(e);
    e2.emit(42);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    ASSERT_EQ(received, 42);
}

TEST(EventTests, moveConstructor) {
    event<int> e;
    int received = 0;
    e += [&received](int i) { received = i; };
    event<int> e2(std::move(e));
    e2.emit(42);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    ASSERT_EQ(received, 42);
}

TEST(EventTests, copyAssignment) {
    event<int> e;
    int received = 0;
    e += [&received](int i) { received = i; };
    event<int> e2;
    e2 = e;
    e2.emit(42);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    ASSERT_EQ(received, 42);
}

TEST(EventTests, moveAssignment) {
    event<int> e;
    int received = 0;
    e += [&received](int i) { received = i; };
    event<int> e2;
    e2 = std::move(e);
    e2.emit(42);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    ASSERT_EQ(received, 42);
}

TEST(EventTests, ensureNoCrashOnDestruction) {
    event<int> e;
    int received = 0;
    {
        std::function<void(int)> f = [&received](int i) { received = i; };
        e.subscribe(f, event_policy::synchronous);
    }
    e.emit(42);
    ASSERT_EQ(received, 42);
}

TEST(EventTests, noCrashIfEventDestructed) {
    int received = 0;
    {
        event<int> e;
        e.subscribe([&received](int i) { received = i; }, event_policy::asynchronous);
        e.emit(42);
    }
}

TEST(EventTests, boolOperator) {
    event<int> e;
    int received = 0;
    e += [&received](int i) { received = i; };
    ASSERT_TRUE(e);
    e.emit(42);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    ASSERT_EQ(received, 42);
    e = event<int>();
    ASSERT_FALSE(e);
}