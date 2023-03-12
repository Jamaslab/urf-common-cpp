#pragma once

#if defined(_WIN32) || defined(_WIN64)
#    include "urf/common/urf_common_export.h"
#else
#    define URF_COMMON_EXPORT
#endif

#include <algorithm>
#include <iostream>
#include <iterator>
#include <memory>
#include <mutex>
#include <type_traits>

namespace urf {
namespace common {
namespace containers {

template <class T>
class URF_COMMON_EXPORT vector {
 public:
    struct iterator {
        typedef std::bidirectional_iterator_tag iterator_category;
        typedef std::ptrdiff_t difference_type;
        typedef T value_type;
        typedef T* pointer;
        typedef T& reference;

        iterator(pointer ptr)
            : ptr_(ptr) { }

        reference operator*() const {
            return *ptr_;
        }

        pointer operator->() {
            return ptr_;
        }

        // Prefix increment
        iterator& operator++() {
            ptr_++;
            return *this;
        }

        iterator& operator+(int value) {
            ptr_ += value;
            return *this;
        }

        iterator& operator-(int value) {
            ptr_ -= value;
            return *this;
        }

        // Postfix increment
        iterator operator++(int) {
            iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        // Prefix increment
        iterator& operator--() {
            ptr_--;
            return *this;
        }

        // Postfix increment
        iterator operator--(int) {
            iterator tmp = *this;
            --(this);
            return tmp;
        }

        bool operator==(const iterator& a) {
            return ptr_ == a.ptr_;
        }

        bool operator!=(const iterator& a) {
            return ptr_ != a.ptr_;
        }

        difference_type operator-(const iterator& a) {
            return ptr_ - a.ptr_;
        }

     private:
        pointer ptr_;
    };

    vector();
    explicit vector(size_t count);
    vector(size_t count, const T& value);
    vector(T* data, size_t size);
    vector(std::initializer_list<T> init);
    template <class InputIt,
              typename std::enable_if<
                  std::is_same<typename std::iterator_traits<InputIt>::value_type, T>::value,
                  int>::type = 0>
    vector(InputIt first, InputIt last);
    vector(const vector<T>&);
    vector(vector<T>&&);
    ~vector();

    vector<T> clone() const;
    T* transfer();
    bool transferred() const;
    size_t use_count() const;

    /** Elements access **/
    T& at(size_t index);
    const T& at(size_t index) const;
    T& operator[](size_t index);
    const T& operator[](size_t index) const;
    T& front();
    const T& front() const;
    T& back();
    const T& back() const;
    T* data();
    const T* data() const;

    /** Capacity **/
    bool empty() const;
    size_t size() const;
    size_t capacity() const;
    void reserve(size_t value);
    void shrink_to_fit();

    /** Modifiers **/
    void clear();
    void pop_back();
    void push_back(const T& value);
    void push_back(T&& value);
    void resize(size_t count);
    void resize(size_t count, const T& value);

    vector<T>& operator=(const vector<T>& other);
    vector<T>& operator=(vector<T>&& other);
    vector<T>& operator=(std::initializer_list<T> ilist);

    template <class K>
    friend bool operator==(const vector<K>& lhs, const vector<K>& rhs);
    template <class K>
    friend bool operator!=(const vector<K>& lhs, const vector<K>& rhs);

    iterator begin() {
        std::scoped_lock lock(handler_->handlerMutex_);
        if (handler_->transferred)
            throw std::runtime_error("Invalid operation on transferred vector");

        if (handler_->size == 0)
            throw std::out_of_range("Index out of range");

        return iterator(&handler_->data[0]);
    }

    const iterator begin() const {
        std::scoped_lock lock(handler_->handlerMutex_);
        if (handler_->transferred)
            throw std::runtime_error("Invalid operation on transferred vector");

        if (handler_->size == 0)
            throw std::out_of_range("Index out of range");

        return iterator(&handler_->data[0]);
    }

