//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PATTERNS_UNARY_NOT_H_
#define ABULAFIA_PATTERNS_UNARY_NOT_H_

#include "abulafia/config.h"

#include "abulafia/parser.h"
#include "abulafia/pattern.h"
#include "abulafia/support/assert.h"

namespace ABULAFIA_NAMESPACE {

template <typename PAT_T>
class Not : public Pattern<Not<PAT_T>> {
  PAT_T child_;

 public:
  Not(const PAT_T& child) : child_(child) {}
  Not(PAT_T&& child) : child_(std::move(child)) {}

  PAT_T const& operand() const { return child_; }
};

template <typename CTX_T, typename CHILD_PAT_T>
class Parser<CTX_T, Nil, Not<CHILD_PAT_T>> : public ParserBase<CTX_T, Nil> {
  using DST_T = Nil;
  using PAT_T = Not<CHILD_PAT_T>;
  using child_parser_t = Parser<CTX_T, DST_T, CHILD_PAT_T>;

  child_parser_t parser_;

 public:
  Parser(CTX_T& ctx, DST_T& dst, PAT_T const& pat)
      : ParserBase<CTX_T, Nil>(ctx, dst), parser_(ctx, dst, pat.operand()) {
    constexpr bool backtrack = !pattern_traits<CHILD_PAT_T, void>::PEEKABLE;

    if (backtrack) {
      ctx.prepare_rollback();
    }
  }

  result consume(CTX_T& ctx, DST_T& dst, PAT_T const& pat) {
    if (this->performSkip(ctx) == result::PARTIAL) {
      return result::PARTIAL;
    }

    constexpr bool backtrack = !pattern_traits<CHILD_PAT_T, void>::PEEKABLE;

    if (backtrack) {
      auto status = parser_.consume(ctx, dst, pat.operand());
      switch (status) {
        case result::SUCCESS:
          // just commit the rollback anyways, this allows us to promise
          // FAILS_CLEANLY
          ctx.commit_rollback();
          return result::FAILURE;

        case result::FAILURE:
          ctx.commit_rollback();
          return result::SUCCESS;

        case result::PARTIAL:
          return result::PARTIAL;
      }

      abu_unreachable();

    } else {
      return peek(ctx, pat);
    }
  }

  result peek(CTX_T& ctx, PAT_T const& pat) {
    auto status = parser_.peek(ctx, pat.operand());

    switch (status) {
      case result::SUCCESS:
        return result::FAILURE;
      case result::FAILURE:
        return result::SUCCESS;
      case result::PARTIAL:
        return result::PARTIAL;
    }
    abu_unreachable();
  }
};

template <typename PAT_T, typename RECUR_TAG>
struct pattern_traits<Not<PAT_T>, RECUR_TAG> : public default_pattern_traits {
  using attr_type = Nil;

  enum {

    // if we can get our info by peeking, then we will not need to backtrack
    BACKTRACKS = !pattern_traits<PAT_T, RECUR_TAG>::PEEKABLE,
    PEEKABLE = pattern_traits<PAT_T, RECUR_TAG>::PEEKABLE,
    FAILS_CLEANLY = true,
    MAY_NOT_CONSUME = true,
    ATOMIC = true,
  };
};

template <typename PAT_T, typename CTX_T>
struct pat_attr_t<Not<PAT_T>, CTX_T> {
  using attr_type = Nil;
};
}  // namespace ABULAFIA_NAMESPACE

#endif
