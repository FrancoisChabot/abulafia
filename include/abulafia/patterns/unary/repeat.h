//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PATTERNS_UNARY_REPEAT_H_
#define ABULAFIA_PATTERNS_UNARY_REPEAT_H_

#include "abulafia/config.h"

#include "abulafia/parser.h"
#include "abulafia/patterns/pattern.h"
#include "abulafia/support/nil.h"

#include <vector>

namespace ABULAFIA_NAMESPACE {

template <typename PAT_T, int MIN_REP, int MAX_REP>
class Repeat : public Pattern<Repeat<PAT_T, MIN_REP, MAX_REP>> {
  PAT_T operand_;

 public:
  Repeat(const PAT_T op) : operand_(std::move(op)) {}

  PAT_T const& operand() const { return operand_; }
};

template <int MIN_REP = 0, int MAX_REP = 0, typename PAT_T>
inline auto repeat(PAT_T pat) {
  return Repeat<pattern_t<PAT_T>, MIN_REP, MAX_REP>(
      make_pattern(std::move(pat)));
}

// *pattern
template <typename PAT_T,
          typename Enable = enable_if_t<is_valid_unary_operand<PAT_T>()>>
auto operator*(PAT_T pat) {
  return repeat<0, 0>(std::move(pat));
}

// +pattern
template <typename PAT_T,
          typename Enable = enable_if_t<is_valid_unary_operand<PAT_T>()>>
auto operator+(PAT_T&& pat) {
  return repeat<1, 0>(forward<PAT_T>(pat));
}

}  // namespace ABULAFIA_NAMESPACE

#endif
