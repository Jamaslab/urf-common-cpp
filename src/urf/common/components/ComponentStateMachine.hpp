#pragma once

#if defined(_WIN32) || defined(_WIN64)
#    include "urf/common/urf_common_export.h"
#else
#    define URF_COMMON_EXPORT
#endif

#include <functional>

#include "urf/common/components/IComponent.hpp"
#include "urf/common/properties/ObservableProperty.hpp"

namespace urf {
namespace common {
namespace components {

class URF_COMMON_EXPORT ComponentStateMachine {
 public:
    ComponentStateMachine(std::shared_ptr<properties::ObservableProperty<StatePair>> componentState,
                          const std::function<bool()>& resetFaultImpl,
                          const std::function<bool()>& switchOnImpl,
                          const std::function<bool()>& shutdownImpl,
                          const std::function<bool()>& disableImpl,
                          const std::function<bool()>& enableImpl,
                          const std::function<bool()>& faultImpl);
    bool resetFault();
    bool switchOn();
    bool shutdown();
    bool disable();
    bool enable();
    bool reconfigure();
    bool fault();

 private:
    bool startTransition(ComponentStateTransitions transition);
    bool failTransition();
    bool transitionCompleted();

    ComponentStates currentState() const;
    ComponentStateTransitions currentTransition() const;

 private:
    std::shared_ptr<properties::ObservableProperty<StatePair>> componentState_;

    std::function<bool()> resetFaultImpl_;
    std::function<bool()> switchOnImpl_;
    std::function<bool()> shutdownImpl_; 
    std::function<bool()> disableImpl_;
    std::function<bool()> enableImpl_;
    std::function<bool()> faultImpl_;
};

} // namespace components
} // namespace common
} // namespace urf