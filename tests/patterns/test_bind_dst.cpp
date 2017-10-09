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

TEST(test_bind_dst, simple_test) {
  auto action = [](char, auto p) -> void { p.bound_dst.get() += 1; };

  auto pattern = bind_dst(*char_()[action]);

  testPatternSuccess("1010", pattern, 4);
  testPatternSuccess("24", pattern, 2);
}
