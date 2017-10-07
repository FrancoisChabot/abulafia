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

TEST(test_char, simple_test) {
  auto any = char_();

  testPatternSuccess("a", any, 'a');
  testPatternFailure<char>("", any);
}

TEST(test_char, expect_specific_char) {
  auto plus = char_('+');

  testPatternSuccess("+", plus, '+');
  testPatternSuccess("+-", plus, '+');
  testPatternFailure<char>("", plus);
  testPatternFailure<char>("a", plus);
}

TEST(test_char, char_range) {
  auto num = char_('0', '9');

  testPatternSuccess("0", num, '0');
  testPatternSuccess("9", num, '9');
  testPatternSuccess("5", num, '5');

  testPatternFailure<char>("a", num);
  testPatternFailure<char>("A", num);
  testPatternFailure<char>("\0", num);
}

TEST(test_char, implicit_char_set) {
  auto space = char_set::set(" \r\n\t");

  testPatternSuccess(" ", space, ' ');
  testPatternSuccess("\n", space, '\n');
  testPatternSuccess("\thh", space, '\t');

  testPatternFailure<char>("a", space);
  testPatternFailure<char>("l ", space);
  testPatternFailure<char>("", space);
}