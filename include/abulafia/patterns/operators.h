//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PATTERNS_OPERATORS_H_
#define ABULAFIA_PATTERNS_OPERATORS_H_

#include "abulafia/config.h"

#include "abulafia/patterns/binary/except.h"
#include "abulafia/patterns/binary/list.h"
#include "abulafia/patterns/nary/alternative.h"
#include "abulafia/patterns/nary/nary.h"
#include "abulafia/patterns/nary/sequence.h"
#include "abulafia/patterns/unary/not.h"
#include "abulafia/patterns/unary/optional.h"
#include "abulafia/patterns/unary/repeat.h"
#include "abulafia/support/type_traits.h"

namespace ABULAFIA_NAMESPACE {

// OPERATORS ON PATTERNS

// UNARY OPERATORS


// !pattern
template <typename PAT_T,
          typename Enable = enable_if_t<is_valid_unary_operand<PAT_T>(),
                                        Not<pattern_t<PAT_T>>>>
auto operator!(PAT_T&& pat) {
  return Not<pattern_t<PAT_T>>(make_pattern(pat));
}

// !pattern
template <typename PAT_T,
          typename Enable = enable_if_t<is_valid_unary_operand<PAT_T>(),
                                        Not<pattern_t<PAT_T>>>>
auto operator-(PAT_T&& pat) {
  return Optional<pattern_t<PAT_T>>(make_pattern(pat));
}


// BINARY OPERATORS

// pattern % pattern

template <
    typename LHS_T, typename RHS_T,
    typename Enable = enable_if_t<are_valid_binary_operands<LHS_T, RHS_T>()>>
auto operator%(LHS_T&& lhs, RHS_T&& rhs) {
  return list(forward<LHS_T>(lhs), forward<RHS_T>(rhs));
}

// pattern - pattern
template <
    typename LHS_T, typename RHS_T,
    typename Enable = enable_if_t<are_valid_binary_operands<LHS_T, RHS_T>()>>
auto operator-(LHS_T&& lhs, RHS_T&& rhs) {
  return except(forward<LHS_T>(lhs), forward<RHS_T>(rhs));
}

// NARY operators

// pattern | pattern
template <typename LHS_T, typename RHS_T>
std::enable_if_t<are_valid_binary_operands<LHS_T, RHS_T>(),
                 typename detail::NaryPatternBuilder<Alt, pattern_t<LHS_T>,
                                                     pattern_t<RHS_T>>::type>
operator|(LHS_T&& lhs, RHS_T&& rhs) {
  return detail::NaryPatternBuilder<Alt, pattern_t<LHS_T>, pattern_t<RHS_T>>::
      build(make_pattern(forward<LHS_T>(lhs)),
            make_pattern(forward<RHS_T>(rhs)));
}

template <typename LHS_T, typename RHS_T>
std::enable_if_t<are_valid_binary_operands<LHS_T, RHS_T>(),
                 typename detail::NaryPatternBuilder<Seq, pattern_t<LHS_T>,
                                                     pattern_t<RHS_T>>::type>
operator>>(LHS_T&& lhs, RHS_T&& rhs) {
  return detail::NaryPatternBuilder<Seq, pattern_t<LHS_T>, pattern_t<RHS_T>>::
      build(make_pattern(forward<LHS_T>(lhs)),
            make_pattern(forward<RHS_T>(rhs)));
}

}  // namespace ABULAFIA_NAMESPACE

#endif
