//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PARSERS_COROUTINE_WITH_SKIPPER_H_
#define ABULAFIA_PARSERS_COROUTINE_WITH_SKIPPER_H_

#include "abulafia/config.h"

#include "abulafia/parser.h"
#include "abulafia/patterns/with_skipper.h"

namespace ABULAFIA_NAMESPACE {
/*
template <typename CTX_T, typename DST_T, typename CHILD_PAT_T, typename SKIP_T>
class WithSkipperImpl : public ParserBase<CTX_T> {
  using ctx_t = CTX_T;
  using dst_t = DST_T;
  using pat_t = WithSkipper<CHILD_PAT_T, SKIP_T>;

  using sub_ctx_t = typename ctx_t::template set_skipper_t<SKIP_T>;
  Parser<sub_ctx_t, dst_t, CHILD_PAT_T> child_parser_;

 public:
   WithSkipperImpl(ctx_t ctx, dst_t dst, pat_t const& pat)
      : ParserBase<CTX_T, DST_T>(ctx, dst),
        child_parser_(ctx., dst,
                      pat.getChild()) {}

  result consume(ctx_t ctx, dst_t dst, pat_t const& pat) {
    sub_ctx_t sub_ctx(ctx, pat.getSkip());
    return child_parser_.consume(sub_ctx, dst, pat.getChild());
  }
};
*/
}  // namespace ABULAFIA_NAMESPACE

#endif
