#include <gtest/gtest.h>
#include <iostream>
#include <string>
#include <vector>
#include "utils/utils.h"
// 导入待测函数
using namespace nbc::utils;

TEST(RSplit1Test, NoDelimiter) {
    // 测试没有分隔符的情况
    std::string str = "hello";
    auto result = rsplit1(str, ',');
    EXPECT_EQ(result.first, "hello");
    EXPECT_EQ(result.second, "");
}

TEST(RSplit1Test, DelimiterAtEnd) {
    // 测试分隔符在末尾的情况
    std::string str = "hello,";
    auto result = rsplit1(str, ',');
    EXPECT_EQ(result.first, "hello");
    EXPECT_EQ(result.second, "");
}

TEST(RSplit1Test, DelimiterInMiddle) {
    // 测试分隔符在中间的情况
    std::string str = "hello,world";
    auto result = rsplit1(str, ',');
    EXPECT_EQ(result.first, "hello");
    EXPECT_EQ(result.second, "world");
}



TEST(SplitTest, BasicSplitting) {
    std::string str = "apple,banana,cherry";
    char delimiter = ',';

    std::vector<std::string> expected = { "apple", "banana", "cherry" };
    std::vector<std::string> result = split(str, delimiter);

    EXPECT_EQ(result, expected);
}

TEST(SplitTest, EmptyString) {
    std::string str = "";
    char delimiter = ',';

    std::vector<std::string> expected = {};
    std::vector<std::string> result = split(str, delimiter);

    EXPECT_EQ(result, expected);
}

TEST(SplitTest, NoDelimiter) {
    std::string str = "applebananacherry";
    char delimiter = ',';

    std::vector<std::string> expected = { "applebananacherry" };
    std::vector<std::string> result = split(str, delimiter);

    EXPECT_EQ(result, expected);
}

TEST(SplitTest, MultipleDelimiters) {
    std::string str = "apple,,banana,cherry";
    char delimiter = ',';

    std::vector<std::string> expected = { "apple", "", "banana", "cherry" };
    std::vector<std::string> result = split(str, delimiter);

    EXPECT_EQ(result, expected);
}

TEST(JoinTest, Basic) {
    std::vector<std::string> strs = { "Hello", "World" };
    char delimiter = ' ';
    std::string expected = "Hello World";
    std::string result = join(strs, delimiter);
    EXPECT_EQ(result, expected);
}

TEST(JoinTest, EmptyVector) {
    std::vector<std::string> strs;
    char delimiter = ' ';
    std::string expected = "";
    std::string result = join(strs, delimiter);
    EXPECT_EQ(result, expected);
}

TEST(JoinTest, OneElement) {
    std::vector<std::string> strs = { "Hello" };
    char delimiter = ' ';
    std::string expected = "Hello";
    std::string result = join(strs, delimiter);
    EXPECT_EQ(result, expected);
}

TEST(JoinTest, DifferentDelimiter) {
    std::vector<std::string> strs = { "Hello", "World" };
    char delimiter = ',';
    std::string expected = "Hello,World";
    std::string result = join(strs, delimiter);
    EXPECT_EQ(result, expected);
}

TEST(Split1Test, BasicSplit) {
    // 测试基本分割情况
    std::string str = "hello,world";
    char delimiter = ',';
    auto result = split1(str, delimiter);
    EXPECT_EQ(result.first, "hello");
    EXPECT_EQ(result.second, "world");
}

TEST(Split1Test, NoDelimiter) {
    // 测试没有分隔符的情况
    std::string str = "hello";
    char delimiter = ',';
    auto result = split1(str, delimiter);
    EXPECT_EQ(result.first, "hello");
    EXPECT_EQ(result.second, "");
}

TEST(Split1Test, EmptyString) {
    // 测试空字符串的情况
    std::string str = "";
    char delimiter = ',';
    auto result = split1(str, delimiter);
    EXPECT_EQ(result.first, "");
    EXPECT_EQ(result.second, "");
}

TEST(Split1Test, DelimiterAtBeginning) {
    // 测试分隔符在开头的情况
    std::string str = ",world,wtf，赛风";
    char delimiter = ',';
    auto result = split1(str, delimiter);
    EXPECT_EQ(result.first, "");
    EXPECT_EQ(result.second, "world,wtf，赛风");
}

TEST(Split1Test, DelimiterAtEnd) {
    // 测试分隔符在结尾的情况
    std::string str = "hello,";
    char delimiter = ',';
    auto result = split1(str, delimiter);
    EXPECT_EQ(result.first, "hello");
    EXPECT_EQ(result.second, "");
}

TEST(is_running, test) {
    EXPECT_EQ(is_running("ls"), false);
    EXPECT_EQ(is_running("./{test-LKJS&}"), false);
    EXPECT_EQ(is_running("./{test-LKJS&}"), true);
    EXPECT_EQ(is_running("utils"), false);
    EXPECT_EQ(is_running("utils"), true);
    EXPECT_EQ(is_running("ls"), true);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
