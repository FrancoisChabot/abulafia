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

template <typename CTX_T, typename DST_T, typename CHILD_PAT_T>
class Parser<CTX_T, DST_T, Optional<CHILD_PAT_T>> : public ParserBase<CTX_T, DST_T> {
  using PAT_T = Optional<CHILD_PAT_T>;
  using child_parser_t = Parser<CTX_T, DST_T, CHILD_PAT_T>;

  child_parser_t parser_;

 public:
  Parser(CTX_T& ctx, DST_T& dst, PAT_T const& pat)
      : ParserBase<CTX_T, DST_T>(ctx, dst), parser_(ctx, dst, pat.operand()) {
    ctx.prepare_rollback();
  }

  result consume(CTX_T& ctx, DST_T& dst, PAT_T const& pat) {
    if (this->performSkip(ctx) == result::PARTIAL) {
      return result::PARTIAL;
    }

    auto status = parser_.consume(ctx, dst, pat.operand());
    switch (status) {
      case result::SUCCESS:
        return result::SUCCESS;
      case result::FAILURE:
        ctx.commit_rollback();
        return result::SUCCESS;

      case result::PARTIAL:
        return result::PARTIAL;
    }
    abu_unreachable();
  }

  result peek(CTX_T& ctx, PAT_T const& pat) {
    return result::SUCCESS;
  }
};
}  // namespace ABULAFIA_NAMESPACE

#endif
