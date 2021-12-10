#pragma once

#if defined(_WIN32) || defined(_WIN64)
#    include "urf/common/urf_common_export.h"
#else
#    define URF_COMMON_EXPORT
#endif

#include <algorithm>
#include <any>
#include <array>
#include <functional>
#include <mutex>
#include <nlohmann/json.hpp>
#include <shared_mutex>
#include <vector>

namespace urf {
namespace common {
namespace properties {

template <class T> struct getTemplateDatatype {
    std::string operator()();
};

template <class T> struct getTemplateDatatype<std::vector<T, std::allocator<T> > > {
    std::string operator()() {
        return "vector/"+getTemplateDatatype<T>()();
    }
};

enum class PropertyType {
    Property,
    Setting,
    RangeSetting,
    ListSetting
};

class IObservableProperty {
 public:
    IObservableProperty() = default;
    virtual ~IObservableProperty() = default;

    virtual bool readonly() const = 0;
    virtual PropertyType type() const = 0;
    virtual std::string datatype() const = 0;

    void onAnyValueChange(const std::function<void(const std::any& previous, const std::any& current)>& callback);

    void to_json(nlohmann::json& j, const IObservableProperty& p);
    void from_json(const nlohmann::json& j, IObservableProperty& p);

    virtual void to_json(nlohmann::json& j, bool only_value = false) const = 0;
    virtual void from_json(const nlohmann::json& j) = 0;

    friend std::ostream& operator<< (std::ostream& stream, const IObservableProperty& prop);
    friend std::ostream& operator<< (std::ostream& stream, std::shared_ptr<IObservableProperty> prop);

 protected:
    std::function<void(const std::any& previous, const std::any& current)> nonTemplatedCallback_;
};

class IObservableSetting {
 public:
    void onAnyRequestedValueChange(const std::function<void(const std::any& previous, const std::any& current)>& callback);
 protected:
    std::function<void(const std::any& previous, const std::any& current)> nonTemplatedRequestedCallback_;
};

template <class T>
class ObservableProperty : public IObservableProperty {
 public:
    ObservableProperty() = default;
    ObservableProperty(const ObservableProperty&);
    ObservableProperty(ObservableProperty&&);
    ~ObservableProperty() override = default;

    bool readonly() const override;
    PropertyType type() const override;
    std::string datatype() const override;

    T getValue() const;
    bool setValue(const T& value);

    void onValueChange(const std::function<void(const T& previous, const T& current)>& callback);

    template <class P>
    friend void to_json(nlohmann::json& j, const ObservableProperty<P>& p);
    template <class P>
    friend void from_json(const nlohmann::json& j, ObservableProperty<P>& p);

    ObservableProperty& operator=(const ObservableProperty<T>& other);

 protected:
    mutable std::shared_mutex valueMtx_;
    T value_;
    std::function<void(const T& previous, const T& current)> callback_;

    void to_json(nlohmann::json& j, bool only_value = false) const override;
    void from_json(const nlohmann::json& j) override;
};

template <class T>
ObservableProperty<T>::ObservableProperty(const ObservableProperty& other) {
    *this = other;
}

template <class T>
ObservableProperty<T>::ObservableProperty(ObservableProperty&& other) {
    value_ = std::move(other.value_);
    callback_ = std::move(other.callback_);
}

template <class T>
bool ObservableProperty<T>::readonly() const {
    return true;
}

template <class T>
PropertyType ObservableProperty<T>::type() const {
    return PropertyType::Property;
}

template <class T>
std::string ObservableProperty<T>::datatype() const {
    return getTemplateDatatype<T>()();
}

template <class T>
T ObservableProperty<T>::getValue() const {
    std::shared_lock lock(valueMtx_);
    return value_;
}

template <class T>
bool ObservableProperty<T>::setValue(const T& value) {
    if (getValue() != value) {
        T prevValue;
        {

            std::scoped_lock lock(valueMtx_);
            prevValue = value_;
            value_ = value;
        }

        if (callback_)
            callback_(prevValue, value);

        if (nonTemplatedCallback_)
            nonTemplatedCallback_(prevValue, value);
    }

    return true;
}

template <class T>
void ObservableProperty<T>::onValueChange(
    const std::function<void(const T& previous, const T& current)>& callback) {
    callback_ = callback;
}

template <class T>
void ObservableProperty<T>::to_json(nlohmann::json& j, bool only_value) const {
    j["value"] = getValue();

    if (!only_value) {
        j["type"] = type();
        j["dtype"] = datatype();
    }
}

template <class T>
void ObservableProperty<T>::from_json(const nlohmann::json& j) {
    setValue(j["value"].get<T>());
}

template <class T>
ObservableProperty<T>& ObservableProperty<T>::operator=(const ObservableProperty<T>& other) {
    std::scoped_lock lock(valueMtx_);
    value_ = other.getValue();
    callback_ = other.callback_;
    return *this;
}

template <class P>
void to_json(nlohmann::json& j, const ObservableProperty<P>& p) {
    p.to_json(j);
}

template <class P>
void from_json(const nlohmann::json& j, ObservableProperty<P>& p) {
    p.from_json(j);
}

template <class T>
class ObservableSetting : public ObservableProperty<T>, IObservableSetting {
 public:
    ObservableSetting() = default;
    ObservableSetting(const ObservableSetting&);
    ObservableSetting(ObservableSetting&&);
    ~ObservableSetting() override = default;

