#pragma once

#define LOGGER_ENABLE_TRACE "LOGGER_ENABLE_TRACE"  // set this env variable to 1 to enable debug logs NOLINT
#define LOGGER_ENABLE_LOGGING "LOGGER_ENABLE_LOGGING"  // set this env variable to 1 to enable debug logs NOLINT
#define LOGGER_ENABLE_FILE_LOGGING "LOGGER_ENABLE_FILE_LOGGING"  // set this env variable to 1 to enable debug logs NOLINT

#if defined(_WIN32) || defined(_WIN64)
    #include "urf/common/urf_common_export.h"
#else
    #define URF_COMMON_EXPORT
#endif

#include <spdlog/spdlog.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <memory>

namespace urf {
namespace common {

namespace logger = spdlog;

URF_COMMON_EXPORT logger::logger& getLoggerInstance(const std::string component);

}  // namespace common
}  // namespace urf
