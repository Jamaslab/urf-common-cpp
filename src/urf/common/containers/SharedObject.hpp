#pragma once

#if defined(_WIN32) || defined(_WIN64)
    #include "urf/common/urf_common_export.h"
#else
    #define URF_COMMON_EXPORT
#endif

#include <string>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <tchar.h>
#endif

#ifdef __linux__
#include <pthread.h>
#endif

namespace urf {
namespace common {
namespace containers {

class URF_COMMON_EXPORT SharedObject {
 public:
    SharedObject(const std::string& name, uint32_t fileSize);
    ~SharedObject();

    bool write(const std::string& bytes, uint32_t offset = 0);
    std::string read(uint32_t length = 0);

    bool lock();
    bool unlock();

 private:
    std::string filename_;
    char* stringStart_;
    uint32_t filesize_;

#ifdef __linux__
    int fd_;

    typedef struct {
        pthread_mutex_t ipc_mutex;
    } mmap_mutex_t;
    mmap_mutex_t* mmapMutex_;

    void* completeMmap_;

#elif _WIN32 || _WIN64
    HANDLE hMapFile_;
    LPCTSTR pBuf_;
    HANDLE mtx_;
#endif
};

}  // namespace containers
}  // namespace common
}  // namespace urf
