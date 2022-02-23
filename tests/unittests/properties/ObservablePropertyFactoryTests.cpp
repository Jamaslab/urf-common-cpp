#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "urf/common/properties/ObservablePropertyFactory.hpp"

using namespace urf::common::properties;

TEST(ObservablePropertyFactoryShould, correctlyCreateDatatypes) {
    auto prop = ObservablePropertyFactory::create("vector/float32", PropertyType::ListSetting);
    ASSERT_EQ(prop->datatype(), "vector/float32");
    ASSERT_EQ(prop->type(), PropertyType::ListSetting);

    prop = ObservablePropertyFactory::create("matrix/float32", PropertyType::Property);
    ASSERT_EQ(prop->datatype(), "matrix/float32");
    ASSERT_EQ(prop->type(), PropertyType::Property);

    prop = ObservablePropertyFactory::create("float32", PropertyType::ListSetting);
    ASSERT_EQ(prop->datatype(), "float32");
    ASSERT_EQ(prop->type(), PropertyType::ListSetting);

    prop = ObservablePropertyFactory::create("float64", PropertyType::RangeSetting);
    ASSERT_EQ(prop->datatype(), "float64");
    ASSERT_EQ(prop->type(), PropertyType::RangeSetting);

    prop = ObservablePropertyFactory::create("bool", PropertyType::Setting);
    ASSERT_EQ(prop->datatype(), "bool");
    ASSERT_EQ(prop->type(), PropertyType::Setting);

    prop = ObservablePropertyFactory::create("uint8", PropertyType::Property);
    ASSERT_EQ(prop->datatype(), "uint8");
    ASSERT_EQ(prop->type(), PropertyType::Property);
}
