//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PATTERNS_LEAF_NUMERIC_UINT_H_
#define ABULAFIA_PATTERNS_LEAF_NUMERIC_UINT_H_

#include "abulafia/config.h"

#include "abulafia/pattern.h"

namespace ABULAFIA_NAMESPACE {

// The Uint Pattern
template <int BASE, std::size_t DIGITS_MIN = 1, std::size_t DIGITS_MAX = 0>
class Uint : public Pattern<Uint<BASE, DIGITS_MIN, DIGITS_MAX>> {
  static_assert(DIGITS_MIN >= 1, "Numeric parser must parse at least 1 digit");
  static_assert(DIGITS_MAX >= DIGITS_MIN || DIGITS_MAX == 0,
                "Max < Min? really?");
};

template <int BASE, std::size_t DIGITS_MIN, std::size_t DIGITS_MAX,
          typename RECUR_TAG>
struct pattern_traits<Uint<BASE, DIGITS_MIN, DIGITS_MAX>, RECUR_TAG>
    : public default_pattern_traits {
  enum {
    ATOMIC = false,
    BACKTRACKS = false,
    FAILS_CLEANLY = DIGITS_MAX == 1,
    PEEKABLE = DIGITS_MIN == 1,
    MAY_NOT_CONSUME = false,
  };
};

template <int BASE, std::size_t DIGITS_MIN, std::size_t DIGITS_MAX,
          typename CTX_T>
struct pat_attr_t<Uint<BASE, DIGITS_MIN, DIGITS_MAX>, CTX_T> {
  using attr_type = unsigned int;
};

}  // namespace ABULAFIA_NAMESPACE

#endif
