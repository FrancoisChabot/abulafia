//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PATTERNS_LEAF_NUMERIC_DIGIT_VALUES_H_
#define ABULAFIA_PATTERNS_LEAF_NUMERIC_DIGIT_VALUES_H_

#include "abulafia/config.h"

#include "abulafia/support/type_traits.h"

namespace ABULAFIA_NAMESPACE {

// Is used to convert a digit character into the appropriate value according
// to the requested base.
template <int BASE, typename Enabled = void>
struct DigitValues;

// Bases 2-10 are pretty straightforward.
template <int BASE>
struct DigitValues<BASE, enable_if_t<(BASE <= 10U)>> {
  static_assert(BASE >= 2, "");
  static_assert(BASE <= 35, "");

  static bool is_valid(char c) { return c >= '0' && c <= ('0' + BASE - 1); }
  static int value(char c) { return c - '0'; }
};

// Supporting bases up to 35, while overkill, is no more work than up to 16.
template <int BASE>
struct DigitValues<BASE, enable_if_t<(BASE > 10U) && (BASE <= 35U)>> {
  static_assert(BASE >= 2, "");
  static_assert(BASE <= 35, "");

  static bool is_valid(char c) {
    return (c >= '0' && c <= '9') || (c >= 'a' && c <= ('a' + BASE - 11)) ||
           (c >= 'A' && c <= ('A' + BASE - 11));
  }

  static int value(char c) {
    if (c >= '0' && c <= '9') {
      return c - '0';
    }
    if (c >= 'a' && c <= 'z') {
      return 10 + c - 'a';
    }
    if (c >= 'A' && c <= 'Z') {
      return 10 + c - 'A';
    }
    return 0;
  }
};
}  // namespace ABULAFIA_NAMESPACE

#endif
