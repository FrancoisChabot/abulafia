//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_CHAR_SET_DELEGATED_H_
#define ABULAFIA_CHAR_SET_DELEGATED_H_

#include "abulafia/config.h"

#include "abulafia/char_set/char_set.h"
#include "abulafia/support/function_traits.h"

namespace ABULAFIA_NAMESPACE {
namespace char_set {

// Delegates the decision as to wether a character belongs to the set or not
// to a callback.
// Abulafia will assume that cb_ is deterministic.
template <typename CHAR_T, typename CB_T>
struct DelegatedSet {
  using char_t = CHAR_T;

  explicit DelegatedSet(CB_T cb) : cb_(std::move(cb)) {}

  template <typename T>
  bool is_valid(T c) const {
    return cb_(c);
  }

 private:
  CB_T cb_;
};

template <typename CHAR_T, typename CB_T>
struct is_char_set<DelegatedSet<CHAR_T, CB_T>> : public std::true_type {};

template <typename CHAR_T, typename CB_T>
auto delegated(CB_T const& cb) {
  return DelegatedSet<CHAR_T, CB_T>(cb);
}

template <typename CB_T>
auto delegated(CB_T const& cb) {
  using char_t = callable_argument_t<CB_T, 0>;
  return DelegatedSet<char_t, CB_T>(cb);
}

}  // namespace char_set

}  // namespace ABULAFIA_NAMESPACE

#endif