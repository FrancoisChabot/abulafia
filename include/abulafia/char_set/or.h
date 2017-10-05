//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_CHAR_SET_OR_H_
#define ABULAFIA_CHAR_SET_OR_H_

#include "abulafia/config.h"

#include "abulafia/char_set/char_set.h"
#include "abulafia/support/type_traits.h"

namespace ABULAFIA_NAMESPACE {
namespace char_set {

template <typename LHS_T, typename RHS_T>
struct Or : public CharacterSet {
  using char_t = typename LHS_T::char_t;

  Or(LHS_T lhs, RHS_T rhs) : lhs_(std::move(lhs)), rhs_(std::move(rhs)) {}

  bool is_valid(char_t const& c) const {
    return lhs_.is_valid(c) || rhs_.is_valid(c);
  }

 private:
  LHS_T lhs_;
  RHS_T rhs_;

  static_assert(is_char_set<LHS_T>::value);
  static_assert(is_char_set<RHS_T>::value);
  static_assert(is_same<typename LHS_T::char_t, typename RHS_T::char_t>::value);
};

template <typename LHS_T, typename RHS_T>
auto or_impl(LHS_T lhs, RHS_T rhs) {
  auto lhs_cs = to_char_set(std::decay_t<LHS_T>(lhs));
  auto rhs_cs = to_char_set(std::decay_t<RHS_T>(rhs));

  return Or<decltype(lhs_cs), decltype(rhs_cs)>(lhs_cs, rhs_cs);
}

template <typename LHS_T, typename RHS_T,
          typename = enable_if_t<is_char_set<LHS_T>::value ||
                                 is_char_set<RHS_T>::value>>
auto operator|(LHS_T lhs, RHS_T rhs) {
  return or_impl(lhs, rhs);
}

}  // namespace char_set
}  // namespace ABULAFIA_NAMESPACE
#endif