//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_CHAR_SET_NOT_H_
#define ABULAFIA_CHAR_SET_NOT_H_

#include "abulafia/config.h"

#include "abulafia/char_set/char_set.h"

#include <utility>
#include <type_traits>

namespace ABULAFIA_NAMESPACE {
namespace char_set {

template <typename ARG_T>
struct Not : public CharacterSet {
  using char_t = typename ARG_T::char_t;

  explicit Not(ARG_T arg) : arg_(std::move(arg)) {}

  bool is_valid(char_t const& c) const {
    return !arg_.is_valid(c);
  }

 private:
  ARG_T arg_;

  static_assert(is_char_set<ARG_T>::value);
};

template <typename ARG_T>
struct is_char_set<Not<ARG_T>> : public std::true_type {};

template <typename ARG_T,
          typename Enable = std::enable_if_t<is_char_set<ARG_T>::value>>
Not<ARG_T> operator~(ARG_T arg) {
  return Not<ARG_T>{std::move(arg)};
}

}  // namespace char_set
}  // namespace ABULAFIA_NAMESPACE
#endif