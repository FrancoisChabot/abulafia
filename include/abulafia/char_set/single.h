//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_CHAR_SET_SINGLE_H_
#define ABULAFIA_CHAR_SET_SINGLE_H_

#include "abulafia/config.h"

#include "abulafia/char_set/char_set.h"

namespace ABULAFIA_NAMESPACE {
namespace char_set {

template <typename CHAR_T>
struct Single : public CharacterSet {
  using char_t = CHAR_T;

  explicit Single(CHAR_T c) : character_(c) {}

  template <typename T>
  bool is_valid(T const& token) const {
    return token == character_;
  }

 private:
  CHAR_T character_;
};

template <typename CHAR_T>
auto single(CHAR_T c) {
  return Single<CHAR_T>(c);
}

template <>
struct to_char_set_impl<char, void> {
  static Single<char> convert(char const& v) { return Single<char>(v); }
};

}  // namespace char_set
}  // namespace ABULAFIA_NAMESPACE
#endif