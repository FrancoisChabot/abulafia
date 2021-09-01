//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include "abulafia/abulafia.h"
#include "gtest/gtest.h"

using namespace abu;


TEST(discard, works) {
  auto sut = discard(tok);

  EXPECT_TRUE(match<sut>("a"));
  EXPECT_TRUE(match<sut>(""));
  
  EXPECT_FALSE(match<sut>(std::string_view("")));
}
