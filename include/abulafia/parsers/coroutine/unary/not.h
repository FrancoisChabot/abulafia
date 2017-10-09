//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PARSERS_COROUTINE_NOT_H_
#define ABULAFIA_PARSERS_COROUTINE_NOT_H_

#include "abulafia/config.h"

#include "abulafia/parser.h"
#include "abulafia/patterns/unary/not.h"

namespace ABULAFIA_NAMESPACE {

template <typename CTX_T, typename DST_T, typename REQ_T, typename CHILD_PAT_T>
class NotImpl {
  using pat_t = Not<CHILD_PAT_T>;

  struct child_req_t : public REQ_T {
    enum {
      ATOMIC = false,
      FAILS_CLEANLY = false,
    };
  };

  using child_parser_t = Parser<CTX_T, DST_T, child_req_t, CHILD_PAT_T>;
  child_parser_t parser_;

 public:
  NotImpl(CTX_T ctx, DST_T dst, pat_t const& pat)
      : parser_(ctx, dst, pat.operand()) {
    ctx.data().prepare_rollback();
  }

  Result consume(CTX_T ctx, DST_T dst, pat_t const& pat) {
    auto status = parser_.consume(ctx, dst, pat.operand());
    switch (status) {
      case Result::SUCCESS:
        // just commit the rollback anyways, this allows us to promise
        // FAILS_CLEANLY
        ctx.data().commit_rollback();
        return Result::FAILURE;

      case Result::FAILURE:
        ctx.data().commit_rollback();
        return Result::SUCCESS;

      case Result::PARTIAL:
        return Result::PARTIAL;
    }

    abu_unreachable();
  }

  Result peek(CTX_T ctx, pat_t const& pat) {
    auto status = parser_.peek(ctx, pat.operand());

    switch (status) {
      case Result::SUCCESS:
        return Result::FAILURE;
      case Result::FAILURE:
        return Result::SUCCESS;
      case Result::PARTIAL:
        return Result::PARTIAL;
    }
    abu_unreachable();
  }
};

template <typename CHILD_PAT_T>
struct ParserFactory<Not<CHILD_PAT_T>> {
  using pat_t = Not<CHILD_PAT_T>;

  static constexpr DstBehavior dst_behavior() { return DstBehavior::IGNORE; }

  enum {
    ATOMIC = true,
    FAILS_CLEANLY = true,
  };

  template <typename CTX_T, typename DST_T, typename REQ_T>
  using type = NotImpl<CTX_T, DST_T, REQ_T, CHILD_PAT_T>;
};

}  // namespace ABULAFIA_NAMESPACE

#endif
