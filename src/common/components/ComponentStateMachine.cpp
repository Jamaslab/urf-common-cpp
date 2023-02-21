#include "urf/common/components/ComponentStateMachine.hpp"

namespace urf {
namespace common {
namespace components {

ComponentStateMachine::ComponentStateMachine(
    std::shared_ptr<properties::ObservableProperty<StatePair>> componentState,
    const std::function<bool()>& resetFaultImpl,
    const std::function<bool()>& switchOnImpl,
    const std::function<bool()>& shutdownImpl,
    const std::function<bool()>& disableImpl,
    const std::function<bool()>& enableImpl,
    const std::function<bool()>& faultImpl)
    : componentState_(componentState)
    , resetFaultImpl_(resetFaultImpl)
    , switchOnImpl_(switchOnImpl)
    , shutdownImpl_(shutdownImpl)
    , disableImpl_(disableImpl)
    , enableImpl_(enableImpl)
    , faultImpl_(faultImpl) { }

bool ComponentStateMachine::resetFault() {
    if (!startTransition(ComponentStateTransitions::ResetFault)) {
        return false;
    }

    if (!resetFaultImpl_()) {
        return false;
    }

    transitionCompleted();
    return true;
}

bool ComponentStateMachine::switchOn() {
    if (!startTransition(ComponentStateTransitions::SwitchOn)) {
        return false;
    }

    if (!switchOnImpl_()) {
        fault();
        return false;
    }

    transitionCompleted();
    return true;
}
bool ComponentStateMachine::shutdown() {
    if ((currentState() == ComponentStates::Enabled) && (!disable())) {
        return false;
    }

    if (!startTransition(ComponentStateTransitions::Shutdown)) {
        return false;
    }

    if (!shutdownImpl_()) {
        fault();
        return false;
    }

    transitionCompleted();
    return true;
}

bool ComponentStateMachine::disable() {
    if (!startTransition(ComponentStateTransitions::Disable)) {
        return false;
    }

    if (!disableImpl_()) {
        fault();
        return false;
    }

    transitionCompleted();
    return true;
}

bool ComponentStateMachine::enable() {
    if ((currentState() == ComponentStates::ReadyToSwitchOn) && (!switchOn())) {
        return false;
    }

    if (!startTransition(ComponentStateTransitions::Enable)) {
        return false;
    }

    if (!enableImpl_()) {
        fault();
        return false;
    }

    transitionCompleted();
    return true;
}

bool ComponentStateMachine::reconfigure() {
    if (!disable()) {
        return false;
    }

    if (!enable()) {
        return false;
    }

    return true;
}

bool ComponentStateMachine::fault() {
    failTransition();

    faultImpl_();

    return true;
}

ComponentStates ComponentStateMachine::currentState() const {
    return componentState_->getValue().first;
}

ComponentStateTransitions ComponentStateMachine::currentTransition() const {
    return componentState_->getValue().second;
}

bool ComponentStateMachine::startTransition(ComponentStateTransitions transition) {
    if (currentTransition() != ComponentStateTransitions::NoTransition) {
        return false;
    }

    if ((transition == ComponentStateTransitions::ResetFault) &&
        (currentState() != ComponentStates::Fault)) {
        return false;
    } else if ((transition == ComponentStateTransitions::SwitchOn) &&
               (currentState() != ComponentStates::ReadyToSwitchOn)) {
        return false;
    } else if ((transition == ComponentStateTransitions::Enable) &&
               (currentState() != ComponentStates::ReadyToSwitchOn) &&
               (currentState() != ComponentStates::SwitchedOn)) {
        return false;
    } else if ((transition == ComponentStateTransitions::Disable) &&
               (currentState() != ComponentStates::Enabled)) {
        return false;
    } else if ((transition == ComponentStateTransitions::Shutdown) &&
               (currentState() == ComponentStates::ReadyToSwitchOn)) {
        return false;
    }

    componentState_->setValue({currentState(), transition});

    return true;
}

bool ComponentStateMachine::failTransition() {
    componentState_->setValue({ComponentStates::Fault, ComponentStateTransitions::NoTransition});

    return true;
}

bool ComponentStateMachine::transitionCompleted() {
    ComponentStates outState;
    if (currentTransition() == ComponentStateTransitions::SwitchOn) {
        outState = ComponentStates::SwitchedOn;
    } else if (currentTransition() == ComponentStateTransitions::Init) {
        outState = ComponentStates::ReadyToSwitchOn;
    } else if (currentTransition() == ComponentStateTransitions::Shutdown) {
        outState = ComponentStates::ReadyToSwitchOn;
    } else if (currentTransition() == ComponentStateTransitions::Enable) {
        outState = ComponentStates::Enabled;
    } else if (currentTransition() == ComponentStateTransitions::Disable) {
        outState = ComponentStates::SwitchedOn;
    } else if (currentTransition() == ComponentStateTransitions::ResetFault) {
        outState = ComponentStates::ReadyToSwitchOn;
    }

    componentState_->setValue({outState, ComponentStateTransitions::NoTransition});
    return true;
}

} // namespace components
} // namespace common
} // namespace urf