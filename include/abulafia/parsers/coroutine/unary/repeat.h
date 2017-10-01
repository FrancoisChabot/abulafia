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
class RepeatImpl : public ParserBase<CTX_T> {
  using ctx_t = CTX_T;
  using dst_t = DST_T;
  using pat_t = Repeat<CHILD_PAT_T, MIN_REP, MAX_REP>;



  struct child_req_t : public ctx_t::req_t {
    enum {
      // The aternative is to push_back on start, and pop_back on failure,
      // which gets a little messy.
      ATOMIC = true,

      // This is extremely important, since we can succeed even if the child parser fails.
      // The exception to this is if MIN_REP == MAX_REP (except for 0). In which case, failure 
      // of the child guarantees failure of the parent.
      FAILS_CLEANLY = MIN_REP != MAX_REP || MAX_REP == 0
    };
  };


  using child_ctx_t = override_context_reqs_t<ctx_t, child_req_t>;
  using child_dst_t = dst_t;
  using child_pat_t = CHILD_PAT_T;
  using child_parser_t = Parser<child_ctx_t, child_dst_t, child_pat_t>;

  std::size_t count_ = 0;
  child_parser_t child_parser_;

 public:
  RepeatImpl(ctx_t ctx, dst_t dst, pat_t const& pat)
      : ParserBase<ctx_t>(ctx),
        child_parser_(ctx.overrideReqs(child_req_t()), child_dst_t(dst), pat.operand()) {
    dst.clear();
  }

  result consume(ctx_t ctx, dst_t dst, pat_t const& pat) {
    if (this->performSkip(ctx) == result::PARTIAL) {
      return result::PARTIAL;
    }

    auto child_ctx = ctx.overrideReqs(child_req_t());
    while (1) {
      auto child_res = child_parser_.consume(child_ctx, child_dst_t(dst), pat.operand());
      switch (child_res) {
        case result::FAILURE:
          if (count_ >= MIN_REP) {
            return result::SUCCESS;
          } else {
            return result::FAILURE;
          }

        case result::PARTIAL:
          return result::PARTIAL;
        case result::SUCCESS:
          count_++;

          if (MAX_REP != 0 && count_ == MAX_REP) {
            return result::SUCCESS;
          }

          // If we are still going, then we need to reset the child's parser
          child_parser_ = child_parser_t(child_ctx, dst, pat.operand());
      }
    }
  }
};

template <typename CTX_T, typename DST_T, typename CHILD_PAT_T,
  std::size_t MIN_REP, std::size_t MAX_REP>
struct ParserFactory<CTX_T, DST_T, Repeat<CHILD_PAT_T, MIN_REP, MAX_REP>> {
  static auto create(CTX_T ctx, DST_T dst, Repeat<CHILD_PAT_T, MIN_REP, MAX_REP> const& pat) {
    // Check which requirements we need to fullfill
    constexpr bool apply_fails_cleanly = MIN_REP != MAX_REP || MAX_REP == 0;

    // Determine if we need to re-wrap the dst

    using a = RepeatImpl<CTX_T, DST_T, CHILD_PAT_T, MIN_REP, MAX_REP>;
    using b = fulfill_req_if_t<a, Req::FAILS_CLEANLY, apply_fails_cleanly>;

    return b(ctx, dst, pat);
  }
};
}  // namespace ABULAFIA_NAMESPACE

#endif