    iterator end() {
        std::scoped_lock lock(handler_->handlerMutex_);
        if (handler_->transferred)
            throw std::runtime_error("Invalid operation on transferred vector");

        if (handler_->size == 0)
            throw std::out_of_range("Index out of range");

        return iterator(&handler_->data[handler_->size]);
    }

    const iterator end() const {
        std::scoped_lock lock(handler_->handlerMutex_);
        if (handler_->transferred)
            throw std::runtime_error("Invalid operation on transferred vector");

        if (handler_->size == 0)
            throw std::out_of_range("Index out of range");

        return iterator(&handler_->data[handler_->size]);
    }

    typename iterator::difference_type distance(iterator first, iterator last) {
        return last - first;
    }

    iterator insert(const iterator& pos, const T& value) {
        return insert(pos, 1, value);
    }

    iterator insert(const iterator& pos, size_t count, const T& value) {
        std::scoped_lock lock(handler_->handlerMutex_);
        if (handler_->transferred)
            throw std::runtime_error("Invalid operation on transferred vector");

        auto offset = std::distance(iterator(&handler_->data[0]), pos);
        if (handler_->size + count > handler_->capacity) {
            size_t newCapacity = static_cast<size_t>((handler_->capacity + count) * 1.61);
            T* newBuf = new T[newCapacity];
            T* oldBuf = handler_->data;
            if (oldBuf) {
                std::copy(oldBuf, oldBuf + offset, newBuf);
                std::copy_backward(
                    oldBuf + offset, oldBuf + handler_->size, newBuf + handler_->size + count);
                std::fill(newBuf + offset, newBuf + offset + count, value);
                delete[] oldBuf;
            }

            handler_->capacity = newCapacity;
            handler_->data = newBuf;
            handler_->size += count;
        } else {
            std::copy_backward(handler_->data + offset,
                               handler_->data + handler_->size,
                               handler_->data + handler_->size + count);
            std::fill(handler_->data + offset, handler_->data + offset + count, value);
            handler_->size += count;
        }
        return iterator(handler_->data + offset);
    }

    template <class InputIt,
              typename std::enable_if<
                  std::is_same<typename std::iterator_traits<InputIt>::value_type, T>::value,
                  int>::type = 0>
    iterator insert(const iterator& pos, InputIt start, InputIt stop) {
        std::scoped_lock lock(handler_->handlerMutex_);
        if (handler_->transferred)
            throw std::runtime_error("Invalid operation on transferred vector");

        auto offset = std::distance(iterator(&handler_->data[0]), pos);
        auto newValuesCount = std::distance(start, stop);

        if (handler_->size + newValuesCount > handler_->capacity) {
            size_t newCapacity = static_cast<size_t>((handler_->capacity + newValuesCount) * 1.61);
            T* newBuf = new T[newCapacity];
            T* oldBuf = handler_->data;
            if (oldBuf) {
                std::copy(oldBuf, oldBuf + offset, newBuf);
                std::copy_backward(oldBuf + offset,
                                   oldBuf + handler_->size,
                                   newBuf + handler_->size + newValuesCount);
                std::copy(start, stop, newBuf + offset);
                delete[] oldBuf;
            }

            handler_->capacity = newCapacity;
            handler_->data = newBuf;
            handler_->size += newValuesCount;
        } else {
            std::copy_backward(handler_->data + offset,
                               handler_->data + handler_->size,
                               handler_->data + handler_->size + newValuesCount);
            std::copy(start, stop, pos);
            handler_->size += newValuesCount;
        }
        return iterator(handler_->data + offset);
    }

    iterator insert(const iterator& pos, std::initializer_list<T> ilist) {
        return insert(pos, ilist.begin(), ilist.end());
    }

 private:
    void reallocate(size_t size);

 private:
    typedef struct vector_handler_t {
        std::mutex handlerMutex_;
        bool transferred;
        size_t size;
        size_t capacity;
        T* data;
    } vector_handler;

