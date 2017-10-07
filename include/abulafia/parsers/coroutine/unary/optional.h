//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PARSERS_COROUTINE_OPTIONAL_H_
#define ABULAFIA_PARSERS_COROUTINE_OPTIONAL_H_

#include "abulafia/config.h"

#include "abulafia/parser.h"
#include "abulafia/patterns/unary/optional.h"

namespace ABULAFIA_NAMESPACE {

template <typename CTX_T, typename DST_T, typename REQ_T, typename CHILD_PAT_T>
class OptImpl {
  using pat_t = Optional<CHILD_PAT_T>;

  struct child_req_t {
    enum {
      // The aternative is to push_back on start, and pop_back on failure,
      // which gets a little messy.
      ATOMIC = true,

      // This is extremely important, since we can succeed even if the child
      // parser fails.
      // The exception to this is if MIN_REP == MAX_REP (except for 0). In which
      // case, failure
      // of the child guarantees failure of the parent.
      FAILS_CLEANLY = false,

      // Propagate
      CONSUMES_ON_SUCCESS = REQ_T::CONSUMES_ON_SUCCESS
    };
  };

  using child_parser_t = Parser<CTX_T, DST_T, child_req_t, CHILD_PAT_T>;
  child_parser_t parser_;

 public:
  OptImpl(CTX_T ctx, DST_T dst, pat_t const& pat)
      : parser_(ctx, dst, pat.operand()) {
    ctx.data().prepare_rollback();
  }

  Result consume(CTX_T ctx, DST_T dst, pat_t const& pat) {
    auto status = parser_.consume(ctx, dst, pat.operand());
    switch (status) {
      case Result::SUCCESS:
        return Result::SUCCESS;
      case Result::FAILURE:
        ctx.data().commit_rollback();
        return Result::SUCCESS;

      case Result::PARTIAL:
        return Result::PARTIAL;
    }
    abu_unreachable();
  }

  Result peek(CTX_T ctx, pat_t const& pat) { return Result::SUCCESS; }
};

template <typename CHILD_PAT_T>
struct ParserFactory<Optional<CHILD_PAT_T>> {
  using pat_t = Optional<CHILD_PAT_T>;

  static constexpr DstBehavior dst_behavior() {
    return ParserFactory<CHILD_PAT_T>::dst_behavior();
  }

  enum {
    ATOMIC = true,
    FAILS_CLEANLY = true,
  };

  template <typename CTX_T, typename DST_T, typename REQ_T>
  using type = OptImpl<CTX_T, DST_T, REQ_T, CHILD_PAT_T>;
};

}  // namespace ABULAFIA_NAMESPACE

#endif
