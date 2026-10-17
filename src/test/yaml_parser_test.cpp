#include <map>
#include <string>

#include "gtest/gtest.h"
#include "src/base/log.h"
#include "yaml-cpp/yaml.h"

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

TEST_F(test_fixture_t, load_file) {
    YAML::Node root = YAML::LoadFile("test.yaml");
    EXPECT_TRUE(root.IsMap());
    EXPECT_EQ("c++", root["language"].as<std::string>());
    const YAML::Node include_node = root["matrix"]["include"];
    ASSERT_TRUE(include_node.IsSequence());
    for (const auto& include : include_node) {
        LOG_DEBUG("os: {}, compiler: {}", include["os"].as<std::string>(), include["compiler"].as<std::string>());
    }
}

TEST_F(test_fixture_t, load_file_error) {
    // 文件不存在抛BadFile，解析失败抛ParserException，两者都是YAML::Exception的派生
    EXPECT_THROW(YAML::LoadFile("no_such_file.yaml"), YAML::BadFile);
    EXPECT_THROW(YAML::Load("[1, 2"), YAML::ParserException);
}

TEST_F(test_fixture_t, missing_key) {
    const YAML::Node root = YAML::Load("a: 1");
    // const的operator[]对缺失的key返回无效节点，所以能用operator bool判断，
    // 但取值会抛InvalidNode而不是返回空值
    const YAML::Node node = root["no_such_key"];
    EXPECT_FALSE(static_cast<bool>(node));
    EXPECT_FALSE(node.IsDefined());
    EXPECT_THROW(node.as<int>(), YAML::InvalidNode);
    // as<T>(fallback)连类型错误一起吞掉，所以它只适合"值可错也无所谓"的场景
    EXPECT_EQ(5, node.as<int>(5));
    EXPECT_EQ(1, root["a"].as<int>(5));
    EXPECT_EQ(7, YAML::Load("a: not_a_number")["a"].as<int>(7));
}

TEST_F(test_fixture_t, default_node) {
    YAML::Node root;
    // 默认构造的节点是个有效的空节点：IsDefined为真，判空要用IsNull
    EXPECT_TRUE(root.IsNull());
    EXPECT_TRUE(root.IsDefined());
    EXPECT_TRUE(static_cast<bool>(root));
}
