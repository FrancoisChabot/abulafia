//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include "abulafia/abulafia.h"
#include "gtest/gtest.h"

using namespace abu;

TEST(test_char_literal, simple_test) {
  auto pattern = lit('a');

  // Basic use case.
  auto status = parse(pattern, "a");
  EXPECT_EQ(status, result::SUCCESS);

  status = parse(pattern, "ab");
  EXPECT_EQ(status, result::SUCCESS);

  status = parse(pattern, "b");
  EXPECT_EQ(status, result::FAILURE);
}

TEST(test_char_literal, test_implicit_usage) {
  auto pat = make_pattern('a');
  static_assert(is_same<decltype(pat), CharLiteral<char> >::value,
                "implicit conversion failed");

  // Basic use case.
  auto status = parse('a', "a");
  EXPECT_EQ(status, result::SUCCESS);

  status = parse('a', "b");
  EXPECT_EQ(status, result::FAILURE);
}

TEST(test_char_literal, UTF32) {
  auto pattern = lit(U'a');

  // Basic use case.
  auto status = parse(pattern, U"a");
  EXPECT_EQ(status, result::SUCCESS);

  status = parse(pattern, U"ab");
  EXPECT_EQ(status, result::SUCCESS);

  status = parse(pattern, U"b");
  EXPECT_EQ(status, result::FAILURE);
}