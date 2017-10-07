//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PATTERNS_TRAITS_H_
#define ABULAFIA_PATTERNS_TRAITS_H_

#include "abulafia/config.h"

#include "abulafia/support/nil.h"
#include "abulafia/support/type_traits.h"

#include <type_traits>

namespace ABULAFIA_NAMESPACE {

// Default set of traits. Nothing qualifies as a pattern unless specified
// otherwise.
template <typename T, typename Enable = void>
struct expr_traits {
  enum { is_pattern = false, converts_to_pattern = false };
};

template <typename T>
constexpr bool is_pattern() {
  return expr_traits<decay_t<T>>::is_pattern;
}

template <typename T>
constexpr bool converts_to_pattern() {
  return expr_traits<decay_t<T>>::converts_to_pattern;
}

// Determines wether a type can be used as the operand of a unary operator.
template <typename T>
constexpr bool is_valid_unary_operand() {
  return is_pattern<T>();
}

// Determines wether two types can be used as binary operator operands.
template <typename LHS_T, typename RHS_T>
constexpr bool are_valid_binary_operands() {
  return (is_pattern<LHS_T>() && converts_to_pattern<RHS_T>()) ||
         (converts_to_pattern<LHS_T>() && is_pattern<RHS_T>()) ||
         (is_pattern<LHS_T>() && is_pattern<RHS_T>());
}

template <typename T>
using pattern_t =
    decay_t<decltype(expr_traits<decay_t<T>>::make_pattern(std::declval<T>()))>;

// Utility function to make a pattern out of a value (if possible).
template <typename T>
inline auto make_pattern(T&& p) {
  static_assert(expr_traits<decay_t<T>>::is_pattern ||
                    expr_traits<decay_t<T>>::converts_to_pattern,
                "Cannot create pattern from T");
  return expr_traits<decay_t<T>>::make_pattern(forward<T>(p));
}

}  // namespace ABULAFIA_NAMESPACE
#endif
