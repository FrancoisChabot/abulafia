//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_CHAR_SET_OPERATORS_H_
#define ABULAFIA_CHAR_SET_OPERATORS_H_

#include "abulafia/config.h"

#include "abulafia/char_set/not.h"
#include "abulafia/char_set/or.h"
#include "abulafia/char_set/single.h"
#include "abulafia/support/type_traits.h"

namespace ABULAFIA_NAMESPACE {

namespace char_set {

// Not (~C)
template <typename ARG_T,
          typename Enable = enable_if_t<is_char_set<ARG_T>::value>>
Not<ARG_T> operator~(ARG_T arg) {
  return Not<ARG_T>{std::move(arg)};
}

// OR (C1 | C2)
template <typename LHS_T, typename RHS_T,
          typename Enable = enable_if_t<is_char_set<LHS_T>::value &&
                                        is_char_set<RHS_T>::value>>
Or<LHS_T, RHS_T> operator|(LHS_T lhs, RHS_T rhs) {
  return Or<LHS_T, RHS_T>{std::move(lhs), std::move(rhs)};
}

// OR (C1 | char)
template <typename LHS_T,
          typename Enable = enable_if_t<is_char_set<LHS_T>::value>>
auto operator|(LHS_T lhs, typename LHS_T::char_t rhs) {
  using single_t = Single<typename LHS_T::char_t>;
  return Or<LHS_T, single_t>{std::move(lhs), single_t(std::move(rhs))};
}

}  // namespace char_set

}  // namespace ABULAFIA_NAMESPACE

#endif
