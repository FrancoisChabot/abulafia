//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include "abu_test_utils.h"
#include "gtest/gtest.h"

using namespace abu;

TEST(token, any_works) {
  ABU_EXPECT_PARSES_AS(tok, "a", 'a');
  ABU_EXPECT_PARSES_AS(tok, "ab", 'a');

  // // The empty string string literal contains the null termination, which is
  // // a valid token in its own right.
  ABU_EXPECT_PARSES_AS(tok, "", '\0');

  // But the actual empty string does not parse
  ABU_EXPECT_PARSE_FAIL(tok, std::string_view{});
}

TEST(token, any_non_null_works) {
  constexpr auto any_non_null = [](char c) { return c != '\0'; };

  // EXPECT_TRUE(match("a", any_non_null));
  ABU_EXPECT_PARSES_AS(any_non_null, "a", 'a');
  ABU_EXPECT_PARSE_FAIL(any_non_null, "");
}

TEST(token, non_trivial_token_type) {
  std::vector<std::string> empty_tokens;
  std::vector<std::string> tokens = {"aaa", "bbb", "ccc"};
  std::vector<std::string> tokens_2 = {"bbb", "aaa", "ccc"};

  constexpr auto not_bbb = [](const std::string& t) { return t != "bbb"; };

  ABU_EXPECT_PARSES_AS(tok, tokens, "aaa");
  ABU_EXPECT_PARSE_FAIL(tok, empty_tokens);
  ABU_EXPECT_PARSE_FAIL(not_bbb, tokens_2);
}