    std::shared_ptr<vector_handler> handler_;
};

template <class T>
vector<T>::vector()
    : handler_(new vector_handler) {
    handler_->transferred = false;
    handler_->size = 0;
    handler_->capacity = 0;
    handler_->data = nullptr;
}

template <class T>
vector<T>::vector(size_t count)
    : vector() {
    handler_->size = count;
    handler_->capacity = count;
    handler_->data = new T[count];
}

template <class T>
vector<T>::vector(size_t count, const T& value)
    : vector() {
    handler_->size = count;
    handler_->capacity = count;
    handler_->data = new T[count];

    std::fill(handler_->data, handler_->data + count, value);
}

template <class T>
vector<T>::vector(T* data, size_t size)
    : vector() {
    handler_->size = size;
    handler_->capacity = size;
    handler_->data = data;
}

template <class T>
vector<T>::vector(std::initializer_list<T> init)
    : vector(init.begin(), init.end()) { }

template <class T>
template <class InputIt,
          typename std::enable_if<
              std::is_same<typename std::iterator_traits<InputIt>::value_type, T>::value,
              int>::type>
vector<T>::vector(InputIt first, InputIt last)
    : vector(std::distance(first, last)) {
    int i = 0;

    std::copy(first, last, handler_->data);
}

template <class T>
vector<T>::vector(const vector<T>& other)
    : handler_(other.handler_) { }

template <class T>
vector<T>::vector(vector<T>&& other)
    : handler_(std::move(other.handler_)) { }

template <class T>
vector<T>::~vector() {
    if (!handler_)
        return;

    {
        std::scoped_lock lock(handler_->handlerMutex_);
        if ((handler_.use_count() == 1) && (!handler_->transferred)) {
            if (handler_->data) {
                delete[] handler_->data;
            }
        }
    }
}

template <class T>
vector<T> vector<T>::clone() const {
    std::scoped_lock lock(handler_->handlerMutex_);
    if (handler_->transferred)
        throw std::runtime_error("Invalid operation on transferred vector");

    vector<T> retval(handler_->size);
    std::copy(handler_->data, handler_->data + handler_->size, retval.handler_->data);
    return retval;
}

template <class T>
T* vector<T>::transfer() {
    std::scoped_lock lock(handler_->handlerMutex_);
    if (handler_->transferred)
        throw std::runtime_error("Invalid operation on transferred vector");

    handler_->transferred = true;
    return handler_->data;
}

template <class T>
bool vector<T>::transferred() const {
    std::scoped_lock lock(handler_->handlerMutex_);
    return handler_->transferred;
}

template <class T>
size_t vector<T>::use_count() const {
    std::scoped_lock lock(handler_->handlerMutex_);
    if (handler_->transferred)
        throw std::runtime_error("Invalid operation on transferred vector");

    return handler_.use_count();
}

template <class T>
T& vector<T>::at(size_t index) {
    std::scoped_lock lock(handler_->handlerMutex_);
    if (handler_->transferred)
        throw std::runtime_error("Invalid operation on transferred vector");

    if (index >= handler_->size)
        throw std::out_of_range("Index out of range");

    return handler_->data[index];
}

template <class T>
const T& vector<T>::at(size_t index) const {
    std::scoped_lock lock(handler_->handlerMutex_);
    if (handler_->transferred)
        throw std::runtime_error("Invalid operation on transferred vector");

    if (index >= handler_->size)
        throw std::out_of_range("Index out of range");

    return handler_->data[index];
}

template <class T>
T& vector<T>::operator[](size_t index) {
    std::scoped_lock lock(handler_->handlerMutex_);
    if (handler_->transferred)
        throw std::runtime_error("Invalid operation on transferred vector");

    if (index >= handler_->size)
        throw std::out_of_range("Index out of range");

    return handler_->data[index];
}

template <class T>
const T& vector<T>::operator[](size_t index) const {
    std::scoped_lock lock(handler_->handlerMutex_);
    if (handler_->transferred)
        throw std::runtime_error("Invalid operation on transferred vector");

    if (index >= handler_->size)
        throw std::out_of_range("Index out of range");

    return handler_->data[index];
}

template <class T>
T& vector<T>::front() {
    std::scoped_lock lock(handler_->handlerMutex_);
    if (handler_->transferred)
        throw std::runtime_error("Invalid operation on transferred vector");

    if (handler_->size == 0)
        throw std::out_of_range("Index out of range");

    return handler_.data[0];
}

template <class T>
const T& vector<T>::front() const {
    std::scoped_lock lock(handler_->handlerMutex_);
    if (handler_->transferred)
        throw std::runtime_error("Invalid operation on transferred vector");

    if (handler_->size == 0)
        throw std::out_of_range("Index out of range");

    return handler_.data[0];
}

template <class T>
T& vector<T>::back() {
    std::scoped_lock lock(handler_->handlerMutex_);
    if (handler_->transferred)
        throw std::runtime_error("Invalid operation on transferred vector");

    if (handler_->size == 0)
        throw std::out_of_range("Index out of range");

    return handler_.data[handler_->size - 1];
}

template <class T>
const T& vector<T>::back() const {
    std::scoped_lock lock(handler_->handlerMutex_);
    if (handler_->transferred)
        throw std::runtime_error("Invalid operation on transferred vector");

    if (handler_->size == 0)
        throw std::out_of_range("Index out of range");

    return handler_.data[handler_->size - 1];
}

template <class T>
T* vector<T>::data() {
    std::scoped_lock lock(handler_->handlerMutex_);
    if (handler_->transferred)
        throw std::runtime_error("Invalid operation on transferred vector");

    return handler_->data;
}

template <class T>
const T* vector<T>::data() const {
    std::scoped_lock lock(handler_->handlerMutex_);
    if (handler_->transferred)
        throw std::runtime_error("Invalid operation on transferred vector");

    return handler_->data;
}

/** Capacity **/
template <class T>
bool vector<T>::empty() const {
    std::scoped_lock lock(handler_->handlerMutex_);
    if (handler_->transferred)
        throw std::runtime_error("Invalid operation on transferred vector");

    return handler_->size == 0;
}

template <class T>
size_t vector<T>::size() const {
    std::scoped_lock lock(handler_->handlerMutex_);
    if (handler_->transferred)
        throw std::runtime_error("Invalid operation on transferred vector");

    return handler_->size;
}

template <class T>
size_t vector<T>::capacity() const {
    std::scoped_lock lock(handler_->handlerMutex_);
    if (handler_->transferred)
        throw std::runtime_error("Invalid operation on transferred vector");

    return handler_->capacity;
}

template <class T>
void vector<T>::reserve(size_t value) {
    std::scoped_lock lock(handler_->handlerMutex_);
    if (handler_->transferred)
        throw std::runtime_error("Invalid operation on transferred vector");

    if (value <= handler_->capacity)
        return;

    reallocate(value);
}

template <class T>
void vector<T>::shrink_to_fit() {
    std::scoped_lock lock(handler_->handlerMutex_);
    if (handler_->transferred)
        throw std::runtime_error("Invalid operation on transferred vector");

    if (handler_->capacity == handler_->size())
        return;

    if (handler_->size == 0)
        return;

    reallocate(handler_->size);
}

template <class T>
void vector<T>::clear() {
    std::scoped_lock lock(handler_->handlerMutex_);
    if (handler_->transferred)
        throw std::runtime_error("Invalid operation on transferred vector");
    handler_->size = 0;
}

template <class T>
void vector<T>::pop_back() {
    std::scoped_lock lock(handler_->handlerMutex_);
    if (handler_->transferred)
        throw std::runtime_error("Invalid operation on transferred vector");

    if (handler_->size == 0)
        throw std::runtime_error("Invalid operation on empty vector");

    handler_->size--;
}

template <class T>
void vector<T>::push_back(const T& value) {
    std::scoped_lock lock(handler_->handlerMutex_);
    if (handler_->transferred)
        throw std::runtime_error("Invalid operation on transferred vector");

    if (handler_->size == handler_->capacity) {
        reallocate(handler_->capacity == 0 ? 10 : static_cast<size_t>(handler_->capacity * 1.61));
    }

    handler_->data[handler_->size] = value;
    handler_->size++;
}

template <class T>
void vector<T>::push_back(T&& value) {
    std::scoped_lock lock(handler_->handlerMutex_);
    if (handler_->transferred)
        throw std::runtime_error("Invalid operation on transferred vector");

    if (handler_->size == handler_->capacity) {
        reallocate(handler_->capacity == 0 ? 10 : static_cast<size_t>(handler_->capacity * 1.61));
    }

    handler_->data[handler_->size] = std::move(value);
    handler_->size++;
}

template <class T>
void vector<T>::resize(size_t count) {
    std::scoped_lock lock(handler_->handlerMutex_);
    if (handler_->transferred)
        throw std::runtime_error("Invalid operation on transferred vector");

    if (handler_->capacity < count) {
        reallocate(count);
    }
    handler_->size = count;
}

template <class T>
void vector<T>::resize(size_t count, const T& value) {
    std::scoped_lock lock(handler_->handlerMutex_);
    if (handler_->transferred)
        throw std::runtime_error("Invalid operation on transferred vector");

    if (handler_->capacity < count) {
        reallocate(count);
    }

    if (handler_->size < count) {
        std::fill(handler_->data + handler_->size, handler_->data + count, value);
    }
    handler_->size = count;
}

template <class T>
vector<T>& vector<T>::operator=(const vector<T>& other) {
    std::scoped_lock lock(handler_->handlerMutex_);
    if ((handler_.use_count() == 1) && (!handler_->transferred)) {
        if (handler_->data) {
            delete[] handler_->data;
        }
    }

    handler_ = other.handler_;
    return *this;
}

template <class T>
vector<T>& vector<T>::operator=(vector<T>&& other) {
    std::scoped_lock lock(handler_->handlerMutex_);
    if ((handler_.use_count() == 1) && (!handler_->transferred)) {
        if (handler_->data) {
            delete[] handler_->data;
        }
    }

    handler_ = std::move(other.handler_);
    return *this;
}

template <class T>
vector<T>& vector<T>::operator=(std::initializer_list<T> ilist) {
    std::scoped_lock lock(handler_->handlerMutex_);
    reallocate(ilist.size());

    std::copy(ilist.begin(), ilist.end(), handler_->data);
    handler_->size = ilist.size();
    return *this;
}

template <class T>
void vector<T>::reallocate(size_t size) {
    T* newBuf = new T[size];
    T* oldBuf = handler_->data;
    if (oldBuf) {
        std::copy(oldBuf, oldBuf + size, newBuf);
        delete[] oldBuf;
    }

    handler_->capacity = size;
    handler_->data = newBuf;
}

template <class K>
bool operator==(const vector<K>& lhs, const vector<K>& rhs) {
    std::scoped_lock lock_lhs(lhs.handler_->handlerMutex_);
    std::scoped_lock lock_rhs(rhs.handler_->handlerMutex_);

    if (lhs.handler_->size != rhs.handler_->size) {
        return false;
    }

    for (size_t i = 0; i < lhs.handler_->size; i++) {
        if (lhs.handler_->data[i] != rhs.handler_->data[i]) {
            return false;
        }
    }

    return true;
}

template <class K>
bool operator!=(const vector<K>& lhs, const vector<K>& rhs) {
    return !(lhs == rhs);
}

} // namespace containers
} // namespace common
} // namespace urf
