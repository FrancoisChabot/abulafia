//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PATTERNS_UNARY_ATTR_CAST_H_
#define ABULAFIA_PATTERNS_UNARY_ATTR_CAST_H_

#include "abulafia/config.h"

#include "abulafia/parser.h"
#include "abulafia/pattern.h"

namespace ABULAFIA_NAMESPACE {

template <typename ATTR_T, typename PAT_T>
class AttrCast : public Pattern<AttrCast<ATTR_T, PAT_T>> {
  PAT_T pat_;

 public:
  AttrCast(PAT_T const& pat) : pat_(pat) {}
  AttrCast(PAT_T&& pat) : pat_(std::move(pat)) {}

  PAT_T const& operand() const { return pat_; }
};

template <typename CTX_T, typename DST_T, typename ATTR_T, typename CHILD_PAT_T>
class Parser<CTX_T, DST_T, AttrCast<ATTR_T, CHILD_PAT_T>>
    : public ParserBase<CTX_T, DST_T, PARSER_OPT_NO_SKIP> {
  using PAT_T = AttrCast<ATTR_T, CHILD_PAT_T>;
  using parser_t = Parser<CTX_T, DST_T, CHILD_PAT_T>;

  parser_t parser_;

 public:
  Parser(CTX_T& ctx, DST_T& dst, PAT_T const& pat)
      : ParserBase<CTX_T, DST_T, PARSER_OPT_NO_SKIP>(ctx, dst),
        parser_(ctx, dst, pat.operand()) {}

  result consume(CTX_T& ctx, DST_T& dst, PAT_T const& pat) {
    return parser_.consume(ctx, dst, pat.operand());
  }

  result peek(CTX_T& ctx, PAT_T const& pat) {
    return parser_.peek(ctx, pat.operand());
  }
};

template <typename ATTR_T, typename PAT_T>
inline auto cast(PAT_T&& pat) {
  using real_pat_t = pattern_t<PAT_T>;
  return AttrCast<ATTR_T, real_pat_t>(make_pattern(forward<PAT_T>(pat)));
}

template <typename PAT_T>
inline auto ignore(PAT_T&& pat) {
  using real_pat_t = pattern_t<PAT_T>;
  return AttrCast<Nil, real_pat_t>(make_pattern(forward<PAT_T>(pat)));
}

template <typename ATTR_T, typename PAT_T, typename RECUR_TAG>
struct pattern_traits<AttrCast<ATTR_T, PAT_T>, RECUR_TAG>
    : public default_pattern_traits {
  enum {
    BACKTRACKS = pattern_traits<PAT_T, RECUR_TAG>::BACKTRACKS,
    FAILS_CLEANLY = pattern_traits<PAT_T, RECUR_TAG>::FAILS_CLEANLY,
    MAY_NOT_CONSUME = pattern_traits<PAT_T, RECUR_TAG>::MAY_NOT_CONSUME,
    PEEKABLE = pattern_traits<PAT_T, RECUR_TAG>::PEEKABLE,
    ATOMIC = pattern_traits<PAT_T, RECUR_TAG>::ATOMIC,
    APPENDS_DST = pattern_traits<PAT_T, RECUR_TAG>::APPENDS_DST,
    STABLE_APPENDS = pattern_traits<PAT_T, RECUR_TAG>::STABLE_APPENDS,
  };
};

template <typename ATTR_T, typename PAT_T, typename CTX_T>
struct pat_attr_t<AttrCast<ATTR_T, PAT_T>, CTX_T> {
  using attr_type = ATTR_T;
};

}  // namespace ABULAFIA_NAMESPACE

#endif
