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

TEST(test_or, simple_test) {
  
  auto a = char_set::single('a');
  auto b = char_set::single('b');

  auto a_or_b = a | b;

  static_assert(char_set::is_char_set<decltype(a_or_b)>::value,
                "must be a valid char set");

  using char_set_type = decltype(a_or_b);
  static_assert(is_same<char_set_type::char_t, char>::value,
                "type inference failure");

  EXPECT_TRUE(a_or_b.is_valid('a'));
  EXPECT_TRUE(a_or_b.is_valid('b'));
  EXPECT_FALSE(a_or_b.is_valid('c'));
}

TEST(test_or, implicit_char) {
  auto a = char_set::single('a');

  auto a_or_b = a | 'b';

  EXPECT_TRUE(a_or_b.is_valid('a'));
  EXPECT_TRUE(a_or_b.is_valid('b'));
  EXPECT_FALSE(a_or_b.is_valid('c'));
}

TEST(test_or, compund) {
  auto a = char_set::single('a');
  auto c = char_set::single('c');

  auto a_b_or_c = a | 'b' | c;

  EXPECT_TRUE(a_b_or_c.is_valid('a'));
  EXPECT_TRUE(a_b_or_c.is_valid('b'));
  EXPECT_TRUE(a_b_or_c.is_valid('c'));
  EXPECT_FALSE(a_b_or_c.is_valid('d'));
}

TEST(test_or, heterogenous) {
  auto a = char_set::single('a');
  auto m_z = char_set::range('m', 'z');

  auto char_set = a | m_z;

  EXPECT_TRUE(char_set.is_valid('a'));
  EXPECT_FALSE(char_set.is_valid('b'));
  EXPECT_TRUE(char_set.is_valid('p'));
}