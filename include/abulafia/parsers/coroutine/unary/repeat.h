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

template <typename CTX_T, typename DST_T, typename REQ_T, typename CHILD_PAT_T,
          std::size_t MIN_REP, std::size_t MAX_REP>
class RepeatImpl {
  static_assert(!REQ_T::CONSUMES_ON_SUCCESS || MIN_REP > 0);

  using ctx_t = CTX_T;
  using dst_t = DST_T;
  using req_t = REQ_T;
  using pat_t = Repeat<CHILD_PAT_T, MIN_REP, MAX_REP>;

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
      FAILS_CLEANLY = MIN_REP != MAX_REP || MAX_REP == 0,

      // Propagate
      CONSUMES_ON_SUCCESS = REQ_T::CONSUMES_ON_SUCCESS
    };
  };

  using child_dst_t = dst_t;
  using child_parser_t = Parser<ctx_t, child_dst_t, child_req_t, CHILD_PAT_T>;

  std::size_t count_ = 0;
  child_parser_t child_parser_;

 public:
  RepeatImpl(ctx_t ctx, dst_t dst, pat_t const& pat)
      : child_parser_(ctx, child_dst_t(dst), pat.operand()) {}

  Result consume(ctx_t ctx, dst_t dst, pat_t const& pat) {
    while (1) {
      auto child_res =
          child_parser_.consume(ctx, child_dst_t(dst), pat.operand());
      switch (child_res) {
        case Result::FAILURE:
          if (count_ >= MIN_REP) {
            return Result::SUCCESS;
          } else {
            return Result::FAILURE;
          }

        case Result::PARTIAL:
          return Result::PARTIAL;
        case Result::SUCCESS:
          count_++;

          if (MAX_REP != 0 && count_ == MAX_REP) {
            return Result::SUCCESS;
          }

          // If we are still going, then we need to reset the child's parser
          child_parser_ = child_parser_t(ctx, dst, pat.operand());
      }
    }
  }
};

template <typename CHILD_PAT_T, std::size_t MIN_REP, std::size_t MAX_REP>
struct ParserFactory<Repeat<CHILD_PAT_T, MIN_REP, MAX_REP>> {
  using pat_t = Repeat<CHILD_PAT_T, MIN_REP, MAX_REP>;

  static constexpr DstBehavior dst_behavior() {
    return ParserFactory<CHILD_PAT_T>::dst_behavior();
  }

  enum {
    ATOMIC = false,
    FAILS_CLEANLY = false,
  };

  template <typename CTX_T, typename DST_T, typename REQ_T>
  using type = RepeatImpl<CTX_T, DST_T, REQ_T, CHILD_PAT_T, MIN_REP, MAX_REP>;
};
}  // namespace ABULAFIA_NAMESPACE

#endif
