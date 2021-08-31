//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include "abulafia/abulafia.h"
#include "gtest/gtest.h"

using namespace abu;

TEST(repeat, builds_string) {
  auto sut = repeat<0, 5>([](char c) { return c >= '0' && c <= '9'; });

  EXPECT_EQ(parse("012", sut), "012");
  EXPECT_EQ(parse("123a", sut), "123");
  EXPECT_EQ(parse("ksajcnas", sut), "");
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