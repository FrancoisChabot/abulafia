//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include "abulafia/abulafia.h"
#include "gtest/gtest.h"

namespace {
constexpr auto any = abu::tok<char>([](char) { return true; });
}

TEST(any_tok, simple_exp) {
  EXPECT_TRUE(check("a", any));
  EXPECT_EQ(*parse("a", any), 'a');
  EXPECT_EQ(*parse("ab", any), 'a');

  // The empty string string literal contains the null termination, which is
  // a valid token in its own right.
  EXPECT_EQ(*parse("", any), '\0');
}

TEST(any_tok, does_not_match_eoi) {
  EXPECT_FALSE(check(std::string_view(""), any));
  EXPECT_FALSE(parse(std::string_view(""), any));
}

namespace {
constexpr auto any_non_null = abu::tok<char>([](char c) { return c != '\0'; });
}

TEST(any_non_null_tok, simple_exp) {
  EXPECT_TRUE(check("a", any_non_null));
  EXPECT_EQ(*parse("a", any_non_null), 'a');
  EXPECT_FALSE(parse("", any_non_null));
  EXPECT_FALSE(check("", any_non_null));
}
