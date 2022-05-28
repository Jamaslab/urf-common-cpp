#pragma once

#if defined(_WIN32) || defined(_WIN64)
#    include "urf/common/urf_common_export.h"
#else
#    define URF_COMMON_EXPORT
#endif

#include <nlohmann/json.hpp>
#include <string>
#include <unordered_map>

#include "urf/common/components/SettingsGroup.hpp"
#include "urf/common/properties/ObservableProperty.hpp"

namespace urf {
namespace common {
namespace components {

enum class ComponentStates {
    Fault = -1,
    Init = 0,
    ReadyToSwitchOn = 1,
    SwitchedOn = 2,
    Enabled = 3
};

enum class ComponentStateTransitions {
    Init = 0,
    ResetFault = 1,
    SwitchOn = 2,
    Shutdown = 3,
    Disable = 4,
    Enable = 5,
    NoTransition = 6
};

typedef std::pair<ComponentStates, ComponentStateTransitions> StatePair;

class URF_COMMON_EXPORT IComponent {
 public:
    virtual ~IComponent() = default;

    virtual bool resetFault() noexcept = 0;

    /** Goes to switched on **/
    virtual bool switchOn() noexcept = 0;

    /** Goes to ready to Switch On **/
    virtual bool shutdown() noexcept = 0;

    /** Goes to switched on **/
    virtual bool disable() noexcept = 0;

    /** Goes to enabled **/
    virtual bool enable() noexcept = 0;

    /** Goes to switched on and back to enabled without changing currentState unless it can't re-enable**/
    virtual bool reconfigure() noexcept = 0;

    /** A fault can be manually triggered due to external conditions **/
    virtual bool fault() noexcept = 0;

    virtual ComponentStates currentState() const = 0;
    virtual ComponentStateTransitions currentTransition() const = 0;

    virtual std::string componentName() const = 0;
    virtual std::vector<std::string> componentClass() const = 0;

    virtual std::unordered_map<std::string, SettingsGroup> settings() = 0;
};

NLOHMANN_JSON_SERIALIZE_ENUM(ComponentStates,
                             {
                                 {ComponentStates::Fault, "fault"},
                                 {ComponentStates::Init, "init"},
                                 {ComponentStates::ReadyToSwitchOn, "ready"},
                                 {ComponentStates::SwitchedOn, "switchedon"},
                                 {ComponentStates::Enabled, "enabled"},
                             })

NLOHMANN_JSON_SERIALIZE_ENUM(ComponentStateTransitions,
                             {
                                 {ComponentStateTransitions::Init, "init"},
                                 {ComponentStateTransitions::ResetFault, "reset_fault"},
                                 {ComponentStateTransitions::SwitchOn, "switchon"},
                                 {ComponentStateTransitions::Shutdown, "shutdown"},
                                 {ComponentStateTransitions::Disable, "disable"},
                                 {ComponentStateTransitions::Enable, "enable"},
                                 {ComponentStateTransitions::NoTransition, "none"},
                             })

} // namespace components
} // namespace common
} // namespace urf