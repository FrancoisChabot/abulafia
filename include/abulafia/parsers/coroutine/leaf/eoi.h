//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PARSERS_COROUTINE_EOI_H_
#define ABULAFIA_PARSERS_COROUTINE_EOI_H_

#include "abulafia/config.h"

#include "abulafia/parser.h"
#include "abulafia/patterns/leaf/eoi.h"
#include "abulafia/support/assert.h"

#include <variant>

namespace ABULAFIA_NAMESPACE {
template <typename CTX_T>
class Parser<CTX_T, Nil, Eoi> : public ParserBase<CTX_T, Nil> {
  using PAT_T = Eoi;

 public:
  Parser(CTX_T& ctx, Nil& dst, PAT_T const&)
      : ParserBase<CTX_T, Nil>(ctx, dst) {}

  result consume(CTX_T& ctx, Nil&, PAT_T const& pat) {
    if (this->performSkip(ctx) == result::PARTIAL) {
      return result::PARTIAL;
    }

    return peek(ctx, pat);
  }

  result peek(CTX_T& ctx, PAT_T const&) {
    if (ctx.empty()) {
      return ctx.final_buffer() ? result::SUCCESS : result::PARTIAL;
    }
    return result::FAILURE;
  }
};
}  // namespace ABULAFIA_NAMESPACE

#endif
