//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PATTERNS_BINARY_LIST_H_
#define ABULAFIA_PATTERNS_BINARY_LIST_H_

#include "abulafia/config.h"

#include "abulafia/pattern.h"
#include "abulafia/patterns/helpers/buffer.h"
#include "abulafia/patterns/unary/repeat.h"
#include "abulafia/support/assert.h"
#include "abulafia/support/nil.h"

#include <variant>

namespace ABULAFIA_NAMESPACE {

// List(a, b) -> a >> *( ignore(b) >> a )
template <typename VAL_PAT_T, typename SEP_PAT_T>
class List : public Pattern<List<VAL_PAT_T, SEP_PAT_T>> {
  VAL_PAT_T val_;
  SEP_PAT_T sep_;

 public:
  using val_pat_t = VAL_PAT_T;
  using sep_pat_t = SEP_PAT_T;

  List(val_pat_t const& val_pat, sep_pat_t const& sep)
      : val_(val_pat), sep_(sep) {}

  val_pat_t const& operand() const { return val_; }
  sep_pat_t const& separator() const { return sep_; }
};

template <typename CTX_T, typename DST_T, typename VAL_PAT_T,
          typename SEP_PAT_T>
class Parser<CTX_T, DST_T, List<VAL_PAT_T, SEP_PAT_T>>
    : public ParserBase<CTX_T, DST_T> {
  using PAT_T = List<VAL_PAT_T, SEP_PAT_T>;

  using child_adapter_t =
      buf_::CollectionParserAdapter<CTX_T, DST_T, VAL_PAT_T>;
  using sep_pat_parser_t = Parser<CTX_T, Nil, SEP_PAT_T>;

  using child_parser_t = std::variant<child_adapter_t, sep_pat_parser_t>;
  child_parser_t child_parser_;

 public:
  Parser(CTX_T& ctx, DST_T& dst, PAT_T const& pat)
      : ParserBase<CTX_T, DST_T>(ctx, dst),
        child_parser_(std::in_place_index_t<0>(), ctx, dst, pat.operand()) {
    constexpr bool backtrack =
        !pattern_traits<VAL_PAT_T, void>::FAILS_CLEANLY ||
        !pattern_traits<SEP_PAT_T, void>::FAILS_CLEANLY;

    if (backtrack) {
      ctx.prepare_rollback();
    }

    dst.clear();
  }

  result consume(CTX_T& ctx, DST_T& dst, PAT_T const& pat) {
    if (this->performSkip(ctx) == result::PARTIAL) {
      return result::PARTIAL;
    }

    constexpr bool backtrack =
        !pattern_traits<VAL_PAT_T, void>::FAILS_CLEANLY ||
        !pattern_traits<SEP_PAT_T, void>::FAILS_CLEANLY;

    while (1) {
      if (child_parser_.index() == 0) {
        // We are parsing a value.
        auto child_res =
            std::get<0>(child_parser_).consume(ctx, dst, pat.operand());
        switch (child_res) {
          case result::SUCCESS: {
            if (backtrack) {
              ctx.cancel_rollback();
              ctx.prepare_rollback();
            }

            child_parser_ = child_parser_t(std::in_place_index_t<1>(), ctx, nil,
                                           pat.separator());
          } break;
          case result::FAILURE:
            // this will cancel the consumption of the separator if there was
            // any
            if (backtrack) {
              ctx.commit_rollback();
            }
            return result::SUCCESS;
          case result::PARTIAL:
            return result::PARTIAL;
        }
      } else {
        abu_assume(child_parser_.index() == 1);
        // We are parsing a separator
        auto child_res =
            std::get<1>(child_parser_).consume(ctx, nil, pat.separator());
        switch (child_res) {
          case result::SUCCESS:
            child_parser_ = child_parser_t(std::in_place_index_t<0>(), ctx, dst,
                                           pat.operand());
            break;
          case result::FAILURE:
            // rollback whatever the separator may have eaten
            if (backtrack) {
              ctx.commit_rollback();
            }
            return result::SUCCESS;
          case result::PARTIAL:
            return result::PARTIAL;
        }
      }
    }
  }
};

template <typename LHS_T, typename RHS_T>
auto list(LHS_T&& lhs, RHS_T&& rhs) {
  return List<pattern_t<LHS_T>, pattern_t<RHS_T>>(
      make_pattern(forward<LHS_T>(lhs)), make_pattern(forward<RHS_T>(rhs)));
}

template <typename LHS_T, typename RHS_T, typename RECUR_TAG>
struct pattern_traits<List<LHS_T, RHS_T>, RECUR_TAG>
    : public default_pattern_traits {
  enum {
    BACKTRACKS = pattern_traits<LHS_T, RECUR_TAG>::BACKTRACKS ||
                 pattern_traits<RHS_T, RECUR_TAG>::BACKTRACKS ||
                 !pattern_traits<RHS_T, RECUR_TAG>::FAILS_CLEANLY ||
                 !pattern_traits<LHS_T, RECUR_TAG>::FAILS_CLEANLY,
    FAILS_CLEANLY = false,  // TODO: not 100% correct.
    MAY_NOT_CONSUME = pattern_traits<LHS_T, RECUR_TAG>::MAY_NOT_CONSUME,
    ATOMIC = false,
    APPENDS_DST = true,
    STABLE_APPENDS = true,
  };
};

template <typename LHS_T, typename RHS_T, typename CTX_T>
struct pat_attr_t<List<LHS_T, RHS_T>, CTX_T> {
  using attr_type = typename std::conditional<
      std::is_same<Nil, abu::attr_t<LHS_T, CTX_T>>::value, Nil,
      std::vector<abu::attr_t<LHS_T, CTX_T>>>::type;
};
}  // namespace ABULAFIA_NAMESPACE

#endif
