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
  constexpr auto sut = repeat([](char c) { return c >= '0' && c <= '9'; }, 0, 5);

  ABU_EXPECT_PARSES_AS(sut, "012", "012");
  ABU_EXPECT_PARSES_AS(sut, "123a", "123");
  ABU_EXPECT_PARSES_AS(sut, "ksajcnas", "");
}

// TEST(repeat, Fixed_size) {
//   auto sut = repeat<2, 2>(tok([](char c) { return c >= '0' && c <= '9'; }));

//   {
//     std::array<char, 2> expected = {'0', '1'};
//     EXPECT_EQ(*parse("012", sut), expected);
//   }

//   {
//     std::array<char, 2> expected = {'1', '2'};
//     EXPECT_EQ(*parse("123a", sut), expected);
//   }

//   EXPECT_FALSE(parse("ksajcnas", sut));
// }