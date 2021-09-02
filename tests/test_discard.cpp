//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include "abu_test_utils.h"
#include "gtest/gtest.h"

using namespace abu;

TEST(discard, works) {
  constexpr auto sut = discard(tok);

  ABU_EXPECT_MATCH(sut, "a");
  ABU_EXPECT_MATCH(sut, "");
  ABU_EXPECT_NO_MATCH(sut, std::string_view{});
}
