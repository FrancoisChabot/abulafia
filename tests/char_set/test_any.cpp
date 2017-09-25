//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include "abulafia/abulafia.h"
#include "gtest/gtest.h"

using namespace abu;

TEST(test_any, test_all_chars) {
  auto base_charset = char_set::any<char>;

  static_assert(char_set::is_char_set<decltype(base_charset)>::value,
                "must be a valid char set");

  using char_set_type = decltype(base_charset);
  static_assert(is_same<char_set_type::char_t, char>::value,
                "type assignation failure");

  for (char i = -128; (int)i < 127; ++i) {
    EXPECT_TRUE(base_charset.is_valid(i));
  }
}
