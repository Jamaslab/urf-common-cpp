#include "urf/common/properties/ObservablePropertyFactory.hpp"

#include <iostream>

namespace urf {
namespace common {
namespace properties {

std::map<std::string, std::map<std::string, std::function<IObservableProperty*()>>> ObservablePropertyFactory::registered_;

std::shared_ptr<IObservableProperty> ObservablePropertyFactory::create(const std::string& datatype, const std::string& type) {
    if (registered_.count(datatype) == 0) {
        throw std::runtime_error("Unregistered datatype");
    }

    if (registered_[datatype].count(type) == 0) {
        throw std::runtime_error("Missing observable property type for specified type");
    }

    std::shared_ptr<IObservableProperty> property((registered_[datatype][type])());

    ObservablePropertyFactory::registerDatatype("float32",
        {
            { "property", []() { return new ObservableProperty<float>(); } },
            { "setting", []() { return new ObservableSetting<float>(); } },
            { "range_setting", []() { return new ObservableSettingRanged<float>(); } },
            { "list_setting", []() { return new ObservableSettingList<float>(); } }
        });

    return property;
}

bool ObservablePropertyFactory::registerDatatype(const std::string& datatype, std::map<std::string, std::function<IObservableProperty*()>> constructors) {
    if (registered_.count(datatype) != 0) {
        throw std::runtime_error("Datatype " + datatype + " already registered");
    }

    registered_.insert({datatype, constructors});

    return true;
}

REGISTER_PROPERTY_DATATYPE("bool", bool)
REGISTER_PROPERTY_DATATYPE("uint8", uint8_t)
REGISTER_PROPERTY_DATATYPE("uint16", uint16_t)
REGISTER_PROPERTY_DATATYPE("uint32", uint32_t)
REGISTER_PROPERTY_DATATYPE("uint64", uint64_t)
REGISTER_PROPERTY_DATATYPE("int8", int8_t)
REGISTER_PROPERTY_DATATYPE("int16", int16_t)
REGISTER_PROPERTY_DATATYPE("int32", int32_t)
REGISTER_PROPERTY_DATATYPE("int64", int64_t)
REGISTER_PROPERTY_DATATYPE("float32", float)
REGISTER_PROPERTY_DATATYPE("float64", double)
REGISTER_PROPERTY_DATATYPE("string", std::string)
REGISTER_PROPERTY_DATATYPE("json", nlohmann::json)

REGISTER_PROPERTY_DATATYPE_VECTOR("bool", bool)
REGISTER_PROPERTY_DATATYPE_VECTOR("uint8", uint8_t)
REGISTER_PROPERTY_DATATYPE_VECTOR("uint16", uint16_t)
REGISTER_PROPERTY_DATATYPE_VECTOR("uint32", uint32_t)
REGISTER_PROPERTY_DATATYPE_VECTOR("uint64", uint64_t)
REGISTER_PROPERTY_DATATYPE_VECTOR("int8", int8_t)
REGISTER_PROPERTY_DATATYPE_VECTOR("int16", int16_t)
REGISTER_PROPERTY_DATATYPE_VECTOR("int32", int32_t)
REGISTER_PROPERTY_DATATYPE_VECTOR("int64", int64_t)
REGISTER_PROPERTY_DATATYPE_VECTOR("float32", float)
REGISTER_PROPERTY_DATATYPE_VECTOR("float64", double)
REGISTER_PROPERTY_DATATYPE_VECTOR("string", std::string)

} // namespace properties
} // namespace common
} // namespace urf