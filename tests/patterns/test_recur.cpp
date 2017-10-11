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

// Test basic syntax without any recursivity involved.
TEST(test_recur, simple_test) {
  RecurMemoryPool pool;

  Recur<struct abc, std::vector<char>> recur(pool);

  ABU_Recur_define(recur, abc, *char_());

  testPatternSuccess("abc", recur, std::string("abc"));
}

// Test with simple recursivity
TEST(test_recur, recur_test) {
  RecurMemoryPool pool;

  Recur<struct abc> recur(pool);

  auto recur_pat = Int<10, 2, 2>() >> *recur;

  ABU_Recur_define(recur, abc, recur_pat);

  testPatternSuccess("123456", recur, nil);
}

// Reproduces https://github.com/FrancoisChabot/abulafia/issues/17
template <typename PAT_T>
auto as_recur(PAT_T const& p, RecurMemoryPool& pool) {
  Recur<struct as_recur_t> as_recur(pool);
  ABU_Recur_define(as_recur, as_recur_t, p);
  return as_recur;
}

TEST(test_recur, recur_delegate_to_template) {
  RecurMemoryPool pool;
  auto pat = Int<10, 2, 2>();

  testPatternSuccess("12", as_recur(pat, pool), 12);
}
