#pragma once

#if defined(_WIN32) || defined(_WIN64)
#    include "urf/common/urf_common_export.h"
#else
#    define URF_COMMON_EXPORT
#endif

#include "urf/common/properties/ObservableProperty.hpp"

#include <map>
#include <tuple>
#include <typeinfo>

namespace urf {
namespace common {
namespace properties {

class ObservablePropertyFactory {
    static std::map<std::string, std::map<std::string, std::function<IObservableProperty*()>>>
        registered_;

    ObservablePropertyFactory() = default;

 public:
    static std::shared_ptr<IObservableProperty> create(const std::string& datatype,
                                                       const std::string& type);
    static bool
        registerDatatype(const std::string& datatype,
                     std::map<std::string, std::function<IObservableProperty*()>> constructors);
};

} // namespace properties
} // namespace common
} // namespace urf

#define REGISTER_PROPERTY_DATATYPE(name, datatype)                          \
namespace _urf_##datatype {                                                 \
    bool tmp = urf::common::properties::ObservablePropertyFactory::registerDatatype(                                                   \
        name,                                                                                 \
        {{"property", []() { return new urf::common::properties::ObservableProperty<datatype>(); }},                           \
         {"setting", []() { return new urf::common::properties::ObservableSetting<datatype>(); }},                             \
         {"range_setting", []() { return new urf::common::properties::ObservableSettingRanged<datatype>(); }},                 \
         {"list_setting", []() { return new urf::common::properties::ObservableSettingList<datatype>(); }}});                  \
}                                                                                                                              \
\
template<> \
std::string urf::common::properties::getTemplateDatatype<datatype>::operator()() { \
    return name; \
}

#define REGISTER_PROPERTY_DATATYPE_VECTOR(name, datatype)                          \
namespace _urf_##datatype {                                                 \
    bool tmp_vec = urf::common::properties::ObservablePropertyFactory::registerDatatype(                                                   \
        "vector/"+std::string(name),                                                                                 \
        {{"property", []() { return new urf::common::properties::ObservableProperty<std::vector<datatype>>(); }},                           \
         {"setting", []() { return new urf::common::properties::ObservableSetting<std::vector<datatype>>(); }},                             \
         {"range_setting", []() { return new urf::common::properties::ObservableSettingRanged<std::vector<datatype>>(); }},                 \
         {"list_setting", []() { return new urf::common::properties::ObservableSettingList<std::vector<datatype>>(); }}});                  \
}

