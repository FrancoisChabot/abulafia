//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include "abulafia/abulafia.h"
#include "gtest/gtest.h"

using namespace abu;

TEST(test_eoi, simple_test) {
  EXPECT_EQ(parse(eoi, "hello"), result::FAILURE);
  EXPECT_EQ(parse(eoi, "a"), result::FAILURE);
  EXPECT_EQ(parse(eoi, std::string("")), result::SUCCESS);
  EXPECT_EQ(parse(eoi, std::string_view("")), result::SUCCESS);

  // the trailing \0 counts, unfortunately
  EXPECT_EQ(parse(eoi, ""), result::FAILURE);
  // EXPECT_EQ(parse('\0' >> eoi, ""), result::SUCCESS);
}
