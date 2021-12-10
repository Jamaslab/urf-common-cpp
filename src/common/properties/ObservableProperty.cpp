#include "urf/common/properties/ObservableProperty.hpp"

namespace urf {
namespace common {
namespace properties {

void IObservableProperty::to_json(nlohmann::json& j, const IObservableProperty& p) {
    p.to_json(j, false);
}

void IObservableProperty::from_json(const nlohmann::json& j, IObservableProperty& p) {
    p.from_json(j);
}

void IObservableProperty::onAnyValueChange(const std::function<void(const std::any& previous, const std::any& current)>& callback) {
    nonTemplatedCallback_ = callback;
}

void IObservableSetting::onAnyRequestedValueChange(const std::function<void(const std::any& previous, const std::any& current)>& callback) {
    nonTemplatedRequestedCallback_ = callback;
}

std::ostream& operator<< (std::ostream& stream, const IObservableProperty& prop) {
    nlohmann::json j;
    prop.to_json(j, false);
    stream << j.dump();
    return stream;
}

std::ostream& operator<< (std::ostream& stream, std::shared_ptr<IObservableProperty> prop) {
    nlohmann::json j;
    prop->to_json(j, false);
    stream << j.dump();
    return stream;
}

}
}
}