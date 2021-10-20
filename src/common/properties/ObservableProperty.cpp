#include "urf/common/properties/ObservableProperty.hpp"

namespace urf {
namespace common {
namespace properties {

template<>
std::string getTemplateDatatype<bool>::operator()() {
    return "bool";
}

template<>
std::string getTemplateDatatype<uint8_t>::operator()() {
    return "uint8";
}

template<>
std::string getTemplateDatatype<uint16_t>::operator()() {
    return "uint16";
}

template<>
std::string getTemplateDatatype<uint32_t>::operator()() {
    return "uint32";
}

template<>
std::string getTemplateDatatype<uint64_t>::operator()() {
    return "uint64";
}

template<>
std::string getTemplateDatatype<int8_t>::operator()() {
    return "int8";
}

template<>
std::string getTemplateDatatype<int16_t>::operator()() {
    return "int16";
}

template<>
std::string getTemplateDatatype<int32_t>::operator()() {
    return "int32";
}

template<>
std::string getTemplateDatatype<int64_t>::operator()() {
    return "int64";
}

template<>
std::string getTemplateDatatype<float>::operator()() {
    return "float32";
}

template<>
std::string getTemplateDatatype<double>::operator()() {
    return "float64";
}

template<>
std::string getTemplateDatatype<std::string>::operator()() {
    return "string";
}

template<>
std::string getTemplateDatatype<nlohmann::json>::operator()() {
    return "json";
}

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

}
}
}