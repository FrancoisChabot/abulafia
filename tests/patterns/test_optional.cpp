//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include "abulafia/abulafia.h"
#include "gtest/gtest.h"

using namespace abu;

TEST(test_optional, test_pass) {

  auto pat = -char_(char_set::any<char>) >> +char_('a', 'z');

  EXPECT_EQ(parse(pat, "hello"), result::SUCCESS);
  EXPECT_EQ(parse(pat, "1ello"), result::SUCCESS);
  EXPECT_EQ(parse(pat, "12ello"), result::FAILURE);
}
