//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include "abulafia/abulafia.h"
#include "gtest/gtest.h"

using namespace abu;

TEST(test_not, simple_test) {
  EXPECT_EQ(parse(!pass, "hello"), result::FAILURE);
  EXPECT_EQ(parse(!char_('a', 'z'), "c"), result::FAILURE);
  EXPECT_EQ(parse(!char_('a', 'z'), "2"), result::SUCCESS);
}

TEST(test_not, double_not) {
  EXPECT_EQ(parse(!!pass, "hello"), result::SUCCESS);
  EXPECT_EQ(parse(!!char_('a', 'z'), "c"), result::SUCCESS);
  EXPECT_EQ(parse(!!char_('a', 'z'), "2"), result::FAILURE);
}
