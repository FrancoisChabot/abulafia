//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PATTERNS_LEAF_NUMERIC_UINT_H_
#define ABULAFIA_PATTERNS_LEAF_NUMERIC_UINT_H_

#include "abulafia/config.h"

#include "abulafia/patterns/leaf/leaf_pattern.h"

namespace ABULAFIA_NAMESPACE {

// Pattern for a unsigned integer
template <std::size_t BASE, std::size_t DIGITS_MIN = 1, std::size_t DIGITS_MAX = 0>
class UInt : public LeafPattern<UInt<BASE, DIGITS_MIN, DIGITS_MAX>> {
public:
  static_assert(DIGITS_MIN >= 1);
  static_assert(DIGITS_MAX >= DIGITS_MIN || DIGITS_MAX == 0);
};

static constexpr UInt<10, 1, 0> uint_;
}  // namespace ABULAFIA_NAMESPACE

#endif
