#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "urf/common/threading/ThreadPool.hpp"

using namespace urf::common::threading;

int addNumbers(int a, int b) {
    return a+b;
}

TEST(ThreadPoolShould, startStop) {
    ThreadPool pool;    
}

TEST(ThreadPoolShould, correctlyExecuteTask) {
    ThreadPool pool;

    auto retval = pool.submit(&addNumbers, 1, 1);

    ASSERT_EQ(retval.get(), 2);
}

TEST(ThreadPoolShould, executeStdFunction) {
    ThreadPool pool;
    
    int result = 0;
    auto f = [&result](int a, int b) {
        result = a+b;
    };

    pool.pushTask(f, 1, 1);
    pool.waitForTasks();
    ASSERT_EQ(result, 2);
}