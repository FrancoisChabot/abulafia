//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PATTERNS_LEAF_CHAR_LITERAL_H_
#define ABULAFIA_PATTERNS_LEAF_CHAR_LITERAL_H_

#include "abulafia/config.h"

#include "abulafia/pattern.h"
#include "abulafia/patterns/leaf/character.h"
#include "abulafia/patterns/unary/attr_cast.h"

namespace ABULAFIA_NAMESPACE {

// Plot twist! There is no such thing as the CharLiteral pattern. It's just a
// casted character pattern.
template <typename CHAR_T>
inline auto lit(CHAR_T chr) {
  auto res = char_(chr);
  return cast<Nil>(res);
}

template <typename T>
using CharLiteral = decltype(lit(std::declval<T>()));


// Allow 'c' to be used as pattern when in a binary expression with another pattern.
template <>
struct expr_traits<char> {
  enum { is_pattern = false, converts_to_pattern = true };

  static auto make_pattern(char v) { return lit(v); }
};

template <>
struct expr_traits<char32_t> {
  enum { is_pattern = false, converts_to_pattern = true };

  static auto make_pattern(char32_t v) { return lit(v); }
};
}  // namespace ABULAFIA_NAMESPACE

#endif
