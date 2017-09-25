//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include "abulafia/abulafia.h"
#include "gtest/gtest.h"

#include <cctype>

using namespace abu;

namespace {
bool validity_test(char c) { return c == 'a' || c == 'z'; }
}  // namespace

TEST(test_delegated, test_function) {
  auto char_set = char_set::delegated(&validity_test);

  static_assert(char_set::is_char_set<decltype(char_set)>::value,
                "any must be a valid char set");

  using char_set_type = decltype(char_set);
  static_assert(is_same<char_set_type::char_t, char>::value,
                "type inference failure");

  EXPECT_TRUE(char_set.is_valid('a'));
  EXPECT_TRUE(char_set.is_valid('z'));
  EXPECT_FALSE(char_set.is_valid('b'));
}

TEST(test_delegated, test_lambda) {
  auto char_set = char_set::delegated([](char c) { return c < 'm'; });

  using char_set_type = decltype(char_set);
  static_assert(is_same<char_set_type::char_t, char>::value,
                "type inference failure");

  EXPECT_TRUE(char_set.is_valid('a'));
  EXPECT_TRUE(char_set.is_valid('l'));
  EXPECT_FALSE(char_set.is_valid('m'));
}

TEST(test_delegated, test_lambda_with_param) {
  char ref = 'a';
  auto char_set = char_set::delegated([&](char c) { return c == ref; });

  using char_set_type = decltype(char_set);
  static_assert(is_same<char_set_type::char_t, char>::value,
                "type inference failure");

  EXPECT_TRUE(char_set.is_valid('a'));
  EXPECT_FALSE(char_set.is_valid('b'));

  ref = 'b';

  EXPECT_FALSE(char_set.is_valid('a'));
  EXPECT_TRUE(char_set.is_valid('b'));
}