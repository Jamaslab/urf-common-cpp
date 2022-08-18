#include "urf/common/properties/ObservableProperty.hpp"
#include "urf/common/properties/ObservablePropertyFactory.hpp"

#include <iostream>

namespace urf {
namespace common {
namespace properties {

uint32_t IObservableProperty::cumulativeId_ = 0;

IObservableProperty::IObservableProperty()
    : id_(cumulativeId_++) { }

uint32_t IObservableProperty::id() const {
    return id_;
}

void IObservableProperty::to_json(nlohmann::json& j, const IObservableProperty& p) {
    p.to_json(j, false);
}

void IObservableProperty::from_json(const nlohmann::json& j, IObservableProperty& p) {
    p.from_json(j);
}

void IObservableProperty::onAnyValueChange(
    const std::function<void(const std::any& previous, const std::any& current)>& callback) {
    nonTemplatedCallback_ = callback;
}

void IObservableSetting::onAnyRequestedValueChange(
    const std::function<void(const std::any& previous, const std::any& current)>& callback) {
    nonTemplatedRequestedCallback_ = callback;
}

std::shared_ptr<IObservableProperty> IObservableProperty::at(const std::string& name) const {
    auto casted = dynamic_cast<const PropertyNode*>(this);
    if (casted == nullptr) {
        throw std::runtime_error("Invalid access. Property is not a PropertyNode");
    }

    return casted->at(name);
}

std::shared_ptr<IObservableProperty>
IObservableProperty::operator[](const std::string& name) const {
    return this->at(name);
}

std::ostream& operator<<(std::ostream& stream, const IObservableProperty& prop) {
    nlohmann::json j;
    prop.to_json(j, false);
    stream << j.dump();
    return stream;
}

std::ostream& operator<<(std::ostream& stream, std::shared_ptr<IObservableProperty> prop) {
    nlohmann::json j;
    prop->to_json(j, false);
    stream << j.dump();
    return stream;
}

std::string getTemplateDatatype<
    std::unordered_map<std::string, std::shared_ptr<IObservableProperty>>>::operator()() {
    return "node";
}

bool PropertyNode::has(const std::string& name) {
    return value_.find(name) != value_.end();
}

void PropertyNode::insert(const std::string& name, std::shared_ptr<IObservableProperty> prop) {
    value_.insert({name, prop});
}

void PropertyNode::remove(const std::string& name) {
    value_.erase(name);
}

std::shared_ptr<IObservableProperty> PropertyNode::at(const std::string& name) const {
    return value_.find(name)->second;
}

std::shared_ptr<IObservableProperty> PropertyNode::operator[](const std::string& name) const {
    return value_.find(name)->second;
}

} // namespace properties
} // namespace common
} // namespace urf

namespace nlohmann {

void adl_serializer<
    std::unordered_map<std::string,
                       std::shared_ptr<urf::common::properties::IObservableProperty>>>::
    to_json(json& j,
            const std::unordered_map<std::string,
                                     std::shared_ptr<urf::common::properties::IObservableProperty>>&
                map) {
    for (auto const& [key, val] : map) {
        j[key] = nlohmann::json();
        val->to_json(j[key]);
    }
}

void adl_serializer<
    std::unordered_map<std::string,
                       std::shared_ptr<urf::common::properties::IObservableProperty>>>::
    from_json(
        const json& j,
        std::unordered_map<std::string,
                           std::shared_ptr<urf::common::properties::IObservableProperty>>& map) {

    for (auto const& variable : j.items()) {
        try {
            if (map.find(variable.key()) != map.end()) {
                map[variable.key()]->from_json(variable.value());
            } else {
                auto property = urf::common::properties::ObservablePropertyFactory::create(
                    variable.value()["dtype"],
                    variable.value()["type"].get<urf::common::properties::PropertyType>());

                property->from_json(variable.value());
                map.insert({variable.key(), property});
            }
        } catch (const std::exception& ex) {
            throw ex;
        }
    }
}

} // namespace nlohmann