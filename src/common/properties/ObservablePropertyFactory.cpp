#include "urf/common/properties/ObservablePropertyFactory.hpp"

#include <iostream>
#include <map>
#include <memory>
#define COMMA ,

#define REGISTER_PROPERTY_DATATYPE_COMPILE(name, datatype)                                         \
    { name, {                                                                                \
        {PropertyType::Property, []() { return new urf::common::properties::ObservableProperty<datatype>(); }},\
        {PropertyType::Setting, []() { return new urf::common::properties::ObservableSetting<datatype>(); }}, \
        {PropertyType::ListSetting, []() { return new urf::common::properties::ObservableSettingList<datatype>(); }}, \
        {PropertyType::RangeSetting, []() { return new urf::common::properties::ObservableSettingRanged<datatype>(); }}\
    }}

#define REGISTER_PROPERTY_DATATYPE_MATRIX_COMPILE(name, datatype)                                         \
    { name, {                                                                                \
        {PropertyType::Property, []() { return new urf::common::properties::ObservableProperty<Eigen::Matrix<datatype, Eigen::Dynamic, Eigen::Dynamic>>(); }},\
        {PropertyType::Setting, []() { return new urf::common::properties::ObservableSetting<Eigen::Matrix<datatype, Eigen::Dynamic, Eigen::Dynamic>>(); }}, \
        {PropertyType::ListSetting, []() { return new urf::common::properties::ObservableSettingList<Eigen::Matrix<datatype, Eigen::Dynamic, Eigen::Dynamic>>(); }}\
    }}

namespace urf {
namespace common {
namespace properties {

std::unordered_map<std::string, std::map<PropertyType, std::function<IObservableProperty*()>>>
    ObservablePropertyFactory::registered_ = {
        {"node",
         {{PropertyType::Property, []() { return (IObservableProperty*)(new PropertyNode()); }}}},
        REGISTER_PROPERTY_DATATYPE_COMPILE("bool", bool),
        REGISTER_PROPERTY_DATATYPE_COMPILE("uint8", uint8_t),
        REGISTER_PROPERTY_DATATYPE_COMPILE("uint16", uint16_t),
        REGISTER_PROPERTY_DATATYPE_COMPILE("uint32", uint32_t),
        REGISTER_PROPERTY_DATATYPE_COMPILE("uint64", uint64_t),
        REGISTER_PROPERTY_DATATYPE_COMPILE("int8", int8_t),
        REGISTER_PROPERTY_DATATYPE_COMPILE("int16", int16_t),
        REGISTER_PROPERTY_DATATYPE_COMPILE("int32", int32_t),
        REGISTER_PROPERTY_DATATYPE_COMPILE("int64", int64_t),
        REGISTER_PROPERTY_DATATYPE_COMPILE("float32", float),
        REGISTER_PROPERTY_DATATYPE_COMPILE("float64", double),
        REGISTER_PROPERTY_DATATYPE_COMPILE("string", std::string),
        REGISTER_PROPERTY_DATATYPE_COMPILE("json", nlohmann::json),
        REGISTER_PROPERTY_DATATYPE_COMPILE("vector/bool", std::vector<bool>),
        REGISTER_PROPERTY_DATATYPE_COMPILE("vector/uint8", std::vector<uint8_t>),
        REGISTER_PROPERTY_DATATYPE_COMPILE("vector/uint16", std::vector<uint16_t>),
        REGISTER_PROPERTY_DATATYPE_COMPILE("vector/uint32", std::vector<uint32_t>),
        REGISTER_PROPERTY_DATATYPE_COMPILE("vector/uint64", std::vector<uint64_t>),
        REGISTER_PROPERTY_DATATYPE_COMPILE("vector/int8", std::vector<int8_t>),
        REGISTER_PROPERTY_DATATYPE_COMPILE("vector/int16", std::vector<int16_t>),
        REGISTER_PROPERTY_DATATYPE_COMPILE("vector/int32", std::vector<int32_t>),
        REGISTER_PROPERTY_DATATYPE_COMPILE("vector/int64", std::vector<int64_t>),
        REGISTER_PROPERTY_DATATYPE_COMPILE("vector/float32", std::vector<float>),
        REGISTER_PROPERTY_DATATYPE_COMPILE("vector/float64", std::vector<double>),
        REGISTER_PROPERTY_DATATYPE_COMPILE("vector/string", std::vector<std::string>),
        REGISTER_PROPERTY_DATATYPE_MATRIX_COMPILE("matrix/bool", bool),
        REGISTER_PROPERTY_DATATYPE_MATRIX_COMPILE("matrix/uint8", uint8_t),
        REGISTER_PROPERTY_DATATYPE_MATRIX_COMPILE("matrix/uint16", uint16_t),
        REGISTER_PROPERTY_DATATYPE_MATRIX_COMPILE("matrix/uint32", uint32_t),
        REGISTER_PROPERTY_DATATYPE_MATRIX_COMPILE("matrix/uint64", uint64_t),
        REGISTER_PROPERTY_DATATYPE_MATRIX_COMPILE("matrix/int8", int8_t),
        REGISTER_PROPERTY_DATATYPE_MATRIX_COMPILE("matrix/int16", int16_t),
        REGISTER_PROPERTY_DATATYPE_MATRIX_COMPILE("matrix/int32", int32_t),
        REGISTER_PROPERTY_DATATYPE_MATRIX_COMPILE("matrix/int64", int64_t),
        REGISTER_PROPERTY_DATATYPE_MATRIX_COMPILE("matrix/float32", float),
        REGISTER_PROPERTY_DATATYPE_MATRIX_COMPILE("matrix/float64", double)
    };

std::shared_ptr<IObservableProperty> ObservablePropertyFactory::create(const std::string& datatype,
                                                                       const PropertyType& type) {
    if (registered_.count(datatype) == 0) {
        throw std::runtime_error("Unregistered datatype");
    }

    if (registered_[datatype].count(type) == 0) {
        throw std::runtime_error("Missing observable property type for specified type");
    }

    std::shared_ptr<IObservableProperty> property((registered_[datatype][type])());
    return property;
}

template <>
std::string urf::common::properties::getTemplateDatatype<bool>::operator()() {
    return "bool";
}

template <>
std::string urf::common::properties::getTemplateDatatype<uint8_t>::operator()() {
    return "uint8";
}

template <>
std::string urf::common::properties::getTemplateDatatype<uint16_t>::operator()() {
    return "uint16";
}

template <>
std::string urf::common::properties::getTemplateDatatype<uint32_t>::operator()() {
    return "uint32";
}

template <>
std::string urf::common::properties::getTemplateDatatype<uint64_t>::operator()() {
    return "uint64";
}

template <>
std::string urf::common::properties::getTemplateDatatype<int8_t>::operator()() {
    return "int8";
}

template <>
std::string urf::common::properties::getTemplateDatatype<int16_t>::operator()() {
    return "int16";
}

template <>
std::string urf::common::properties::getTemplateDatatype<int32_t>::operator()() {
    return "int32";
}

template <>
std::string urf::common::properties::getTemplateDatatype<int64_t>::operator()() {
    return "int64";
}

template <>
std::string urf::common::properties::getTemplateDatatype<float>::operator()() {
    return "float32";
}

template <>
std::string urf::common::properties::getTemplateDatatype<double>::operator()() {
    return "float64";
}

template <>
std::string urf::common::properties::getTemplateDatatype<std::string>::operator()() {
    return "string";
}

template <>
std::string urf::common::properties::getTemplateDatatype<nlohmann::json>::operator()() {
    return "json";
}

} // namespace properties
} // namespace common
} // namespace urf
