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
struct Or {
  using char_t = typename LHS_T::char_t;

  Or(LHS_T lhs, RHS_T rhs) : lhs_(std::move(lhs)), rhs_(std::move(rhs)) {}

  template <typename T>
  bool is_valid(T const& c) const {
    return lhs_.is_valid(c) || rhs_.is_valid(c);
  }

 private:
  LHS_T lhs_;
  RHS_T rhs_;

  static_assert(is_char_set<LHS_T>::value,
                "Trying to Or something that's not a character set.");
  static_assert(is_char_set<RHS_T>::value,
                "Trying to Or something that's not a character set.");
  static_assert(is_same<typename LHS_T::char_t, typename RHS_T::char_t>::value,
                "character set mismatch");
};

template <typename LHS_T, typename RHS_T>
struct is_char_set<Or<LHS_T, RHS_T>> : public std::true_type {};

}  // namespace char_set
}  // namespace ABULAFIA_NAMESPACE
#endif
