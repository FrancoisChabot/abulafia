//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_SUPPORT_VARIANT_UTILS_H
#define ABULAFIA_SUPPORT_VARIANT_UTILS_H

#include "abulafia/config.h"

#include <variant>
#include "abulafia/support/type_traits.h"

namespace ABULAFIA_NAMESPACE {

// convenience to detect if a type is a variant
template <typename>
struct is_variant : public std::false_type {};

template <typename... ALL_T>
struct is_variant<std::variant<ALL_T...>> : public std::true_type {};

// Concatenates types into a variant
template <typename LHS_T, typename RHS_T>
struct variant_cat {
  using type =
      typename std::conditional<std::is_same<LHS_T, RHS_T>::value, LHS_T,
                                std::variant<LHS_T, RHS_T>>::type;
};

template <typename LHS_T, typename... RHS_VAR_T>
struct variant_cat<LHS_T, std::variant<RHS_VAR_T...>> {
  using type =
      typename std::conditional<is_one_of<LHS_T, RHS_VAR_T...>::value,
                                std::variant<RHS_VAR_T...>,
                                std::variant<LHS_T, RHS_VAR_T...>>::type;
};

template <typename... LHS_VAR_T, typename RHS_T>
struct variant_cat<std::variant<LHS_VAR_T...>, RHS_T> {
  using type =
      typename std::conditional<is_one_of<RHS_T, LHS_VAR_T...>::value,
                                std::variant<LHS_VAR_T...>,
                                std::variant<LHS_VAR_T..., RHS_T>>::type;
};
}  // namespace ABULAFIA_NAMESPACE

#endif