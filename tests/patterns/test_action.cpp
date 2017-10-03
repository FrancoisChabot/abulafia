//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <unordered_set>
#include "abulafia/abulafia.h"
#include "gtest/gtest.h"

using namespace abu;

TEST(test_action, simple_action) {
  int count = 0;
  auto pattern = *apply_action(UInt<10, 2, 2>(), [&]() { ++count; });

  auto success = parse(pattern, "123456");
  EXPECT_EQ(result::SUCCESS, success);
  EXPECT_EQ(3, count);
}

TEST(test_action, consuming_action) {
  int count = 0;
  auto pattern =
      *apply_action(UInt<10, 2, 2>(), [&](unsigned int v) { count += v; });

  auto success = parse(pattern, "123456");
  EXPECT_EQ(result::SUCCESS, success);
  EXPECT_EQ(12 + 34 + 56, count);
}

TEST(test_action, emmiting_action) {
  auto action = [&](unsigned int v) { return v * v; };
  static_assert(
      std::is_same<callable_result_t<decltype(action)>, unsigned int>::value,
      "");
  static_assert(
      std::is_same<act_::determine_emmited_type<decltype(action)>::type,
                   unsigned int>::value,
      "");

  auto pattern = *apply_action(UInt<10, 2, 2>(), action);

  std::vector<int> result;
  auto success = parse(pattern, "123456", result);
  EXPECT_EQ(result::SUCCESS, success);
  EXPECT_EQ(std::vector<int>({12 * 12, 34 * 34, 56 * 56}), result);
}
