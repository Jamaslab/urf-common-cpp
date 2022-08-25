#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "urf/common/components/IComponent.hpp"

using namespace urf::common::properties;
using namespace urf::common::components;

class TestComponent : public IComponent {
 public:
    TestComponent();

    bool resetFault() noexcept override {
        return true;
    }

    /** Goes to switched on **/
    bool switchOn() noexcept override {
        return true;
    }

    /** Goes to ready to Switch On **/
    bool shutdown() noexcept override {
        return true;
    }

    /** Goes to switched on **/
    bool disable() noexcept override {
        return true;
    }

    /** Goes to enabled **/
    bool enable() noexcept override {
        return true;
    }

    /** Goes to switched on and back to enabled without changing currentState unless it can't re-enable**/
    bool reconfigure() noexcept override {
        return true;
    }

    /** A fault can be manually triggered due to external conditions **/
    bool fault() noexcept override {
        return true;
    }

    ComponentStates currentState() const override {
        return ComponentStates::Enabled;
    }
    ComponentStateTransitions currentTransition() const override {
        return ComponentStateTransitions::NoTransition;
    }

    std::string componentName() const override {
        return "test";
    }
    std::vector<std::string> componentClass() const override {
        return {"test"};
    }

    PropertyNode settings() const override {
        return settings_;
    }

 private:
    PropertyNode settings_;
};

TestComponent::TestComponent() {
    auto setting = std::make_shared<ObservableProperty<float>>();
    auto ranged = std::make_shared<ObservableSettingRanged<uint32_t>>();

    auto subnode = std::make_shared<PropertyNode>();
    auto subsetting = std::make_shared<ObservableSetting<uint32_t>>();

    subnode->insert("subsetting", subsetting);

    settings_.insert("setting", setting);
    settings_.insert("ranged", ranged);
    settings_.insert("node", subnode);
}

TEST(ComponentsShould, correctlyGet) {
    TestComponent component;

    ASSERT_FALSE(component.get<std::string>("asdasd"));
    ASSERT_FALSE(component.get<std::string>("subnode"));
    ASSERT_FALSE(component.get<std::string>("setting"));
    ASSERT_TRUE(component.get<float>("setting"));

    ASSERT_FALSE(component.get<float>("node", "Asdasd"));
    ASSERT_FALSE(component.get<float>("node", "subsetting"));
    ASSERT_TRUE(component.get<uint32_t>("node", "subsetting"));
}

TEST(ComponentsShould, correctlySet) {
    TestComponent component;

    ASSERT_FALSE(component.set<std::string>("test", "setting"));
    ASSERT_FALSE(component.set<float>(0.05f, "setting"));

    ASSERT_TRUE(component.set<uint32_t>(123, "node", "subsetting"));
}