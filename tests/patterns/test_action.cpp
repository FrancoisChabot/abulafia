//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <unordered_set>
#include "abulafia/abulafia.h"
#include "gtest/gtest.h"
#include "test_utils.h"
using namespace abu;

TEST(test_action, simple_action) {
  int count = 0;
  auto pattern = *apply_action(UInt<10, 2, 2>(), [&]() { ++count; });

  auto success = parse("123456", pattern);
  EXPECT_EQ(Result::SUCCESS, success);
  EXPECT_EQ(3, count);
}

TEST(test_action, consuming_action) {
  int count = 0;
  auto pattern =
      *apply_action(UInt<10, 2, 2>(), [&](unsigned int v) { count += v; });

  auto success = parse("123456", pattern);
  EXPECT_EQ(Result::SUCCESS, success);
  EXPECT_EQ(12 + 34 + 56, count);
}

TEST(test_action, emmiting_action) {
  auto action = [&](unsigned int v) { return v * v; };
  auto pattern = *apply_action(UInt<10, 2, 2>(), action);

  std::vector<int> result;
  auto success = parse("123456", pattern, result);
  EXPECT_EQ(Result::SUCCESS, success);
  EXPECT_EQ(std::vector<int>({12 * 12, 34 * 34, 56 * 56}), result);
}

TEST(test_action, nil_action_in_sequence) {
  auto action = [&](int) {};

  auto pattern = apply_skipper(int_ >> int_[action] >> int_, lit(' '));

  testPatternSuccess("1 2 3", pattern, std::tuple<int, int>(1, 3));
}
