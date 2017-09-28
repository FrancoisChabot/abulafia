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

#include "abulafia/parser.h"
#include "abulafia/pattern.h"

namespace ABULAFIA_NAMESPACE {

// The Character pattern checks the next token against its character
// set. If the test passes, the next character is emmited and it succeeds.
template <typename CHARSET_T>
class Char : public Pattern<Char<CHARSET_T>> {
  CHARSET_T char_set_;

 public:
  Char(CHARSET_T&& chars) : char_set_(std::move(chars)) {}
  Char(CHARSET_T const& chars) : char_set_(chars) {}

  CHARSET_T const& char_set() const { return char_set_; }
};

// Traits for the character pattern.
template <typename CHARSET_T, typename RECUR_TAG>
struct pattern_traits<Char<CHARSET_T>, RECUR_TAG>
    : public default_pattern_traits {
  enum {
    ATOMIC = true,
    BACKTRACKS = false,
    FAILS_CLEANLY = true,
    PEEKABLE = true,
    MAY_NOT_CONSUME = false,
  };
};

template <typename CHARSET_T, typename CTX_T>
struct pat_attr_t<Char<CHARSET_T>, CTX_T> {
  using attr_type = typename CHARSET_T::char_t;
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

template <typename CHAR_T, std::size_t LEN>
inline auto char_(const CHAR_T (&l)[LEN]) {
  return char_(char_set::set(l));
}

template <typename T>
auto char_(T begin, T end) {
  return Char<char_set::Range<T>>(char_set::Range<T>(begin, end));
}

template <typename T>
auto char_(std::initializer_list<T> l) {
  return Char<char_set::Set<T>>(char_set::Set<T>(l));
}

template <typename CHAR_SET_T>
struct expr_traits<CHAR_SET_T,
                   enable_if_t<char_set::is_char_set<CHAR_SET_T>::value>> {
  enum { is_pattern = false, converts_to_pattern = true };

  static Char<CHAR_SET_T> make_pattern(CHAR_SET_T const& v) { return char_(v); }
};

template <typename CTX_T, typename DST_T, typename CHARSET_T>
class Parser<CTX_T, DST_T, Char<CHARSET_T>> : public ParserBase<CTX_T, DST_T> {
  using PAT_T = Char<CHARSET_T>;

 public:
  Parser(CTX_T& ctx, DST_T& dst, PAT_T const&)
      : ParserBase<CTX_T, DST_T>(ctx, dst) {}

  result consume(CTX_T& ctx, DST_T& dst, PAT_T const& pat) {
    // std::cout << "char is writing at: " << std::hex << (uint64_t)&dst <<
    // std::endl;
    if (this->performSkip(ctx) == result::PARTIAL) {
      return result::PARTIAL;
    }

    if (ctx.empty()) {
      return ctx.final_buffer() ? result::FAILURE : result::PARTIAL;
    }

    auto next = ctx.next();
    if (pat.char_set().is_valid(next)) {
      dst = next;
      ctx.advance();
      return result::SUCCESS;
    }
    return result::FAILURE;
  }

  result peek(CTX_T const& ctx, PAT_T const& pat) const {
    if (ctx.empty()) {
      return ctx.final_buffer() ? result::FAILURE : result::PARTIAL;
    }

    return pat.char_set().is_valid(ctx.next()) ? result::SUCCESS
                                               : result::FAILURE;
  }
};

}  // namespace ABULAFIA_NAMESPACE

#endif
