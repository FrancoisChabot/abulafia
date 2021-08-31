//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <cctype>

#include "abulafia/abulafia.h"
#include "gtest/gtest.h"

using namespace abu;

namespace {
constexpr auto const_pattern =
    except([](char c) { return c >= 'a' && c <= 'z'; },
           [](char c) { return c == 'm'; });

static_assert(match("a", const_pattern));
//static_assert(parse("a", const_pattern) == 'a');
}  // namespace

inline constexpr bool exception(char c) { return c == 'm'; }

TEST(except, works) {
  auto sut = except([](char c) { return c >= 'a' && c <= 'z'; },
                    [](char c) { return c == 'm'; });

  EXPECT_TRUE(match("a", sut));
  EXPECT_TRUE(match("l", sut));
  EXPECT_FALSE(match("m", sut));
  EXPECT_TRUE(match("n", sut));

  EXPECT_EQ(parse("a", sut), 'a');
}
