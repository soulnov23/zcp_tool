#include "src/base/log.h"

#include <stdio.h>

#include <fstream>
#include <regex>
#include <string>
#include <thread>
#include <vector>

#include "gtest/gtest.h"

namespace {

    const char* LOG_DIR = "./log_test_data";

    std::string log_file_path(const std::string& file_name) { return std::string(LOG_DIR) + "/" + file_name + ".log"; }

    logger_config make_config(const std::string& file_name, spdlog::level::level_enum level = spdlog::level::trace,
                              const std::string& format = DEFAULT_FORMAT) {
        logger_config config;
        config.name = "test_logger";
        config.level = level;
        config.format = format;
        config.file_name = log_file_path(file_name);
        return config;
    }

    std::vector<std::string> read_lines(const std::string& file_path) {
        std::vector<std::string> lines;
        std::ifstream file(file_path);
        std::string line;
        while (std::getline(file, line)) {
            if (!line.empty()) {
                lines.push_back(line);
            }
        }
        return lines;
    }

}  // namespace

class log_test_t : public testing::Test {
public:
    // 每个用例自己调set_config换掉logger，这里只清掉上一轮的日志文件
    void SetUp() override { ::system((std::string("rm -rf ") + LOG_DIR).c_str()); }
};

// console和file同时输出，两边内容一致
TEST_F(log_test_t, console_and_file) {
    ASSERT_EQ(0, logger::instance().set_config(make_config("both")));
    // 捕获stdout，验证console那一路也写了
    ::testing::internal::CaptureStdout();
    LOG_INFO("hello {}", "world");
    std::string console_out = ::testing::internal::GetCapturedStdout();

    std::vector<std::string> lines = read_lines(log_file_path("both"));
    ASSERT_EQ(1u, lines.size());
    EXPECT_TRUE(lines[0].ends_with(" hello world")) << lines[0];
    // console的内容和文件里的一模一样，只多一个换行
    EXPECT_EQ(lines[0] + "\n", console_out);
}

// 六个级别，级别名是spdlog的叫法
TEST_F(log_test_t, level) {
    ASSERT_EQ(0, logger::instance().set_config(make_config("level")));
    std::string hello("hello");
    LOG_TRACE("trace message");
    LOG_DEBUG("{} {}", hello, "world");
    LOG_INFO("{1} {0}", "world", hello);
    LOG_WARN("warn message");
    LOG_ERROR("error message");
    LOG_CRITICAL("critical message");

    std::vector<std::string> lines = read_lines(log_file_path("level"));
    ASSERT_EQ(6u, lines.size());
    // DEFAULT_FORMAT的整行形态：
    // [2026-10-12 22:30:00.123456] [12345] [debug] [log_test.cpp:42 TestBody()] hello world
    EXPECT_TRUE(std::regex_match(lines[1], std::regex(R"(^\[\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2}\.\d{6}\] \[\d+\] )"
                                                      R"(\[debug\] \[log_test\.cpp:\d+ TestBody\(\)\] hello world$)")))
        << lines[1];
    const char* expect_levels[] = {"trace", "debug", "info", "warning", "error", "critical"};
    for (size_t i = 0; i < lines.size(); ++i) {
        EXPECT_NE(std::string::npos, lines[i].find(std::string("] [") + expect_levels[i] + "] [")) << lines[i];
    }
    // 占位符按位置和序号两种写法都要支持
    EXPECT_TRUE(lines[2].ends_with(" hello world")) << lines[2];
}

// 低于配置级别的日志整条丢弃
TEST_F(log_test_t, level_filter) {
    ASSERT_EQ(0, logger::instance().set_config(make_config("filter", spdlog::level::warn)));
    LOG_DEBUG("dropped");
    LOG_INFO("dropped");
    LOG_WARN("kept");
    LOG_ERROR("kept");

    std::vector<std::string> lines = read_lines(log_file_path("filter"));
    ASSERT_EQ(2u, lines.size());
    EXPECT_NE(std::string::npos, lines[0].find("] [warning] ["));
    EXPECT_NE(std::string::npos, lines[1].find("] [error] ["));
}

// format就是spdlog的pattern，可以自由增删字段
TEST_F(log_test_t, custom_format) {
    // 只留级别、名字、函数名和消息，去掉时间和文件行号
    ASSERT_EQ(0, logger::instance().set_config(make_config("custom", spdlog::level::trace, "[%n] [%l] %!() %v")));
    LOG_INFO("hello");

    std::vector<std::string> lines = read_lines(log_file_path("custom"));
    ASSERT_EQ(1u, lines.size());
    // %!是函数名，在gtest用例体里就是TestBody
    EXPECT_EQ("[test_logger] [info] TestBody() hello", lines[0]);
}

// errno和错误描述附在消息末尾
TEST_F(log_test_t, system_error) {
    ASSERT_EQ(0, logger::instance().set_config(make_config("syserr")));
    errno = ENOENT;
    LOG_SYSTEM_ERROR("open file: {}", "/not/exist");

    std::vector<std::string> lines = read_lines(log_file_path("syserr"));
    ASSERT_EQ(1u, lines.size());
    EXPECT_NE(std::string::npos, lines[0].find("] [error] ["));
    EXPECT_TRUE(lines[0].ends_with(" open file: /not/exist errno: 2, errmsg: No such file or directory")) << lines[0];
}

// 多线程并发写，不能出现交错或丢行
TEST_F(log_test_t, concurrent_write) {
    ASSERT_EQ(0, logger::instance().set_config(make_config("concurrent")));
    constexpr int THREAD_NUM = 8;
    constexpr int PER_THREAD = 125;
    std::vector<std::thread> threads;
    for (int i = 0; i < THREAD_NUM; ++i) {
        threads.emplace_back([i]() {
            for (int j = 0; j < PER_THREAD; ++j) {
                LOG_INFO("thread {} seq {}", i, j);
            }
        });
    }
    for (std::thread& thread : threads) {
        thread.join();
    }

    std::vector<std::string> lines = read_lines(log_file_path("concurrent"));
    ASSERT_EQ(static_cast<size_t>(THREAD_NUM * PER_THREAD), lines.size());
    // 每行都必须是完整的一条，交错写入会破坏这个形态
    std::vector<int> count(THREAD_NUM, 0);
    std::regex pattern(R"(^.+ \[info\] \[log_test\.cpp:\d+ .+\(\)\] thread (\d+) seq \d+$)");
    for (const std::string& line : lines) {
        std::smatch match;
        ASSERT_TRUE(std::regex_match(line, match, pattern)) << line;
        ++count[std::stoi(match[1])];
    }
    for (int i = 0; i < THREAD_NUM; ++i) {
        EXPECT_EQ(PER_THREAD, count[i]);
    }
}

// 文件打不开时返回-1，且不影响已经生效的输出
TEST_F(log_test_t, invalid_config) {
    ASSERT_EQ(0, logger::instance().set_config(make_config("valid")));
    logger_config bad = make_config("bad");
    // 目录位置是个已存在的普通文件，创建目录会失败
    bad.file_name = log_file_path("valid") + "/sub/run";
    EXPECT_EQ(-1, logger::instance().set_config(bad));

    LOG_INFO("still works");
    std::vector<std::string> lines = read_lines(log_file_path("valid"));
    // 一条是set_config失败时打的错误日志，一条是上面这条
    ASSERT_EQ(2u, lines.size());
    EXPECT_NE(std::string::npos, lines[0].find("] [error] ["));
    EXPECT_TRUE(lines[1].ends_with(" still works"));
}
