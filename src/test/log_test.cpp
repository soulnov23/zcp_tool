#include "src/base/log.h"

#include <string>

#include "gtest/gtest.h"

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

TEST_F(test_fixture_t, console) {
    std::string hello("hello");
    std::string world("world");
    CONSOLE_DEBUG("hello world");
    CONSOLE_DEBUG("{} {}", hello, world);
    CONSOLE_DEBUG("{} {}", world, hello);
    logger_config config;
    config.filename = "test.log";
    EXPECT_EQ(-1, logger::get_instance_atomic()->set_config(config));
    config.logger_name = "test_logger";
    EXPECT_EQ(0, logger::get_instance_atomic()->set_config(config));
    CONSOLE_DEBUG("{} {}", hello, world);
    CONSOLE_DEBUG("{} {}", world, hello);
}

TEST_F(test_fixture_t, log) {
    std::string hello("hello");
    std::string world("world");
    LOG_DEBUG("hello world");
    LOG_DEBUG("{} {}", hello, world);
    LOG_DEBUG("{1} {0}", world, hello);
    logger_config config;
    config.filename = "test.log";
    EXPECT_EQ(-1, logger::get_instance_atomic()->set_config(config));
    config.logger_name = "test_logger";
    EXPECT_EQ(0, logger::get_instance_atomic()->set_config(config));
    LOG_DEBUG("{} {}", hello, world);
    LOG_DEBUG("{1} {0}", world, hello);
}