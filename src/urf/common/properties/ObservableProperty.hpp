#pragma once

#if defined(_WIN32) || defined(_WIN64)
#    include "urf/common/urf_common_export.h"
#else
#    define URF_COMMON_EXPORT
#endif

#include <algorithm>
#include <array>
#include <functional>
#include <nlohmann/json.hpp>
#include <vector>

namespace urf {
namespace common {
namespace properties {

template<class T>
std::string getTemplateDatatype();

class IObservableProperty {
 public:
    virtual ~IObservableProperty() = default;

    virtual std::string getType() const = 0;
    virtual std::string getDatatype() const = 0;

    void to_json(nlohmann::json& j, const IObservableProperty& p);
    void from_json(const nlohmann::json& j, IObservableProperty& p);

    virtual void to_json(nlohmann::json& j) const = 0;
    virtual void from_json(const nlohmann::json& j) = 0;
};

template <class T>
class ObservableProperty : public IObservableProperty {
 public:
    ObservableProperty() = default;
    ObservableProperty(const ObservableProperty&) = default;
    ObservableProperty(ObservableProperty&&) = default;
    ~ObservableProperty() override = default;

    std::string getType() const override;
    std::string getDatatype() const override;

    T getValue() const;
    bool setValue(const T& value);

    void onValueChange(const std::function<void(const T& previous, const T& current)>& callback);

    template <class P>
    friend void to_json(nlohmann::json& j, const ObservableProperty<P>& p);
    template <class P>
    friend void from_json(const nlohmann::json& j, ObservableProperty<P>& p);

    ObservableProperty& operator=(const ObservableProperty<T>&) = default;

 protected:
    T value_;
    std::function<void(const T& previous, const T& current)> callback_;

    void to_json(nlohmann::json& j) const override;
    void from_json(const nlohmann::json& j) override;
};

template <class T>
std::string ObservableProperty<T>::getType() const {
    return "property";
}

template <class T>
std::string ObservableProperty<T>::getDatatype() const {
    return getTemplateDatatype<T>();
}

template <class T>
T ObservableProperty<T>::getValue() const {
    return value_;
}

template <class T>
bool ObservableProperty<T>::setValue(const T& value) {
    if (value_ != value) {
        auto prevValue = value_;
        value_ = value;

        if (callback_)
            callback_(prevValue, value_);
    }
    return true;
}

template <class T>
void ObservableProperty<T>::onValueChange(
    const std::function<void(const T& previous, const T& current)>& callback) {
    callback_ = callback;
}

template <class T>
void ObservableProperty<T>::to_json(nlohmann::json& j) const {
    j["value"] = value_;
    j["type"] = getType();
    j["dtype"] = getDatatype();
}

template <class T>
void ObservableProperty<T>::from_json(const nlohmann::json& j) {
    setValue(j["value"].get<T>());
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
class ObservableSetting : public ObservableProperty<T> {
 public:
    ObservableSetting() = default;
    ObservableSetting(const ObservableSetting&) = default;
    ObservableSetting(ObservableSetting&&) = default;
    ~ObservableSetting() override = default;

    std::string getType() const override;
    T getRequestedValue() const;
    virtual bool setRequestedValue(const T& value);

    void onRequestedValueChange(
        const std::function<void(const T& previous, const T& current)>& callback);

    ObservableSetting& operator=(const ObservableSetting<T>&) = default;

 protected:
    T requestedValue_;
    std::function<void(const T& previous, const T& current)> reqValueCallback_;

    void to_json(nlohmann::json& j) const override;
    void from_json(const nlohmann::json& j) override;
};

template <class T>
std::string ObservableSetting<T>::getType() const {
    return "setting";
}

template <class T>
T ObservableSetting<T>::getRequestedValue() const {
    return requestedValue_;
}

template <class T>
bool ObservableSetting<T>::setRequestedValue(const T& value) {
    if (requestedValue_ != value) {
        auto prevValue = requestedValue_;
        requestedValue_ = value;

        if (reqValueCallback_)
            reqValueCallback_(prevValue, requestedValue_);
    }
    return true;
}

template <class T>
void ObservableSetting<T>::onRequestedValueChange(
    const std::function<void(const T& previous, const T& current)>& callback) {
    reqValueCallback_ = callback;
}

template <class T>
void ObservableSetting<T>::to_json(nlohmann::json& j) const {
    j["req_value"] = requestedValue_;
    j["value"] = ObservableProperty<T>::getValue();
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

    std::string getType() const override;
    std::array<T, 2> getRange() const;
    void setRange(const std::array<T, 2>& range);
    bool setRequestedValue(const T& value) override;

    ObservableSettingRanged& operator=(const ObservableSettingRanged<T>&) = default;

 protected:
    std::array<T, 2> range_;

    void to_json(nlohmann::json& j) const override;
    void from_json(const nlohmann::json& j) override;
};

template <class T>
ObservableSettingRanged<T>::ObservableSettingRanged(const std::array<T, 2>& range)
    : range_(range) { }

template <class T>
std::string ObservableSettingRanged<T>::getType() const {
    return "range_setting";
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
void ObservableSettingRanged<T>::to_json(nlohmann::json& j) const {
    ObservableSetting<T>::to_json(j);
    j["range"] = range_;
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

    std::string getType() const override;
    std::vector<T> getList() const;
    void setList(const std::vector<T>& list);
    bool setRequestedValue(const T& value) override;

    ObservableSettingList& operator=(const ObservableSettingList<T>&) = default;

 protected:
    std::vector<T> list_;

    void to_json(nlohmann::json& j) const override;
    void from_json(const nlohmann::json& j) override;
};

template <class T>
ObservableSettingList<T>::ObservableSettingList(const std::vector<T>& list)
    : list_(list) { }

template <class T>
std::string ObservableSettingList<T>::getType() const {
    return "list_setting";
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
void ObservableSettingList<T>::to_json(nlohmann::json& j) const {
    ObservableSetting<T>::to_json(j);
    j["list"] = list_;
}

template <class T>
void ObservableSettingList<T>::from_json(const nlohmann::json& j) {
    if (j.find("list") != j.end()) {
        setList(j["list"].get<std::vector<T>>());
    }

    ObservableSetting<T>::from_json(j);
}

} // namespace properties
} // namespace common
} // namespace urf
