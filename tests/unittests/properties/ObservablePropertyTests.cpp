#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "urf/common/properties/ObservableProperty.hpp"

using namespace urf::common::properties;

TEST(ObservablePropertyShould, correctlytype) {
    ObservableProperty<float> prop;
    ASSERT_EQ(prop.type(), PropertyType::Property);

    ObservableSetting<int32_t> setting;
    ASSERT_EQ(setting.type(), PropertyType::Setting);

    ObservableSettingRanged<uint32_t> rangeSetting;
    ASSERT_EQ(rangeSetting.type(), PropertyType::RangeSetting);

    ObservableSettingList<std::string> listSetting;
    ASSERT_EQ(listSetting.type(), PropertyType::ListSetting);
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
    setting.setRequestedValue(4);

    j = setting;
    ASSERT_EQ(j["value"].get<int32_t>(), 10);
    ASSERT_EQ(j["req_value"].get<int32_t>(), 4);
    j["value"] = 5;
    j["req_value"] = 2;
    setting = j;
    ASSERT_EQ(setting.getValue(), 5);
    ASSERT_EQ(setting.getRequestedValue(), 2);
}

TEST(ObservablePropertyShould, correctlyReceiveValueChangeUpdate) {
    bool valueChanged = false;
    ObservableProperty<float> prop;
    prop.setValue(0);
    prop.onValueChange([&valueChanged](float prev, float current) {
        valueChanged = true;
        ASSERT_NEAR(prev, 0, 1e-5);
        ASSERT_NEAR(current, 0.43f, 1e-5);
    });
    prop.setValue(0.43f);
    ASSERT_TRUE(valueChanged);
}

TEST(ObservablePropertyShould, correctlyReceiveValueChangeUpdateNonTemplated) {
    bool valueChanged = false;
    ObservableProperty<float> prop;
    prop.setValue(0);
    prop.onAnyValueChange([&valueChanged](auto, auto) {
        valueChanged = true;
    });
    prop.setValue(0.43f);
    ASSERT_TRUE(valueChanged);
}

TEST(ObservablePropertyShould, correctlyReceiveRequestedValueChangeUpdate) {
    bool valueChanged = false;
    ObservableSetting<float> prop;
    prop.setRequestedValue(0);
    prop.onRequestedValueChange([&valueChanged](float prev, float current) {
        valueChanged = true;
        ASSERT_NEAR(prev, 0, 1e-5);
        ASSERT_NEAR(current, 0.43f, 1e-5);
    });
    prop.setRequestedValue(0.43f);
    ASSERT_TRUE(valueChanged);
}

TEST(ObservablePropertyShould, correctlyGetDatatypes) {
    ASSERT_EQ(getTemplateDatatype<std::vector<float>>()(), "vector/float32");
}

TEST(ObservablePropertyShould, correctlyAssignEigenMatrix) {
    ObservableProperty<Eigen::MatrixXf> matrix;

    ASSERT_EQ(matrix.getValue().rows(), 0);
    matrix.setValue(Eigen::MatrixXf(4,4));
    ASSERT_EQ(matrix.getValue().rows(), 4);
}

TEST(ObservablePropertyShould, buildNodeStructure) {
    PropertyNode node;

    auto matrix = std::make_shared<ObservableProperty<Eigen::MatrixXf>>();
    auto setting = std::make_shared<ObservableSetting<float>>();
    auto ranged = std::make_shared<ObservableSettingRanged<uint32_t>>();

    auto subnode = std::make_shared<PropertyNode>();
    auto subsetting = std::make_shared<ObservableSetting<float>>();

    subnode->insert("subsetting", subsetting);

    node.insert("matrix", matrix);
    node.insert("setting", setting);
    node.insert("ranged", ranged);
    node.insert("node", subnode);

    nlohmann::json serialized = node;
    PropertyNode backNode;
    ASSERT_NO_THROW(from_json(serialized, backNode));

    ASSERT_TRUE(backNode.has("matrix"));
    ASSERT_TRUE(backNode.has("setting"));
    ASSERT_TRUE(backNode.has("ranged"));
    ASSERT_TRUE(backNode.has("node"));
}
