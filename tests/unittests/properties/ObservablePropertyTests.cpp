#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "urf/common/properties/ObservableProperty.hpp"

using namespace urf::common::properties;

TEST(ObservablePropertyShould, correctlyGetType) {
    ObservableProperty<float> prop;
    ASSERT_EQ(prop.getType(), "property");

    ObservableSetting<int32_t> setting;
    ASSERT_EQ(setting.getType(), "setting");

    ObservableSettingRanged<uint32_t> rangeSetting;
    ASSERT_EQ(rangeSetting.getType(), "range_setting");

    ObservableSettingList<std::string> listSetting;
    ASSERT_EQ(listSetting.getType(), "list_setting");
}

TEST(ObservablePropertyShould, correctlySerializeDeserialize) {
    nlohmann::json j;
    ObservableProperty<float> prop;
    prop.setValue(0.43f);
    j = prop;
    ASSERT_NEAR(j["value"].get<float>(), 0.43f, 1e-5);
    j["value"] = 0.23f;
    prop = j;
    ASSERT_NEAR(prop.getValue(), 0.23f, 1e-5);

    ObservableSetting<int32_t> setting;
    setting.setValue(10);
    
    j = setting;
    ASSERT_EQ(j["value"].get<int32_t>(), 10);
    j["value"] = 5;
    setting = j;
    ASSERT_EQ(setting.getValue(), 5);

    ObservableSettingRanged<uint32_t> rangeSetting;
    rangeSetting.setValue(10);
    j = rangeSetting;
    ASSERT_EQ(j["value"].get<uint32_t>(), 10);
    j["value"] = 3;
    rangeSetting = j;
    ASSERT_EQ(rangeSetting.getValue(), 3);

    ObservableSettingList<std::string> listSetting;
    listSetting.setValue("hello");
    j = listSetting;
    ASSERT_EQ(j["value"].get<std::string>(), "hello");
    j["value"] = "world";
    listSetting = j;
    ASSERT_EQ(listSetting.getValue(), "world");
}