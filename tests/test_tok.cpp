//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include "abulafia/abulafia.h"
#include "gtest/gtest.h"

using namespace abu;

// // Constexpr usage
static_assert(match<tok>("a"));
static_assert(parse<tok>("a") == 'a');

TEST(token, any_works) {
  EXPECT_TRUE(match<tok>("a"));
  EXPECT_EQ(parse<tok>("a"), 'a');
  EXPECT_EQ(parse<tok>("ab"), 'a');

  // // The empty string string literal contains the null termination, which is
  // // a valid token in its own right.
  EXPECT_EQ(parse<tok>(""), '\0');

  EXPECT_FALSE(match<tok>(std::string_view("")));
  EXPECT_THROW(parse<tok>(std::string_view("")), no_match_error);
}

TEST(token, any_non_null_works) {
  constexpr auto any_non_null = [](char c) { return c != '\0'; };

  // EXPECT_TRUE(match("a", any_non_null));
  EXPECT_EQ(parse<any_non_null>("a"), 'a');
  EXPECT_THROW(parse<any_non_null>(""), no_match_error);
  EXPECT_FALSE(match<any_non_null>(""));
}

TEST(token, non_trivial_token_type) {
  std::vector<std::string> empty_tokens;
  std::vector<std::string> tokens = {"aaa", "bbb", "ccc"};
  std::vector<std::string> tokens_2 = {"bbb", "aaa", "ccc"};

  constexpr auto not_bbb = [](const std::string &t) { return t != "bbb"; };

  EXPECT_TRUE(match<tok>(tokens));
  EXPECT_EQ(parse<tok>(tokens), "aaa");
  EXPECT_THROW(parse<tok>(empty_tokens), no_match_error);
  EXPECT_FALSE(match<not_bbb>(tokens_2));
}
