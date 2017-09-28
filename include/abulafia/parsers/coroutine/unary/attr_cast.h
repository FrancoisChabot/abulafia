//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PARSERS_COROUTINE_ATTR_CAST_H_
#define ABULAFIA_PARSERS_COROUTINE_ATTR_CAST_H_

#include "abulafia/config.h"

#include "abulafia/parser.h"
#include "abulafia/patterns/unary/attr_cast.h"

namespace ABULAFIA_NAMESPACE {

template <typename CTX_T, typename DST_T, typename ATTR_T, typename CHILD_PAT_T>
class Parser<CTX_T, DST_T, AttrCast<ATTR_T, CHILD_PAT_T>>
    : public ParserBase<CTX_T, DST_T, PARSER_OPT_NO_SKIP> {
  using PAT_T = AttrCast<ATTR_T, CHILD_PAT_T>;
  using parser_t = Parser<CTX_T, DST_T, CHILD_PAT_T>;

  parser_t parser_;

 public:
  Parser(CTX_T& ctx, DST_T& dst, PAT_T const& pat)
      : ParserBase<CTX_T, DST_T, PARSER_OPT_NO_SKIP>(ctx, dst),
        parser_(ctx, dst, pat.operand()) {}

  result consume(CTX_T& ctx, DST_T& dst, PAT_T const& pat) {
    return parser_.consume(ctx, dst, pat.operand());
  }

  result peek(CTX_T& ctx, PAT_T const& pat) {
    return parser_.peek(ctx, pat.operand());
  }
};
}  // namespace ABULAFIA_NAMESPACE

#endif
