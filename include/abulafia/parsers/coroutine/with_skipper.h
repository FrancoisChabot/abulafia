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
}  // namespace ABULAFIA_NAMESPACE

#endif
