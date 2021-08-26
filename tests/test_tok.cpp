//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include "abulafia/abulafia.h"
#include "gtest/gtest.h"

namespace {
constexpr auto any = abu::token_set([](char) { return true; });
}  // namespace

TEST(token, any_works) {
  EXPECT_TRUE(check("a", any));
  EXPECT_EQ(*parse("a", any), 'a');
  EXPECT_EQ(*parse("ab", any), 'a');

  // The empty string string literal contains the null termination, which is
  // a valid token in its own right.
  EXPECT_EQ(*parse("", any), '\0');

  EXPECT_FALSE(check(std::string_view(""), any));
  EXPECT_FALSE(parse(std::string_view(""), any));
}

namespace {
constexpr auto any_non_null = abu::token_set([](char c) { return c != '\0'; });
}  // namespace

TEST(token, any_non_null_works) {
  EXPECT_TRUE(check("a", any_non_null));
  EXPECT_EQ(*parse("a", any_non_null), 'a');
  EXPECT_FALSE(parse("", any_non_null));
  EXPECT_FALSE(check("", any_non_null));
}

TEST(token, reverse) {
  auto abc =
      abu::token_set([](char c) { return c == 'a' || c == 'b' || c == 'c'; });

  auto abc_inv = ~abc;

  EXPECT_TRUE(check("a", abc));
  EXPECT_TRUE(check("b", abc));
  EXPECT_TRUE(check("c", abc));
  EXPECT_FALSE(check("d", abc));

  EXPECT_FALSE(check("a", abc_inv));
  EXPECT_FALSE(check("b", abc_inv));
  EXPECT_FALSE(check("c", abc_inv));
  EXPECT_TRUE(check("d", abc_inv));
}

TEST(token, exclude) {
  auto abc_tok =
      abu::token_set([](char c) { return c == 'a' || c == 'b' || c == 'c'; });

  auto b_tok = abu::token_set([](char c) { return c == 'b'; });

  auto sut = abc_tok - b_tok;

  EXPECT_TRUE(check("a", sut));
  EXPECT_FALSE(check("b", sut));
  EXPECT_TRUE(check("c", sut));
  EXPECT_FALSE(check("d", sut));
}
