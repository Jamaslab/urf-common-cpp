#include "urf/common/properties/ObservableProperty.hpp"

namespace urf {
namespace common {
namespace properties {

template<>
std::string getTemplateDatatype<uint8_t>() {
    return "uint8";
}

template<>
std::string getTemplateDatatype<uint16_t>() {
    return "uint16";
}

template<>
std::string getTemplateDatatype<uint32_t>() {
    return "uint32";
}

template<>
std::string getTemplateDatatype<uint64_t>() {
    return "uint64";
}

template<>
std::string getTemplateDatatype<int8_t>() {
    return "int8";
}

template<>
std::string getTemplateDatatype<int16_t>() {
    return "int16";
}

template<>
std::string getTemplateDatatype<int32_t>() {
    return "int32";
}

template<>
std::string getTemplateDatatype<int64_t>() {
    return "int64";
}

template<>
std::string getTemplateDatatype<float>() {
    return "float32";
}

template<>
std::string getTemplateDatatype<double>() {
    return "float64";
}

template<>
std::string getTemplateDatatype<std::string>() {
    return "string";
}

template<class T> std::string getTemplateDatatype<std::vector<T>>() {
    return "vec_"+getTemplateDatatype<T>();
}

template<>
std::string getTemplateDatatype<nlohmann::json>() {
    return "json";
}

}
}
}