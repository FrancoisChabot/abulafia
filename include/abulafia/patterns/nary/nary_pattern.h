//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PATTERNS_NARY_NARY_H_
#define ABULAFIA_PATTERNS_NARY_NARY_H_

#include "abulafia/config.h"

#include "abulafia/support/type_traits.h"

#include <variant>

namespace ABULAFIA_NAMESPACE {
namespace detail {
template <typename T, template <typename...> typename PAT_T>
struct is_nary_pattern : public std::false_type {};

template <typename... T, template <typename...> typename PAT_T>
struct is_nary_pattern<PAT_T<T...>, PAT_T> : public std::true_type {};

template <template <typename...> typename PAT_T, typename LHS_T, typename RHS_T,
          typename Enable = void>
struct NaryPatternBuilder {
  using type = PAT_T<decay_t<LHS_T>, decay_t<RHS_T>>;

  template <typename LHS_P_T, typename RHS_P_T>
  static auto build(LHS_P_T&& lhs, RHS_P_T&& rhs) {
    return type(std::make_tuple(forward<LHS_P_T>(lhs), forward<RHS_P_T>(rhs)));
  }
};

template <template <typename...> typename PAT_T, typename RHS_T,
          typename... LHS_T>
struct NaryPatternBuilder<PAT_T, PAT_T<LHS_T...>, RHS_T,
                          enable_if_t<!is_nary_pattern<RHS_T, PAT_T>()>> {
  using type = PAT_T<LHS_T..., decay_t<RHS_T>>;

  template <typename LHS_P_T, typename RHS_P_T>
  static auto build(LHS_P_T&& lhs, RHS_P_T&& rhs) {
    return type(
        std::tuple_cat(lhs.childs(), std::make_tuple(forward<RHS_P_T>(rhs))));
  }
};

template <template <typename...> typename PAT_T, typename LHS_T,
          typename... RHS_T>
struct NaryPatternBuilder<PAT_T, LHS_T, PAT_T<RHS_T...>,
                          enable_if_t<!is_nary_pattern<LHS_T, PAT_T>()>> {
  using type = PAT_T<decay_t<LHS_T>, RHS_T...>;

  template <typename LHS_P_T, typename RHS_P_T>
  static auto build(LHS_P_T&& lhs, RHS_P_T&& rhs) {
    return type(
        std::tuple_cat(std::make_tuple(forward<LHS_P_T>(lhs)), rhs.childs()));
  }
};

template <template <typename...> typename PAT_T, typename... LHS_T,
          typename... RHS_T>
struct NaryPatternBuilder<PAT_T, PAT_T<LHS_T...>, PAT_T<RHS_T...>, void> {
  using type = PAT_T<LHS_T..., RHS_T...>;

  template <typename LHS_P_T, typename RHS_P_T>
  static auto build(LHS_P_T&& lhs, RHS_P_T&& rhs) {
    return type(std::tuple_cat(lhs.childs(), rhs.childs()));
  }
};
}  // namespace detail

}  // namespace ABULAFIA_NAMESPACE

#endif