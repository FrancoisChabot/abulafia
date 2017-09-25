//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_WITH_SKIPPER_H_
#define ABULAFIA_WITH_SKIPPER_H_

#include "abulafia/config.h"

#include "abulafia/contexts/skipper_adapter.h"
#include "abulafia/parser.h"
#include "abulafia/pattern.h"
#include "abulafia/support/type_traits.h"

#include <memory>

namespace ABULAFIA_NAMESPACE {

// Pattern for a signed integer
template <typename CHILD_PAT_T, typename SKIP_T>
class WithSkipper : public Pattern<WithSkipper<CHILD_PAT_T, SKIP_T>> {
  CHILD_PAT_T child_pat_;
  SKIP_T skip_pat_;

 public:
  CHILD_PAT_T const& getChild() const { return child_pat_; }
  SKIP_T const& getSkip() const { return skip_pat_; }

  WithSkipper(CHILD_PAT_T const& c, SKIP_T const& s)
      : child_pat_(c), skip_pat_(s) {}
};

template <typename CHILD_PAT_T, typename SKIP_T, typename CTX_T>
struct pat_attr_t<WithSkipper<CHILD_PAT_T, SKIP_T>, CTX_T> {
  using attr_type = attr_t<CHILD_PAT_T, CTX_T>;
};

template <typename CHILD_PAT_T, typename SKIP_T, typename RECUR_TAG>
struct pattern_traits<WithSkipper<CHILD_PAT_T, SKIP_T>, RECUR_TAG>
    : public default_pattern_traits {
  enum {
    ATOMIC = pattern_traits<CHILD_PAT_T, RECUR_TAG>::ATOMIC,
    BACKTRACKS = true,  // Maybe not...
    FAILS_CLEANLY = pattern_traits<CHILD_PAT_T, RECUR_TAG>::FAILS_CLEANLY,
    PEEKABLE = false,
    MAY_NOT_CONSUME = pattern_traits<CHILD_PAT_T, RECUR_TAG>::MAY_NOT_CONSUME,
  };
};

template <typename CTX_T, typename DST_T, typename CHILD_PAT_T, typename SKIP_T>
class Parser<CTX_T, DST_T, WithSkipper<CHILD_PAT_T, SKIP_T>>
    : public ParserBase<CTX_T, DST_T, PARSER_OPT_NO_SKIP> {
  // PARSER_OPT_NO_SKIP because we need to kill any existing skipper
  using PAT_T = WithSkipper<CHILD_PAT_T, SKIP_T>;

  using sub_ctx_t = SkipperAdapter<CTX_T, SKIP_T>;
  Parser<sub_ctx_t, DST_T, CHILD_PAT_T> child_parser_;

 public:
  Parser(CTX_T& ctx, DST_T& dst, PAT_T const& pat)
      : ParserBase<CTX_T, DST_T, PARSER_OPT_NO_SKIP>(ctx, dst),
        child_parser_(force_lvalue(sub_ctx_t(ctx, pat.getSkip())), dst,
                      pat.getChild()) {}

  result consume(CTX_T& ctx, DST_T& dst, PAT_T const& pat) {
    sub_ctx_t sub_ctx(ctx, pat.getSkip());
    return child_parser_.consume(sub_ctx, dst, pat.getChild());
  }
};

template <typename PAT_T, typename SKIP_T>
auto apply_skipper(PAT_T&& pat, SKIP_T&& skip) {
  return WithSkipper<std::decay_t<PAT_T>, std::decay_t<SKIP_T>>(
      std::forward<PAT_T>(pat), std::forward<SKIP_T>(skip));
}

}  // namespace ABULAFIA_NAMESPACE

#endif
