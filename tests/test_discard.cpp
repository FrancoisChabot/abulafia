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

  auto parse_result = parse("a", sut);
  static_assert(std::is_same_v<decltype(parse_result), result<void>>);

  EXPECT_TRUE(check("a", sut));
  EXPECT_TRUE(parse("a", sut));

  EXPECT_TRUE(check("", sut));
  EXPECT_TRUE(parse("", sut));

  EXPECT_FALSE(check(std::string_view(""), sut));
  EXPECT_FALSE(parse(std::string_view(""), sut));
}
