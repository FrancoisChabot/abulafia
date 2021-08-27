//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include "abulafia/abulafia.h"
#include "gtest/gtest.h"

TEST(result, success_with_value) {
  abu::result<int> r{3};

  EXPECT_TRUE(r);
  EXPECT_EQ(*r, 3);

#ifndef NDEBUG
  EXPECT_DEATH(r.get_error(), "");
#endif
}

TEST(result, failure_with_value) {
  auto err = abu::error{};

  abu::result<int> r{err};

  EXPECT_FALSE(r);
  EXPECT_THROW(*r, abu::bad_result_access);
}

TEST(result, success_void) {
  abu::result<void> r{};

  EXPECT_TRUE(r);

#ifndef NDEBUG
  EXPECT_DEATH(r.get_error(), "");
#endif
}

TEST(result, failure_void) {
  auto err = abu::error{};
  abu::result<void> r{err};

  EXPECT_FALSE(r);
}
