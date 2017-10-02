//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include "abulafia/abulafia.h"
#include "gtest/gtest.h"

using namespace abu;

TEST(test_char, simple_test) {
  auto any = char_();

  char dst = 0;
  auto status = parse("a", any, dst);
  EXPECT_EQ(status, Result::SUCCESS);
  EXPECT_EQ(dst, 'a');

  status = parse(std::string(""), any);
  EXPECT_EQ(status, Result::FAILURE);
}

TEST(test_char, expect_specific_char) {
  auto plus = char_('+');

  char dst = 0;
  auto status = parse("+", plus, dst);
  EXPECT_EQ(status, Result::SUCCESS);
  EXPECT_EQ(dst, '+');

  status = parse("+-", plus, dst);
  EXPECT_EQ(status, Result::SUCCESS);
  EXPECT_EQ(dst, '+');

  status = parse(std::string(""), plus);
  EXPECT_EQ(status, Result::FAILURE);

  status = parse("a", plus);
  EXPECT_EQ(status, Result::FAILURE);
}

TEST(test_char, char_range) {
  auto num = char_('0', '9');

  EXPECT_EQ(parse("0", num), Result::SUCCESS);
  EXPECT_EQ(parse("5", num), Result::SUCCESS);
  EXPECT_EQ(parse("9", num), Result::SUCCESS);

  EXPECT_EQ(parse("a", num), Result::FAILURE);
  EXPECT_EQ(parse("A", num), Result::FAILURE);
  EXPECT_EQ(parse("\0", num), Result::FAILURE);
}

TEST(test_char, implicit_char_set) {
  auto space = char_set::set(" \r\n\t");

  EXPECT_EQ(parse("a", space), Result::FAILURE);
  EXPECT_EQ(parse("l ", space), Result::FAILURE);
  EXPECT_EQ(parse("", space), Result::FAILURE);

  EXPECT_EQ(parse(" ", space), Result::SUCCESS);
  EXPECT_EQ(parse("\n", space), Result::SUCCESS);
  EXPECT_EQ(parse("\thh", space), Result::SUCCESS);
  //  EXPECT_EQ(parse(space >> 'b', "\nb"), result::SUCCESS);
}