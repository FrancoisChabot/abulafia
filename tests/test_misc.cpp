//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include "abu_test_utils.h"
#include "gtest/gtest.h"

using namespace abu;

TEST(misc, pass_works) {
  ABU_EXPECT_MATCH(pass, "a");
  ABU_EXPECT_MATCH(pass, "");
  ABU_EXPECT_MATCH(pass, std::string_view{});
}

TEST(misc, fail_works) {
  ABU_EXPECT_NO_MATCH(fail, "a");
  ABU_EXPECT_NO_MATCH(fail, "");
  ABU_EXPECT_NO_MATCH(fail, std::string_view{});
}

TEST(misc, eoi_works) {
  ABU_EXPECT_NO_MATCH(eoi, "a");
  ABU_EXPECT_NO_MATCH(eoi, "");
  ABU_EXPECT_MATCH(eoi, std::string_view{});
}