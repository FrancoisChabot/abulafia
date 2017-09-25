//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include "abulafia/abulafia.h"
#include "gtest/gtest.h"

using namespace abu;

TEST(test_pass, simple_test) {
  EXPECT_EQ(parse(pass, "hello"), result::SUCCESS);
  EXPECT_EQ(parse(pass, "a"), result::SUCCESS);
  EXPECT_EQ(parse(pass, std::string("")), result::SUCCESS);
  EXPECT_EQ(parse(pass, ""), result::SUCCESS);
}

TEST(test_fail, simple_test) {
  EXPECT_EQ(parse(fail, "hello"), result::FAILURE);
  EXPECT_EQ(parse(fail, "a"), result::FAILURE);
  EXPECT_EQ(parse(fail, std::string("")), result::FAILURE);
  EXPECT_EQ(parse(fail, ""), result::FAILURE);
}
