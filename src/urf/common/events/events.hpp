#pragma once

#include "urf/common/threading/ThreadPool.hpp"

#include <functional>
#include <memory>

namespace urf {
namespace common {
namespace events {

enum class event_policy { synchronous, asynchronous };

class event_base {
 public:
    virtual ~event_base() = default;

 protected:
    static threading::ThreadPool _threadPool;
};

template <typename... Args>
class event : public event_base {
 public:
    event() = default;
    event(const event&);
    event(event&&) noexcept;
    ~event() override;

    void emit(const Args&... args);
    void subscribe(const std::function<void(Args...)>& callback,
                   event_policy policy = event_policy::asynchronous);

    explicit operator bool() const noexcept;
    void operator+=(const std::function<void(Args...)>& callback);

    event& operator=(const event&);
    event& operator=(event&&) noexcept;

 private:
    mutable std::mutex _mutex;
    std::vector<std::tuple<std::function<void(Args...)>, event_policy>> _callbacks;
};

template <typename... Args>
event<Args...>::~event() {
    _threadPool.waitForTasks();
}

template <typename... Args>
event<Args...>::event(const event& other) {
    std::lock_guard<std::mutex> lock(other._mutex);
    _callbacks = other._callbacks;
}

template <typename... Args>
event<Args...>::event(event&& other) noexcept {
    std::lock_guard<std::mutex> lock(other._mutex);
    _callbacks = std::move(other._callbacks);
}

template <typename... Args>
void event<Args...>::emit(const Args&... args) {
    std::lock_guard<std::mutex> lock(_mutex);
    for (auto& callback : _callbacks) {
        if (std::get<1>(callback) == event_policy::synchronous) {
            std::get<0>(callback)(args...);
        } else {
            _threadPool.pushTask(std::get<0>(callback), args...);
        }
    }
}

template <typename... Args>
void event<Args...>::subscribe(const std::function<void(Args...)>& callback, event_policy policy) {
    std::lock_guard<std::mutex> lock(_mutex);
    _callbacks.emplace_back(callback, policy);
}

template <typename... Args>
event<Args...>::operator bool() const noexcept {
    std::lock_guard<std::mutex> lock(_mutex);
    return !_callbacks.empty();
}

template <typename... Args>
void event<Args...>::operator+=(const std::function<void(Args...)>& callback) {
    subscribe(callback);
}

template <typename... Args>
event<Args...>& event<Args...>::operator=(const event& other) {
    std::lock_guard<std::mutex> lock(other._mutex);
    _callbacks = other._callbacks;
    return *this;
}

template <typename... Args>
event<Args...>& event<Args...>::operator=(event&& other) noexcept {
    std::lock_guard<std::mutex> lock(other._mutex);
    _callbacks = std::move(other._callbacks);
    return *this;
}

} // namespace events
} // namespace common
} // namespace urf