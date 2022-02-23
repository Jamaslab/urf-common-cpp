#pragma once

#if defined(_WIN32) || defined(_WIN64)
#    include "urf/common/urf_common_export.h"
#else
#    define URF_COMMON_EXPORT
#endif

// This is to silence some Eigen warnings with C++ 17
#define _SILENCE_CXX17_ADAPTOR_TYPEDEFS_DEPRECATION_WARNING

#include "urf/common/properties/ObservableProperty.hpp"

#include <functional>
#include <map>
#include <tuple>
#include <typeinfo>

namespace urf {
namespace common {
namespace properties {

class URF_COMMON_EXPORT ObservablePropertyFactory {
    static std::map<std::string, std::map<PropertyType, std::function<IObservableProperty*()>>>
        registered_;

    ObservablePropertyFactory() = default;

 public:
    static std::shared_ptr<IObservableProperty> create(const std::string& datatype,
                                                       const PropertyType& type);
    template <class T>
    static bool registerDatatype(const std::string& datatype);

    template <class T>
    static bool registerRangedDatatype(const std::string& datatype);
};

template <class T>
bool ObservablePropertyFactory::registerDatatype(const std::string& datatype) {
    if (registered_.count(datatype) != 0) {
        throw std::runtime_error("Datatype " + datatype + " already registered");
    }

    registered_.insert({datatype,
                        {{PropertyType::Property,
                          []() { return new urf::common::properties::ObservableProperty<T>(); }},
                         {PropertyType::Setting,
                          []() { return new urf::common::properties::ObservableSetting<T>(); }},
                         {PropertyType::ListSetting, []() {
                              return new urf::common::properties::ObservableSettingList<T>();
                          }}}});

    return true;
}

template <class T>
bool ObservablePropertyFactory::registerRangedDatatype(const std::string& datatype) {
    if (registered_.count(datatype) != 0) {
        registered_[datatype].insert(
            {PropertyType::RangeSetting,
             []() { return new urf::common::properties::ObservableSettingRanged<T>(); }});
    } else {
        registered_.insert(
            {datatype, {{PropertyType::RangeSetting, []() {
                             return new urf::common::properties::ObservableSettingRanged<T>();
                         }}}});
    }

    return true;
}

} // namespace properties
} // namespace common
} // namespace urf

#define REGISTER_PROPERTY_DATATYPE(name, datatype)                                                 \
    namespace _urf_ {                                                                              \
    bool _tmp_##name =                                                                             \
        urf::common::properties::ObservablePropertyFactory::registerDatatype<datatype>(#name);     \
    }                                                                                              \
                                                                                                   \
    template <>                                                                                    \
    std::string urf::common::properties::getTemplateDatatype<datatype>::operator()() {             \
        return #name;                                                                              \
    }

#define REGISTER_PROPERTY_DATATYPE_COMPARABLE(name, datatype)                                      \
    namespace _urf_ {                                                                              \
    bool _tmp_##name =                                                                             \
        urf::common::properties::ObservablePropertyFactory::registerDatatype<datatype>(#name) &&   \
        urf::common::properties::ObservablePropertyFactory::registerRangedDatatype<datatype>(      \
            #name);                                                                                \
    }                                                                                              \
                                                                                                   \
    template <>                                                                                    \
    std::string urf::common::properties::getTemplateDatatype<datatype>::operator()() {             \
        return #name;                                                                              \
    }

#define REGISTER_PROPERTY_DATATYPE_VECTOR(name, datatype)                                          \
    namespace _urf_ {                                                                              \
    bool _tmp_vec_##name =                                                                         \
        urf::common::properties::ObservablePropertyFactory::registerDatatype<                      \
            std::vector<datatype>>("vector/" + std::string(#name)) &&                              \
        urf::common::properties::ObservablePropertyFactory::registerRangedDatatype<datatype>(      \
            "vector/" + std::string(#name));                                                       \
    }

#define REGISTER_PROPERTY_DATATYPE_MATRIX(name, datatype)                                          \
    namespace _urf_ {                                                                              \
    bool _tmp_mat_##name = urf::common::properties::ObservablePropertyFactory::registerDatatype<   \
        Eigen::Matrix<datatype, Eigen::Dynamic, Eigen::Dynamic>>("matrix/" + std::string(#name));   \
    }
