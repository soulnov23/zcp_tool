#include "src/base/yaml_parser.h"

#include "gtest/gtest.h"
#include "src/base/log.h"

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

TEST_F(test_fixture_t, yaml_load_file) {
    YAML::Node root;
    // IsDefined无法判断节点是否有效，还是要用IsNull
    EXPECT_FALSE(root.IsDefined());
    EXPECT_TRUE(root.IsNull());
    EXPECT_EQ(0, yaml_load_file("test.yaml", root));
    EXPECT_TRUE(root.IsDefined());
    EXPECT_FALSE(root.IsNull());
    try {
        std::string language = yaml_get_value<std::string>(root, "language");
        YAML::Node matrix_node = yaml_get_value<YAML::Node>(root, "matrix");
        YAML::Node include_node = yaml_get_value<YAML::Node>(matrix_node, "include");
        for (auto include : include_node) {
            std::string os = yaml_get_value<std::string>(include, "os");
            std::string compiler = yaml_get_value<std::string>(include, "compiler");
            LOG_DEBUG("os: {}, compiler: {}", os, compiler);
        }

    } catch (const std::exception& ex) {
        LOG_ERROR("{}", ex.what());
    } catch (...) {
        LOG_ERROR("unknow exception");
    }
}