    bool readonly() const override;
    PropertyType type() const override;
    T getRequestedValue() const;
    virtual bool setRequestedValue(const T& value);

    void onRequestedValueChange(
        const std::function<void(const T& previous, const T& current)>& callback);

    ObservableSetting& operator=(const ObservableSetting<T>&);

 protected:
    mutable std::shared_mutex requestedValueMtx_;
    T requestedValue_;
    std::function<void(const T& previous, const T& current)> reqValueCallback_;

    void to_json(nlohmann::json& j, bool only_value = false) const override;
    void from_json(const nlohmann::json& j) override;
};

template <class T>
ObservableSetting<T>::ObservableSetting(const ObservableSetting& other)
    : ObservableProperty<T>(other)
    , requestedValue_(other.requestedValue_)
    , reqValueCallback_(other.reqValueCallback_) { }

template <class T>
ObservableSetting<T>::ObservableSetting(ObservableSetting&& other)
    : ObservableProperty<T>(other)
    , requestedValue_(std::move(other.requestedValue_))
    , reqValueCallback_(std::move(other.reqValueCallback_)) {}

template <class T>
bool ObservableSetting<T>::readonly() const {
    return false;
}

template <class T>
PropertyType ObservableSetting<T>::type() const {
    return PropertyType::Setting;
}

template <class T>
T ObservableSetting<T>::getRequestedValue() const {
    std::shared_lock lock(requestedValueMtx_);
    return requestedValue_;
}

template <class T>
bool ObservableSetting<T>::setRequestedValue(const T& value) {
    if (getRequestedValue() != value) {
        T prevValue;
        {
            std::scoped_lock lock(requestedValueMtx_);
            prevValue = requestedValue_;
            requestedValue_ = value;
        }

        if (reqValueCallback_)
            reqValueCallback_(prevValue, value);

        if (nonTemplatedRequestedCallback_)
            nonTemplatedRequestedCallback_(prevValue, value);
    } else {
    }

    return true;
}

template <class T>
void ObservableSetting<T>::onRequestedValueChange(
    const std::function<void(const T& previous, const T& current)>& callback) {
    reqValueCallback_ = callback;
}

template <class T>
ObservableSetting<T>& ObservableSetting<T>::operator=(const ObservableSetting<T>& other) {
    {
        std::scoped_lock lock(requestedValueMtx_);
        requestedValue_ = other.getRequestedValue();
    }

    reqValueCallback_ = other.reqValueCallback_;

    ObservableProperty<T>::operator=(other);
    return *this;
}

template <class T>
void ObservableSetting<T>::to_json(nlohmann::json& j, bool only_value) const {
    ObservableProperty<T>::to_json(j, only_value);

    if (!only_value) {
        j["req_value"] = requestedValue_;
    }
}

template <class T>
void ObservableSetting<T>::from_json(const nlohmann::json& j) {
    if (j.find("req_value") != j.end()) {
        setRequestedValue(j["req_value"].get<T>());
    }

    if (j.find("value") != j.end()) {
        ObservableProperty<T>::setValue(j["value"].get<T>());
    }
}

template <class T>
class ObservableSettingRanged : public ObservableSetting<T> {
 public:
    ObservableSettingRanged() = default;
    ObservableSettingRanged(const std::array<T, 2>& range);
    ObservableSettingRanged(const ObservableSettingRanged&) = default;
    ObservableSettingRanged(ObservableSettingRanged&&) = default;
    ~ObservableSettingRanged() override = default;

