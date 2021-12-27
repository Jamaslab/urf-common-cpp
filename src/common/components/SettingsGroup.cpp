#include "urf/common/components/SettingsGroup.hpp"

namespace urf {
namespace common {
namespace components {

SettingsGroup::SettingsGroup(const std::string& name)
    : name_(name) { }

std::string SettingsGroup::name() const {
    return name_;
}

void SettingsGroup::name(const std::string& name) {
    name_ = name;
}

} // namespace components
} // namespace common
} // namespace urf