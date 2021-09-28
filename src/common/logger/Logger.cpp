#include "urf/common/logger/Logger.hpp"

namespace urf {
namespace common {

logger::logger getLoggerInstance(const std::string component) {
    std::vector<logger::sink_ptr> sinks;
    if (std::getenv(LOGGER_ENABLE_LOGGING)) {
        sinks.push_back(std::make_shared<logger::sinks::stdout_color_sink_mt>());
    }

    if (std::getenv(LOGGER_ENABLE_FILE_LOGGING)) {
        sinks.push_back(std::make_shared<logger::sinks::rotating_file_sink_mt>("logs/log.txt" , 1024 * 1024, 5, true));
    }

    logger::logger log(component, begin(sinks), end(sinks));
    // auto log = std::make_unique<logger::logger>(component, begin(sinks), end(sinks));

    if (std::getenv(LOGGER_ENABLE_TRACE)) {
        log.set_level(logger::level::trace);
    } else {
        log.set_level(logger::level::info);
    }
    log.set_pattern("[%Y-%m-%d %H:%M:%S.%e] <PID:%P> <Thread:%t> [%l] [%n] : %v");
    return log;
}

}  // namespace common
}  // namespace urf