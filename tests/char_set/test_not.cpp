//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include "abulafia/abulafia.h"
#include "gtest/gtest.h"

using namespace abu;

TEST(test_not, simple_test) {
  auto base_charset = char_set::single('a');
  auto reversed = ~base_charset;

  static_assert(char_set::is_char_set<decltype(reversed)>::value,
                "must be a valid char set");

  using char_set_type = decltype(reversed);
  static_assert(is_same<char_set_type::char_t, char>::value,
                "type inference failure");

  EXPECT_FALSE(reversed.is_valid('a'));
  EXPECT_TRUE(reversed.is_valid('b'));
  EXPECT_TRUE(reversed.is_valid('A'));
  EXPECT_TRUE(reversed.is_valid(' '));
}
