//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include "abulafia/abulafia.h"
#include "gtest/gtest.h"

using namespace abu;

TEST(test_symbol, simple_test) {
  std::map<std::string, int> symbols{
      {"a", 1}, {"b", 2}, {"ab", 3}, {"baf", 4}, {"longer", 5}};

  auto pattern = symbol(symbols);

  int dst;
  auto status = parse(pattern, "a", dst);
  EXPECT_EQ(status, result::SUCCESS);
  EXPECT_EQ(1, dst);

  status = parse(pattern, "b", dst);
  EXPECT_EQ(status, result::SUCCESS);
  EXPECT_EQ(2, dst);

  status = parse(pattern, "c", dst);
  EXPECT_EQ(status, result::FAILURE);

  status = parse(pattern, "ab", dst);
  EXPECT_EQ(status, result::SUCCESS);
  EXPECT_EQ(3, dst);

  status = parse(pattern, "baf", dst);
  EXPECT_EQ(status, result::SUCCESS);
  EXPECT_EQ(4, dst);

  status = parse(pattern, "longer", dst);
  EXPECT_EQ(status, result::SUCCESS);
  EXPECT_EQ(5, dst);
}

TEST(test_symbol, edge_cases) {
  std::map<std::string, int> symbols{
      {"a", 1}, {"b", 2}, {"ab", 3}, {"baf", 4}, {"longer", 5}};

  auto pattern = symbol(symbols);

  int dst;
  auto status = parse(pattern, "ac", dst);
  EXPECT_EQ(status, result::SUCCESS);
  EXPECT_EQ(1, dst);

  status = parse(pattern, "ba", dst);
  EXPECT_EQ(status, result::SUCCESS);
  EXPECT_EQ(2, dst);

  status = parse(pattern, "longe", dst);
  EXPECT_EQ(status, result::FAILURE);
}
