//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include "abulafia/abulafia.h"
#include "gtest/gtest.h"

using namespace abu;

TEST(test_digit_values, binary) {
  using binary_values_t = DigitValues<2>;

  EXPECT_TRUE(binary_values_t::is_valid('0'));
  EXPECT_TRUE(binary_values_t::is_valid('1'));
  EXPECT_FALSE(binary_values_t::is_valid('2'));

  EXPECT_EQ(0, binary_values_t::value('0'));
  EXPECT_EQ(1, binary_values_t::value('1'));
}

TEST(test_digit_values, ternary) {
  using ternary_values_t = DigitValues<3>;

  EXPECT_TRUE(ternary_values_t::is_valid('0'));
  EXPECT_TRUE(ternary_values_t::is_valid('1'));
  EXPECT_TRUE(ternary_values_t::is_valid('2'));
  EXPECT_FALSE(ternary_values_t::is_valid('3'));

  EXPECT_EQ(0, ternary_values_t::value('0'));
  EXPECT_EQ(1, ternary_values_t::value('1'));
  EXPECT_EQ(2, ternary_values_t::value('2'));
}

TEST(test_digit_values, hexadecimal) {
  using hex_values_t = DigitValues<16>;

  for (char i = 0; i < 10; ++i) {
    EXPECT_TRUE(hex_values_t::is_valid('0' + i));
  }
  for (char i = 0; i < 6; ++i) {
    EXPECT_TRUE(hex_values_t::is_valid('a' + i));
    EXPECT_TRUE(hex_values_t::is_valid('A' + i));
  }

  EXPECT_EQ(10, hex_values_t::value('a'));
  EXPECT_EQ(10, hex_values_t::value('A'));
  EXPECT_EQ(15, hex_values_t::value('f'));
  EXPECT_EQ(15, hex_values_t::value('F'));
}