#include "src/base/xml_parser.h"

#include <iostream>
#include <map>
#include <string>

#include "gtest/gtest.h"
#include "src/base/file_utils.h"

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
    void SetUp() override {}

    // Tears down the test fixture.
    void TearDown() override {}

    std::map<std::string, std::string> map_data_;
};

TEST_F(test_fixture_t, xml_load_file) {
    tinyxml2::XMLDocument doc;
    EXPECT_TRUE(xml_load_file("test.xml", doc));
}

TEST_F(test_fixture_t, xml_load_data) {
    tinyxml2::XMLDocument doc;
    std::string data = file_to_string("test.xml");
    EXPECT_FALSE(data.empty());
    EXPECT_TRUE(xml_load_data(data, doc));
}