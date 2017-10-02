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
template <std::size_t BASE, std::size_t DIGITS_MIN = 1, std::size_t DIGITS_MAX = 0>
class Uint : public Pattern<Uint<BASE, DIGITS_MIN, DIGITS_MAX>> {
public:
  static constexpr std::size_t base() { return BASE; }
  static constexpr std::size_t digits_min() { return DIGITS_MIN; }
  static constexpr std::size_t digits_max() { return DIGITS_MAX; }


  static_assert(DIGITS_MIN >= 1, "Numeric parser must parse at least 1 digit");
  static_assert(DIGITS_MAX >= DIGITS_MIN || DIGITS_MAX == 0,
                "Max < Min? really?");
};

}  // namespace ABULAFIA_NAMESPACE

#endif
