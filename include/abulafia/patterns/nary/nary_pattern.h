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

// Checks wether T is a PAT_T<...>
template <typename T, template <typename...> typename PAT_T>
struct is_nary_pattern : public std::false_type {};

template <typename... T, template <typename...> typename PAT_T>
struct is_nary_pattern<PAT_T<T...>, PAT_T> : public std::true_type {};

// A op B, where neither A or B are the resulting nary pattern type
template <template <typename...> typename PAT_T, typename LHS_T, typename RHS_T,
          typename Enable = void>
struct NaryPatternBuilder {
  using type = PAT_T<decay_t<LHS_T>, decay_t<RHS_T>>;

  static auto build(LHS_T lhs, RHS_T rhs) {
    return type(std::make_tuple(std::move(lhs), std::move(rhs)));
  }
};

// A op B op C
//        ^^
template <template <typename...> typename PAT_T, typename RHS_T,
          typename... LHS_T>
struct NaryPatternBuilder<PAT_T, PAT_T<LHS_T...>, RHS_T,
                          enable_if_t<!is_nary_pattern<RHS_T, PAT_T>()>> {
  using type = PAT_T<LHS_T..., RHS_T>;

  static auto build(PAT_T<LHS_T...> const& lhs, RHS_T rhs) {
    return type(
        std::tuple_cat(lhs.childs(), std::make_tuple(std::move(rhs))));
  }
};

// A op (B op C)
//   ^^ 
template <template <typename...> typename PAT_T, typename LHS_T,
          typename... RHS_T>
struct NaryPatternBuilder<PAT_T, LHS_T, PAT_T<RHS_T...>,
                          enable_if_t<!is_nary_pattern<LHS_T, PAT_T>()>> {
  using type = PAT_T<LHS_T, RHS_T...>;

  static auto build(LHS_T lhs, PAT_T<RHS_T...> const& rhs) {
    return type(
        std::tuple_cat(std::make_tuple(std::move(lhs)), rhs.childs()));
  }
};

// (A op B) op (C op D)
//          ^^
template <template <typename...> typename PAT_T, typename... LHS_T,
          typename... RHS_T>
struct NaryPatternBuilder<PAT_T, PAT_T<LHS_T...>, PAT_T<RHS_T...>, void> {
  using type = PAT_T<LHS_T..., RHS_T...>;

  static auto build(PAT_T<LHS_T...> const& lhs, PAT_T<RHS_T...> const& rhs) {
    return type(std::tuple_cat(lhs.childs(), rhs.childs()));
  }
};

}  // namespace detail

}  // namespace ABULAFIA_NAMESPACE

#endif
