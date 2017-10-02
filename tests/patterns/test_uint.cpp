//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include "abulafia/abulafia.h"
#include "gtest/gtest.h"

using namespace abu;

TEST(test_uint, test_default_pattern) {
  auto pattern = uint_;

  unsigned int dst = 0;

  // Basic use case.
  auto status = parse("12", pattern, dst);
  EXPECT_EQ(status, Result::SUCCESS);
  EXPECT_EQ(12U, dst);

  status = parse("1", pattern, dst);
  EXPECT_EQ(status, Result::SUCCESS);
  EXPECT_EQ(1U, dst);

  // Interrupted number
  //  status = parse(pattern >> ' ', "54 456", dst);
  // EXPECT_EQ(status, result::SUCCESS);
  // EXPECT_EQ(54U, dst);

  // Empty string.
  status = parse("", pattern, dst);
  EXPECT_EQ(status, Result::FAILURE);

  // Bad string
  status = parse("a123", pattern, dst);
  EXPECT_EQ(status, Result::FAILURE);
}

TEST(test_uint, test_arbitrary_base) {
  auto pattern = Uint<3>();

  unsigned int dst = 0;

  // Basic use case.
  auto status = parse("12", pattern, dst);
  EXPECT_EQ(status, Result::SUCCESS);
  EXPECT_EQ(5U, dst);
}

TEST(test_uint, test_min_digits) {
  auto pattern = Uint<10, 3>();

  unsigned int dst = 0;

  // Perfect match.
  auto status = parse("123", pattern, dst);
  EXPECT_EQ(status, Result::SUCCESS);
  EXPECT_EQ(123U, dst);

  // More than expected.
  status = parse("1234", pattern, dst);
  EXPECT_EQ(status, Result::SUCCESS);
  EXPECT_EQ(1234U, dst);

  // Not enough digits.
  status = parse("12", pattern);
  EXPECT_EQ(status, Result::FAILURE);
}

TEST(test_uint, test_max_digits) {
  auto pattern = Uint<10, 1, 3>();

  unsigned int dst = 0;

  // Perfect match.
  auto status = parse("123", pattern, dst);
  EXPECT_EQ(status, Result::SUCCESS);
  EXPECT_EQ(123U, dst);

  // Off by one.
  status = parse("1234", pattern, dst);
  EXPECT_EQ(status, Result::SUCCESS);
  EXPECT_EQ(123U, dst);

  // make sure that the stream is in the right place once we reached the end
  //  auto status = parse(pattern >> '4', "1234");
  // EXPECT_EQ(status, result::SUCCESS);
}
