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

TEST(test_int, test_default_pattern) {
  auto pattern = int_;

  testPatternSuccess("12", pattern, 12);
  testPatternSuccess("-1", pattern, -1);
  testPatternSuccess("+14", pattern, 14);
  testPatternFailure<int>("", pattern);
  testPatternFailure<int>("a123", pattern);
}
