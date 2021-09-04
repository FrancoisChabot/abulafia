//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include "abu_test_utils.h"
#include "gtest/gtest.h"

using namespace abu;

TEST(repeat, builds_string) {
  constexpr auto sut =
      repeat([](char c) { return c >= '0' && c <= '9'; }, 0, 5);

  ABU_EXPECT_PARSES_AS(sut, "012", "012");
  ABU_EXPECT_PARSES_AS(sut, "123a", "123");
  ABU_EXPECT_PARSES_AS(sut, "ksajcnas", "");
  ABU_EXPECT_PARSES_AS(sut, std::string_view{}, "");
}

TEST(repeat, fails_if_not_enough) {
  constexpr auto sut =
      repeat([](char c) { return c >= '0' && c <= '9'; }, 3, 5);

  ABU_EXPECT_PARSES_AS(sut, "012", "012");
  ABU_EXPECT_PARSES_AS(sut, "123a", "123");
  ABU_EXPECT_NO_MATCH(sut, "01");
  ABU_EXPECT_NO_MATCH(sut, std::string_view{});
}

TEST(repeat, early_stop) {
  constexpr auto sut =
      repeat([](char c) { return c >= '0' && c <= '9'; }, 0, 2);

  ABU_EXPECT_PARSES_AS(sut, "0", "0");
  ABU_EXPECT_PARSES_AS(sut, "01", "01");
  ABU_EXPECT_PARSES_AS(sut, "012", "01");
  ABU_EXPECT_PARSES_AS(sut, "12a", "12");
  ABU_EXPECT_PARSES_AS(sut, "123a", "12");
  ABU_EXPECT_PARSES_AS(sut, "ksajcnas", "");
  ABU_EXPECT_PARSES_AS(sut, std::string_view{}, "");
}

TEST(repeat, two_levels) {
  constexpr auto dig = [](char c) { return c >= '0' && c <= '9'; };
  constexpr auto sut = repeat(repeat(dig, 2, 2), 0, 0);

  auto expected = std::vector<std::string>{"00", "11", "22"};
  ABU_EXPECT_PARSES_AS(sut, "001122", expected);
}
