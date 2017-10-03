//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include "abulafia/abulafia.h"
#include "gtest/gtest.h"

using namespace abu;

TEST(test_except, simple_test) {
  // an unsigned int as long as it does not start with a 1
  auto pattern = uint_ - '1';

  // Basic use case.
  auto status = parse(pattern, "234");
  EXPECT_EQ(status, result::SUCCESS);

  // failure due to RHS success
  status = parse(pattern, "1234");
  EXPECT_EQ(status, result::FAILURE);

  // failure due to LHS failure
  status = parse(pattern, "abc1234");
  EXPECT_EQ(status, result::FAILURE);

  // failure due to out of data
  status = parse(pattern, "");
  EXPECT_EQ(status, result::FAILURE);

  // only hit RHS
  status = parse(pattern, "1");
  EXPECT_EQ(status, result::FAILURE);
}
/*
TEST(test_except, test_partial_match) {
  auto pattern = *(UInt<10, 2, 2>() - "23");

  std::vector<unsigned int> dst;
  // Basic use case.
  auto status = parse(pattern, "24562322", dst);
  EXPECT_EQ(status, result::SUCCESS);
  EXPECT_EQ(2U, dst.size());
  EXPECT_EQ(24U, dst[0]);
  EXPECT_EQ(56U, dst[1]);
}
*/