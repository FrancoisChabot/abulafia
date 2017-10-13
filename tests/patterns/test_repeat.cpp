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

TEST(test_repeat, string_test) {
  auto pattern = *char_();

  testPatternSuccess("abc", pattern, std::string("abc"));
}

TEST(test_repeat, simple_test) {
  auto pattern = *UInt<10, 2, 2>();

  testPatternSuccess("2345", pattern, std::vector<int>({23, 45}));
  testPatternSuccess("abc", pattern, std::vector<int>());
  testPatternSuccess("", pattern, std::vector<int>());
}

TEST(test_repeat, test_plus) {
  auto pattern = +UInt<10, 2, 2>();

  testPatternSuccess("23", pattern, std::vector<int>({23}));
  testPatternFailure<std::vector<int>>("abc", pattern);
  testPatternFailure<std::vector<int>>("", pattern);
}

TEST(test_repeat, test_max_count) {
  auto pattern = repeat<0, 3>(UInt<10, 1, 1>());

  std::vector<int> dst;

  testPatternSuccess("1234", pattern, std::vector<int>({1, 2, 3}));
}

TEST(test_repeat, test_exact_count) {
  auto pattern = repeat<2, 2>(lit('9'));

  testPatternSuccess("99", pattern, nil);
  testPatternFailure<Nil>("9", pattern);
}
/*
TEST(test_repeat, test_chained_repeat_with_min) {
  auto pattern = *(repeat<2, 2>(char_('a', 'z')));

  std::string dst;

  auto status = parse("aabbcc", pattern, dst);
  EXPECT_EQ(status, Result::SUCCESS);
  EXPECT_EQ("aabbcc", dst);

  // make sure that failures do not affect result
  status = parse("aabbccd", pattern, dst);
  EXPECT_EQ(status, Result::SUCCESS);
  EXPECT_EQ("aabbcc", dst);
}

TEST(test_repeat, test_chained_repeat_with_min_double) {
  // this will cause us to insert() on a wrapper
  auto pattern = *+(repeat<2, 2>(char_('a', 'z')));

  std::string dst;

  auto status = parse(pattern, "aabbcc", dst);
  EXPECT_EQ(status, result::SUCCESS);
  EXPECT_EQ("aabbcc", dst);

  // make sure that failures do not affect result
  status = parse(pattern, "aabbccd", dst);
  EXPECT_EQ(status, result::SUCCESS);
  EXPECT_EQ("aabbcc", dst);
}
*/