    PropertyType type() const override;
    std::array<T, 2> getRange() const;
    void setRange(const std::array<T, 2>& range);
    bool setRequestedValue(const T& value) override;

    ObservableSettingRanged& operator=(const ObservableSettingRanged<T>&) = default;

 protected:
    std::array<T, 2> range_;

    void to_json(nlohmann::json& j, bool only_value = false) const override;
    void from_json(const nlohmann::json& j) override;
};

template <class T>
ObservableSettingRanged<T>::ObservableSettingRanged(const std::array<T, 2>& range)
    : range_(range) { }

template <class T>
PropertyType ObservableSettingRanged<T>::type() const {
    return PropertyType::RangeSetting;
}

template <class T>
std::array<T, 2> ObservableSettingRanged<T>::getRange() const {
    return range_;
}

template <class T>
void ObservableSettingRanged<T>::setRange(const std::array<T, 2>& range) {
    range_ = range;
}

template <class T>
bool ObservableSettingRanged<T>::setRequestedValue(const T& value) {
    if ((value < range_[0]) || (value > range_[1])) {
        return false;
    }

    return ObservableSetting<T>::setRequestedValue(value);
}

template <class T>
void ObservableSettingRanged<T>::to_json(nlohmann::json& j, bool only_value) const {
    ObservableSetting<T>::to_json(j, only_value);

    if (only_value) {
        j["range"] = range_;
    }
}

template <class T>
void ObservableSettingRanged<T>::from_json(const nlohmann::json& j) {
    if (j.find("range") != j.end()) {
        setRange(j["range"].get<std::array<T, 2>>());
    }

    ObservableSetting<T>::from_json(j);
}

template <class T>
class ObservableSettingList : public ObservableSetting<T> {
 public:
    ObservableSettingList() = default;
    ObservableSettingList(const std::vector<T>& list);
    ObservableSettingList(const ObservableSettingList&) = default;
    ObservableSettingList(ObservableSettingList&&) = default;
    ~ObservableSettingList() override = default;

    PropertyType type() const override;
    std::vector<T> getList() const;
    void setList(const std::vector<T>& list);
    bool setRequestedValue(const T& value) override;

    ObservableSettingList& operator=(const ObservableSettingList<T>&) = default;

 protected:
    std::vector<T> list_;

    void to_json(nlohmann::json& j, bool only_value = false) const override;
    void from_json(const nlohmann::json& j) override;
};

template <class T>
ObservableSettingList<T>::ObservableSettingList(const std::vector<T>& list)
    : list_(list) { }

template <class T>
PropertyType ObservableSettingList<T>::type() const {
    return PropertyType::ListSetting;
}

template <class T>
std::vector<T> ObservableSettingList<T>::getList() const {
    return list_;
}

template <class T>
void ObservableSettingList<T>::setList(const std::vector<T>& list) {
    list_ = list;
}

template <class T>
bool ObservableSettingList<T>::setRequestedValue(const T& value) {
    if (std::find(list_.begin(), list_.end(), value) == list_.end()) {
        return false;
    }

    return ObservableSetting<T>::setRequestedValue(value);
}

template <class T>
void ObservableSettingList<T>::to_json(nlohmann::json& j, bool only_value) const {
    ObservableSetting<T>::to_json(j, only_value);

    if (!only_value) {
        j["list"] = list_;
    }
}

template <class T>
void ObservableSettingList<T>::from_json(const nlohmann::json& j) {
    if (j.find("list") != j.end()) {
        setList(j["list"].get<std::vector<T>>());
    }

    ObservableSetting<T>::from_json(j);
}

NLOHMANN_JSON_SERIALIZE_ENUM(PropertyType, {
    {PropertyType::Property, "property"},
    {PropertyType::Setting, "setting"},
    {PropertyType::RangeSetting, "range_setting"},
    {PropertyType::ListSetting, "list_setting"},
})

} // namespace properties
} // namespace common
} // namespace urf
