//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PARSERS_COROUTINE_REPEAT_H_
#define ABULAFIA_PARSERS_COROUTINE_REPEAT_H_

#include "abulafia/config.h"

#include "abulafia/parser.h"
#include "abulafia/patterns/unary/repeat.h"

namespace ABULAFIA_NAMESPACE {

template <typename CTX_T, typename DST_T, typename CHILD_PAT_T,
          std::size_t MIN_REP, std::size_t MAX_REP>
class Parser<CTX_T, DST_T, Repeat<CHILD_PAT_T, MIN_REP, MAX_REP>>
    : public ParserBase<CTX_T, DST_T> {
  using PAT_T = Repeat<CHILD_PAT_T, MIN_REP, MAX_REP>;
  using child_adapter_t =
      buf_::CollectionParserAdapter<CTX_T, DST_T, CHILD_PAT_T>;

  std::size_t count_ = 0;
  child_adapter_t child_parser_;

  enum { needs_backtrack = !pattern_traits<PAT_T, void>::FAILS_CLEANLY };

 public:
  Parser(CTX_T& ctx, DST_T& dst, PAT_T const& pat)
      : ParserBase<CTX_T, DST_T>(ctx, dst),
        child_parser_(ctx, dst, pat.operand()) {
    dst.clear();
    if (needs_backtrack) {
      ctx.prepare_rollback();
    }
  }

  result consume(CTX_T& ctx, DST_T& dst, PAT_T const& pat) {
    if (this->performSkip(ctx) == result::PARTIAL) {
      return result::PARTIAL;
    }

    while (1) {
      auto child_res = child_parser_.consume(ctx, dst, pat.operand());
      switch (child_res) {
        case result::FAILURE:
          // cancel the child's data consumption
          // Technically, we could cancel the rollback in the failure branch,
          // but guaranteeing FAILS_CLEANLY is better.
          if (needs_backtrack) {
            ctx.commit_rollback();
          }
          if (count_ >= MIN_REP) {
            return result::SUCCESS;
          } else {
            return result::FAILURE;
          }

        case result::PARTIAL:
          return result::PARTIAL;
        case result::SUCCESS:
          count_++;
          if (needs_backtrack) {
            ctx.cancel_rollback();
          }

          if (MAX_REP != 0 && count_ == MAX_REP) {
            return result::SUCCESS;
          }

          if (needs_backtrack) {
            ctx.prepare_rollback();
          }

          // If we are still going, then we need to reset the child's parser
          child_parser_ = child_adapter_t(ctx, dst, pat.operand());
      }
    }
  }
};
}  // namespace ABULAFIA_NAMESPACE

#endif
