//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PATTERNS_LEAF_NUMERIC_INT_H_
#define ABULAFIA_PATTERNS_LEAF_NUMERIC_INT_H_

#include "abulafia/config.h"

#include "abulafia/patterns/pattern.h"

namespace ABULAFIA_NAMESPACE {

// Pattern for a signed integer
template <int BASE, int DIGITS_MIN = 1, int DIGITS_MAX = 0>
class Int : public Pattern<Int<BASE, DIGITS_MIN, DIGITS_MAX>> {
 public:
  static_assert(DIGITS_MIN >= 1);
  static_assert(DIGITS_MAX >= DIGITS_MIN || DIGITS_MAX == 0);
};

static constexpr Int<10, 1, 0> int_;

}  // namespace ABULAFIA_NAMESPACE

#endif
