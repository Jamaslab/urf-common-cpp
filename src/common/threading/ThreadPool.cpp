#include "urf/common/threading/ThreadPool.hpp"

namespace urf {
namespace common {
namespace threading {

ThreadPool::ThreadPool(const concurrency_t thread_count)
    : thread_count_(determineThreadCount(thread_count))
    , threads(std::make_unique<std::thread[]>(thread_count_)) {
    createThreads();
}

ThreadPool::~ThreadPool() {
    waitForTasks();
    destroyThreads();
}

concurrency_t ThreadPool::getThreadCount() const {
    return thread_count_;
}

void ThreadPool::createThreads() {
    running = true;
    for (concurrency_t i = 0; i < thread_count_; ++i) {
        threads[i] = std::thread(&ThreadPool::worker, this);
    }
}

void ThreadPool::destroyThreads() {
    running = false;
    task_available_cv.notify_all();
    for (concurrency_t i = 0; i < thread_count_; ++i) {
        threads[i].join();
    }
}

concurrency_t ThreadPool::determineThreadCount(const concurrency_t thread_count_) {
    if (thread_count_ > 0)
        return thread_count_;
    else {
        if (std::thread::hardware_concurrency() > 0)
            return std::thread::hardware_concurrency();
        else
            return 1;
    }
}

void ThreadPool::worker() {
    while (running) {
        std::function<void()> task;
        std::unique_lock<std::mutex> tasks_lock(tasks_mutex);
        task_available_cv.wait(tasks_lock, [this] { return !tasks.empty() || !running; });
        if (running) {
            task = std::move(tasks.front());
            tasks.pop();
            tasks_lock.unlock();
            task();
            tasks_lock.lock();
            --tasks_total;
            if (waiting)
                task_done_cv.notify_one();
        }
    }
}

void ThreadPool::waitForTasks() {
    waiting = true;
    std::unique_lock<std::mutex> tasks_lock(tasks_mutex);
    task_done_cv.wait(tasks_lock, [this] { return (tasks_total == 0); });
    waiting = false;
}

} // namespace threading
} // namespace common
} // namespace urf