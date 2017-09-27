//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include "abulafia/abulafia.h"
#include "gtest/gtest.h"

using namespace abu;

TEST(test_recur, weaken_non_recur) {
  struct abc;
  using Recur_t = Recur<abc>;
  auto pattern = *char_();

  using pattern_t = decltype(pattern);
  using weakened_t = decltype(weaken_recur<Recur_t>(pattern));

  static_assert(std::is_same<pattern_t, weakened_t>::value, "");

  auto weakened = weaken_recur<Recur_t>(pattern);

  static_assert(std::is_same<decltype(weakened), weakened_t>::value, "");
}

// Test basic syntax with any recursivity involved.
TEST(test_recur, simple_test) {
  struct abc;
  Recur<abc, std::vector<char>> recur;

  ABU_Recur_define(recur, abc, *char_());

  std::string dst;
  auto status = parse(recur, std::string("abc"), dst);
  EXPECT_EQ(status, result::SUCCESS);
  EXPECT_EQ("abc", dst);
}

// Test with simple recursivity
TEST(test_recur, recur_test) {
  Recur<struct abc> recur;

  auto recur_pat = Int<10, 2, 2>() >> *recur;

  ABU_Recur_define(recur, abc, recur_pat);

  //  std::vector<int> dst;
  auto status = parse(recur, std::string("123456") /*, dst*/);

  EXPECT_EQ(status, result::SUCCESS);
  //  EXPECT_EQ(std::vector<int>({12,34,56}), dst);
}
