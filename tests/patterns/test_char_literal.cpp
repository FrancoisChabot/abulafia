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

TEST(test_char_literal, simple_test) {
  auto pattern = lit('a');

  testPatternSuccess("a", pattern, nil);
  testPatternSuccess("ab", pattern, nil);
  testPatternFailure<Nil>("b", pattern);
}
