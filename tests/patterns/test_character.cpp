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
  auto status = parse(any, "a", dst);
  EXPECT_EQ(status, result::SUCCESS);
  EXPECT_EQ(dst, 'a');

  status = parse(any, std::string(""));
  EXPECT_EQ(status, result::FAILURE);
}

TEST(test_char, expect_specific_char) {
  auto plus = char_('+');

  char dst = 0;
  auto status = parse(plus, "+", dst);
  EXPECT_EQ(status, result::SUCCESS);
  EXPECT_EQ(dst, '+');

  status = parse(plus, "+-", dst);
  EXPECT_EQ(status, result::SUCCESS);
  EXPECT_EQ(dst, '+');

  status = parse(plus, std::string(""));
  EXPECT_EQ(status, result::FAILURE);

  status = parse(plus, "a");
  EXPECT_EQ(status, result::FAILURE);
}

TEST(test_char, char_range) {
  auto num = char_('0', '9');

  EXPECT_EQ(parse(num, "0"), result::SUCCESS);
  EXPECT_EQ(parse(num, "5"), result::SUCCESS);
  EXPECT_EQ(parse(num, "9"), result::SUCCESS);

  EXPECT_EQ(parse(num, "a"), result::FAILURE);
  EXPECT_EQ(parse(num, "A"), result::FAILURE);
  EXPECT_EQ(parse(num, "\0"), result::FAILURE);
}

TEST(test_char, char_set) {
  auto num = char_({'a', 'l', 'z'});

  EXPECT_EQ(parse(num, "a"), result::SUCCESS);
  EXPECT_EQ(parse(num, "l"), result::SUCCESS);
  EXPECT_EQ(parse(num, "z"), result::SUCCESS);

  EXPECT_EQ(parse(num, "b"), result::FAILURE);
  EXPECT_EQ(parse(num, "0"), result::FAILURE);
  EXPECT_EQ(parse(num, "g"), result::FAILURE);
}

TEST(test_char, implicit_char_set) {
  auto space = char_set::set(" \r\n\t");

  EXPECT_EQ(parse(space, "a"), result::FAILURE);
  EXPECT_EQ(parse(space, "l "), result::FAILURE);
  EXPECT_EQ(parse(space, ""), result::FAILURE);

  EXPECT_EQ(parse(space, " "), result::SUCCESS);
  EXPECT_EQ(parse(space, "\n"), result::SUCCESS);
  EXPECT_EQ(parse(space, "\thh"), result::SUCCESS);
  //  EXPECT_EQ(parse(space >> 'b', "\nb"), result::SUCCESS);
}