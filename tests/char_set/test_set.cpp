//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include "abulafia/abulafia.h"
#include "gtest/gtest.h"

using namespace abu;

TEST(test_set, create_from_vector) {
  std::vector<char> v{'a', 'g', 'z'};
  auto set = char_set::set(v.begin(), v.end());

  static_assert(char_set::is_char_set<decltype(set)>::value,
                "must be a valid char set");

  using char_set_type = decltype(set);
  static_assert(is_same<char_set_type::char_t, char>::value,
                "type inference failure");

  EXPECT_TRUE(set.is_valid('a'));
  EXPECT_TRUE(set.is_valid('g'));
  EXPECT_TRUE(set.is_valid('z'));
  EXPECT_FALSE(set.is_valid('A'));
  EXPECT_FALSE(set.is_valid('f'));
}

TEST(test_set, create_from_string) {
  std::string string_data = "agz";
  auto set = char_set::set(string_data.begin(), string_data.end());

  static_assert(char_set::is_char_set<decltype(set)>::value,
                "must be a valid char set");

  using char_set_type = decltype(set);
  static_assert(is_same<char_set_type::char_t, char>::value,
                "type inference failure");

  EXPECT_TRUE(set.is_valid('a'));
  EXPECT_TRUE(set.is_valid('g'));
  EXPECT_TRUE(set.is_valid('z'));
  EXPECT_FALSE(set.is_valid('A'));
  EXPECT_FALSE(set.is_valid('f'));
}

TEST(test_set, create_string_literal) {
  auto set = char_set::set("agz");

  static_assert(char_set::is_char_set<decltype(set)>::value,
                "must be a valid char set");

  using char_set_type = decltype(set);
  static_assert(is_same<char_set_type::char_t, char>::value,
                "type inference failure");

  EXPECT_TRUE(set.is_valid('a'));
  EXPECT_TRUE(set.is_valid('g'));
  EXPECT_TRUE(set.is_valid('z'));
  EXPECT_FALSE(set.is_valid('A'));
  EXPECT_FALSE(set.is_valid('f'));

  // tests for bug: the null terminatpr of the string is counted.
  EXPECT_FALSE(set.is_valid('\0'));
}

TEST(test_set, string_literal_in_expression) {
  auto set = char_set::range('a', 'z') | "_!@";

  static_assert(char_set::is_char_set<decltype(set)>::value,
                "must be a valid char set");

  using char_set_type = decltype(set);
  static_assert(is_same<char_set_type::char_t, char>::value,
                "type inference failure");

  EXPECT_TRUE(set.is_valid('a'));
  EXPECT_TRUE(set.is_valid('g'));
  EXPECT_TRUE(set.is_valid('!'));
  EXPECT_FALSE(set.is_valid('A'));

  // tests for bug: the null terminatpr of the string is counted.
  EXPECT_FALSE(set.is_valid('\0'));
}
