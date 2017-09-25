//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PATTERNS_BINARY_EXCEPT_H_
#define ABULAFIA_PATTERNS_BINARY_EXCEPT_H_

#include "abulafia/config.h"

#include "abulafia/parser.h"
#include "abulafia/pattern.h"
#include "abulafia/support/assert.h"

#include <variant>

namespace ABULAFIA_NAMESPACE {

template <typename OP_T, typename NEG_T>
class Except : public Pattern<Except<OP_T, NEG_T>> {
  OP_T op_;
  NEG_T neg_;

 public:
  using op_t = OP_T;
  using neg_t = NEG_T;

  Except(op_t const& op_p, neg_t const& neg_p) : op_(op_p), neg_(neg_p) {}

  op_t const& op() const { return op_; }
  neg_t const& neg() const { return neg_; }
};

template <typename CTX_T, typename DST_T, typename OP_T, typename NEG_T>
class Parser<CTX_T, DST_T, Except<OP_T, NEG_T>>
    : public ParserBase<CTX_T, DST_T> {
  using PAT_T = Except<OP_T, NEG_T>;

  using op_parser_t = Parser<CTX_T, DST_T, OP_T>;
  using neg_parser_t = Parser<CTX_T, Nil, NEG_T>;

  using child_parsers_t = std::variant<neg_parser_t, op_parser_t>;
  child_parsers_t child_parsers_;

 public:
  Parser(CTX_T& ctx, DST_T& dst, PAT_T const& pat)
      : ParserBase<CTX_T, DST_T>(ctx, dst),
        child_parsers_(std::in_place_index_t<0>(), ctx, nil, pat.neg()) {
    constexpr bool backtrack = !pattern_traits<NEG_T, void>::FAILS_CLEANLY;

    if (backtrack) {
      ctx.prepare_rollback();
    }
  }

  result consume(CTX_T& ctx, DST_T& dst, PAT_T const& pat) {
    if (this->performSkip(ctx) == result::PARTIAL) {
      return result::PARTIAL;
    }

    constexpr bool backtrack = !pattern_traits<NEG_T, void>::FAILS_CLEANLY;

    if (child_parsers_.index() == 0) {
      auto res = std::get<0>(child_parsers_).consume(ctx, nil, pat.neg());

      switch (res) {
        case result::PARTIAL:
          return result::PARTIAL;
        case result::SUCCESS:
          if (backtrack) {
            constexpr bool commit = pattern_traits<OP_T, void>::FAILS_CLEANLY;
            if (commit) {
              ctx.commit_rollback();
            } else {
              // we cannot promise FAILS_CLEANLY since LHS_T does not, so we
              // might as well just cancel the rollback.
              ctx.cancel_rollback();
            }
          }
          return result::FAILURE;
        case result::FAILURE:
          if (backtrack) {
            ctx.commit_rollback();
          }
          child_parsers_ =
              child_parsers_t(std::in_place_index_t<1>(), ctx, dst, pat.op());
      }
    }
    abu_assume(child_parsers_.index() == 1);
    return std::get<1>(child_parsers_).consume(ctx, dst, pat.op());
  }
};

template <typename LHS_T, typename RHS_T>
auto except(LHS_T&& lhs, RHS_T&& rhs) {
  return Except<pattern_t<LHS_T>, pattern_t<RHS_T>>(
      make_pattern(forward<LHS_T>(lhs)), make_pattern(forward<RHS_T>(rhs)));
}

template <typename LHS_T, typename RHS_T, typename RECUR_TAG>
struct pattern_traits<Except<LHS_T, RHS_T>, RECUR_TAG> {
  enum {
    ATOMIC = pattern_traits<LHS_T, RECUR_TAG>::ATOMIC,
    BACKTRACKS = pattern_traits<LHS_T, RECUR_TAG>::BACKTRACKS ||
                 pattern_traits<RHS_T, RECUR_TAG>::BACKTRACKS ||
                 !pattern_traits<RHS_T, RECUR_TAG>::FAILS_CLEANLY,
    FAILS_CLEANLY = pattern_traits<LHS_T, RECUR_TAG>::FAILS_CLEANLY,
    PEEKABLE = pattern_traits<LHS_T, RECUR_TAG>::PEEKABLE &&
               pattern_traits<RHS_T, RECUR_TAG>::PEEKABLE,
    MAY_NOT_CONSUME = pattern_traits<LHS_T, RECUR_TAG>::MAY_NOT_CONSUME,
    APPENDS_DST = pattern_traits<LHS_T, RECUR_TAG>::APPENDS_DST,
    STABLE_APPENDS = pattern_traits<LHS_T, RECUR_TAG>::STABLE_APPENDS,
  };
};

template <typename LHS_T, typename RHS_T, typename CTX_T>
struct pat_attr_t<Except<LHS_T, RHS_T>, CTX_T> {
  using attr_type = abu::attr_t<LHS_T, CTX_T>;
};

}  // namespace ABULAFIA_NAMESPACE

#endif
