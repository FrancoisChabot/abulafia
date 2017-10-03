//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PATTERNS_LEAF_CHARACTER_H_
#define ABULAFIA_PATTERNS_LEAF_CHARACTER_H_

#include "abulafia/config.h"

#include "abulafia/char_set/any.h"
#include "abulafia/char_set/char_set.h"
#include "abulafia/char_set/range.h"
#include "abulafia/char_set/set.h"
#include "abulafia/char_set/single.h"

#include "abulafia/patterns/leaf/leaf_pattern.h"

namespace ABULAFIA_NAMESPACE {

// The Character pattern checks the next token against its character
// set. If the test passes, the next character is emmited and it succeeds.
template <typename CHARSET_T>
class Char : public LeafPattern<Char<CHARSET_T>> {
  CHARSET_T char_set_;

 public:
  Char(CHARSET_T chars) : char_set_(std::move(chars)) {}

  CHARSET_T const& char_set() const { return char_set_; }
};

template <typename T = char>
auto char_() {
  return Char<char_set::Any<T>>(char_set::Any<T>());
}

template <typename T>
enable_if_t<!char_set::is_char_set<T>::value, Char<char_set::Single<T>>> char_(
    T const& c) {
  return Char<char_set::Single<T>>(char_set::Single<T>(c));
}

template <typename T>
enable_if_t<char_set::is_char_set<decay_t<T>>::value, Char<decay_t<T>>> char_(
    T&& chars) {
  return Char<decay_t<T>>(forward<T>(chars));
}

template <typename T>
auto char_(T begin, T end) {
  return Char<char_set::Range<T>>(char_set::Range<T>(begin, end));
}

inline auto char_(const char * str) {
  return char_(char_set::set(str));
}

template <typename CHAR_SET_T>
struct expr_traits<CHAR_SET_T,
                   enable_if_t<char_set::is_char_set<CHAR_SET_T>::value>> {
  enum { is_pattern = false, converts_to_pattern = true };

  static Char<CHAR_SET_T> make_pattern(CHAR_SET_T const& v) { return char_(v); }
};

}  // namespace ABULAFIA_NAMESPACE

#endif
