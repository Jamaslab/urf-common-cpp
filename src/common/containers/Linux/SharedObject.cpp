#include "urf/common/containers/SharedObject.hpp"

#include <cstring>
#include <iostream>

#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

namespace urf {
namespace common {
namespace containers {

SharedObject::SharedObject(const std::string& name, uint32_t fileSize) :
    filename_("/tmp/"+name+".sobj"),
    stringStart_(),
    filesize_(fileSize),
    fd_(0),
    mmapMutex_(),
    completeMmap_() {
        fd_ = ::open(filename_.c_str(), O_RDWR | O_NONBLOCK, 0666);

        bool created = false;
        if (fd_ == -1) {
            std::cout << "I have to create it" << std::endl;
            fd_ = ::open(filename_.c_str(), O_RDWR |  O_CREAT | O_TRUNC | O_NONBLOCK, 0666);

            if (ftruncate(fd_, filesize_ + sizeof(mmap_mutex_t)) == -1) {
                throw std::runtime_error(std::strerror(errno));
            }
            created = true;
        }

        completeMmap_ = mmap(NULL, filesize_ + sizeof(mmap_mutex_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd_, 0);
        mmapMutex_ = reinterpret_cast<mmap_mutex_t*>(completeMmap_);
        stringStart_ = reinterpret_cast<char*>(completeMmap_) + sizeof(mmap_mutex_t);

        if (pthread_mutex_lock(&mmapMutex_->ipc_mutex) == -1) {
            created = true;
        } else {
            pthread_mutex_unlock(&mmapMutex_->ipc_mutex);
        }

        if (created) {
            std::memset(completeMmap_, 0, filesize_ + sizeof(mmap_mutex_t));

            pthread_mutexattr_t mutex_attr;
            pthread_mutexattr_init(&mutex_attr);
            pthread_mutexattr_setpshared(&mutex_attr, PTHREAD_PROCESS_SHARED);
            pthread_mutex_init(&mmapMutex_->ipc_mutex, &mutex_attr);
        }
}

SharedObject::~SharedObject() {
    unlock();
    ::close(fd_);
}

bool SharedObject::write(const std::string& bytes, uint32_t offset) {
    if (bytes.length() + offset > filesize_) {
        return false;
    }

    std::memset(stringStart_+offset+bytes.length(), 0, filesize_-offset-bytes.length());
    std::memcpy(stringStart_+offset, bytes.c_str(), bytes.length());
    return true;
}

std::string SharedObject::read(uint32_t length) {
    if (length > filesize_) {
        return std::string();
    }

    if (length == 0) {
        return std::string(stringStart_);
    }
    return std::string(stringStart_, length);
}

bool SharedObject::lock() {
    return pthread_mutex_lock(&mmapMutex_->ipc_mutex) != -1;
}

bool SharedObject::unlock() {
    return pthread_mutex_unlock(&mmapMutex_->ipc_mutex) != -1;
}

}  // namespace containers
}  // namespace common
}  // namespace urf
