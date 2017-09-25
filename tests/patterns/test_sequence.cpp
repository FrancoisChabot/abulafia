//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include "abulafia/abulafia.h"
#include "gtest/gtest.h"

using namespace abu;

TEST(test_sequence, simple_test) {
  auto pattern = lit("a") >> lit("b");

  auto status = parse(pattern, std::string("ab"));
  EXPECT_EQ(result::SUCCESS, status);
}

TEST(test_sequence, sequence_into_value) {
  auto pattern = lit('-') >> char_() >> lit('-');

  char dst = '0';

  auto status = parse(pattern, std::string("-c-"), dst);
  EXPECT_EQ(result::SUCCESS, status);
  EXPECT_EQ('c', dst);
}

TEST(test_sequence, sequence_into_vector) {
  auto pattern = *char_('a') >> lit('-') >> *char_('b');

  std::string dst("junk");
  auto status = parse(pattern, std::string("aaa-bb"), dst);
  EXPECT_EQ(result::SUCCESS, status);
  EXPECT_EQ("aaabb", dst);
}

TEST(test_sequence, sequence_into_tuple) {
  auto pattern = int_ >> lit('-') >> int_ >> lit('-') >> int_;

  std::tuple<int, int, int> dst;
  std::tuple<int, int, int> expected(1, 2, 3);
  auto status = parse(pattern, std::string("1-2-3"), dst);
  EXPECT_EQ(result::SUCCESS, status);
  EXPECT_EQ(expected, dst);
}
