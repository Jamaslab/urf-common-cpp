#include "urf/common/properties/ObservableProperty.hpp"

namespace urf {
namespace common {
namespace properties {

template<>
std::string getTemplateDatatype<uint8_t>() {
    return "uint8";
}

template<class T> std::string getTemplateDatatype<std::vector<T>>() {
    return "vec_"+getTemplateDatatype<T>();
}

ObservableProperty<std::vector<uint8_t>> test;

}
}
}