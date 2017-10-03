//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include "abulafia/abulafia.h"
#include "gtest/gtest.h"
#include "test_utils.h"

using namespace abu;

TEST(test_uint, test_default_pattern) {
  auto pattern = uint_;

  // Basic use case.
  testPatternSuccess("12", pattern, 12U);
  testPatternSuccess("1", pattern, 1U);

  // Empty string
  testPatternFailure<unsigned int>("", pattern);

  // Bad String
  testPatternFailure<unsigned int>("a123", pattern);
}

TEST(test_uint, test_arbitrary_base) {
  testPatternSuccess("12", UInt<3>(), 5U);
}

TEST(test_uint, test_min_digits) {
  auto pattern = UInt<10, 3>();

  // Perfect match.
  testPatternSuccess("123", pattern, 123U);

  // More than expected.
  testPatternSuccess("1234", pattern, 1234U);

  // Not enough digits.
  testPatternFailure<unsigned int>("12", pattern);
}

TEST(test_uint, test_max_digits) {
  auto pattern = UInt<10, 1, 3>();


  // Perfect match.
  testPatternSuccess("123", pattern, 123U);

  // Off by one.
  testPatternSuccess("1234", pattern, 123U);

  // make sure that the stream is in the right place once we reached the end
  //testPatternSuccess("1234", pattern >> '4', 123U);
}
