//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include "abulafia/abulafia.h"
#include "gtest/gtest.h"

using namespace abu;

// Constexpr usage
static_assert(check("a", tok));
static_assert(*parse("a", tok) == 'a');

TEST(token, any_works) {
  EXPECT_TRUE(check("a", tok));
  EXPECT_EQ(*parse("a", tok), 'a');
  EXPECT_EQ(*parse("ab", tok), 'a');

  // The empty string string literal contains the null termination, which is
  // a valid token in its own right.
  EXPECT_EQ(*parse("", tok), '\0');

  EXPECT_FALSE(check(std::string_view(""), tok));
  EXPECT_FALSE(parse(std::string_view(""), tok));
}

TEST(token, any_non_null_works) {
  constexpr auto any_non_null = [](char c) { return c != '\0'; };

  EXPECT_TRUE(check("a", any_non_null));
  EXPECT_EQ(*parse("a", any_non_null), 'a');
  EXPECT_FALSE(parse("", any_non_null));
  EXPECT_FALSE(check("", any_non_null));
}

TEST(token, non_trivial_token_type) {
  std::vector<std::string> empty_tokens;
  std::vector<std::string> tokens = {"aaa", "bbb", "ccc"};
  std::vector<std::string> tokens_2 = {"bbb", "aaa", "ccc"};

  constexpr auto not_bbb = [](const std::string& t) { return t != "bbb"; };

  EXPECT_TRUE(check(tokens, tok));
  EXPECT_EQ(*parse(tokens, tok), "aaa");
  EXPECT_FALSE(parse(empty_tokens, tok));
  EXPECT_FALSE(check(tokens_2, not_bbb));
}
