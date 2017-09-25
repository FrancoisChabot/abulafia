//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include "abulafia/abulafia.h"

#include "gtest/gtest.h"

using namespace abu;

TEST(test_int, test_default_pattern) {
  auto pattern = int_;

  int dst = 0;

  // Basic use case.
  auto status = parse(pattern, "12", dst);
  EXPECT_EQ(status, result::SUCCESS);
  EXPECT_EQ(12, dst);

  status = parse(pattern, "-1", dst);
  EXPECT_EQ(status, result::SUCCESS);
  EXPECT_EQ(-1, dst);

  // Empty string.
  status = parse(pattern, "", dst);
  EXPECT_EQ(status, result::FAILURE);

  // Bad string
  status = parse(pattern, "a123", dst);
  EXPECT_EQ(status, result::FAILURE);
}
