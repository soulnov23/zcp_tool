#include <iostream>
#include <map>
#include <string>

#include "gtest/gtest.h"
#include "src/base/map_util.h"

class test_fixture_t : public testing::Test {
public:
    // Sets up the stuff shared by all tests in this test suite.
    //
    // Google Test will call Foo::SetUpTestSuite() before running the first
    // test in test suite Foo.  Hence a sub-class can define its own
    // SetUpTestSuite() method to shadow the one defined in the super
    // class.
    static void SetUpTestSuite() {}

    // Tears down the stuff shared by all tests in this test suite.
    //
    // Google Test will call Foo::TearDownTestSuite() after running the last
    // test in test suite Foo.  Hence a sub-class can define its own
    // TearDownTestSuite() method to shadow the one defined in the super
    // class.
    static void TearDownTestSuite() {}

    // Sets up the test fixture.
    void SetUp() override {
        map_data_["a"] = "1";
        map_data_["b"] = "2";
        map_data_["c"] = "3";
    }

    // Tears down the test fixture.
    void TearDown() override {}

    std::map<std::string, std::string> map_data_;
};

TEST_F(test_fixture_t, map_get_value) {
    EXPECT_EQ("1", map_get_value(map_data_, "a"));
    EXPECT_EQ("2", map_get_value(map_data_, "b"));
    EXPECT_EQ("3", map_get_value(map_data_, "c"));
    EXPECT_EQ("", map_get_value(map_data_, "e"));
}

TEST_F(test_fixture_t, map_get_default_value) {
    EXPECT_EQ("1", map_get_value(map_data_, "d", std::move(std::string("1"))));
    EXPECT_EQ("2", map_get_value(map_data_, "e", std::move(std::string("2"))));
    EXPECT_EQ("3", map_get_value(map_data_, "f", std::move(std::string("3"))));
    EXPECT_EQ("", map_get_value(map_data_, "g", std::move(std::string(""))));
}