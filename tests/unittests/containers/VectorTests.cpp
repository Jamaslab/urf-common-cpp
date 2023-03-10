#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <urf/common/containers/vector.hpp>

using urf::common::containers::vector;

TEST(VectorShould, correctlyAllocate) {
    vector<uint8_t> vec;

    ASSERT_EQ(vec.use_count(), 1);
    ASSERT_EQ(vec.size(), 0);
    ASSERT_TRUE(vec.empty());
}

TEST(VectorShould, correctlyAllocateWithSize) {
    vector<uint8_t> vec(20);
    ASSERT_EQ(vec.size(), 20);
    ASSERT_FALSE(vec.empty());
}

TEST(VectorShould, correctlyAllocateWithValue) {
    vector<uint8_t> vec(20, 2);
    ASSERT_EQ(vec.size(), 20);
    ASSERT_FALSE(vec.empty());

    for (size_t i = 0; i < vec.size(); i++) {
        ASSERT_EQ(vec[i], 2);
    }
}

TEST(VectorShould, correctlyAllocateFromStdVector) {
    std::vector<uint8_t> stdVec({0, 1, 2, 3, 4});
    vector<uint8_t> vec(stdVec.begin(), stdVec.end());
    ASSERT_EQ(vec.size(), 5);
    ASSERT_FALSE(vec.empty());

    for (size_t i = 0; i < vec.size(); i++) {
        ASSERT_EQ(vec[i], i);
    }
}

TEST(VectorShould, updateRefCount) {
    vector<uint8_t> vec;

    ASSERT_EQ(vec.use_count(), 1);

    vector<uint8_t> vec2(vec);

    ASSERT_EQ(vec.use_count(), 2);
    ASSERT_EQ(vec2.use_count(), 2);

    vector<uint8_t> vec3(std::move(vec));
    ASSERT_EQ(vec3.use_count(), 2);
    ASSERT_EQ(vec2.use_count(), 2);
}

TEST(VectorShould, clone) {
    vector<uint8_t> vec(20, 2);

    auto vec2 = vec.clone();

    ASSERT_EQ(vec.use_count(), 1);
    ASSERT_EQ(vec2.use_count(), 1);

    for (size_t i = 0; i < vec.size(); i++) {
        ASSERT_EQ(vec[i], vec2[i]);
    }
}

TEST(VectorShould, transfer) {
    vector<uint8_t> vec(20, 2);

    auto ptr = vec.transfer();
    ASSERT_THROW(vec.use_count(), std::runtime_error);

    ASSERT_NE(ptr, nullptr);

    for (size_t i = 0; i < 20; i++) {
        ASSERT_EQ(ptr[i], 2);
    }
}

TEST(VectorShould, iterator) {
    vector<uint8_t> vec(20, 2);

    int i = 0;
    for (auto& elem : vec) {
        elem = i;
        i++;
    }

    i = 0;
    for (auto const& elem : vec) {
        ASSERT_EQ(elem, vec[i]);
        i++;
    }
}

TEST(VectorShould, capacityTest) {
    vector<uint8_t> vec;

    ASSERT_EQ(vec.size(), 0);
    ASSERT_TRUE(vec.empty());
}

TEST(VectorShould, pushBack) {
    vector<uint8_t> vec;

    vec.push_back(1);

    ASSERT_EQ(vec.size(), 1);
    ASSERT_FALSE(vec.empty());

    ASSERT_EQ(vec[0], 1);
    ASSERT_EQ(vec.at(0), 1);
}

TEST(VectorShould, reserve) {
    vector<uint8_t> vec;

    vec.reserve(40);

    ASSERT_EQ(vec.size(), 0);
    ASSERT_EQ(vec.capacity(), 40);
    vec.resize(10);
    ASSERT_EQ(vec.size(), 10);
    ASSERT_EQ(vec.capacity(), 40);

    vec.resize(50);

    ASSERT_EQ(vec.size(), 50);
    ASSERT_EQ(vec.capacity(), 50);
}

TEST(VectorShould, resize) {
    vector<uint8_t> vec(20, 2);

    vec.resize(40);

    ASSERT_EQ(vec.size(), 40);

    vec.resize(10);

    ASSERT_EQ(vec.size(), 10);
}

TEST(VectorShould, resizeWithInit) {
    vector<uint8_t> vec(20, 2);

    vec.resize(40, 3);

    for (size_t i = 20; i < 40; i++) {
        ASSERT_EQ(vec[i], 3);
    }
}

TEST(VectorShould, comparison) {
    vector<uint8_t> vec(20, 2);
    vector<uint8_t> vec2(20, 2);

    ASSERT_TRUE(vec == vec2);
    ASSERT_FALSE(vec != vec2);
}

TEST(VectorShould, initializerList) {
    vector<uint8_t> vec({3, 4, 5, 6});
    ASSERT_EQ(vec.size(), 4);

    ASSERT_EQ(vec[0], 3);
    ASSERT_EQ(vec[1], 4);
    ASSERT_EQ(vec[2], 5);
    ASSERT_EQ(vec[3], 6);
}

TEST(VectorShould, insert) {
    vector<int> vec(20, 2);
    auto it = vec.insert(vec.begin() + 10, {0, 1, 2, 3});
    ASSERT_EQ(vec.size(), 24);

    ASSERT_EQ(*it, 0);

    for (int i = 0; i < 10; i++) {
        ASSERT_EQ(vec[i], 2);
    }

    ASSERT_EQ(vec[10], 0);
    ASSERT_EQ(vec[11], 1);
    ASSERT_EQ(vec[12], 2);
    ASSERT_EQ(vec[13], 3);

    for (size_t i = 14; i < vec.size(); i++) {
        ASSERT_EQ(vec[i], 2);
    }

    vec.insert(vec.begin(), {0, 1, 2, 3});
    ASSERT_EQ(vec.size(), 28);
    for (int i = 0; i < 4; i++) {
        ASSERT_EQ(vec[i], i);
    }

    vec.insert(vec.end(), {0, 1, 2, 3});
    ASSERT_EQ(vec.size(), 32);

    ASSERT_EQ(*(vec.end()-1), 3);
    ASSERT_EQ(*(vec.end()-2), 2);
    ASSERT_EQ(*(vec.end()-3), 1);
    ASSERT_EQ(*(vec.end()-4), 0);
}


TEST(VectorShould, insertCopy) {
    vector<int> vec(20, 2);
    vec.insert(vec.begin(), 10, 3);
    ASSERT_EQ(vec.size(), 30);

    for (int i = 0; i < 10; i++) {
        ASSERT_EQ(vec[i], 3);
    }

}
