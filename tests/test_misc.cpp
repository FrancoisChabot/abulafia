//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include "abulafia/abulafia.h"
#include "gtest/gtest.h"

using namespace abu;
TEST(misc, pass_works) {

  EXPECT_TRUE(match("a", pass));
  EXPECT_TRUE(match("", pass));
  EXPECT_TRUE(match(std::string_view(""), pass));
}

TEST(misc, fail_works) {

  EXPECT_FALSE(match("a", fail));
  EXPECT_FALSE(match("", fail));
  EXPECT_FALSE(match(std::string_view(""), fail));
}

TEST(opt, eoi_works) {
  EXPECT_FALSE(match("a", eoi));
  EXPECT_FALSE(match("", eoi));
  EXPECT_TRUE(match(std::string_view(""), eoi));
}
