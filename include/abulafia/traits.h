//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_TRAITS_H_
#define ABULAFIA_TRAITS_H_

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
  static void make_pattern(T) {
    // We use is_same here to ensure the assert does not get evaluated until
    // the template is instantiated.
    static_assert(!is_same<T, T>::value,
                  "type is not a pattern, or convertible to one");
  }
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
using pattern_t = decay_t<decltype(
    expr_traits<decay_t<T>>::make_pattern(*reinterpret_cast<decay_t<T>*>(0)))>;

// Utility function to make a pattern out of a value (if possible).
template <typename T>
inline auto make_pattern(T&& p) {
  static_assert(expr_traits<decay_t<T>>::is_pattern ||
                    expr_traits<decay_t<T>>::converts_to_pattern,
                "Cannot create pattern from T");
  return expr_traits<decay_t<T>>::make_pattern(forward<T>(p));
}

template <typename T, typename Enable = void>
struct is_collection : public std::false_type {};

template <typename T>
struct is_collection<T, typename blank_type_<typename T::value_type>::type>
    : public std::true_type {};

template <typename T, typename ENABLE = void>
struct reset_if_collection {
  static void exec(T&) {}
};

template <typename T>
struct reset_if_collection<T, enable_if_t<is_collection<T>::value>> {
  static void exec(T& c) { c.clear(); }
};
}  // namespace ABULAFIA_NAMESPACE
#endif
