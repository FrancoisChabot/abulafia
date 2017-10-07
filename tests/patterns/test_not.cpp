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

TEST(test_not, simple_test) {
  testPatternFailure<Nil>("hello", !pass);
  testPatternFailure<Nil>("c", !char_('a', 'z'));
  testPatternSuccess("2", !char_('a', 'z'), nil);
}

TEST(test_not, double_not) {
  testPatternSuccess("hello", !!pass, nil);
  testPatternSuccess("c", !!char_('a', 'z'), nil);
  testPatternFailure<Nil>("2", !!char_('a', 'z'));
}
