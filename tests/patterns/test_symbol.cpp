//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include "abulafia/abulafia.h"
#include "gtest/gtest.h"
#include "test_utils.h"

using namespace abu;

TEST(test_symbol, simple_test) {
  std::map<std::string, int> symbols{
      {"a", 1}, {"b", 2}, {"ab", 3}, {"baf", 4}, {"longer", 5}};

  auto pattern = symbol(symbols);

  testPatternSuccess("a", pattern, 1);

  testPatternSuccess("b", pattern, 2);
  testPatternSuccess("ab", pattern, 3);
  testPatternSuccess("baf", pattern, 4);
  testPatternSuccess("longer", pattern, 5);
  testPatternFailure<int>("xyz", pattern);
}

TEST(test_symbol, edge_cases) {
  std::map<std::string, int> symbols{
      {"a", 1}, {"b", 2}, {"ab", 3}, {"baf", 4}, {"longer", 5}};

  auto pattern = symbol(symbols);

  testPatternSuccess("ac", pattern, 1);
  testPatternSuccess("ba", pattern, 2);
  testPatternFailure<int>("longe", pattern);
}
