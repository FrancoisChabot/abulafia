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

TEST(test_except, simple_test) {
  // an unsigned int as long as it does not start with a 1
  auto pattern = uint_ - '1';

  testPatternSuccess("234", pattern, 234U);
  testPatternFailure<int>("1234", pattern);
  testPatternFailure<int>("abc1234", pattern);
  testPatternFailure<int>("", pattern);
  testPatternFailure<int>("1", pattern);
}

TEST(test_except, test_partial_match) {
  auto pattern = *(UInt<10, 2, 2>() - "23");

  testPatternSuccess("24562322", pattern, std::vector<int>({24, 56}));
}
