//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include "abulafia/abulafia.h"
#include "gtest/gtest.h"

using namespace abu;

/*
TEST(test_readme, the_readme) {
  // opening bracket, followed by a comma-delimited list of integers, followed
  // by a closing bracket.
  auto with_brackets = '[' >> (abu::int_ % ',') >> ']';

  // ignore whitespace
  auto ignoring_whitespace =
      abu::apply_skipper(with_brackets, abu::char_(" \t\r\n"));

  std::vector<int> dst;
  auto status = abu::parse("[1, 2, 3, 4, 5]", ignoring_whitespace, dst);

  EXPECT_EQ(status, abu::Result::SUCCESS);
  EXPECT_EQ(dst[0], 1);
  EXPECT_EQ(dst[1], 2);
  EXPECT_EQ(dst[2], 3);
  EXPECT_EQ(dst[3], 4);
  EXPECT_EQ(dst[4], 5);
}
*/