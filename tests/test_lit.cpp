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
static_assert(check("a", lit(std::string_view("a"))));
static_assert(check("abc", lit(std::string_view("a"))));
static_assert(check("abc", lit(std::string_view("abc"))));

static_assert(!check("", lit(std::string_view("abc"))));
static_assert(!check("aba", lit(std::string_view("abc"))));
static_assert(!check("aabc", lit(std::string_view("abc"))));

static_assert(check("a", lit('a')));
static_assert(!check("b", lit('a')));

TEST(literals, ambiguous_value_or_range) {
  struct silly_string {
    bool operator==(const char& c) const { return data.size() >= 1 && data[0] == c; }
    bool operator==(const std::string_view& sv) const { return data == sv; }

    std::string data;
  };

  std::vector<silly_string> tokens = {silly_string{"aaa"}, silly_string{"bbb"}};


  EXPECT_TRUE(check(tokens, lit('a')));

  // If a token type is both comparable to a value and a range of that value,
  // then the range version takes priority.
  EXPECT_TRUE(check(tokens, lit(std::string_view("aaa"))));
}