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

TEST(test_pass, simple_test) {
  testPatternSuccess("a", pass, nil);
  testPatternSuccess("hello", pass, nil);
  testPatternSuccess("", pass, nil);
  testPatternSuccess("\0", pass, nil);
}

TEST(test_fail, simple_test) {
  testPatternFailure<Nil>("a", fail);
  testPatternFailure<Nil>("hello", fail);
  testPatternFailure<Nil>("", fail);
  testPatternFailure<Nil>("\0", fail);
}
