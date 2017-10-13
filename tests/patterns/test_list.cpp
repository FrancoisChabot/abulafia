//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include "abulafia/abulafia.h"
#include "gtest/gtest.h"
#include "test_utils.h"

using namespace abu;

TEST(test_list, simple_test) {
  auto pattern = uint_ % ',';

  testPatternSuccess("1,5,12,125", pattern, std::vector<int>{{1, 5, 12, 125}});
}

TEST(test_list, long_separator) {
  auto pattern = char_('a', 'z') % uint_;

  std::vector<char> dst;

  testPatternSuccess("a12b0c2354d", pattern, std::string("abcd"));
}

TEST(test_list, single_val) {
  // comma-separated list of uints
  auto pattern = uint_ % ',';

  testPatternSuccess("12", pattern, std::vector<int>{{12}});
}

TEST(test_list, list_of_lists) {
  // comma-separated list of uints
  auto pattern = (uint_ % ',') % ';';

  // unlike other repeat parsers, the list parser DOES create
  // collections of collections by default
  std::vector<std::vector<int>> expected;

  expected.emplace_back(std::vector<int>{12, 13});
  expected.emplace_back(std::vector<int>{14, 15, 16});

  testPatternSuccess("12,13;14,15,16", pattern, expected);
  /*

    // While this probably won't see much use, it still should work because
    std::vector<int> alt_dst;
    status = parse(pattern, "12,13;14,15,16", alt_dst);
    EXPECT_EQ(status, result::SUCCESS);

    ASSERT_EQ(5U, alt_dst.size());
    EXPECT_EQ(12U, alt_dst[0]);
    EXPECT_EQ(13U, alt_dst[1]);
    EXPECT_EQ(14U, alt_dst[2]);
    EXPECT_EQ(15U, alt_dst[3]);
    EXPECT_EQ(16U, alt_dst[4]);
    */
}

TEST(test_list, list_of_unstable_append) {
  // comma separated list of between 2 and three single digit uints
  auto pattern = (repeat<2, 3>(UInt<10, 1, 1>()) % ',');

  std::vector<std::vector<int>> expected;
  expected.emplace_back(std::vector<int>{1, 1});
  expected.emplace_back(std::vector<int>{2, 3, 4});

  testPatternSuccess("11,234", pattern, expected);

  /*
    std::vector<unsigned int> dst_alt;
    status = parse(pattern, "11,234", dst_alt);
    EXPECT_EQ(status, result::SUCCESS);
    ASSERT_EQ(5U, dst_alt.size());
    EXPECT_EQ(1U, dst_alt[0]);
    EXPECT_EQ(1U, dst_alt[1]);
    EXPECT_EQ(2U, dst_alt[2]);
    EXPECT_EQ(3U, dst_alt[3]);
    EXPECT_EQ(4U, dst_alt[4]);
    */
}
