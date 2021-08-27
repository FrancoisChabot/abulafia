//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include "abulafia/abulafia.h"
#include "gtest/gtest.h"

using namespace abu;
TEST(opt, works) {
  auto sut = opt(tok([](char c) { return c >= '0' && c <= '9'; }));

  EXPECT_TRUE(check("a", sut));
  EXPECT_TRUE(check("b", sut));
  EXPECT_TRUE(check(std::string_view(""), sut));
  EXPECT_TRUE(check(std::string_view("0"), sut));
}

TEST(opt, parses_correctly) {
  auto sut = opt(tok([](char c) { return c >= '0' && c <= '9'; }));

  EXPECT_EQ(*parse("a", sut), std::nullopt);
  EXPECT_EQ(*parse("b", sut), std::nullopt);
  EXPECT_EQ(*parse(std::string_view(""), sut), std::nullopt);
  EXPECT_EQ(**parse(std::string_view("0"), sut), '0');
}
