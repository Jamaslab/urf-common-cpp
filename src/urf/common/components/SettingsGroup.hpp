#pragma once

#if defined(_WIN32) || defined(_WIN64)
#    include "urf/common/urf_common_export.h"
#else
#    define URF_COMMON_EXPORT
#endif

#include <string>
#include <unordered_map>

#include "urf/common/properties/ObservableProperty.hpp"

namespace urf {
namespace common {
namespace components {

class URF_COMMON_EXPORT SettingsGroup
    : public std::unordered_map<std::string, std::shared_ptr<properties::IObservableProperty>> {
 public:
    SettingsGroup() = default;
    explicit SettingsGroup(const std::string& name);
    SettingsGroup(const SettingsGroup&) = default;
    SettingsGroup(SettingsGroup&&) = default;
    ~SettingsGroup() = default;

    std::string name() const;
    void name(const std::string&);

    SettingsGroup& operator=(const SettingsGroup&) = default;

 private:
    std::string name_;
};

} // namespace components
} // namespace common
} // namespace urf