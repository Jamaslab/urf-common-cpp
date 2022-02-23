#include "urf/common/properties/ObservablePropertyFactory.hpp"

#include <iostream>

namespace urf {
namespace common {
namespace properties {

std::map<std::string, std::map<PropertyType, std::function<IObservableProperty*()>>> ObservablePropertyFactory::registered_;

std::shared_ptr<IObservableProperty> ObservablePropertyFactory::create(const std::string& datatype, const PropertyType& type) {
    if (registered_.count(datatype) == 0) {
        throw std::runtime_error("Unregistered datatype");
    }

    if (registered_[datatype].count(type) == 0) {
        throw std::runtime_error("Missing observable property type for specified type");
    }

    std::shared_ptr<IObservableProperty> property((registered_[datatype][type])());
    return property;
}

REGISTER_PROPERTY_DATATYPE_COMPARABLE(bool, bool)
REGISTER_PROPERTY_DATATYPE_COMPARABLE(uint8, uint8_t)
REGISTER_PROPERTY_DATATYPE_COMPARABLE(uint16, uint16_t)
REGISTER_PROPERTY_DATATYPE_COMPARABLE(uint32, uint32_t)
REGISTER_PROPERTY_DATATYPE_COMPARABLE(uint64, uint64_t)
REGISTER_PROPERTY_DATATYPE_COMPARABLE(int8, int8_t)
REGISTER_PROPERTY_DATATYPE_COMPARABLE(int16, int16_t)
REGISTER_PROPERTY_DATATYPE_COMPARABLE(int32, int32_t)
REGISTER_PROPERTY_DATATYPE_COMPARABLE(int64, int64_t)
REGISTER_PROPERTY_DATATYPE_COMPARABLE(float32, float)
REGISTER_PROPERTY_DATATYPE_COMPARABLE(float64, double)
REGISTER_PROPERTY_DATATYPE_COMPARABLE(string, std::string)
REGISTER_PROPERTY_DATATYPE_COMPARABLE(json, nlohmann::json)


REGISTER_PROPERTY_DATATYPE_VECTOR(bool, bool)
REGISTER_PROPERTY_DATATYPE_VECTOR(uint8, uint8_t)
REGISTER_PROPERTY_DATATYPE_VECTOR(uint16, uint16_t)
REGISTER_PROPERTY_DATATYPE_VECTOR(uint32, uint32_t)
REGISTER_PROPERTY_DATATYPE_VECTOR(uint64, uint64_t)
REGISTER_PROPERTY_DATATYPE_VECTOR(int8, int8_t)
REGISTER_PROPERTY_DATATYPE_VECTOR(int16, int16_t)
REGISTER_PROPERTY_DATATYPE_VECTOR(int32, int32_t)
REGISTER_PROPERTY_DATATYPE_VECTOR(int64, int64_t)
REGISTER_PROPERTY_DATATYPE_VECTOR(float32, float)
REGISTER_PROPERTY_DATATYPE_VECTOR(float64, double)
REGISTER_PROPERTY_DATATYPE_VECTOR(string, std::string)

REGISTER_PROPERTY_DATATYPE_MATRIX(bool, bool)
REGISTER_PROPERTY_DATATYPE_MATRIX(uint8, uint8_t)
REGISTER_PROPERTY_DATATYPE_MATRIX(uint16, uint16_t)
REGISTER_PROPERTY_DATATYPE_MATRIX(uint32, uint32_t)
REGISTER_PROPERTY_DATATYPE_MATRIX(uint64, uint64_t)
REGISTER_PROPERTY_DATATYPE_MATRIX(int8, int8_t)
REGISTER_PROPERTY_DATATYPE_MATRIX(int16, int16_t)
REGISTER_PROPERTY_DATATYPE_MATRIX(int32, int32_t)
REGISTER_PROPERTY_DATATYPE_MATRIX(int64, int64_t)
REGISTER_PROPERTY_DATATYPE_MATRIX(float32, float)
REGISTER_PROPERTY_DATATYPE_MATRIX(float64, double)

} // namespace properties
} // namespace common
} // namespace urf

