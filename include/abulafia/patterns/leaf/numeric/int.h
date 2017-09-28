//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PATTERNS_LEAF_NUMERIC_INT_H_
#define ABULAFIA_PATTERNS_LEAF_NUMERIC_INT_H_

#include "abulafia/config.h"

#include "abulafia/pattern.h"

namespace ABULAFIA_NAMESPACE {

// Pattern for a signed integer
template <int BASE, std::size_t DIGITS_MIN = 1, std::size_t DIGITS_MAX = 0>
class Int : public Pattern<Int<BASE, DIGITS_MIN, DIGITS_MAX>> {
  static_assert(DIGITS_MIN >= 1);
  static_assert(DIGITS_MAX >= DIGITS_MIN || DIGITS_MAX == 0);
};

template <int BASE, std::size_t DIGITS_MIN, std::size_t DIGITS_MAX,
          typename RECUR_TAG>
struct pattern_traits<Int<BASE, DIGITS_MIN, DIGITS_MAX>, RECUR_TAG>
    : public default_pattern_traits {
  enum {
    ATOMIC = false,
    BACKTRACKS = false,
    FAILS_CLEANLY = false,
    PEEKABLE = false,  // we cannot peek because "-" is valid
    MAY_NOT_CONSUME = false,
  };
};

template <int BASE, std::size_t DIGITS_MIN, std::size_t DIGITS_MAX,
          typename CTX_T>
struct pat_attr_t<Int<BASE, DIGITS_MIN, DIGITS_MAX>, CTX_T> {
  using attr_type = int;
};

}  // namespace ABULAFIA_NAMESPACE

#endif
