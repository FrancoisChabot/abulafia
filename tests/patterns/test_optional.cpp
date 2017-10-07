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

TEST(test_optional, test_pass) {
  auto pattern = -char_(char_set::any<char>) >> +char_('a', 'z');

  testPatternSuccess("hello", pattern, nil);
  testPatternSuccess("1ello", pattern, nil);
  testPatternFailure<Nil>("12ello", pattern);
}
