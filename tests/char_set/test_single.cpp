//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include "abulafia/abulafia.h"
#include "gtest/gtest.h"

using namespace abu;

static_assert(char_set::is_char_set<char>::value == false);
static_assert(char_set::is_char_set<char_set::Single<char>>::value == true);
static_assert(
    char_set::is_char_set<decltype(char_set::to_char_set('a'))>::value == true);

TEST(test_single, simple_test) {
  auto c_set = char_set::single('a');

  static_assert(char_set::is_char_set<decltype(c_set)>::value,
                "must be a valid char set");

  using char_set_type = decltype(c_set);
  static_assert(is_same<char_set_type::char_t, char>::value,
                "type inference failure");

  EXPECT_TRUE(c_set.is_valid('a'));
  EXPECT_FALSE(c_set.is_valid('b'));
  EXPECT_FALSE(c_set.is_valid('g'));
  EXPECT_FALSE(c_set.is_valid('A'));
  EXPECT_FALSE(c_set.is_valid(' '));
  EXPECT_FALSE(c_set.is_valid('\0'));
}
