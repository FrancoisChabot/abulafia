//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <unordered_set>
#include "abulafia/abulafia.h"
#include "gtest/gtest.h"

using namespace abu;

TEST(test_alternative, simple_test) {
  auto pattern = Uint<2, 4, 4>() | Uint<10, 2, 2>();
  std::unordered_set<int> test;
  unsigned int result = 0;

  auto success = parse(pattern, "1010", result);
  EXPECT_EQ(result::SUCCESS, success);
  EXPECT_EQ(10U, result);

  success = parse(pattern, "24", result);
  EXPECT_EQ(result::SUCCESS, success);
  EXPECT_EQ(24U, result);

  success = parse(pattern, "2", result);
  EXPECT_EQ(result::FAILURE, success);
}
