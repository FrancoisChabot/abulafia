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

TEST(test_skipper, simple_test) {
  auto pattern = apply_skipper(uint_ % ',', lit(' '));

  testPatternSuccess("1,2,3", pattern, std::vector<int>{1, 2, 3});
  testPatternSuccess("4, 5, 6", pattern, std::vector<int>{4, 5, 6});
  testPatternSuccess(" 7 , 8 , 9 ", pattern, std::vector<int>{7, 8, 9});
}
