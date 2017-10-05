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

TEST(test_sequence, simple_test) {
  auto pattern = lit("a") >> lit("b");

  testPatternSuccess("ab", pattern, nil);
}

TEST(test_sequence, sequence_into_value) {
  auto pattern = lit('-') >> char_() >> lit('-');

  testPatternSuccess("-c-", pattern, 'c');
}

TEST(test_sequence, sequence_into_vector) {
  auto pattern = *char_('a') >> lit('-') >> *char_('b');

  testPatternSuccess("aaa-bb", pattern, std::string("aaabb"));
}

TEST(test_sequence, sequence_into_tuple) {
  auto pattern = int_ >> lit('-') >> int_ >> lit('-') >> int_;

  testPatternSuccess("1-2-3", pattern, std::tuple<int, int, int>(1, 2, 3));
}
