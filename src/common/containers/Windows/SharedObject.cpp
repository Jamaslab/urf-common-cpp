#include "urf/common/containers/SharedObject.hpp"

#include <iostream>

namespace urf {
namespace common {
namespace containers {

SharedObject::SharedObject(const std::string& name, uint32_t fileSize) :
    filename_("Local\\"+name),
    stringStart_(),
    filesize_(fileSize),
    hMapFile_(),
    pBuf_(),
    mtx_() {
        hMapFile_ = OpenFileMapping(
                        FILE_MAP_ALL_ACCESS,        // read/write access
                        FALSE,                      // do not inherit the name
                        filename_.c_str());         // name of mapping object


        bool created = false;
        if (hMapFile_ == NULL) {
            hMapFile_ = CreateFileMapping(
                INVALID_HANDLE_VALUE,               // use paging file
                NULL,                               // default security
                PAGE_READWRITE,                     // read/write access
                0,                                  // maximum object size (high-order DWORD)
                filesize_,                          // maximum object size (low-order DWORD)
                filename_.c_str());                 // name of mapping object

            if (hMapFile_ == NULL) {
                throw std::runtime_error("Could not create memory mapped object: " + std::to_string(GetLastError()));
            }

            created = true;
        }

        stringStart_ = (char*) MapViewOfFile(hMapFile_,   // handle to map object
                    FILE_MAP_ALL_ACCESS, // read/write permission
                    0,
                    0,
                    filesize_);

        if (stringStart_ == NULL) {
            throw std::runtime_error("Could not create map view of file: " + std::to_string(GetLastError()));
        }

        std::string mutexName("mmap_mutex_" + name);
        if (created) {
            mtx_ = CreateMutex(NULL, false, mutexName.c_str());
        } else {
            mtx_ = OpenMutex(MUTEX_ALL_ACCESS, FALSE, mutexName.c_str());
        }
}

SharedObject::~SharedObject() {
    unlock();
    UnmapViewOfFile(pBuf_);
    CloseHandle(hMapFile_);
}

bool SharedObject::write(const std::string& bytes, uint32_t offset) {
    if (bytes.length() + offset > filesize_) {
        return false;
    }

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
    return (WaitForSingleObject(mtx_, INFINITE) == WAIT_OBJECT_0);
}

bool SharedObject::unlock() {
    return ReleaseMutex(mtx_);
}

}  // namespace containers
}  // namespace common
}  // namespace urf
