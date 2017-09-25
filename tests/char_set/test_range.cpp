//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include "abulafia/abulafia.h"
#include "gtest/gtest.h"

using namespace abu;

TEST(test_range, simple_test) {
  auto range = char_set::range('a', 'g');

  static_assert(char_set::is_char_set<decltype(range)>::value,
                "must be a valid char set");

  using char_set_type = decltype(range);
  static_assert(is_same<char_set_type::char_t, char>::value,
                "type inference failure");

  EXPECT_TRUE(range.is_valid('a'));
  EXPECT_TRUE(range.is_valid('b'));
  EXPECT_TRUE(range.is_valid('g'));
  EXPECT_FALSE(range.is_valid('A'));
  EXPECT_FALSE(range.is_valid('h'));
}
