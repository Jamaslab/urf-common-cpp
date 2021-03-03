#pragma once

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <optional>
#include <queue>

namespace urf {
namespace common {
namespace containers {

template<class T>
class ThreadSafeQueue {
 public:
    ThreadSafeQueue();
    ThreadSafeQueue(const ThreadSafeQueue&);
    ThreadSafeQueue(ThreadSafeQueue&&) = delete;
    ~ThreadSafeQueue() = default;

    void push(const T& element);
    void push(T&& element);

    std::optional<T> pop();
    std::optional<T> pop(const std::chrono::milliseconds& timeout);

    size_t size();
    void clear();
    bool empty();

    void notifyAll();

    ThreadSafeQueue& operator=(const ThreadSafeQueue&);
 private:
    std::queue<T> queue_;
    std::mutex mtx_;
    std::condition_variable cv_;

    bool notifySent_;
};

template<class T>
ThreadSafeQueue<T>::ThreadSafeQueue() :
    queue_(),
    mtx_(),
    cv_(),
    notifySent_(false) { }
template<class T>
ThreadSafeQueue<T>::ThreadSafeQueue(const ThreadSafeQueue& queue) :
    queue_(queue.queue_),
    mtx_(),
    cv_(),
    notifySent_(false) {}

template<class T>
void ThreadSafeQueue<T>::push(const T& element) {
    std::scoped_lock<std::mutex> guard(mtx_);
    queue_.push(element);
    cv_.notify_one();
}

template<class T>
void ThreadSafeQueue<T>::push(T&& element) {
    std::scoped_lock<std::mutex> guard(mtx_);
    queue_.push(std::move(element));
    cv_.notify_one();
}

template<class T>
std::optional<T> ThreadSafeQueue<T>::pop() {
    std::unique_lock<std::mutex> lock(mtx_);
    notifySent_ = false;
    if (queue_.empty()) {
        cv_.wait(lock, [this]() { return (!queue_.empty() || notifySent_); });
    }

    if (notifySent_) {
        return std::nullopt;
    }

    T elem = queue_.front();
    queue_.pop();
    return elem;
}

template<class T>
std::optional<T> ThreadSafeQueue<T>::pop(const std::chrono::milliseconds& timeout) {
std::unique_lock<std::mutex> lock(mtx_);
    notifySent_ = false;
    if (queue_.empty() && !cv_.wait_for(lock, timeout, [this]() { return (!queue_.empty() || notifySent_); })) {
        return std::nullopt;
    }

    T elem = queue_.front();
    queue_.pop();
    return elem;
}

template<class T>
size_t ThreadSafeQueue<T>::size() {
    std::scoped_lock<std::mutex> guard(mtx_);
    return queue_.size();
}

template<class T>
void ThreadSafeQueue<T>::clear() {
    std::scoped_lock<std::mutex> guard(mtx_);
    while(!queue_.empty()) queue_.pop();
}

template<class T>
bool ThreadSafeQueue<T>::empty() {
    std::scoped_lock<std::mutex> guard(mtx_);
    return queue_.empty();
}

template<class T>
void ThreadSafeQueue<T>::notifyAll() {
    std::scoped_lock<std::mutex> guard(mtx_);
    notifySent_ = true;
    cv_.notify_all();
}

template<class T>
ThreadSafeQueue<T>& ThreadSafeQueue<T>::operator=(const ThreadSafeQueue<T>& queue) {
    return *this;
}

}  // namespace containers
}  // namespace common
}  // namespace urf
