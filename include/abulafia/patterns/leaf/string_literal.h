//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PATTERNS_LEAF_STRING_LITERAL_H_
#define ABULAFIA_PATTERNS_LEAF_STRING_LITERAL_H_

#include "abulafia/config.h"

#include "abulafia/parser.h"
#include "abulafia/pattern.h"

#include <cassert>
#include <string>

namespace ABULAFIA_NAMESPACE {

// The string literal matches agaisnt a sequence of tokens without
// emmiting anything.
template <typename CHAR_T>
class StringLiteral : public Pattern<StringLiteral<CHAR_T>> {
  std::basic_string<CHAR_T> str_;

 public:
  StringLiteral(std::basic_string<CHAR_T> const& str) : str_(str) {
    assert(str_.size() > 0);
  }

  StringLiteral(std::basic_string<CHAR_T>&& str) : str_(std::move(str)) {
    assert(str_.size() > 0);
  }

  auto begin() const { return str_.begin(); }
  auto end() const { return str_.end(); }
};

template <typename CHAR_T>
inline auto lit(CHAR_T const* str) {
  return StringLiteral<decay_t<CHAR_T>>(str);
}

template <>
struct expr_traits<char const*> {
  enum { is_pattern = false, converts_to_pattern = true };

  static StringLiteral<char> make_pattern(char const* v) { return lit(v); }
};

template <>
struct expr_traits<char32_t const*> {
  enum { is_pattern = false, converts_to_pattern = true };

  static StringLiteral<char32_t> make_pattern(char32_t const* v) {
    return lit(v);
  }
};

template <typename T, typename RECUR_TAG>
struct pattern_traits<StringLiteral<T>, RECUR_TAG>
    : public default_pattern_traits {
  using attr_type = Nil;

  enum {
    ATOMIC = true,
    BACKTRACKS = false,
    PEEKABLE = false,
    FAILS_CLEANLY = false,
    MAY_NOT_CONSUME = false,
  };
};

template <typename T, typename CTX_T>
struct pat_attr_t<StringLiteral<T>, CTX_T> {
  using attr_type = Nil;
};

}  // namespace ABULAFIA_NAMESPACE

#endif
