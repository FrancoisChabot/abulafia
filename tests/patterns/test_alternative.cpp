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

TEST(test_alternative, simple_test) {
  auto any = char_();

  static_assert(is_base_of_template<decltype(any), Pattern>::value);
  auto pattern = UInt<2, 4, 4>() | UInt<10, 2, 2>();

  testPatternSuccess("1010", pattern, 10U);
  testPatternSuccess("24", pattern, 24U);
  testPatternFailure<int>("2", pattern);
}
